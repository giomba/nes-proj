#ifndef STATUS_H
#define STATUS_H

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/process.h"

#include "../common/supermarket_net.h"
#include "event.h"
#include "sendrecv.h"

enum CartStatus {
	NOT_ASSOCIATED,
	ASSOCIATED,
	SHOPPING,
	CASHOUT
};

extern enum CartStatus status;
extern struct etimer broadcast_timer;

void s_not_associated(process_event_t ev, process_data_t data);

#endif
