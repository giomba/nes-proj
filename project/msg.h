
enum message_type{ASSOCIATION_REQUEST_MSG, ASSOCIATION_REPLY_MSG, BATTERY_STATUS_MSG, ASSIGNMENT_MSG, CASH_OUT_MSG, PRODUCT_MSG, ITEM_ELEM_MSG, BASKET_MSG, START_OF_LIST_PRODUCTS_MSG};

/*typedef struct assoc_req_msg	
{
	uint8_t battery_percentage;

}assoc_req_msg;


typedef struct assoc_reply_msg
{
	//linkaddr_t* assigner_address;
}assoc_reply_msg;


typedef struct battery_msg
{
	uint8_t battery_percentage;
}battery_msg;


typedef struct assign_msg
{
	uint32_t customer_id;
}assing_msg;
*/

typedef struct basket_msg
{
	uint8_t n_products;
	uint8_t customer_id;
	linkaddr_t* address;
	
	
}basket_msg;


typedef struct cash_out_msg
{
	uint8_t customer_id;
}cash_out_msg;


typedef struct product_msg{
	
	uint8_t customer_id;
	uint8_t product_id;
	float prize;

}product_msg;

//-----------------------Definition of the type of messages exchanged between the modules, with their useful informations.  The significant fields are discriminated by the msg_type ---------


typedef struct assigner_msg   //Message for communications between assigner and carts
{
	enum message_type msg_type;
	//assoc_req_msg request;
	//assoc_reply_msg reply;
	uint8_t battery_percentage;
	uint32_t customer_id;
}a_msg;

typedef struct cash_desk_msg
{
	enum message_type msg_type;
	cash_out_msg cash_out;
	product_msg product;
	basket_msg basket;    
}cd_msg;
