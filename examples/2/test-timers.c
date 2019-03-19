#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"
#include "sys/timer.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

PROCESS(contiki2, "Timer process");
AUTOSTART_PROCESSES(&contiki2);

PROCESS_THREAD(contiki2, ev, data) {
	PROCESS_BEGIN();

	static struct etimer sec3_timer;

	printf("Starting...\n");

	etimer_set(&sec3_timer, 3 * CLOCK_SECOND);

	while (true) {
		PROCESS_WAIT_EVENT();

		printf("E\n");
		etimer_reset(&sec3_timer);
	}

	PROCESS_END();
}


