/*
 * Copyright (c) 2017, RISE SICS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

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

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

// !! TO INIT WITH CART MAC ADDRESS !! //
static linkaddr_t dest_addr = {{0x00, 0x12, 0x4b, 0x00, 0x0f, 0x82, 0x18, 0x04}};


struct Product {
	char name[20];
	char expiration_date[8]; // gg/mm/yy
	double price;
};

struct Product product_list[] = {
	{ "latte"     , "21/12/19", 1.05 },
	{ "pane"      , "12/05/19", 3.25 },
	{ "cioccolato", "05/05/21", 2.50 }
};


PROCESS(product_proc, "product random generator");
AUTOSTART_PROCESSES(&product_proc);

void scan_product(struct Product *p){
	nullnet_buf = (uint8_t *)p;
	nullnet_len = sizeof(*p);
	NETSTACK_NETWORK.output(&dest_addr);
	LOG_INFO("Product %s scanned from ", p->name);
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
