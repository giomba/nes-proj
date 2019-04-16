#include "sendrecv.h"

struct MacPkt pkt;

uint8_t net_buffer[256];

void net_init() {
    nullnet_buf = net_buffer;
    nullnet_set_input_callback(net_recv);
}

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

    LOG_INFO("Received %d bytes from ", len); LOG_INFO_LLADDR(src);
    LOG_INFO("type %d", ((msg*)data)->msg_type);
    LOG_INFO("\n");

    switch ( ((msg*)data)->msg_type ) {
        case ASSOCIATION_REPLY_MSG:
            event = CART_EVENT_ASSOCIATED;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        case ASSIGNMENT_MSG:
            event = CART_EVENT_ASSIGNED;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        case ITEM_MSG:
            event = CART_EVENT_NEW_PRODUCT;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        case CASH_OUT_MSG:
            event = CART_EVENT_CASH_OUT;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        case START_OF_LIST_PRODUCTS_MSG:
            event = CART_EVENT_CASH_OUT_ACK;
            process_post(&cart_main_process, PROCESS_EVENT_MSG, NULL);
        break;
        default:
            LOG_INFO("[W] message type %d unknown\n", ((msg*)data)->msg_type);
            break;
    }
}

void net_send(const void* data, uint16_t len, const linkaddr_t* dst) {
    memcpy(net_buffer, data, len);
    nullnet_len = len;
    NETSTACK_NETWORK.output(dst);
}

