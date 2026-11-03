/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_API_H__
#define	__LWPS_API_H__

#include <local/config.h>
#include <lwps/lwps.h>

#if LWPS_ENABLE

/*
 * Include Files
 */

#include <common_types.h>
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
typedef enum lwps_l3type_e {
    LWPS_L3TYPE_IP = 4
} lwps_l3type_t;

typedef enum lwps_l4type_e {
    LWPS_L4TYPE_TCP = 6,
    LWPS_L4TYPE_UDP = 17
} lwps_l4type_t;

typedef void (*lwps_handler_connect_f)(int32 parentSocketId, int32 socketId, ip_addr_t ip, uint16 port) reentrant;
typedef void (*lwps_handler_disconnect_f)(int32 socketId) reentrant;
typedef void (*lwps_handler_recv_f)(int32 socketId, uint8 *pData, uint32 length) reentrant;
typedef void (*lwps_handler_recvfrom_f)(int32 socketId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant;
typedef struct lwps_handler_s {
    lwps_handler_connect_f      event_connect;
    lwps_handler_disconnect_f   event_disconnect;
    lwps_handler_recv_f         event_recv;
    lwps_handler_recvfrom_f     event_recvfrom;
} lwps_handler_t;

/*
 * Macro Definition
 */

#define DBG_LWPS_API_ENABLE    (0)
#if DBG_LWPS_API_ENABLE
  #define DBG_LWPS_API_PRINTF   OSAL_PRINTF
#else /* else of DBG_LWPS_API_ENABLE */
  #define DBG_LWPS_API_PRINTF
#endif/* end of else of DBG_LWPS_API_ENABLE */



/*
 * Data Declaration
 */
extern const ip_addr_t xdata ip_addr_any;
extern uint16   xdata lwps_events_parm0;
extern uint8    xdata lwps_events;



/*
 * Function Declaration
 */


#if !LWPS_API
#define lwps_event_handler()  (0)
#define lwps_api_init()       (0)
#else 
extern int32 lwps_api_init(void) large;
extern int32 lwps_socket(uint8 l3type, uint8 l4type, lwps_handler_t handler) large;
extern void lwps_close(int32 socketId) large;

extern void lwps_socket_setCookie(int32 socketId, void *pCookie) large;
extern void *lwps_socket_getCookie(int32 socketId) large;

/* IPv4 API */
extern int32 lwps_bind(int32 socketId, ip_addr_t ip, uint16 port) large;
extern int32 lwps_listen(int32 socketId, int32 backlog) large;
extern int32 lwps_connect(int32 socketId, ip_addr_t ip, uint16 port) large;
//extern int32 lwps_from_to(int32 socketId, ip_addr_t ip, uint16 port) large;

/* Common API */
extern int32 lwps_send(int32 socketId, uint8 *send_data, int32 length) large;
extern int32 lwps_sendto(int32 socketId, uint8 *send_data, int32 length, ip_addr_t ip, uint16 port) large;

/* ARP related API */
extern int32 lwps_arp_check(ip_addr_t ip) large;
extern int32 lwps_arp_request(ip_addr_t ip) large;
extern int32 lwps_event_handler(void) large;
#endif

#else  /* else of LWPS_ENABLE */
#define lwps_api_init()
#define lwps_event_handler() (LWPS_OK)

#endif /* end of else of LWPS_ENABLE */
    
#endif	/* __LWPS_API_H__ */

