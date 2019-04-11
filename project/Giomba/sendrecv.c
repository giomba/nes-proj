#include "sendrecv.h"

struct MacPkt pkt;

void net_recv(const void* data, uint16_t len, const linkaddr_t* src, const linkaddr_t* dst) {
    /* discard too long packet */
    if (len > 128) {
        LOG_INFO("[WW] dropping too long packet %d\n", len);
        return;
    }

    /* fill packet to pass to upper processing layer */
    memcpy(pkt.data, data, len);
    pkt.len = len;
    pkt.src = *src;
    pkt.dst = *dst;

    LOG_INFO("Received %d bytes from ", len); LOG_INFO_LLADDR(src);	LOG_INFO("\n");

    switch ( ((struct Msg*)data)->type ) {
        case ASSOCIATION_REPLY_MSG:
            event = CART_EVENT_ASSOCIATED;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        default:
            LOG_INFO("[W] message type unknown\n");
            break;
    }
}

