#include <stdio.h>
#include <string.h>

#include "os/dev/leds.h"
#include "batmon-sensor.h"

#include "../common/supermarket_net.h"
#include "event.h"
#include "log.h"
#include "sendrecv.h"

//static linkaddr_t destination_address = {{ 0x00, 0x12, 0x4b, 0x00, 0x0f, 0x8f, 0x18, 0x11 }};

PROCESS(cart_main_process, "Cart Process");
AUTOSTART_PROCESSES(&cart_main_process);

enum CartStatus {
	NOT_ASSOCIATED,
	ASSOCIATED,
	SHOPPING,
	CASHOUT
} status;

static uint8_t net_buffer[256];
static struct etimer broadcast_timer;

void s_not_associated(process_event_t ev, process_data_t data) {
    if (ev == PROCESS_EVENT_TIMER) {
        // send broadcast message to request association with assigner
        if (etimer_expired(&broadcast_timer)) {
            //net_send(/*TODO*/);
        }
    }
    else if (ev == PROCESS_EVENT_MSG && *((enum CartEvent*)data) == CART_EVENT_ASSOCIATED) {
        status = ASSOCIATED;
    }
}


PROCESS_THREAD(cart_main_process, ev, data) {
	PROCESS_BEGIN();
//	SENSORS_ACTIVATE(batmon_sensor);

    /* Local variables allocation */

    /*** Variables initialization ***/
    /* Finite State Machine Status */
    status = CASHOUT;

    /* Network initialization */
	nullnet_buf = net_buffer;
	nullnet_set_input_callback(net_recv);



    /* fixme garbage */
    etimer_set(&broadcast_timer, 10 * CLOCK_SECOND);



    /* now actually start */
	printf("Hello! I'm the cart.\n");

	while (true) {
        PROCESS_WAIT_EVENT();

		switch(status) {
			case NOT_ASSOCIATED:
                s_not_associated(ev, data);
                break;
            break;
			case ASSOCIATED: break;
			case SHOPPING: break;
			case CASHOUT: break;
			default: status = NOT_ASSOCIATED; break;
		}

/*
		if (ev == PROCESS_EVENT_TIMER) {
			printf("Transmitting %d...\n", counter);
			sprintf(message, "#%d Hello. It's me. -- by Adele", counter);
			nullnet_len = strlen(message) + 1;
			counter++;
			NETSTACK_NETWORK.output(&destination_address);
			etimer_reset(&my_timer);
		}
*/
	}

	PROCESS_END();
}


