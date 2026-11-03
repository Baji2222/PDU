#pragma STRING (far)
/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */
#include <local/lwps_config.h>

#if LWPS_ENABLE && LWPS_API

/*
 * Include Files
 */
#include <common_types.h>
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
#define LWPS_SOCKET_MAX         ((TCP_CONNECTION_MAX) + (UDP_MAX_CONNECTION))
#define LWPS_SOCKET_TCP_BEGIN   (0)
#define LWPS_SOCKET_TCP_END     (TCP_CONNECTION_MAX - 1)
#define LWPS_SOCKET_UDP_BEGIN   (TCP_CONNECTION_MAX)
#define LWPS_SOCKET_UDP_END     ((TCP_CONNECTION_MAX) + (UDP_MAX_CONNECTION) - 1)

typedef struct lwps_socket_s {
    int8   valid;
    uint8  l3type;
    uint8  l4type;
    uint8  status;

    int32 backlog_cnt;
    int32 backlog_max;

    struct lwps_socket_s *parent;
    struct lwps_handler_s handler;
    void *cookie;   /* for User used */
} lwps_socket_t;


/*
 * Data Declaration
 */
static lwps_socket_t xdata lwps_socket_entry[LWPS_SOCKET_MAX];
const ip_addr_t      xdata ip_addr_any = { { 0, 0, 0, 0 } };
      uint8          xdata lwps_events;
      uint16         xdata lwps_events_parm0;


/*
 * Macro Definition
 */
#define SOCKETID_INVALID_RET(sid, ret)  do {    \
    if ((sid < 0) || (sid >= LWPS_SOCKET_MAX) || (TRUE != lwps_socket_entry[sid].valid))    \
    {                                                                                       \
        OSAL_PRINTF("%s():%d Socket ID (%d) is INVALID!\n", __FILE__, (uint16)__LINE__, sid);   \
        return ret;                                                                         \
    }                                                                                       \
} while (0)


/*
 * Function Declaration
 */
/* TCP Handler */
// TCP Handler Connect: Callback function invoked when TCP connection is established
// Synchronizes handler from parent socket and calls user's event_connect callback
void tcp_handler_connect_func(int32 parentPcbId, int32 pcbId, ip_addr_t ip, uint16 port) reentrant
{
    int32 parentSocketId = LWPS_SOCKET_TCP_BEGIN + parentPcbId;
    int32 socketId = LWPS_SOCKET_TCP_BEGIN + pcbId;

    //DBG_LWPS_API_PRINTF("%s():%d - debug\n", __FILE__, (uint16)__LINE__);

    DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d, port = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)pcbId, (uint16)socketId, port);

    /* [FIXME] Check backlog limitation here */

    /* Sync handler */
    if (parentSocketId != socketId)
    {
        DBG_LWPS_API_PRINTF("%s():%d Sync Handler - parentSocketId = %d, socketId = %d\n", \
            __FILE__, (uint16)__LINE__, (uint16)parentSocketId, (uint16)socketId);

        lwps_socket_entry[socketId].handler = lwps_socket_entry[parentSocketId].handler;
        lwps_socket_entry[socketId].valid = TRUE;
    }

    if (lwps_socket_entry[socketId].handler.event_connect == NULL)
    {
        OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, (uint16)pcbId, (uint16)socketId);

        return;
    }

    lwps_socket_entry[socketId].handler.event_connect(parentSocketId, socketId, ip, port);
}

// TCP Handler Disconnect: Callback function invoked when TCP connection is closed
// Calls user's event_disconnect callback and clears the socket entry
void tcp_handler_disconnect_func(int32 pcbId) reentrant
{
    int32 socketId = LWPS_SOCKET_TCP_BEGIN + pcbId;

    DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId, socketId);

    if (lwps_socket_entry[socketId].valid != TRUE)
    {
        return;
    }

    if (lwps_socket_entry[socketId].handler.event_disconnect == NULL)
    {
        OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, pcbId, socketId);

        return;
    }

    lwps_socket_entry[socketId].handler.event_disconnect(socketId);

    /* clear this slot */
    lwps_socket_entry[socketId].handler.event_connect = NULL;
    lwps_socket_entry[socketId].handler.event_disconnect = NULL;
    lwps_socket_entry[socketId].handler.event_recv = NULL;
    lwps_socket_entry[socketId].handler.event_recvfrom = NULL;
    lwps_socket_entry[socketId].valid = FALSE;
}

// TCP Handler Receive: Callback function invoked when TCP data is received
// Calls user's event_recv callback with received data and length
void tcp_handler_recv_func(int32 pcbId, uint8 *pData, uint32 length) reentrant
{
    int32 socketId = LWPS_SOCKET_TCP_BEGIN + pcbId;

    DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId, socketId);

    if (lwps_socket_entry[socketId].valid != TRUE)
    {
        return;
    }

#if 0
    /* dump packet (data ~ tail) */
    {
        uint32 i;
        
        OSAL_PRINTF("[TCP-tcp_handler_recv_func] Receive data ... length = %d\n", length);
        for (i=0; i<length; i++)
        {
            if ((i & 0xf) == 0x0) OSAL_PRINTF("[%03X] ", i);
            OSAL_PRINTF("%02X ", *(pData + i));
            if ((i & 0xf) == 0xf) OSAL_PRINTF("\n");
        }
        if ((i & 0xf) != 0x1) OSAL_PRINTF("\n");
    }
#endif

    if (lwps_socket_entry[socketId].handler.event_recv == NULL)
    {
        OSAL_PRINTF("%s():%d - pcbId = %ld, socketId = %ld (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, pcbId, socketId);

        return;
    }

    lwps_socket_entry[socketId].handler.event_recv(socketId, pData, length);
}

/* UDP Handler */
// UDP Handler Connect: Callback for UDP connection establishment (connectionless)
// Synchronizes handler and calls user's event_connect callback
void udp_handler_connect_func(int32 parentPcbId, int32 pcbId, ip_addr_t ip, uint16 port) reentrant
{
    int32 parentSocketId = LWPS_SOCKET_TCP_BEGIN + parentPcbId;
    int32 socketId = LWPS_SOCKET_TCP_BEGIN + pcbId;

    OSAL_PRINTF("%s():%d - debug\n", __FILE__, (uint16)__LINE__);

    OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId, socketId);

    /* [FIXME] Check backlog limitation here */

    /* Sync handler */
    if (parentSocketId != socketId)
    {
        OSAL_PRINTF("%s():%d Sync Handler - parentSocketId = %d, socketId = %d\n", \
            __FILE__, (uint16)__LINE__, parentSocketId, socketId);

        lwps_socket_entry[socketId].handler = lwps_socket_entry[parentSocketId].handler;
        lwps_socket_entry[socketId].valid = TRUE;
    }

    if (lwps_socket_entry[socketId].handler.event_connect == NULL)
    {
        OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, pcbId, socketId);

        return;
    }

    lwps_socket_entry[socketId].handler.event_connect(parentSocketId, socketId, ip, port);
}

// UDP Handler Disconnect: Callback for UDP disconnection
// Calls user's event_disconnect callback when UDP socket is closed
void udp_handler_disconnect_func(int32 pcbId) reentrant
{
    int32 socketId = LWPS_SOCKET_TCP_BEGIN + pcbId;

    OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId, socketId);

    if (lwps_socket_entry[socketId].handler.event_disconnect == NULL)
    {
        OSAL_PRINTF("%s():%d - pcbId = %d, socketId = %d (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, pcbId, socketId);

        return;
    }

    lwps_socket_entry[socketId].handler.event_disconnect(socketId);
}

// UDP Handler Receive From: Callback invoked when UDP data is received from a specific source
// Calls user's event_recvfrom callback with data, length, source IP, and port
void udp_handler_recvfrom_func(int32 pcbId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant
{
    int32 socketId = LWPS_SOCKET_UDP_BEGIN + pcbId;

    DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)pcbId, (uint16)socketId);

#if 0
    /* dump packet (data ~ tail) */
    {
        uint32 i;
        
        OSAL_PRINTF("[TCP-tcp_handler_recv_func] Receive data ...\n");
        for (i=0; i<length; i++)
        {
            if ((i & 0xf) == 0x0) OSAL_PRINTF("[%03X] ", i);
            OSAL_PRINTF("%02X ", *(pData + i));
            if ((i & 0xf) == 0xf) OSAL_PRINTF("\n");
        }
        if ((i & 0xf) != 0x1) OSAL_PRINTF("\n");
    }
#endif

    if (lwps_socket_entry[socketId].handler.event_recvfrom == NULL)
    {
        DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d (NO HANDLER)!\n", \
            __FILE__, (uint16)__LINE__, pcbId, socketId);

        return;
    }

    DBG_LWPS_API_PRINTF("%s():%d - pcbId = %d, socketId = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)pcbId, (uint16)socketId);
#if 0
#if LWPS_SNMP
    if(lwps_socket_entry[socketId].handler.event_recvfrom == snmp_recv)
    {
        snmp_recv(socketId, pData, length, ip, port);
    }   
    else
#endif
#endif
    { 
        lwps_socket_entry[socketId].handler.event_recvfrom(socketId, pData, length, ip, port);
    }
    DBG_LWPS_API_PRINTF("%s():%d %d.%d.%d", __FILE__, __LINE__, ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3]);
}

// LWPS API Initialize: Initializes the socket API layer and registers protocol handlers
// Sets up TCP and UDP handlers, initializes socket table, and associates PCB with sockets
int32 lwps_api_init(void) large
{
    tcp_handler_t xdata tcpHandler;
    tcp_pcb_t     xdata *pTcpPCB;
    udp_handler_t xdata udpHandler;
    udp_pcb_t     xdata *pUdpPCB;
    uint32        xdata i;

    DBG_LWPS_API_PRINTF("%s():%d, sizeof(lwps_socket_t) = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)sizeof(lwps_socket_t));
        
        
    lwps_events = 0;

#if LWPS_TCP
    /* Set TCP handler */
    tcpHandler.event_connect    = tcp_handler_connect_func;
    tcpHandler.event_disconnect = tcp_handler_disconnect_func;
    tcpHandler.event_recv       = tcp_handler_recv_func;
    if (LWPS_OK != tcp_setHandler(&tcpHandler))
    {
        OSAL_PRINTF("%s():%d - Set TCP Handler failed!\n", __FILE__, (uint16)__LINE__);
        return LWPS_ERR;
    }
#endif /* end of LWPS_TCP */

#if LWPS_UDP
    /* Set UDP handler */
    udpHandler.event_connect    = udp_handler_connect_func;
    udpHandler.event_disconnect = udp_handler_disconnect_func;
    udpHandler.event_recvfrom   = udp_handler_recvfrom_func;
    if (LWPS_OK != udp_setHandler(&udpHandler))
    {
        OSAL_PRINTF("%s():%d - Set UDP Handler failed!\n", __FILE__, (uint16)__LINE__);
        return LWPS_ERR;
    }
#endif /* end of LWPS_UDP */


    for (i=0; i<LWPS_SOCKET_MAX; i++)
    {
        memset(&lwps_socket_entry[i], 0, sizeof(lwps_socket_t));
        lwps_socket_entry[i].l3type = LWPS_L3TYPE_IP;
        lwps_socket_entry[i].handler.event_connect = NULL;
        lwps_socket_entry[i].handler.event_disconnect = NULL;
        lwps_socket_entry[i].handler.event_recv = NULL;
        lwps_socket_entry[i].handler.event_recvfrom = NULL;
        lwps_socket_entry[i].valid = FALSE;

#if LWPS_TCP
        if ((i >= LWPS_SOCKET_TCP_BEGIN) && (i <= LWPS_SOCKET_TCP_END))
        {
            lwps_socket_entry[i].l4type = LWPS_L4TYPE_TCP;
            if (LWPS_OK != tcp_getPCB((i - LWPS_SOCKET_TCP_BEGIN), (tcp_pcb_t **)&pTcpPCB))
            {
                OSAL_PRINTF("%s():%d - tcp_getPCB error!\n", __FILE__, (uint16)__LINE__);
                return LWPS_ERR;
            }
            pTcpPCB->cookie = (void *)&lwps_socket_entry[i];
        }
#endif /* end of LWPS_TCP */
#if LWPS_UDP
        if ((i >= LWPS_SOCKET_UDP_BEGIN) && (i <= LWPS_SOCKET_UDP_END))
        {
            lwps_socket_entry[i].l4type = LWPS_L4TYPE_UDP;
            if (LWPS_OK != udp_getPCB((i - LWPS_SOCKET_UDP_BEGIN), (udp_pcb_t **)&pUdpPCB))
            {
                OSAL_PRINTF("%s():%d - udp_getPCB error!\n", __FILE__, (uint16)__LINE__);
                return LWPS_ERR;
            }
            pUdpPCB->cookie = (void *)&lwps_socket_entry[i];
        }
#endif /* end of LWPS_UDP */

    }

    return LWPS_OK;
}

// LWPS Socket: Creates a new socket of specified type (TCP or UDP)
// Allocates socket entry, sets handlers, and returns socket ID
int32 lwps_socket(uint8 l3type, uint8 l4type, lwps_handler_t handler) large
{
    int32 xdata i;

	
    DBG_LWPS_API_PRINTF("%s():%d l3type = %d, l4type = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)l3type, (uint16)l4type);
    

    if (LWPS_L3TYPE_IP != l3type)
    {
        OSAL_PRINTF("%s():%d l3type (%d) is NOT support!\n", \
            __FILE__, (uint16)__LINE__, (uint16)l3type);
        return -1;
    }

    if (LWPS_L4TYPE_TCP == l4type)
    {
        /* Find a empty TCP socket */
        for (i=LWPS_SOCKET_TCP_BEGIN; i<=LWPS_SOCKET_TCP_END; i++)
        {
            if (lwps_socket_entry[i].valid == FALSE)
            {
                lwps_socket_entry[i].handler.event_connect = handler.event_connect;
                lwps_socket_entry[i].handler.event_disconnect = handler.event_disconnect;
                lwps_socket_entry[i].handler.event_recv = handler.event_recv;
                lwps_socket_entry[i].handler.event_recvfrom = NULL;
                lwps_socket_entry[i].valid = TRUE;

                DBG_LWPS_API_PRINTF("%s():%d - Get TCP Socket, socketId = %d, l4type = %d\n", \
                    __FILE__, (uint16)__LINE__, (uint16)i, (uint16)lwps_socket_entry[i].l4type);

                return i;
            }
        }
    }
    else if (LWPS_L4TYPE_UDP == l4type)
    {
        /* Find a empty UDP socket */
        for (i=LWPS_SOCKET_UDP_BEGIN; i<=LWPS_SOCKET_UDP_END; i++)
        {
        	DBG_LWPS_API_PRINTF("%s():%d [%d].value=0x%04X\n", \
                    __FILE__, (uint16)__LINE__, (uint16)i, (uint16)lwps_socket_entry[i].valid);
            if (lwps_socket_entry[i].valid == FALSE)
            {
                lwps_socket_entry[i].handler.event_connect = handler.event_connect;
                lwps_socket_entry[i].handler.event_disconnect = handler.event_disconnect;
                lwps_socket_entry[i].handler.event_recv = NULL;
                lwps_socket_entry[i].handler.event_recvfrom = handler.event_recvfrom;
                lwps_socket_entry[i].valid = TRUE;

                DBG_LWPS_API_PRINTF("%s():%d - Get UDP Socket, socketId = %d, l4type = %d\n", \
                    __FILE__, (uint16)__LINE__, (uint16)i, (uint16)lwps_socket_entry[i].l4type);
                
                return i;
            }
        }
    }

    return -1;   /* SOCKET NOT FOUND */
}

// LWPS Close: Closes a socket and releases associated resources
// Performs close operation based on socket type (TCP or UDP)
void lwps_close(int32 socketId) large
{
    DBG_LWPS_API_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, (uint16)socketId);

    SOCKETID_INVALID_RET(socketId, );

    /* do something */

    DBG_LWPS_API_PRINTF("%s():%d - Close Socket, socketId = %d, l4type = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, (uint16)lwps_socket_entry[socketId].l4type);

    /* Closing */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_TCP:
    	lwps_events |= LWPS_EVENT_TCP_CLOSE;
    	lwps_events_parm0 = (socketId - LWPS_SOCKET_TCP_BEGIN);
    	/*
        if (LWPS_OK != tcp_close((socketId - LWPS_SOCKET_TCP_BEGIN)))
        {
            OSAL_PRINTF("%s():%d - tcp_close(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_TCP_BEGIN));

            return;
        }
        */
        break;
    case LWPS_L4TYPE_UDP:
    	//lwps_events |= LWPS_EVENT_UDP_CLOSE;
    	//lwps_events_parm0 = (socketId - LWPS_SOCKET_UDP_BEGIN);
#if LWPS_UDP
        if (LWPS_OK != udp_close((socketId - LWPS_SOCKET_UDP_BEGIN)))
        {
            OSAL_PRINTF("%s():%d - udp_close(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_UDP_BEGIN));

            return;
        }
		lwps_socket_entry[socketId].handler.event_connect = NULL;
		lwps_socket_entry[socketId].handler.event_disconnect = NULL;
		lwps_socket_entry[socketId].handler.event_recv = NULL;
		lwps_socket_entry[socketId].handler.event_recvfrom = NULL;
		lwps_socket_entry[socketId].valid = FALSE;
#endif /* end of LWPS_UDP */        
        break;
    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n");
        return;
        break;
    }

}

// LWPS Socket Set Cookie: Stores user-defined cookie pointer in socket entry
// Allows application to associate custom data with socket
void lwps_socket_setCookie(int32 socketId, void *pCookie) large
{
    SOCKETID_INVALID_RET(socketId, );

    lwps_socket_entry[socketId].cookie = pCookie;
}

// LWPS Socket Get Cookie: Retrieves user-defined cookie pointer from socket entry
// Returns previously stored custom data associated with socket
void *lwps_socket_getCookie(int32 socketId) large
{
    SOCKETID_INVALID_RET(socketId, NULL);

    return lwps_socket_entry[socketId].cookie;
}

// LWPS Bind: Binds a socket to a specific IP address and port
// Associates socket with local address/port for listening or sending
int32 lwps_bind(int32 socketId, ip_addr_t ip, uint16 port) large
{
    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    DBG_LWPS_API_PRINTF("%s():%d socketId = %d, L3TYPE = %d, L4TYPE = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, \
        (uint16)lwps_socket_entry[socketId].l3type, \
        (uint16)lwps_socket_entry[socketId].l4type);

    /* Binding */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_TCP:
#if LWPS_TCP
        if (LWPS_OK != tcp_bind((socketId - LWPS_SOCKET_TCP_BEGIN), ip, port))
        {
            OSAL_PRINTF("%s():%d - tcp_bind(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_TCP_BEGIN));

            return LWPS_ERR;
        }
#endif /* end of LWPS_TCP */
        break;
    case LWPS_L4TYPE_UDP:
#if LWPS_UDP
        if (LWPS_OK != udp_bind((socketId - LWPS_SOCKET_UDP_BEGIN), ip, port))
        {
            OSAL_PRINTF("%s():%d - udp_bind(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_UDP_BEGIN));

            return LWPS_ERR;
        }
#endif /* end of LWPS_UDP */ 
        break;
    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n", \
            __FILE__, (uint16)__LINE__, lwps_socket_entry[socketId].l4type);
        return LWPS_ERR;
        break;
    }

    return LWPS_OK;
}

// LWPS Listen: Places a TCP socket in listening mode for incoming connections
// Sets backlog limit for pending connection requests
int32 lwps_listen(int32 socketId, int32 backlog) large
{
    DBG_LWPS_API_PRINTF("%s():%d socketId = %d, backlog = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, (uint16)backlog);

    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    DBG_LWPS_API_PRINTF("%s():%d socketId = %d, L3TYPE = %d, L4TYPE = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, \
        (uint16)lwps_socket_entry[socketId].l3type, \
        (uint16)lwps_socket_entry[socketId].l4type);

    /* Listening */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_TCP:
#if LWPS_TCP
        if (LWPS_OK != tcp_listen((socketId - LWPS_SOCKET_TCP_BEGIN), backlog))
        {
            OSAL_PRINTF("%s():%d - tcp_listen(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_TCP_BEGIN));

            return LWPS_ERR;
        }
#endif /* end of LWPS_TCP */
        break;

    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n");
        return LWPS_ERR;
        break;
    }

    return LWPS_OK;
}

#if 0 /* lwps_connect function */
int32 lwps_connect(int32 socketId, ip_addr_t ip, uint16 port) large
{
    DBG_LWPS_API_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);

    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    return LWPS_OK;
}
#endif /* end of lwps_connect function */

#if 0 /* lwps_from_to function */
int32 lwps_from_to(int32 socketId, ip_addr_t ip, uint16 port) large
{
    OSAL_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);

    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    OSAL_PRINTF("%s():%d socketId = %d, L3TYPE = %d, L4TYPE = %d\n", \
        __FILE__, (uint16)__LINE__, socketId, \
        lwps_socket_entry[socketId].l3type, \
        lwps_socket_entry[socketId].l4type);

    /* Transmit */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_UDP:
        if (LWPS_OK != udp_from_to((socketId - LWPS_SOCKET_UDP_BEGIN), ip, port))
        {
            OSAL_PRINTF("%s():%d - udp_from_to(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_UDP_BEGIN));

            return LWPS_ERR;
        }
        break;

    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n");
        return LWPS_ERR;
        break;
    }

    return LWPS_OK;
}
#endif /* end of lwps_from_to function */

// LWPS Send: Sends data over a TCP socket connection
// Transmits data to the connected peer via TCP protocol
int32 lwps_send(int32 socketId, uint8 *send_data, int32 length) large
{
    DBG_LWPS_API_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);

    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    DBG_LWPS_API_PRINTF("%s():%d socketId = %d, L3TYPE = %d, L4TYPE = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, \
        (uint16)lwps_socket_entry[socketId].l3type, \
        (uint16)lwps_socket_entry[socketId].l4type);

    /* Transmit */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_TCP:
#if LWPS_TCP
        if (LWPS_OK != tcp_send((socketId - LWPS_SOCKET_TCP_BEGIN), send_data, length))
        {
            OSAL_PRINTF("%s():%d - tcp_send(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_TCP_BEGIN));

            return LWPS_ERR;
        }
#endif /* end of LWPS_TCP */
        break;

    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n");
        return LWPS_ERR;
        break;
    }

    return LWPS_OK;
}

#if (defined( LWS_A_SET) || defined( UPGRADE_TFTP))
// LWPS Send To: Sends data over UDP socket to a specified destination
// Transmits UDP datagram to target IP address and port
int32 lwps_sendto(int32 socketId, uint8 *send_data, int32 length, ip_addr_t ip, uint16 port) large
{
    DBG_LWPS_API_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, (uint16)socketId);

    SOCKETID_INVALID_RET(socketId, LWPS_ERR);

    DBG_LWPS_API_PRINTF("%s():%d socketId = %d, L3TYPE = %d, L4TYPE = %d\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, \
        (uint16)lwps_socket_entry[socketId].l3type, \
        (uint16)lwps_socket_entry[socketId].l4type);

    /* Transmit */
    switch (lwps_socket_entry[socketId].l4type)
    {
    case LWPS_L4TYPE_UDP:
#if LWPS_UDP
        if (LWPS_OK != udp_sendto((socketId - LWPS_SOCKET_UDP_BEGIN), send_data, length, ip, port))
        {
            OSAL_PRINTF("%s():%d - udp_output(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, (socketId - LWPS_SOCKET_UDP_BEGIN));

            return LWPS_ERR;
        }
#endif /* end of LWPS_UDP */
        break;

    default:
        OSAL_PRINTF("%s():%d - l4type (%d) is NOT support!\n");
        return LWPS_ERR;
        break;
    }

    return LWPS_OK;
}
#endif

/* ARP related API */
// LWPS ARP Check: Checks if IP address exists in ARP table
// Wrapper function for etharp_check API
int32 lwps_arp_check(ip_addr_t ip) large
{
    return etharp_check(ip);
}

// LWPS ARP Request: Sends ARP request to resolve IP address
// Wrapper function for etharp_request API
int32 lwps_arp_request(ip_addr_t ip) large
{
    return etharp_request(ip);
}

// LWPS Event Handler: Processes pending socket events (close operations)
// Handles deferred TCP/UDP close operations from event queue
int32 lwps_event_handler(void){
#if LWPS_TCP
	if( lwps_events&LWPS_EVENT_TCP_CLOSE )
    {
		if (LWPS_OK != tcp_close(lwps_events_parm0))
        {
            OSAL_PRINTF("%s():%d - tcp_close(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, lwps_events_parm0);

            return LWPS_ERR;
        }
	}
#endif /* end of LWPS_TCP */
#if LWPS_UDP
    if( lwps_events&LWPS_EVENT_UDP_CLOSE )
    {
		if (LWPS_OK != udp_close(lwps_events_parm0))
        {
            OSAL_PRINTF("%s():%d - udp_close(%d) failed!\n", \
                __FILE__, (uint16)__LINE__, lwps_events_parm0);

            return LWPS_ERR;
        }
	}
#endif /* end of LWPS_UDP */
	lwps_events = 0;
	return LWPS_OK;
}

#endif /* end of LWPS_ENABLE && LWPS_API */