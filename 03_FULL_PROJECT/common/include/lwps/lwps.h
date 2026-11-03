/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Header file of Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_H__
#define	__LWPS_H__

/*
 * Include Files
 */
#include <local/lwps_config.h>
#include <common_types.h>
#include <lwps/pktbuf.h>
#include <lwps/eth.h>
#include <lwps/etharp.h>
#include <lwps/ip.h>
#include <lwps/ipv6.h>
#include <lwps/icmp.h>
#include <lwps/igmp_snooping.h>
#include <lwps/tcp.h>

#include <lwps/udp.h>
#include <lwps/lwps_api.h>
#include <stdio.h>
#include <string.h>


/*
 * Symbol Definition
 */

#define LWPS_EVENT_TCP_CLOSE  (1<<0)
#define LWPS_EVENT_UDP_CLOSE  (1<<1)

/*
 * Macro Definition
 */
#ifndef LWPS_OK
#define LWPS_OK             (0)
#endif /* end of LWPS_OK */

#ifndef LWPS_ERR
#define LWPS_ERR            (-1)
#endif /* end of LWPS_ERR */

#ifndef LWPS_ERR_L2_NDEF
#define LWPS_ERR_L2_NDEF    (-2)
#endif /* end of LWPS_ERR_L2_NDEF */

#ifndef LWPS_ERR_NULLPOINT
#define LWPS_ERR_NULLPOINT  (1)
#endif /* end of LWPS_ERR_NULLPOINT */

#ifndef LWPS_ERR_NOTEXIST
#define LWPS_ERR_NOTEXIST   (2)
#endif

#ifndef OSAL_PRINTF
#define OSAL_PRINTF         printf
#endif

#define ntohl(x)  (x)    /* 32-bit */
#define ntohs(x)  (x)    /* 16-bit */

#define htonl(x)  (x)    /* 32-bit */
#define htons(x)  (x)    /* 16-bit */

#define lwps_pbuf_tx   txPkt
#define lwps_pbuf_free 

/*
 * Data Declaration
 */

  /* for LWPS */
extern pktBuf_t  xdata pktbuf;
extern uint8     xdata *pkt_head;
extern uint8     xdata p_tx_buffer[];
extern uint8     xdata this_mac[6];
extern uint8     xdata this_ip[4];
extern uint8     xdata target_mac[6];
extern uint8     xdata target_ip[4];
extern uint8     xdata netmask[4];
extern ip_addr_t xdata default_gateway;

/*
 * Function Declaration
 */


#endif  /* __LWPS_H__ */

