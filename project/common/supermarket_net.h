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
    uint8_t test;
};

struct MsgBatteryStatus {
    enum MsgType type;
    uint8_t battery_percentage;
};

struct MsgAssignment {
    enum MsgType type;
    uint32_t customer_id;
};

struct MsgProduct {
    enum MsgType type;
    uint32_t product_id;
    float price;
};

struct MsgCashOut {
    enum MsgType type;
    uint32_t customer_id;
};

struct MsgItemElem {
    enum MsgType type;
    float price;
    bool last;
};

#endif
