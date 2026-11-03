/*
* Copyright Realtek Semiconductor Corporation, 2008
* All rights reserved.
*
* Program  : ARP (Address Resolution Protocol) of Light-weight Protocol Stack
* Abstract : 
* Author   : 
* $Id: $
*/


#ifndef	__LWPS_ETHARP_H__
#define	__LWPS_ETHARP_H__

#include "local/config.h"

#if LWPS_ETHARP

/*
 * Include Files
 */
#include <lwps/lwps.h>
#include <lwps/ip.h>
#include <lwps/eth.h>

/*
 * Symbol Definition
 */
typedef enum etharp_hwtype_e {
    ETHARP_HWTYPE_ETHERNET = 1
} etharp_hwtype_t;

typedef enum etharp_prototype_e {
    ETHARP_PROTOTYPE_IP = 0x0800
} etharp_prototype_t;

typedef enum etharp_state_e {
    ETHARP_STATE_EMPTY,
    ETHARP_STATE_PENDING,
    ETHARP_STATE_STABLE,
    ETHARP_STATE_EXPIRED
} etharp_state_t;

typedef enum etharp_opcode_e {
    ETHARP_OPCODE_ARP_REQUEST   = 1,
    ETHARP_OPCODE_ARP_REPLY     = 2,
    ETHARP_OPCODE_RARP_REQUEST  = 3,
    ETHARP_OPCODE_RARP_REPLY    = 4
} etharp_opcode_t;

typedef	struct etharp_entry_s {
    struct ip_addr_s    ipaddr;
    struct eth_addr_s   ethaddr;
    enum etharp_state_e state;
    uint8  ctime;
} etharp_entry_t;

typedef struct etharp_hdr_s {
//    struct eth_hdr_s ethhdr;

    uint16 hw_type;
    uint16 proto_type;

    uint8  hlen;
    uint8  plen;
    uint16 opcode;

    struct eth_addr_s   sender_hwaddr;
    struct ip_addr_s    sender_ipaddr;
    struct eth_addr_s   target_hwaddr;
    struct ip_addr_s    target_ipaddr;
} etharp_hdr_t;

extern int gw_mac_need;

/*
 * Macro Definition
 */
#define DBG_LWPS_ETHARP_ENABLE   (0)
#if DBG_LWPS_ETHARP_ENABLE
  #define DBG_LWPS_ETHARP_PRINTF   OSAL_PRINTF
#else 
  #define DBG_LWPS_ETHARP_PRINTF
#endif

#define LWPS_ETHARP_GATEWAY_QUERY_TIME  (10) /* range: 1~255 */

/*
 * Function Declaration
 */
extern void   etharp_timer(void) large;
extern int32  etharp_init(void) large;
extern int32  etharp_up(pktBuf_t *) large;
extern int32  etharp_entry_update(ip_addr_t *pIpAddr, eth_addr_t *pEthAddr) large;
extern int32  etharp_entry_find(ip_addr_t *pIpAddr, eth_addr_t *pEthAddr) large;

/* ARP related API */
extern int32 etharp_check(ip_addr_t ip) large;
extern int32 etharp_request(ip_addr_t ip) large;

#else  /* else of LWPS_ENABLE */

#define etharp_init()
#define etharp_timer()

#endif /* end of else of LWPS_ETHARP */

#endif	/* __LWPS_ETHARP_H__ */

