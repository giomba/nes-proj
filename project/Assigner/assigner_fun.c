#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <stdlib.h>
#include "sys/log.h"
#include "os/dev/leds.h"

#include "../common/supermarket_net.h"
#include "assigner_fun.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO


//function invoked in order to look for the most charged cart to assign to the new arrived client	
cart* cart_selection()
{	
	uint8_t highest_battery = 0;
	cart* selected = NULL;
	cart* current = cart_list;
	while(current)
	{
		if(!(current->assigned) && (current->battery_status > highest_battery))
		{
			highest_battery = current->battery_status;
			selected = current;
		}
		current = current->next;
	}
	return selected;		
}

//Insert a new cart in the list with the battery info just arrived
bool insert_cart(uint8_t new_req_battery, linkaddr_t mac_cart_addr)
{
	cart* new_arrived_cart = (cart*)malloc(sizeof(cart));
	if(new_arrived_cart==NULL)
	{
		printf("Association Failed");
		return false;
	}
	else
	{
		new_arrived_cart->cart_address = mac_cart_addr;
		new_arrived_cart->battery_status = new_req_battery;
		new_arrived_cart->assigned = false;
		new_arrived_cart->next = cart_list;
		cart_list = new_arrived_cart;
		LOG_INFO("Nuovo carrello inserito con mac_address: ");
		LOG_INFO_LLADDR(&(new_arrived_cart->cart_address));
		printf("\n");
	}
	return true;
}

//Upgrade the battery status of a cart
bool bat_update(linkaddr_t src_cart_addr, uint8_t battery_level)
{
	cart* c = cart_list;
	//looking for cart->address = address of who sent the message with the battery 
	while(c)
	{
		if(linkaddr_cmp(&c->cart_address,&src_cart_addr) == 0)
			c = c->next;
		else
			break;
	}
	if(!c)
	{
		LOG_INFO("Cart not associated yet!\n");
		return false;
	}
	c->battery_status = battery_level;
	c->assigned = false;  //a battery status is sent only when the cart is in his place, not with a client. So if the cart was out and the the battery status is received, it is now come back in place. 
	return true;	
}

void handle_association_request(linkaddr_t src, const void* data)
{
	//turn on the led for association msg received
	assoc_req_msg ar_msg;
	memcpy (&ar_msg, data, sizeof ((assoc_req_msg *)data));
	if(insert_cart(ar_msg.battery_percentage, src))
	{
		assoc_reply_msg notification;
		notification.msg_type = ASSOCIATION_REPLY_MSG;
		LOG_INFO("\n");
		LOG_INFO("Sending acknowledgment of successfull association\n");
	
		nullnet_buf = (uint8_t*)&notification;
		nullnet_len = sizeof(notification);
		NETSTACK_NETWORK.output(&src);
	}
	LOG_INFO("New cart associated\n");
}

void handle_battery_msg(linkaddr_t src, const void* data)
{
	//turn on the led for battery msg received
	battery_msg bt_msg;
	memcpy (&bt_msg, data, sizeof ((battery_msg *)data));
	if(bat_update(src, bt_msg.battery_percentage))
	{
		LOG_INFO("Battery level upgraded of ");
		LOG_INFO_LLADDR(&src);
		printf("\n");	
		printf("Battery: %d\n", bt_msg.battery_percentage);
	}	
}

//callback function for the ctimer that checks if all the carts have been replaced when the supermarket close
void check(void *ptr)
{
	supermarket_open = !supermarket_open;
	leds_toggle(LEDS_ALL);
	if(!supermarket_open)
	{
		printf("Supermarket is closed!\n");		
		cart* c = cart_list;
		//the assigner checks if all the carts are back in their place or are still out
		while(c)
		{	
			if(c->assigned)
				printf("Customer id %d hasn't replaced his cart\n", (int)c->customer_id);
			c = c->next;
		}
	}
	else
		printf("Supermarket is open!\n");
	process_poll(&assigner_process);
		
}



