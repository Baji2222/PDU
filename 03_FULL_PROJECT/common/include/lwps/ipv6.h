/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_IPV6_H__
#define	__LWPS_IPV6_H__

#include "local/config.h"

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */

#ifndef IPV6_ADDR_LEN
#define IPV6_ADDR_LEN         16
#endif  /* end of IPV6_ADDR_LEN */

typedef struct ipv6_addr_s {
    uint8 addr[IPV6_ADDR_LEN];
} ipv6_addr_t;


typedef	struct ipv6_hdr_s {
    uint8  trafC_1:4;      /* header length */
    uint8  version:4;      /* version */
    uint8  flow_label_1:4;
    uint8  trafC_2:4;
    uint16 flow_label_2;
    
    uint16 payload_len;
    uint8  next_hdr;
    uint8  hop_limit;
    
    uint8  src_ip[IPV6_ADDR_LEN];      /* source IP address */
    uint8  dst_ip[IPV6_ADDR_LEN];      /* destination IP address */
} ipv6_hdr_t;

typedef enum ipv6_next_hdr_e {
    IPV6_PROTO_HBHO    = 0x00,  /* Hop-By-Hop Options */
    IPV6_PROTO_ROUTING = 0x2B,  /* Routing */
    IPV6_PROTO_FRAG    = 0x2C,  /* Fragment */
    IPV6_PROTO_AH      = 0x33,  /* Authentication Header */
    IPV6_PROTO_ESP     = 0x32,  /* Encapsulating Security Payload */
    IPV6_PROTO_DO      = 0x3C,  /* Destination Options */
    IPV6_PROTO_NNH     = 0x3B,  /* No Next Header */
    IPV6_PROTO_ICMPV6  = 0x3A,  /* ICMPv6 */
    IPV6_PROTO_ICMP    = 0x01,
	IPV6_PROTO_IGMP    = 0x02,
    IPV6_PROTO_TCP     = 0x06,
    IPV6_PROTO_UDP     = 0x11
} ipv6_next_hdr_t;


/*
 * Macro Definition
 */

#define DBG_LWPS_IPV6_ENABLE    (0)
#if DBG_LWPS_IPV6_ENABLE
  #define DBG_LWPS_IPV6_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_LWPS_IPV6_ENABLE */
  #define DBG_LWPS_IPV6_PRINTF(x)  //
  #define ipv6_hdr_dump(x)
#endif /* end of else of DBG_LWPS_IPV6_ENABLE */


#if LWPS_IPV6

/*
 * Function Declaration
 */
extern uint32 ipv6_init(void);
extern uint32 ipv6_up(pktBuf_t *);

#else  /* else of LWPS_IPV6 */
#define ipv6_init()

#endif /* end of else of LWPS_IPV6 */

#endif	/* __LWPS_IPV6_H__ */

