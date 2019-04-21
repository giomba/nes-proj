#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "os/net/linkaddr.h"
#include "os/dev/button-hal.h"
#include "os/dev/leds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/log.h"
#include "sys/clock.h"
#include "contiki.h"

#include "../common/product.h"
#include "../common/supermarket_net.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Hardcoded MAC Address for cart */
/* This is used only to emulate the RFID tag */
static linkaddr_t dest_addr = {{0x00, 0x12, 0x4b, 0x00, 0x0f, 0x82, 0x00, 0x04}};

product_t product_list[] = {
	{ 1, "21/12/19", 105 },
	{ 2, "12/05/19", 325 },
	{ 3, "05/05/21", 250 }
};

PROCESS(product_proc, "product random generator");
AUTOSTART_PROCESSES(&product_proc);

void scan_product(product_t* p){
    item_msg m;
    m.msg_type = ITEM_MSG;
    memcpy(&m.p, p, sizeof(*p));

	nullnet_buf = (uint8_t*)&m;
	nullnet_len = sizeof(m);

	NETSTACK_NETWORK.output(&dest_addr);
	LOG_INFO("Product id [%d, %d] scanned from ", (int)p->product_id, (int)p->price);
	LOG_INFO_LLADDR(&dest_addr);
	LOG_INFO_("\n");
}

PROCESS_THREAD(product_proc, ev, data){
	PROCESS_BEGIN();
	// init random number generator
	unsigned int magic_seed = 12; // oooh :o
	srand(magic_seed);

	while(1) {
		PROCESS_YIELD();
		if (ev == button_hal_press_event){
			// product generation
			unsigned int index = rand() % 3;
			scan_product(&product_list[index]);
		}
	}
	PROCESS_END();
}
