#include "status.h"

enum CartStatus status;
struct etimer broadcast_timer;
linkaddr_t assigner_address;

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


