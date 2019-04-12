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

typedef struct cart
{
	linkaddr_t* cart_address;
	uint8_t battery_status;
	bool assigned;
	uint32_t customer_id;
	struct cart *next;
}cart;

cart* cart_selection();
bool insert_cart(uint8_t new_req_battery, linkaddr_t* mac_cart_addr);
bool bat_upgrade(linkaddr_t* src_cart_addr, uint8_t battery_level);
//static void input_callback(const void* data, uint16_t len, const linkaddr_t* source_address, const linkaddr_t* destination_address);
//void check(void *ptr);
