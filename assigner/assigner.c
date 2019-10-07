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

#include "../common/supermarket_net.h"
#include "assigner_fun.h"

#define OPENING_PERIOD (1800*CLOCK_SECOND)


PROCESS(assigner_process, "Assigner process");
AUTOSTART_PROCESSES(&assigner_process);

cart *cart_list = NULL;
bool supermarket_open = true;


//Handle the incoming messages, according to the msg_type
static void msg_recv(const void* data, uint16_t len, const linkaddr_t* source_address, const linkaddr_t* destination_address) 
{
	linkaddr_t src = *source_address;	
	
	LOG_INFO("Received data from: ");
	LOG_INFO_LLADDR(&src);	
	printf("\n");

	enum message_type type = ((msg*)data)->msg_type;
	switch(type)
	{
		case ASSOCIATION_REQUEST_MSG:
			handle_association_request(src,data);
			break;
		case BATTERY_STATUS_MSG:
			handle_battery_msg(src,data);
			break;
		default:
			printf("Invalide type! This message is not for the Assigner\n");
			break;
	}

}


PROCESS_THREAD(assigner_process, ev, data) 
{

	static uint32_t customer_id;
	static assign_msg selection_msg;
	linkaddr_t dest_addr;
	static struct ctimer opening_timer;
	
	PROCESS_BEGIN();

	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	nullnet_set_input_callback(msg_recv);

	ctimer_set(&opening_timer, OPENING_PERIOD, check, NULL);	
	
	printf("Supermarket is open!\n");
	leds_on(LEDS_GREEN);
	printf("Welcome! Please, insert your card id\n");

	while (true) 
	{
		PROCESS_WAIT_EVENT();
		if(ev == serial_line_event_message)
		{
			if(!supermarket_open)
			{
				printf("Supermarket is closed! Please, come back tomorrow!\n");
			}
			else
			{
				printf("Customer's id: %s\n", (char*)data);
				customer_id = atoi(data);
			
				cart* cart_selected = cart_selection();
				if(!cart_selected)
				{
					printf("No cart available!\n");
				}
			
				else
				{	
					
					cart_selected->assigned = true;
					cart_selected->customer_id = (uint32_t)customer_id;
				

					//send a notification to selected cart with the associated customer id
					selection_msg.msg_type = ASSIGNMENT_MSG;
					selection_msg.customer_id = customer_id;
				
					dest_addr = cart_selected->cart_address;

					LOG_INFO("Selected cart with address: ");
					LOG_INFO_LLADDR(&dest_addr);
					printf("\n");
	 
					nullnet_buf = (uint8_t*)&selection_msg;
					nullnet_len = sizeof(selection_msg);
					NETSTACK_NETWORK.output(&dest_addr);

					printf("Cart unblocked!\n");
				}
			}
		}
		
		else if(ev == PROCESS_EVENT_POLL)
			ctimer_reset(&opening_timer);
	}

	PROCESS_END();
}

