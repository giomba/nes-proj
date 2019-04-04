#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "os/dev/serial-line.h"
#include "os/net/linkaddr.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"
//#include "os/dev/leds.h"
//#include "os/dev/button-hal.h"
#include "batmon-sensor.h"

static linkaddr_t destination_address = {{ 0x00, 0x12, 0x4b, 0x00, 0x0f, 0x8f, 0x18, 0x11 }};

PROCESS(contiki_main_process, "Networking connectivity test");
AUTOSTART_PROCESSES(&contiki_main_process);

static void input_callback(const void* data, uint16_t len, const linkaddr_t* source_address, const linkaddr_t* destination_address) {
	char received_data[strlen((char*)data) + 1];
	if (len == strlen((char*)data) + 1) {
		memcpy(&received_data, data, strlen((char*)data) + 1);
		LOG_INFO("Received \"%s\" from ", (char*)data);
		LOG_INFO_LLADDR(source_address);
		LOG_INFO("\n");
	}
}

PROCESS_THREAD(contiki_main_process, ev, data) {
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(batmon_sensor);

	static int counter = 0;
	static struct etimer my_timer;
	static char message[128];

	nullnet_buf = (uint8_t*)message;
	nullnet_set_input_callback(input_callback);

	etimer_set(&my_timer, 5 * CLOCK_SECOND);
	
	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	printf("Hello world\n");

	while (true) {
		PROCESS_WAIT_EVENT();

		if (ev == PROCESS_EVENT_TIMER) {
			printf("Transmitting %d...\n", counter);
			sprintf(message, "#%d Hello. It's me. -- by Adele", counter);
			nullnet_len = strlen(message) + 1;
			counter++;
			NETSTACK_NETWORK.output(&destination_address);
			etimer_reset(&my_timer);
		}
	}

		PROCESS_END();
}


