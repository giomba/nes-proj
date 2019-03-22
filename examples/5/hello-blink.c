#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "os/dev/serial-line.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"
#include "os/dev/leds.h"
#include "os/dev/button-hal.h"


PROCESS(contiki_main_process, "Serial Line Test");
AUTOSTART_PROCESSES(&contiki_main_process);

PROCESS_THREAD(contiki_main_process, ev, data) {
	PROCESS_BEGIN();

	static struct etimer my_timer;
	static short status = 1;

	etimer_set(&my_timer, 0.2 * CLOCK_SECOND);
	
	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	printf("Hello world\n");

	while (true) {
		PROCESS_WAIT_EVENT();

		if (ev == serial_line_event_message) {
			printf("received: %s\n", (char*)data);
		} else
		if (ev == PROCESS_EVENT_TIMER) {
			if (status == 1) {
				leds_toggle(LEDS_ALL);
			}
			etimer_reset(&my_timer);
		} else
		if (ev == button_hal_press_event) {
			/* PAY ATTENTION: Never allocate things on the stack */
			button_hal_button_t* button = data;
			printf("Received button event from %d\n", button->unique_id);
			if (button->unique_id == 0) {
				status = (status == 0) ? 1 : 0; /* redundant */
			}
			else {
				status = 0;
				leds_off(LEDS_ALL);
			}
		}
	}

	PROCESS_END();
}


