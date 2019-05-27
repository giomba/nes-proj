#include "status.h"

#define MAX_PRODUCT 20

enum CartStatus status;
struct etimer assigner_timer;   /* timer to periodically make a report to the assigner */
struct etimer battery_timer;    /* timer to periodically read battery status */
struct ctimer led_timer;        /* timer to shortly blink leds */
int8_t battery_charge = 50;
linkaddr_t assigner_address;
linkaddr_t cash_address;
uint32_t customer_id = 1234;
uint8_t nprod = 0;
uint8_t remaining = 0;
uint16_t totalPrice = 0;
uint8_t nprod_index = 0; 	//variable used to keep track of the index of the product to be sent
product_t list[MAX_PRODUCT];

void s_not_associated(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_TIMER) {
        /* at time expiration, send broadcast message to request association with assigner */
        if (etimer_expired(&assigner_timer)) {
            printf("[I] Sending association request msg\n");
            msg m;
            m.msg_type = ASSOCIATION_REQUEST_MSG;
            net_send(&m, sizeof(m), NULL);
            etimer_reset(&assigner_timer);
        }
    }
    else    /* if a msg is received from network and represents an association event, then associate */
    if (ev == PROCESS_EVENT_MSG && event == CART_EVENT_ASSOCIATED) {
        printf("[I] Associated with Assigner\n");

        assigner_address = pkt.src;
        nprod = 0;
        status = ASSOCIATED;

        /* blink led */
        led_blink(NULL);
    }
}

void s_associated(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_TIMER) {
        /* now send battery level */
        printf("[I] Sending battery level\n");
        struct battery_msg m;
        m.msg_type = BATTERY_STATUS_MSG;
        m.battery_percentage = battery_charge;
        net_send(&m, sizeof(m), &assigner_address);
        etimer_reset(&assigner_timer);
    }
    if (ev == PROCESS_EVENT_MSG && event == CART_EVENT_ASSIGNED) {
        /* cart has been assigned to a new customer */
        customer_id = ((assign_msg*)pkt.data)->customer_id;
        printf("[I] Assigned to customer id %d\n", (int)customer_id);
        leds_on(LEDS_GREEN);
        status = SHOPPING;
    }
}

void s_shopping(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_MSG) {
        if (event == CART_EVENT_NEW_PRODUCT) {
            /* add product to list */
            if (nprod < MAX_PRODUCT) {
                item_msg* m = (item_msg*)pkt.data;
                memcpy(&list[nprod], &m->p, sizeof(product_t));
                printf("[I] Adding item #%d, id %d, price %d to cart\n", nprod, (int)list[nprod].product_id, (int)list[nprod].price);
                totalPrice+=list[nprod].price;
                nprod++;
            } else {
                printf("[W] Too many products. Dropping.\n");
            }
        }
        if (event == CART_EVENT_CASH_OUT) {
            /* answer the cash if you are the one with that customer_id */
            if (((cash_out_msg*)pkt.data)->customer_id == customer_id) {
                printf("[I] It's me! I'm cashing out :-)\n");
                cash_address = pkt.src;
                basket_msg m;
                m.msg_type = BASKET_MSG;
                m.n_products = nprod;
                m.customer_id = customer_id;    /* this is needed to handle multiple customers at the cash register */
                net_send(&m, sizeof(m), &cash_address);
                status = CASH_OUT_WAIT4ACK;
            }
            else {
                printf("[I] I am customer id %d; customer id %d is cashing out nearby\n", (int)customer_id, (int)((cash_out_msg*)pkt.data)->customer_id );
            }
        }
    }
    if (ev == button_hal_release_event) {   /* any button will do */
        /* ask product to send items to me */
        printf("[I] Dear product Launchpad, please send items to me\n");
        msg m;
        m.msg_type = START_SHOPPING_MSG;
        net_send(&m, sizeof(m), NULL);
    }
}

void s_cash_out_wait4ack(process_event_t ev, process_data_t data) {
    /* Just wait for cash ack */
    if (event == CART_EVENT_CASH_OUT_ACK) {
        printf("[I] Acknoweledgment received from cash. Total price is %d. Now I'll send the list.\n", totalPrice);
        remaining = nprod;
        status = CASH_OUT_SEND_LIST;
        /* this wakes up the process that otherwise would wait indefinitely for an event that will never occur */
        process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
    }
}

void s_cash_out_send_list(process_event_t ev, process_data_t data) {
    /* Send list, then go back to initial state */

    printf("[I] Remaining %d products\n", (int)remaining);

    product_partial_list_msg m;
    m.msg_type = PRODUCT_PARTIAL_LIST_MSG;
    m.customer_id = customer_id;
    m.array_len = (remaining < PRODUCT_ARRAY_MAX_LEN) ? remaining : PRODUCT_ARRAY_MAX_LEN;

    uint8_t done = nprod - remaining;

    for (uint8_t i = 0; i < ((remaining < PRODUCT_ARRAY_MAX_LEN) ? remaining : PRODUCT_ARRAY_MAX_LEN); ++i) {
        printf("[I] Adding %d to the list\n", (int)i);
        m.p[i] = list[done + i];
    }

    printf("[I] sending list right now\n");
    net_send(&m, sizeof(m), &cash_address);

    remaining -= (remaining < PRODUCT_ARRAY_MAX_LEN) ? remaining : PRODUCT_ARRAY_MAX_LEN;

    printf("[I] and now remaining %d products\n", (int)remaining);

    if (remaining == 0) {
        printf("[I] it is the end, so now I reset the state\n");
        /* reset everything */
        nprod = 0;
        nprod_index = 0;
        customer_id = 1234;
        totalPrice = 0;
        memset(&cash_address, 0, sizeof(cash_address));

        printf("[I] List END. Go back to ASSOCIATED status\n");
        etimer_restart(&assigner_timer);
        leds_off(LEDS_GREEN);
        status = ASSOCIATED;
    }

}

