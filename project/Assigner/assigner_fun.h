#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/log.h"


typedef struct cart
{
	linkaddr_t cart_address;
	uint8_t battery_status;
	bool assigned;
	uint32_t customer_id;
	struct cart *next;
}cart;

extern bool supermarket_open;
extern cart* cart_list;
extern struct process assigner_process;

cart* cart_selection();
bool insert_cart(uint8_t new_req_battery, linkaddr_t mac_cart_addr);
bool bat_update(linkaddr_t src_cart_addr, uint8_t battery_level);
void handle_association_request(linkaddr_t src, const void* data);
void handle_battery_msg(linkaddr_t src, const void* data);
void check(void *ptr);
