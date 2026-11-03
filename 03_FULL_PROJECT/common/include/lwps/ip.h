/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_IP_H__
#define	__LWPS_IP_H__

#include <local/config.h>

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
#define IP_HDR_DEFAULT_TTL  (64)
#define IP_ADDR_LEN         (4)

typedef enum ip_flag_e {
    IP_FLAG_RSVD = 0x8000,
    IP_FLAG_DF = 0x4000,
    IP_FLAG_MF = 0x2000
} ip_flag_t;

typedef	struct ip_hdr_s {
  #if LWPS_IP_VERSION_SWAP
    uint8  hdr_len:4;      /* header length */
    uint8  version:4;      /* version */
  #else
    uint8  version:4;      /* version */
    uint8  hdr_len:4;      /* header length */
  #endif
    uint8  tos;            /* type of service */
    uint16 length;         /* total length */
    
    uint16 id_number;      /* identification */
    uint16 frag_offset;    /* fragment offset field */
    
    uint8  ttl;            /* time to live */
    uint8  proto;          /* protocol */
    uint16 chksum;         /* checksum */
    
    uint8  src_ip[4];      /* source IP address */
    uint8  dst_ip[4];      /* destination IP address */
} ip_hdr_t;

typedef enum ip_proto_e {
    IP_PROTO_ICMP   = 0x01,
	IP_PROTO_IGMP   = 0x02,
    IP_PROTO_TCP    = 0x06,
    IP_PROTO_UDP    = 0x11
} ip_proto_t;


/*
 * Macro Definition
 */
#define IPADDR_COPY(dip, sip)   do {    \
    dip[0] = sip[0]; \
    dip[1] = sip[1]; \
    dip[2] = sip[2]; \
    dip[3] = sip[3]; } while(0)
#define IS_IPADDR_EQUAL(ip1, ip2)   \
    ((ip1[0]==ip2[0]) && \
     (ip1[1]==ip2[1]) && \
     (ip1[2]==ip2[2]) && \
     (ip1[3]==ip2[3]))

#define DBG_LWPS_IP_ENABLE    (0)
#if DBG_LWPS_IP_ENABLE
  #define DBG_LWPS_IP_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_LWPS_IP_ENABLE */
  #define DBG_LWPS_IP_PRINTF(x)
  #define ip_hdr_dump(x)
#endif /* end of else of DBG_LWPS_IP_ENABLE */


#if LWPS_IP

/*
 * Function Declaration
 */
extern uint32 ip_init(void);
extern uint32 ip_up(pktBuf_t *);
extern uint32 ip_down(pktBuf_t *, ip_addr_t *);

#else  /* else of LWPS_IP */
#define ip_init()

#endif /* end of else of LWPS_IP */

#endif	/* __LWPS_IP_H__ */

