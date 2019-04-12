#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/log.h"
#include "sys/clock.h"
#include "sys/ctimer.h"
#include "os/dev/leds.h"
#include "os/dev/serial-line.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"

#include "../msg.h"
#include "assigner_fun.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define OPENING_PERIOD (300*CLOCK_SECOND)


/*typedef struct cart
{
	linkaddr_t* cart_address;
	uint8_t battery_status;
	bool assigned;
	uint32_t customer_id;
	struct cart *next;
}cart;*/



/*typedef struct assigner_msg
{
	enum message_type msg_type;
	//assoc_req_msg request;
	//assoc_reply_msg reply;
	uint8_t battery_percentage;
	uint32_t customer_id;
}assigner_msg;
*/

PROCESS(assigner_process, "Assigner process");
AUTOSTART_PROCESSES(&assigner_process);

cart* cart_list = NULL;
static bool supermarket_open = true;
/*
//function invoked in order to looking for the most charged cart to assign to the new arrived client	
static cart* cart_selection()
{	
	uint8_t highest_battery = 0;
	cart* selected = NULL;
	cart* current = cart_list;
	while(current)
	{
		if(!current->assigned && current->battery_status > highest_battery)
		{
			highest_battery = current->battery_status;
			selected = current;
		}
		current = current->next;
	}
	return selected;		
}

//Insert a new cart in the list with the battery info just arrived
static bool insert_cart(uint8_t new_req_battery, linkaddr_t* mac_cart_addr)
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
	}
	return true;
}

//Upgrade the battery status of a cart
static bool bat_upgrade(linkaddr_t* src_cart_addr, uint8_t battery_level)
{
	cart* c = cart_list;
	//for(c; c && (c->cart_address != src_cart_addr); c = c->next);
	while(c)
	{
		if(c->cart_address != src_cart_addr)
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
*/

//Handle the incoming messages, according to the msg_type
static void input_callback(const void* data, uint16_t len, const linkaddr_t* source_address, const linkaddr_t* destination_address) 
{
	a_msg received_msg;
	linkaddr_t* src = (linkaddr_t*)source_address;	
	
	//if(len == sizeof((a_msg *)data)) 
	
		memcpy (&received_msg, data, sizeof ((a_msg *)data));
		LOG_INFO("Received data from: ");
		LOG_INFO_LLADDR(source_address);	
		LOG_INFO("\n");
		
		if(received_msg.msg_type == ASSOCIATION_REQUEST_MSG) 
		{
			//accendere led blu (x mes broadcast)
			if(insert_cart(received_msg.battery_percentage, src))
			{
				a_msg notification;
				notification.msg_type = ASSOCIATION_REPLY_MSG;
				LOG_INFO("Sending acknowledgment of successfull association\n");
				
				nullnet_buf = (uint8_t*)&notification;
				nullnet_len = sizeof(notification);
				NETSTACK_NETWORK.output(src);
			}
			LOG_INFO("New cart associated\n");
		}

		if(received_msg.msg_type == BATTERY_STATUS_MSG) 
		{
			//accendere led purple (mex unicast)
			if(bat_upgrade(src, received_msg.battery_percentage))
			{
				LOG_INFO("Battery level upgraded of ");
				LOG_INFO_LLADDR(src);
				LOG_INFO("\n");	
			}	
		}
	
}

//callback function for the ctimer that checks if all the carts have been replaced when the supermarket close
void check(void *ptr)
{
	supermarket_open = !supermarket_open;
	if(!supermarket_open)
	{
		printf("Supermarket closed\n");		
		cart* c = cart_list;
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

PROCESS_THREAD(assigner_process, ev, data) 
{

	static uint8_t customer_id;
	static a_msg selection_msg;
	linkaddr_t* dest_addr;
	static struct ctimer opening_timer;
	
	PROCESS_BEGIN();

	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	nullnet_set_input_callback(input_callback);

	ctimer_set(&opening_timer, OPENING_PERIOD, check, NULL);	
	
	printf("Supermarket is open!\n");
	printf("Welcome! Please, insert your card id\n");

	while (true) 
	{
		PROCESS_WAIT_EVENT();
		if(ev == serial_line_event_message)
		{
			if(!supermarket_open)
			{
				printf("Supermarket is closed! Please, come back tomorrow!\n");
				leds_on(LEDS_RED);
			}
			else
			{
				printf("Customer's id: %s\n", (char*)data);
				customer_id = atoi(data);
				printf("id: %d\n", (int)customer_id);
			
				cart* cart_selected = cart_selection();
				if(!cart_selected)
				{
					printf("No cart available!\n");
					leds_on(LEDS_RED);
				}
			
				else
				{
					cart_selected->assigned = true;
					cart_selected->customer_id = customer_id;
				

					//send a notification to the selected cart with the associated customer id
					selection_msg.msg_type = ASSIGNMENT_MSG;
					selection_msg.customer_id = customer_id;
				
					dest_addr = cart_selected->cart_address;
	 
					nullnet_buf = (uint8_t*)&selection_msg;
					nullnet_len = sizeof(selection_msg);
					NETSTACK_NETWORK.output(dest_addr);

					printf("Cart unblocked!\n");
					leds_on(LEDS_GREEN);
				}
			}
		}
		
		else if(ev == PROCESS_EVENT_POLL)
			ctimer_reset(&opening_timer);
	}

	PROCESS_END();
}

