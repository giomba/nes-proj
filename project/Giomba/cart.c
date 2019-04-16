#include <stdio.h>
#include <string.h>

#include "os/dev/leds.h"
#include "batmon-sensor.h"

#include "../common/supermarket_net.h"
#include "event.h"
#include "log.h"
#include "sendrecv.h"
#include "status.h"

//static linkaddr_t destination_address = {{ 0x00, 0x12, 0x4b, 0x00, 0x0f, 0x8f, 0x18, 0x11 }};

PROCESS(cart_main_process, "Cart Process");
AUTOSTART_PROCESSES(&cart_main_process);

PROCESS_THREAD(cart_main_process, ev, data) {
	PROCESS_BEGIN();
    //	SENSORS_ACTIVATE(batmon_sensor);

    /*** Variables initialization ***/
    // status = NOT_ASSOCIATED; // TODO DEBUG
    status = NOT_ASSOCIATED;
    etimer_set(&broadcast_timer, 5 * CLOCK_SECOND);

    /*** Subsystem initialization ***/
    net_init();

    /* START */
	printf("Hello! I'm the cart.\n");

	while (true) {
        PROCESS_WAIT_EVENT();

		switch(status) {
			case NOT_ASSOCIATED: s_not_associated(ev, data); break;
			case ASSOCIATED: s_associated(ev, data); break;
			case SHOPPING: s_shopping(ev, data); break;
			case CASH_OUT_WAIT4ACK: s_cash_out_wait4ack(ev, data); break;
            case CASH_OUT_SEND_LIST: s_cash_out_send_list(ev, data); break;
			default:
                printf("[E] Invalid status. Resetting status.\n");
                status = NOT_ASSOCIATED;
            break;
		}
	}

	PROCESS_END();
}


