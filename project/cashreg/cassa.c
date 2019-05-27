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
	uint32_t total_sum;
	uint32_t customer_id;
	uint8_t empty;
    linkaddr_t address_basket;

}user_invoice;

struct MacPkt {
    char data[256];
    uint16_t len;
    linkaddr_t src;
    linkaddr_t dst;
}pkt;
//struct MacPkt pkt;

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
	msg *received_msg;
	static user_invoice invoices[MAX_CUSTOMERS];

	/* fill packet to pass to upper processing layer */
	memcpy(pkt.data, data, len);
	pkt.len = len;
	pkt.src = *source_address;
	pkt.dst = *destination_address;

	LOG_INFO("Received %d bytes from ", len); LOG_INFO_LLADDR(source_address);
	LOG_INFO("type %d", ((msg*)data)->msg_type);
	LOG_INFO("\n");
	received_msg = (msg*) (&pkt.data);

	//we need to receive an additional message to start the process of receiving the products because if we start receiving the products immediately
	//in the case of parallel processes we wouldnt know to what client and what basket that product is associated with
	if (received_msg->msg_type == BASKET_MSG) {
		basket_msg *basket = (basket_msg*) (received_msg);
		uint8_t index = index_free_spot(invoices);
		if (index != -1 ) {
			printf("basket->n_products %d\n", basket->n_products);
			invoices[index].n_prods = basket->n_products;

			invoices[index].total_sum = 0;
			invoices[index].customer_id = basket->customer_id;
       		        memcpy(&invoices[index].address_basket, source_address, sizeof(linkaddr_t));


			msg start_sending_list;
			start_sending_list.msg_type = START_OF_LIST_PRODUCTS_MSG;
			nullnet_buf = (uint8_t*)&start_sending_list;

			LOG_INFO("Sending acknowledgment to start sending list of products to ");
        		LOG_INFO_LLADDR(&(invoices[index].address_basket));
			nullnet_len = sizeof(start_sending_list);
			NETSTACK_NETWORK.output(&(invoices[index].address_basket));
		} else
			printf("Reached max number of customers\n");
	} else if (received_msg->msg_type == PRODUCT_PARTIAL_LIST_MSG) {
		product_partial_list_msg* product_list = (product_partial_list_msg*)(received_msg);

    	uint8_t index = invoice_index(product_list->customer_id, invoices);

        printf("Receiving %d items from customer %d\n", ((int)product_list->array_len) & 0xff, (int)product_list->customer_id);

        for (uint8_t i = 0; i < product_list->array_len; ++i) {
            product_t p = product_list->p[i];

    		printf("Received id: %d, price %d\n", (int)p.product_id, (int)p.price);

    		if (index != -1) {
	    		if (invoices[index].n_prods > 0) {
		    		invoices[index].total_sum += p.price;
    				invoices[index].n_prods--;
			    }
    			if (invoices[index].n_prods == 0) {
	    			printf("Total sum for client %d is %d\n", (int)invoices[index].customer_id, (int)invoices[index].total_sum);
		    		invoices[index].empty = 1;
    			}
       		} else
	    	    printf("Customer with that id is not associated to any basket!\n");
        }

        //send an ack to the cart so that it knows the cash register has received the partial product list and can send the next part
    	msg product_ack;
    	product_ack.msg_type = PRODUCT_PARTIAL_LIST_ACK;
    	nullnet_buf = (uint8_t*)&product_ack;
		LOG_INFO("Sending acknowledgment for the partial product list\n ");
   		nullnet_len = sizeof(product_ack);
    	NETSTACK_NETWORK.output(&(invoices[index].address_basket));

    }
}


PROCESS_THREAD(cassa_main_process, ev, data) {
	PROCESS_BEGIN();

	//initialization of the serial input line
	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	//initialization of the message handler for receiving messages
	nullnet_set_input_callback(input_callback);

	while (true) {
		printf("Dear customer, insert your card id\n");
		PROCESS_WAIT_EVENT();
		if(ev == serial_line_event_message) {
			uint32_t customer_id;
			printf("[CASSA INFO] Customer's id: %s\n", (char*)data);
			customer_id = atoi(data);

			cash_out_msg m;
			m.msg_type = CASH_OUT_MSG;
			m.customer_id = customer_id;

			//send the customer's id as a broadcast message
			nullnet_buf = (uint8_t*)&m;
			nullnet_len = sizeof(m);
			NETSTACK_NETWORK.output(NULL);
		}
	}

	PROCESS_END();
}

