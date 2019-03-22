#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "os/dev/serial-line.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"

PROCESS(contiki_main_process, "Serial Line Test");
AUTOSTART_PROCESSES(&contiki_main_process);

PROCESS_THREAD(contiki_main_process, ev, data) {
	PROCESS_BEGIN();

	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	printf("Hello world\n");

	while (true) {
		PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message);

		printf("received: %s\n", (char*)data);
	}

	PROCESS_END();
}


