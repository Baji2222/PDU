/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */

#ifndef	__LWPS_UDP_H__
#define	__LWPS_UDP_H__

#include <local/config.h>

/*  
 * Include Files 
 */
#include <lwps/lwps.h>

/* 
 * Symbol Definition 
 */
#define UDP_MAX_CONNECTION  (8)
typedef struct udp_hdr_s {
    uint16 src_port;       /* Source port */
    uint16 dst_port;       /* Destination port */

    uint16 length;         /* Length */
    uint16 chksum;         /* Checksum */
} udp_hdr_t;

typedef void (*udp_handler_connect_f)(int32 parentPcbId, int32 pcbId, ip_addr_t ip, uint16 port) reentrant;
typedef void (*udp_handler_disconnect_f)(int32 pcbId) reentrant;
typedef void (*udp_handler_recvfrom_f)(int32 pcbId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant;
typedef struct udp_handler_s {
    udp_handler_connect_f       event_connect;
    udp_handler_disconnect_f    event_disconnect;
    udp_handler_recvfrom_f      event_recvfrom;
} udp_handler_t;

/* the UDP protocol control block */
typedef struct udp_pcb_s {
    ip_addr_t local_ip;     /* Local IP addr */
    ip_addr_t remote_ip;    /* Remote IP addr */
    uint16 local_port;     /* Local port */
    uint16 remote_port;    /* Remote port */

    int32  parent_id;      /* Parent TCP PCB ID */
    int32  backlog;        /* Backlog */

    void *cookie;           /* Cookie */
} udp_pcb_t;
/*
 * Macro Definition
 */

#define DBG_LWPS_UDP_ENABLE    (0)

#if DBG_LWPS_UDP_ENABLE
  #define DBG_LWPS_UDP_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else  /* else of DBG_LWPS_TCP_ENABLE */
  #define DBG_LWPS_UDP_PRINTF(x)
#endif /* end of else of DBG_LWPS_TCP_ENABLE */

#if LWPS_UDP
/*
 * Function Declaration 
 */
extern void udp_timer(void) large;
extern int16 udp_getPCB(uint16 idx, udp_pcb_t **ppUdpPCB) large;
extern int16 udp_setHandler(udp_handler_t *pUdpHandler) large;
extern int16 udp_init(void) large;
extern int16 udp_up(pktBuf_t *pBuf) large;

extern int16 udp_bind(uint16 pcbId, ip_addr_t ip, uint16 port) large;
//extern int32 udp_from_to(int32 pcbId, ip_addr_t ip, uint16 port) large;
extern int16 udp_sendto(uint16 pcbId, uint8 *pData, uint16 length, ip_addr_t ip, uint16 port) large;
extern int16 udp_close(uint16 pcbId) large;
	
//===============================  Monty Add for UDP snmp oid Start =======================
	
extern uint32 udp_get_datagrams_delivered(void) large;
extern int16 udp_reset_datagrams_delivered(void) large;	

extern uint32 udp_get_datagrams_no_app(void) large;
extern int16 udp_reset_datagrams_no_app(void) large;

extern uint32 udp_get_datagrams_failed_other(void) large;
extern int16 udp_reset_datagrams_failed_other(void) large;

extern uint32 udp_get_datagrams_sent(void) large;
extern int16 udp_reset_datagrams_sent(void) large;

//===============================  Monty Add for UDP snmp oid End =======================
#else  /* else of LWPS_UDP */
#define udp_init()
#define udp_timer()

#endif /* end of else of LWPS_UDP */

#endif	/* __LWPS_UDP_H__ */

