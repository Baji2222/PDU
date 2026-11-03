#include <common_types.h>
#include <lwps/pktBuf.h>

/* called every 5ms */
void handler_timer()
{
}
#if 0
/* called when link down port is detected */
void handler_linkdown(uint16 pmsk)
{
    /* kill warning message */
	pmsk = 0;
}

/* called when link up port is detected */
void handler_linkup(uint16 pmsk)
{
    /* kill warning message */
	pmsk = 0;
}
#endif
void handler_net_l2(pktBuf_t *pBuf)
{
    /* kill warning message */
    *(uint8*)pBuf = 0;
}

/* called at system initilization time */
void handler_init()
{
}
