/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_TCP_H__
#define	__LWPS_TCP_H__

#include "local/config.h"

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
#define TCP_CONNECTION_MAX          (16)
#define TCP_LOCAL_MSS               (1460)
#define TCP_WINDOW_SIZE             (1 * 1460)
#define TCP_TIMER_REST_TIMEOUT      (60)
#define TCP_TIMER_REST_TIMEOUT_FAST (1)


enum tcp_state_e {
    TCP_STATE_NONE = 0,
    TCP_STATE_LISTEN,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT,
    TCP_STATE_CLOSED,
    TCP_MAX_STATES
};

enum tcp_flag_e {
    TCP_FLAG_FIN = 0x01,    /* No more data from sender */
    TCP_FLAG_SYN = 0x02,    /* Synchronize sequence numbers */
    TCP_FLAG_RST = 0x04,    /* Reset */
    TCP_FLAG_PSH = 0x08,    /* Push */
    TCP_FLAG_ACK = 0x10,    /* Acknowledgment */
    TCP_FLAG_URG = 0x20,    /* Urgent */
    TCP_FLAG_ECE = 0x40,    /* ECN echo */
    TCP_FLAG_CWR = 0x80     /* Congestion window reduced */
};

typedef	struct tcp_hdr_s {
    uint16 src_port;       /* Source port */
    uint16 dst_port;       /* Destination port */

    uint32 seq_num;        /* Sequence number */

    uint32 ack_num;        /* Acknowledgment number */
  #if LWPS_TCP_DATA_OFFSET_SWAP
    uint8  reserved:4;     /* Reserved */
    uint8  data_offset:4;  /* Data offset */
  #else
    uint8  data_offset:4;  /* Data offset */
    uint8  reserved:4;     /* Reserved */
  #endif
    uint8  ctrl_flags;     /* Control flags */
    uint16 window_size;    /* Window size */

    uint16 chksum;         /* Checksum */
    uint16 urgent_ptr;     /* Urgent pointer */
} tcp_hdr_t;

/* Options (optional) */
enum tcp_opt_e {
    TCP_OPS_EOL = 0,    /* End of options list */
    TCP_OPT_NOP = 1,    /* No operation (NOP, Padding) */
    TCP_OPT_MSS = 2,    /* Maximum segment size */
    TCP_OPT_WDS = 3,    /* Window scale */
    TCP_OPT_SAO = 4,    /* Selective Acknowledgement ok */
    TCP_OPT_TSP = 8     /* Timestamp */
};

typedef struct tcp_hdr_opt_mss_s {
    uint8  opt_kind;       /* mss = 2 */
    uint8  opt_len;        /* if mss, opt_len is 4 */
    uint16 opt_mss;        /* Maximum seqment size */
} tcp_hdr_opt_mss_t;

typedef void (*tcp_handler_connect_f)(int32 parentPcbId, int32 pcbId, ip_addr_t ip, uint16 port) reentrant;
typedef void (*tcp_handler_disconnect_f)(int32 pcbId) reentrant;
typedef void (*tcp_handler_recv_f)(int32 pcbId, uint8 *pData, uint32 length) reentrant;
typedef struct tcp_handler_s {
    tcp_handler_connect_f       event_connect;
    tcp_handler_disconnect_f    event_disconnect;
    tcp_handler_recv_f          event_recv;
} tcp_handler_t;

/* the TCP protocol control block */
typedef struct tcp_pcb_s {
    ip_addr_t local_ip;     /* Local IP addr */
    ip_addr_t remote_ip;    /* Remote IP addr */
    uint16 local_port;     /* Local port */
    uint16 remote_port;    /* Remote port */

    uint32 local_seq;      /* Local sequence number */
    uint32 remote_seq;     /* Remote sequence number */
    uint16 remote_mss;     /* Remote MSS */
    uint8  state;          /* Current state */
    uint8  reserved;       /* Reserved */

    int32  timer_alive;    /* Timer: alive */
    int32  timer_rest;     /* Timer: rest */

    int32  parent_id;      /* Parent TCP PCB ID */
    int32  backlog;        /* Backlog */

    void *cookie;           /* Cookie */
} tcp_pcb_t;

/*
 * Macro Definition
 */

#define DBG_LWPS_TCP_ENABLE    (0)
#if DBG_LWPS_TCP_ENABLE
  #define DBG_LWPS_TCP_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_LWPS_TCP_ENABLE */
  #define DBG_LWPS_TCP_PRINTF(x)
#endif /* end of else of DBG_LWPS_TCP_ENABLE */


#if LWPS_TCP
/*
 * Function Declaration
 */
extern void  tcp_timer(void) large;
extern int16 tcp_getPCB(uint16 idx, tcp_pcb_t **ppTcpPCB) large;
extern int16 tcp_setHandler(tcp_handler_t *pTcpHandler) large;
extern int16 tcp_init(void) large;
extern int16 tcp_up(pktBuf_t *pBuf) large;

extern int16 tcp_bind(uint16 pcbId, ip_addr_t ip, uint16 port) large;
extern int16 tcp_listen(uint16 pcbId, uint16 backlog) large;
//extern int32 tcp_connect(int32 xdata tcp_connectpcbId, ip_addr_t xdata ip, uint16 xdata port) large;
extern int16 tcp_send(uint16 pcbId, uint8 *pData, uint16 length) large;
extern int16 tcp_close(uint16 pcbId) large;

#else  /* else of LWPS_TCP */
#define tcp_init()
#define tcp_timer()

#endif /* end of else of LWPS_TCP */

#endif	/* __LWPS_TCP_H__ */

