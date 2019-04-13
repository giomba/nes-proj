#include "status.h"

#define MAX_PRODUCT 10

enum CartStatus status;
struct etimer broadcast_timer;
linkaddr_t assigner_address;
linkaddr_t cash_address;
uint32_t customer_id;
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
        printf("[I] Assigned to customer\n");
        customer_id = ((assign_msg*)pkt.data)->customer_id;
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
            basket_msg m;
            m.n_products = nprod - 1;
            m.customer_id = customer_id;    /* TODO -- is this really needed? */
            net_send(&m, sizeof(m), &cash_address);
            status = CASH_OUT_WAIT4ACK;
        }
    }
}

void s_cash_out_wait4ack(process_event_t ev, process_data_t data) {
    /* Just wait for cash ack */
    if (ev == CART_EVENT_CASH_OUT_ACK) {
        status = CASH_OUT_SEND_LIST;
    }
}

void s_cash_out_send_list(process_event_t ev, process_data_t data) {
    /* Send list, then go back to initial state */
    for (uint8_t i = 0; i < nprod; ++i) {
        printf("[I] Sending product %d of %d...", i, nprod - 1);
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

    status = ASSOCIATED;
}

