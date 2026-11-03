/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : ARP (Address Resolution Protocol) of Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_ETH_H__
#define	__LWPS_ETH_H__

#include <local/config.h>

#if LWPS_ENABLE

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
#define ETH_HWADDR_LEN      (6)
#define ETH_VLANTAG_LEN     (4)

typedef enum eth_type_e {
    ETH_TYPE_STP_TCN   = 0x0007,
    ETH_TYPE_STP       = 0x0026,
    ETH_TYPE_RSTP      = 0x0027,
    ETH_TYPE_IP        = 0x0800,
    ETH_TYPE_ARP       = 0x0806,
    ETH_TYPE_VLAN      = 0x8100,
    ETH_TYPE_IPV6      = 0x86DD,
    ETH_TYPE_LACP      = 0x8809,
    ETH_TYPE_REALTEK   = 0x8899,
    ETH_TYPE_LLDP      = 0x88CC
} eth_type_t;

#define RTK_CPUTAG_PROTO_ID (4)

typedef struct eth_addr_s {
    uint8 addr[ETH_HWADDR_LEN];
} eth_addr_t;

typedef struct eth_hdr_s {
    struct eth_addr_s dst_mac;
    struct eth_addr_s src_mac;
    uint16 ether_type;
} eth_hdr_t;


/*
 * Macro Definition
 */
#define ETHADDR_COPY(da, sa)   do {    \
        da[0] = sa[0]; \
        da[1] = sa[1]; \
        da[2] = sa[2]; \
        da[3] = sa[3]; \
        da[4] = sa[4]; \
        da[5] = sa[5]; } while(0)
#define IS_ETHADDR_EQUAL(eth1, eth2)   \
        ((eth1[0]==eth2[0]) && \
         (eth1[1]==eth2[1]) && \
         (eth1[2]==eth2[2]) && \
         (eth1[3]==eth2[3]) && \
         (eth1[4]==eth2[4]) && \
         (eth1[5]==eth2[5]))

#define DBG_LWPS_ETH_ENABLE   (0)
#if DBG_LWPS_ETH_ENABLE
  #define DBG_LWPS_ETH_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_LWPS_ETH_ENABLE */
  #define DBG_LWPS_ETH_PRINTF(x)
#endif/* end of else of DBG_LWPS_ETH_ENABLE */


/*
 * Function Declaration
 */
int32 eth_init(void);
int32 eth_up(pktBuf_t *pBuf);
int32 eth_down(pktBuf_t *);

extern void txPkt(pktBuf_t *pBuf);
#else  /* else of LWPS_ENABLE */

#define eth_init()
#define eth_up(x)

#endif /* end of else of LWPS_ENABLE */

#endif	/* __LWPS_ETHARP_H__ */

