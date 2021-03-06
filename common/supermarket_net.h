#ifndef SUPERMARKET_NET_H
#define SUPERMARKET_NET_H

#include "product.h"

#define PRODUCT_ARRAY_MAX_LEN 5

enum message_type {
    ASSOCIATION_REQUEST_MSG,
    ASSOCIATION_REPLY_MSG,
    BATTERY_STATUS_MSG,
    ASSIGNMENT_MSG,
    CASH_OUT_MSG,
    ITEM_MSG,                   /* from item to cart */
    PRODUCT_PARTIAL_LIST_MSG,   /* from cart to cash */
    BASKET_MSG,
    START_OF_LIST_PRODUCTS_MSG,
    PRODUCT_PARTIAL_LIST_ACK,   /* everytime the cash register receives a product message, it sends an ack to the cart so that the cart can send the next product message */
    START_SHOPPING_MSG
};

typedef struct msg {
    enum message_type msg_type;
}msg;

typedef struct assoc_req_msg
{
	enum message_type msg_type;
	uint8_t battery_percentage;

}assoc_req_msg;


typedef struct assoc_reply_msg
{
	enum message_type msg_type;
}assoc_reply_msg;


typedef struct battery_msg
{
	enum message_type msg_type;
	uint8_t battery_percentage;
}battery_msg;


typedef struct assign_msg
{
	enum message_type msg_type;
	uint32_t customer_id;
}assign_msg;


typedef struct basket_msg
{
	enum message_type msg_type;
	uint8_t n_products;
	uint32_t customer_id;
}basket_msg;

typedef struct item_msg
{
    enum message_type msg_type;
    product_t p;
} item_msg;

typedef struct cash_out_msg
{
	enum message_type msg_type;
	uint32_t customer_id;
}cash_out_msg;


typedef struct product_partial_list_msg
{
	enum message_type msg_type;
    uint32_t customer_id;
    uint8_t array_len;
    product_t p[PRODUCT_ARRAY_MAX_LEN];
}product_partial_list_msg;


#endif
