#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "os/dev/serial-line.h"
#include "arch/cpu/cc26x0-cc13x0/dev/cc26xx-uart.h"
//#include "os/dev/leds.h"
//#include "os/dev/button-hal.h"
#include "batmon-sensor.h"

PROCESS(contiki_main_process, "Battery monitor sensor test");
AUTOSTART_PROCESSES(&contiki_main_process);


void print_battery_status(void) {
	static int temp, volt;
	temp = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
	volt = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);
	printf("Battery: temperature: %d, volt %d\n", temp, (volt * 125) >> 5);
	

}

PROCESS_THREAD(contiki_main_process, ev, data) {
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(batmon_sensor);

	static struct etimer my_timer;

	etimer_set(&my_timer, 2 * CLOCK_SECOND);
	
	cc26xx_uart_set_input(serial_line_input_byte);
	serial_line_init();

	printf("Hello world\n");

	while (true) {
		PROCESS_WAIT_EVENT();

		if (ev == PROCESS_EVENT_TIMER) {
			print_battery_status();
			etimer_reset(&my_timer);
		}
	}

		PROCESS_END();
}


