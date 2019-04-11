#ifndef SUPERMARKET_NET_H
#define SUPERMARKET_NET_H

enum MsgType {
    ASSOCIATION_REQUEST_MSG,
    ASSOCIATION_REPLY_MSG,
    BATTERY_STATUS_MSG,
    ASSIGNMENT_MSG,
    PRODUCT_MSG,
    CASHOUT_MSG,
    ITEM_ELEM_MSG
};

struct Msg {
    enum MsgType type;
    int test;
};

#endif
