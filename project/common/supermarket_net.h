#ifndef SUPERMARKET_NET_H
#define SUPERMARKET_NET_H

enum message_type {
    ASSOCIATION_REQUEST_MSG,
    ASSOCIATION_REPLY_MSG,
    BATTERY_STATUS_MSG,
    ASSIGNMENT_MSG,
    CASH_OUT_MSG,
    PRODUCT_MSG,   
    ITEM_ELEM_MSG,
    BASKET_MSG,
    START_OF_LIST_PRODUCTS_MSG	
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
	uint8_t customer_id;
	
}basket_msg;


typedef struct cash_out_msg
{
	enum message_type msg_type;
	uint8_t customer_id;
}cash_out_msg;


typedef struct product_msg
{
	enum message_type msg_type;	
	uint8_t customer_id;
	uint8_t product_id;
	float prize;

}product_msg;

typedef struct user_invoice
{
	uint8_t n_prods;
	float total_sum;
	uint8_t customer_id;
	linkaddr_t* address_basket;
	uint8_t empty;
	
	
}user_invoice;

#endif
