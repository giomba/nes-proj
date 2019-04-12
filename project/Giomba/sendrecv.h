#ifndef SENDRECV_H
#define SENDRECV_H

#include <string.h>

#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "os/net/linkaddr.h"

#include "../common/supermarket_net.h"

#include "event.h"
#include "log.h"

struct MacPkt {
    char data[128];
    uint16_t len;
    linkaddr_t src;
    linkaddr_t dst;
};

extern void* ResetISR;

extern struct MacPkt pkt;

extern struct process cart_main_process;

void net_init(void);
void net_recv(const void* data, uint16_t len, const linkaddr_t* src, const linkaddr_t* dst);
void net_send(const void* data, uint16_t len, const linkaddr_t* dst);

#endif
