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
	CASH_OUT_WAIT4ACK,
    CASH_OUT_SEND_LIST
};

extern enum CartStatus status;
extern struct etimer broadcast_timer;

void s_not_associated(process_event_t ev, process_data_t data);
void s_associated(process_event_t ev, process_data_t data);
void s_shopping(process_event_t ev, process_data_t data);
void s_cash_out_wait4ack(process_event_t ev, process_data_t data);
void s_cash_out_send_list(process_event_t ev, process_data_t data);

#endif
