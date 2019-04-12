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

extern cart* cart_list;

//struct cart cart;

//function invoked in order to looking for the most charged cart to assign to the new arrived client	
cart* cart_selection()
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
bool insert_cart(uint8_t new_req_battery, linkaddr_t* mac_cart_addr)
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
bool bat_upgrade(linkaddr_t* src_cart_addr, uint8_t battery_level)
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



