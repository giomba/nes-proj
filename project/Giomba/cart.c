#include <stdio.h>
#include <string.h>

#include "../common/supermarket_net.h"
#include "batmon.h"
#include "event.h"
#include "log.h"
#include "sendrecv.h"
#include "status.h"

PROCESS(cart_main_process, "Cart Process");
AUTOSTART_PROCESSES(&cart_main_process);

PROCESS_THREAD(cart_main_process, ev, data) {
	PROCESS_BEGIN();

    /*** Variables initialization ***/
    status = NOT_ASSOCIATED; // TODO DEBUG
    //status = SHOPPING;

    /*** Timer initialization ***/
    etimer_set(&assigner_timer, 5 * CLOCK_SECOND);
    etimer_set(&battery_timer, 10 * CLOCK_SECOND);

    /*** Subsystem initialization ***/
    net_init();

    /* START */
	printf("Hello! I'm the cart.\n");

	while (true) {
        PROCESS_WAIT_EVENT();

        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&battery_timer)) batmon();

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


