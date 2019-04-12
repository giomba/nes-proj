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
            struct Msg msg;
            msg.type = ASSOCIATION_REQUEST_MSG;
            net_send(&msg, sizeof(msg), NULL);
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
        struct MsgBatteryStatus msg;
        msg.type = BATTERY_STATUS_MSG;
        msg.battery_percentage = 77;
        net_send(&msg, sizeof(msg), &assigner_address);
        etimer_reset(&broadcast_timer);
    }
    if (ev == PROCESS_EVENT_MSG && *((enum CartEvent*)data) == CART_EVENT_ASSIGNED) {
        /* cart has been assigned to a new customer */
        printf("[I] Assigned to customer\n");
        customer_id = ((MsgAssignment*)pkt.data)->customer_id;
        status = SHOPPING;
    }

}


