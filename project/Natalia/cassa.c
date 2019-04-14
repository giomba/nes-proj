#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "os/dev/serial-line.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"

#include <stdlib.h>
#include "../common/supermarket_net.h"



#define MAX_CUSTOMERS 20

typedef struct user_invoice
{
	uint8_t n_prods;
	float total_sum;
	uint32_t customer_id;
	uint8_t empty;
    linkaddr_t address_basket;

}user_invoice;


PROCESS(cassa_main_process, "Cassa process");
AUTOSTART_PROCESSES(&cassa_main_process);

static uint8_t invoice_index(uint32_t customer_id, user_invoice invoices[])
{

	uint8_t i = 0;
	for(i = 0; i< MAX_CUSTOMERS;i++) {
		if (invoices[i].empty==0 && customer_id == invoices[i].customer_id)
				return i;

	}
	return -1;

}

static uint8_t index_free_spot(user_invoice invoices[])
{
	uint8_t i = 0;
	for(i = 0;i<MAX_CUSTOMERS;i++) {
		if (invoices[i].empty == 1) {
			return i;
		}

	}
	return -1;

}

static void input_callback(const void* data, uint16_t len, const linkaddr_t* source_address, const linkaddr_t* destination_address)
{
	msg received_msg;

	static user_invoice invoices[MAX_CUSTOMERS];


//	if (len == sizeof(*data)) {
		memcpy (&received_msg, data, sizeof(received_msg));
		LOG_INFO("Received data ");
		LOG_INFO_LLADDR(source_address);	//this is the link layer address
		LOG_INFO("\n");
		//we need to receive an additional message to start the process of receiving the products because if we start receiving the products immediately
		//in the case of parallel processes we wouldnt know to what client and what basket that product is assosiated with
		if (received_msg.msg_type == BASKET_MSG) {
			basket_msg *basket = (basket_msg*) (&received_msg);
			uint8_t index = index_free_spot(invoices);
			if (index != -1 ) {
				invoices[index].n_prods = basket->n_products;
				invoices[index].total_sum = 0;
				invoices[index].customer_id = basket->customer_id;
                memcpy(&invoices[index].address_basket, source_address, sizeof(linkaddr_t));
				// invoices[index].address_basket = source_address;

				msg start_sending_list;
				start_sending_list.msg_type = START_OF_LIST_PRODUCTS_MSG;
				nullnet_buf = (uint8_t*)&start_sending_list;

				LOG_INFO("Sending acknowledgment to start sending list of products to ");
                LOG_INFO_LLADDR(&(invoices[index].address_basket));
				nullnet_len = sizeof(start_sending_list);
				NETSTACK_NETWORK.output(&(invoices[index].address_basket));
			} else
			printf("Reached max number of customers\n");
		}
		if (received_msg.msg_type == PRODUCT_MSG) {
			product_msg *product = (product_msg*)(&received_msg);
            printf("Received id: %d, price %f\n", (int)product->product_id, product->price);
			uint8_t index = invoice_index(product->customer_id, invoices);
			if (index != -1) {
				if (invoices[index].n_prods > 0) {
					invoices[index].total_sum += product->price;
					invoices[index].n_prods--;
				}
				if (invoices[index].n_prods == 0) {
					printf("Total sum for client %d is %f\n", (int)invoices[index].customer_id, invoices[index].total_sum);
					invoices[index].empty = 1;
				}
			}else
			printf("Customer with that id is not associated to any basket!\n");
		}
//	}
}

PROCESS_THREAD(cassa_main_process, ev, data) {
	PROCESS_BEGIN();

	static uint32_t customer_id;
	static cash_out_msg bro_customer_id;

	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	nullnet_buf = (uint8_t*)&bro_customer_id;
	nullnet_set_input_callback(input_callback);	//this should be moved down?


	printf("Dear customer, insert your card id\n");

	while (true) {
		PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message);

		printf("Customer's id: %s\n", (char*)data);
		customer_id = atoi(data);
		printf("id: %d\n", (int)customer_id);

		bro_customer_id.msg_type = CASH_OUT_MSG;
		bro_customer_id.customer_id = customer_id;

		LOG_INFO("Sending BROADCAST customer id: %d\n", (int)customer_id);
		LOG_INFO_LLADDR(NULL);
		LOG_INFO_("\n");
		nullnet_len = sizeof(bro_customer_id);
		NETSTACK_NETWORK.output(NULL);
	}

	PROCESS_END();
}

