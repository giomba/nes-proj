#include "status.h"

#define MAX_PRODUCT 10

enum CartStatus status;
struct etimer broadcast_timer;
linkaddr_t assigner_address;
linkaddr_t cash_address;
uint32_t customer_id = 1234;
uint8_t nprod;
product_t list[MAX_PRODUCT];

void s_not_associated(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_TIMER) {
        /* at time expiration, send broadcast message to request association with assigner */
        if (etimer_expired(&broadcast_timer)) {
            printf("[I] Sending association request msg\n");
            msg m;
            m.msg_type = ASSOCIATION_REQUEST_MSG;
            net_send(&m, sizeof(m), NULL);
            etimer_reset(&broadcast_timer);
        }
    }
    else    /* if a msg is received from network and represents an association event, then associate */
    if (ev == PROCESS_EVENT_MSG && event == CART_EVENT_ASSOCIATED) {
        printf("[I] Associated with Assigner\n");
        assigner_address = pkt.src;
        nprod = 0;
        status = ASSOCIATED;
    }
}

void s_associated(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_TIMER) {
        /* now send battery level */
        printf("[I] Sending battery level\n");
        struct battery_msg m;
        m.msg_type = BATTERY_STATUS_MSG;
        m.battery_percentage = 77;
        net_send(&m, sizeof(m), &assigner_address);
        etimer_reset(&broadcast_timer);
    }
    if (ev == PROCESS_EVENT_MSG && event == CART_EVENT_ASSIGNED) {
        /* cart has been assigned to a new customer */
        customer_id = ((assign_msg*)pkt.data)->customer_id;
        printf("[I] Assigned to customer id %d\n", (int)customer_id);
        status = SHOPPING;
    }
}

void s_shopping(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_MSG) {
        if (event == CART_EVENT_NEW_PRODUCT) {
            /* add product to list */
            if (nprod < MAX_PRODUCT) {
                printf("[I] Adding item %d to cart\n", nprod);
                item_msg* m = (item_msg*)pkt.data;
                memcpy(&list[nprod++], &m->p, sizeof(product_t));
            } else {
                printf("[W] Too many products. Dropping.\n");
            }
        }
        if (event == CART_EVENT_CASH_OUT) {
            /* answer the cash if you are the one with that customer_id */
            if (((cash_out_msg*)pkt.data)->customer_id == customer_id) {
                printf("[I] It's me! I'm cashing out :-)\n");
                basket_msg m;
                m.msg_type = BASKET_MSG;
                m.n_products = nprod - 1;
                m.customer_id = customer_id;    /* TODO -- is this really needed? */
                net_send(&m, sizeof(m), &cash_address);
                status = CASH_OUT_WAIT4ACK;
            }
            else {
                printf("[I] I am customer id %d; customer id %d is cashing out nearby\n", (int)customer_id, (int)((cash_out_msg*)pkt.data)->customer_id );
            }
        }
    }
}

void s_cash_out_wait4ack(process_event_t ev, process_data_t data) {
    /* Just wait for cash ack */
    if (event == CART_EVENT_CASH_OUT_ACK) {
        printf("[I] Acknoweledgment received fromc cash. Now I'll send the list.\n");
        status = CASH_OUT_SEND_LIST;
        /* this wakes up the process that otherwise would wait indefinitely for an event that will never occurr */
        process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
    }
}

void s_cash_out_send_list(process_event_t ev, process_data_t data) {
    /* Send list, then go back to initial state */
    for (uint8_t i = 0; i < nprod; ++i) {
        printf("[I] Sending product %d of %d...\n", i, nprod - 1);
        product_msg m;
        m.msg_type = PRODUCT_MSG;
        m.customer_id = customer_id;
        m.product_id = list[i].product_id;
        m.price = list[i].price;
        net_send(&m, sizeof(m), &cash_address);
    }
    nprod = 0;

    customer_id = 0;
    memset(&cash_address, 0, sizeof(cash_address));

    printf("[I] END. Go back to ASSOCIATED status\n");
    etimer_reset(&broadcast_timer);
    status = ASSOCIATED;
}

