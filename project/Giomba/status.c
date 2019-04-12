#include "status.h"

enum CartStatus status;
struct etimer broadcast_timer;
linkaddr_t assigner_address;
uint32_t customer_id;

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
    if (ev == PROCESS_EVENT_MSG && *((enum CartEvent*)data) == CART_EVENT_ASSOCIATED) {
        printf("[I] Associated with Assigner\n");
        assigner_address = pkt.src;
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
    if (ev == PROCESS_EVENT_MSG && *((enum CartEvent*)data) == CART_EVENT_ASSIGNED) {
        /* cart has been assigned to a new customer */
        printf("[I] Assigned to customer\n");
        customer_id = ((assign_msg*)pkt.data)->customer_id;
        status = SHOPPING;
    }

}


