#ifndef __HANDLER_H__
#define __HANDLER_H__

#include <common_types.h>
#include <lwps/pktBuf.h>

void handler_timer();
void handler_linkdown(uint16 pmsk);
void handler_linkup(uint16 pmsk);
void handler_net_l2(pktBuf_t *pBuf);
void handler_init();

#endif

