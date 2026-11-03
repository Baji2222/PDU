#pragma STRING (far)

/*
 *  TFTP Client for LwPS
 */


/*
 * Include Files
 */
#include <lwps/lwps_api.h>
#include <utility/firmware_upgrade.h>

#include <tftpd/tftpc.h>
#include <salfwug.h>

/*
 * Symbol Definition
 */
typedef enum tftpc_state_e {
    TFTPC_STATE_NONE = 0,
    TFTPC_STATE_READ_BEGIN,
    TFTPC_STATE_READ_ARP_REQUEST,
    TFTPC_STATE_READ_ARP_WAIT_REPLY,
    TFTPC_STATE_READ_REQUEST,
    TFTPC_STATE_READ_WAIT_REPLY,
    TFTPC_STATE_READ_WAIT_DATA,
    TFTPC_STATE_READ_OK
} tftpc_state_t;
typedef struct tftpc_session_s {
    int32  slot_id;
    int32  socket_id;

    ip_addr_t server_ip;
    uint16 server_port;
    uint16 cntBlock;

    uint8  state;
    uint8  status;         /* status flags */
    uint8  filename[24];
    uint32 blksize;        /* block size (Byte) */

    uint8  timeout;        /* timeout (Left Second) */
    uint8  retries;        /* retry times */
    uint8  reserved;
    uint8  errorcode;      /* Error code */

    uint8  *pBufAddr;      /* buffer addr */
    uint32 bufSize;        /* buffer size */
    uint32 cntSize;        /* count the process size */
} tftpc_session_t;


/*
 * Data Declaration
 */
static tftpc_session_t xdata tftpc_slot[TFTPC_CLIENT_MAX];


/*
 * Function Declaration
 */
static int32 tftpc_request_rrq(int32 slot_id)
{
    uint8 xdata tftpc_pktbuf[128];
    int32 xdata tftpc_pktsize = 0;

    /* Packet Format:
     * Opcode: Read Request (1)     [2B]
     * Source File: filename        [nB]    ('\0' specified the end)
     * Type: octet                  [6B]    ("octet\0")
     * (Option: split the name and value with '\0')
     * opt-name\0opt-value\0
     */
    tftpc_pktsize += sprintf(tftpc_pktbuf + tftpc_pktsize, "%c%c", 0, TFTP_OPCODE_RRQ);
    tftpc_pktsize += sprintf(tftpc_pktbuf + tftpc_pktsize, "%s%c", tftpc_slot[slot_id].filename, 0);
    tftpc_pktsize += sprintf(tftpc_pktbuf + tftpc_pktsize, "octet%c", 0);
    tftpc_pktsize += sprintf(tftpc_pktbuf + tftpc_pktsize, "timeout%c%d%c", 0, (uint16)TFTPC_TIMEOUT, 0);
    tftpc_pktsize += sprintf(tftpc_pktbuf + tftpc_pktsize, "blksize%c%d%c", 0, (uint16)tftpc_slot[slot_id].blksize, 0);
    if (LWPS_OK != lwps_sendto(tftpc_slot[slot_id].socket_id, tftpc_pktbuf, tftpc_pktsize, tftpc_slot[slot_id].server_ip, UDP_PORT_TFTP))
    {
        DBG_LWPS_TFTPC_PRINTF("%s():%d tftpc_request_rrq failed!\n", __FILE__, (uint16)__LINE__);
        return FAILED;
    }
	DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
    return SUCCESS;
}

static int32 tftpc_reply_ack(int32 slot_id)
{
    uint8 xdata tftpc_pktbuf[64];
    int32 xdata tftpc_pktsize = 0;

    /* [2B] Opcode: Acknowledgement (4) */
    *(uint16 *)&tftpc_pktbuf[0] = TFTP_OPCODE_ACK;
    tftpc_pktsize += 2;
    /* [2B] Block: cntBlock */
    *(uint16 *)&tftpc_pktbuf[2] = tftpc_slot[slot_id].cntBlock;
    tftpc_pktsize += 2;
    if (LWPS_OK != lwps_sendto(tftpc_slot[slot_id].socket_id, tftpc_pktbuf, tftpc_pktsize, tftpc_slot[slot_id].server_ip, tftpc_slot[slot_id].server_port))
    {
        DBG_LWPS_TFTPC_PRINTF("%s():%d tftpc_reply_ack failed!\n", __FILE__, (uint16)__LINE__);
        return FAILED;
    }

    return SUCCESS;
}

static int32 tftpc_active(int32 slot_id)
{
#if 0   /* caller have to check status of running */
    if ((status & TFTPC_STATUS_RUNNING) == 0)
    {
        return FAILED;
    }
#endif

    /* Timeout? */
    if (tftpc_slot[slot_id].timeout == 0)
    {
        /* retry? */
        if (tftpc_slot[slot_id].retries > 0) {
            tftpc_slot[slot_id].timeout = TFTPC_TIMEOUT;
            tftpc_slot[slot_id].retries -= 1;
            tftpc_slot[slot_id].status |= TFTPC_STATUS_RETRY;
        } else {
            /* really timeout */
            tftpc_slot[slot_id].status |= TFTPC_STATUS_TIMEOUT;
            tftpc_slot[slot_id].status |= TFTPC_STATUS_FAILED;
            tftpc_slot[slot_id].status |= TFTPC_STATUS_COMPLETED;
            tftpc_slot[slot_id].status &= ~(TFTPC_STATUS_RUNNING);
            return FAILED;
        }
    }
//    OSAL_PRINTF("TFTPC[%02d] State-Machine: state = %d\n", slot_id, tftpc_slot[slot_id].state);
//    OSAL_PRINTF("TFTPC[%02d] status = %d\n", slot_id, tftpc_slot[slot_id].status);
    switch (tftpc_slot[slot_id].state)
    {
    case TFTPC_STATE_READ_BEGIN:
    case TFTPC_STATE_READ_ARP_REQUEST:
        if (LWPS_OK == lwps_arp_check(tftpc_slot[slot_id].server_ip))
        {
            tftpc_slot[slot_id].state = TFTPC_STATE_READ_REQUEST;
            DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        } else {
            if (LWPS_OK == lwps_arp_request(tftpc_slot[slot_id].server_ip))
            {
                tftpc_slot[slot_id].timeout = 1;    /* set timeout to 1 sec */
                tftpc_slot[slot_id].state = TFTPC_STATE_READ_ARP_WAIT_REPLY;
                DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
            }
            DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        }
        break;

    case TFTPC_STATE_READ_ARP_WAIT_REPLY:
        if (LWPS_OK == lwps_arp_check(tftpc_slot[slot_id].server_ip))
        {
            tftpc_slot[slot_id].state = TFTPC_STATE_READ_REQUEST;
            //DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        }
        else if (tftpc_slot[slot_id].status & TFTPC_STATUS_RETRY)
        {
            /* timeout => resend the ARP request */
            tftpc_slot[slot_id].state = TFTPC_STATE_READ_ARP_REQUEST;
            //DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        }
        //DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        break;

    case TFTPC_STATE_READ_REQUEST:      /* Send RRQ(Read Request) */
        if (SUCCESS == tftpc_request_rrq(slot_id))
        {
            tftpc_slot[slot_id].state = TFTPC_STATE_READ_WAIT_REPLY;
        }
        DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        break;

    case TFTPC_STATE_READ_WAIT_REPLY:   /* Wait Reply */
        if (tftpc_slot[slot_id].status & TFTPC_STATUS_RETRY)
        {
            tftpc_slot[slot_id].state = TFTPC_STATE_READ_REQUEST;
        }
        break;

    case TFTPC_STATE_READ_WAIT_DATA:    /* Wait Read DATA */
        if (tftpc_slot[slot_id].status & TFTPC_STATUS_RETRY)
        {
            /* ACK again (maybe ack lost) */
            if (SUCCESS != tftpc_reply_ack(slot_id))
            {
                tftpc_slot[slot_id].retries += 1;
            }
        }
        DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        break;

    case TFTPC_STATE_READ_OK:
        tftpc_slot[slot_id].status |= TFTPC_STATUS_SUCCESS;
        tftpc_slot[slot_id].status |= TFTPC_STATUS_COMPLETED;
        tftpc_slot[slot_id].status &= ~(TFTPC_STATUS_RUNNING);
        DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        break;

    default:
        OSAL_PRINTF("%s():%d BUG - state (%d) is not exist!\n", __FILE__, (uint16)__LINE__, tftpc_slot[slot_id].state);
        return FAILED;
        break;
    }

    /* update status */
    tftpc_slot[slot_id].status &= ~(TFTPC_STATUS_RETRY);
    if (tftpc_slot[slot_id].status & TFTPC_STATUS_COMPLETED) {
        tftpc_slot[slot_id].status &= ~(TFTPC_STATUS_RUNNING);
    }

    return SUCCESS;
}

#if 0
static int32 tftpc_passive(int32 slot_id, ip_addr_t ip, uint16 port) large
{
    /* parse the packet */

    switch (tftpc_slot[slot_id].state)
    {
    case TFTPC_STATE_READ_WAIT_REPLY:
        tftpc_slot[slot_id].state = TFTPC_STATE_READ_WAIT_DATA;
        break;
    default:
        break;
    }

    return SUCCESS;
}
#endif

#if 0
static void lwps_tftpc_connect(int32 parentSocketId, int32 socketId, ip_addr_t ip, uint16 port) reentrant
{
    DBG_LWPS_TFTPC_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);

    DBG_LWPS_TFTPC_PRINTF("%s():%d parentSocketId = %d, socketId = %d, remote_host = (%d.%d.%d.%d:%d)\n", \
        __FILE__, (uint16)__LINE__, parentSocketId, socketId, \
        ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3], port);

    /* do something */
    if (tftpc_slot[0].socket_id < 0) {
        tftpc_slot[0].socket_id = socketId;
        lwps_socket_setCookie(socketId, (void *)&tftpc_slot[0]);

        return; /* ACCEPT */
    }

    /* REJECT: connection is full -> close this session */
    lwps_close(socketId);
}

static void lwps_tftpc_disconnect(int32 socketId) reentrant
{
    tftpc_session_t xdata *pSlot = (tftpc_session_t *)lwps_socket_getCookie(socketId);

    DBG_LWPS_TFTPC_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);

    /* CLOSE */
    pSlot->socket_id = -1;
}
#endif

void lwps_tftpc_recvfrom(int32 socketId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant
{
    tftpc_session_t xdata *pSlot = (tftpc_session_t *)lwps_socket_getCookie(socketId);
    uint16 xdata opcode;
    //uint16 xdata error_code;

    DBG_LWPS_TFTPC_PRINTF("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);
    DBG_LWPS_TFTPC_PRINTF("%s():%d remove_host = %d.%d.%d.%d:%d\n", __FILE__, (uint16)__LINE__, 
        ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3], port);

    DBG_LWPS_TFTPC_PRINTF("%s():%d lwps_socket_getCookie(socketId = %d) = pSlot = %p\n", \
        __FILE__, (uint16)__LINE__, socketId, pSlot);

    /* Update Host Addr */
    pSlot->server_port = port;

#if 0
    /* dump packet (data ~ tail) */
    {
        uint32 i;

        OSAL_PRINTF("-- [lwps_httpd_receive] -------------------------------------\n");
        for (i=0; i<length; i++)
        {
            OSAL_PRINTF("%c", *(pData + i));
        }
        OSAL_PRINTF("-------------------------------------------------------------\n");
    }
#endif

    if ((pSlot->status & TFTPC_STATUS_RUNNING) &&
        !(pSlot->status & TFTPC_STATUS_COMPLETED))
    {
        /*
         * parse packet
         */
        opcode = *(uint16 *)pData;
        switch (opcode)
        {
        case TFTP_OPCODE_DATA:
            {
                switch (tftpc_slot[pSlot->slot_id].state)
                {
                case TFTPC_STATE_READ_WAIT_DATA:
                    tftpc_slot[pSlot->slot_id].timeout = TFTPC_TIMEOUT; /* Reset timeout */
                    DBG_LWPS_TFTPC_PRINTF("%s():%d TFTPC_STATE_READ_WAIT_DATA (block = %d, size = %d)\n", \
                        __FILE__, (uint16)__LINE__, *(uint16 *)(pData + 2), length - 4);
                    if (*(uint16 *)(pData + 2) == (tftpc_slot[pSlot->slot_id].cntBlock + 1))
                    {
                        /* Write flash */
                        if ((tftpc_slot[pSlot->slot_id].cntSize + (length + 4)) <= tftpc_slot[pSlot->slot_id].bufSize)
                        {
                            DBG_LWPS_TFTPC_PRINTF("%s():%d tftpc_slot[pSlot->slot_id].pBufAddr = %08X\n", __FILE__, (uint16)__LINE__,
                                (uint32)tftpc_slot[pSlot->slot_id].pBufAddr);
                            DBG_LWPS_TFTPC_PRINTF("%s():%d tftpc_slot[pSlot->slot_id].pBufAddr = %08X\n", __FILE__, (uint16)__LINE__,
                                tftpc_slot[pSlot->slot_id].cntSize);
                            DBG_LWPS_TFTPC_PRINTF("%s():%d Upgrade Swap Image: %08X ~ %08X (Size: %d)\n", __FILE__, (uint16)__LINE__,
                                (uint32)tftpc_slot[pSlot->slot_id].pBufAddr + tftpc_slot[pSlot->slot_id].cntSize,
                                (uint32)tftpc_slot[pSlot->slot_id].pBufAddr + tftpc_slot[pSlot->slot_id].cntSize + (length - 4) - 1,
                                (length - 4));

                            /* flash space? */
                            if (1) {
                                sal_fwug_image_write(pData + 4, length - 4);
                            } else {
                            	//print_buf(pData + 4, (length - 4), "TFTP:");
                                //memcpy
                            }
                        } else {
                            //OSAL_PRINTF("%s():%d Error: file is too large!\n", __FILE__, (uint16)__LINE__);
                        }

                        tftpc_slot[pSlot->slot_id].cntBlock += 1;
                        tftpc_slot[pSlot->slot_id].cntSize += (length - 4);
                    }
                    if (SUCCESS == tftpc_reply_ack(pSlot->slot_id))
                    {
                        /* complete? */
                        if ((length - 4) != tftpc_slot[pSlot->slot_id].blksize)
                        {
                            tftpc_slot[pSlot->slot_id].state = TFTPC_STATE_READ_OK;
                            sal_fwug_termination_set();

                            tftpc_slot[pSlot->slot_id].socket_id = -1;
                            tftpc_slot[pSlot->slot_id].state = TFTPC_STATE_NONE;
                            tftpc_slot[pSlot->slot_id].status = TFTPC_STATUS_NONE;
                        }
                    }
                    break;

                default:
                    OSAL_PRINTF("%s():%d Error: TFTP_OPCODE_DATA (state = %d)\n", __FILE__, (uint16)__LINE__, \
                        tftpc_slot[pSlot->slot_id].state);
                    break;
                }
            }
            break;

        case TFTP_OPCODE_ERROR:
            {
                tftpc_slot[pSlot->slot_id].errorcode = (uint8)(*(uint16 *)(pData + 2));

                /* Display error message */
                switch (tftpc_slot[pSlot->slot_id].errorcode)
                {
                case TFTP_ERROR_NOTFOUND:
                    sal_fwug_status_set(SAL_FWUG_STATUS_FILE_NOT_FOUND);
                    OSAL_PRINTF("TFTP: File not found.\n");
                    break;
                case TFTP_ERROR_ACCESS:
                    OSAL_PRINTF("TFTP: Access violation.\n");
                    break;
                case TFTP_ERROR_NOSPACE:
                    OSAL_PRINTF("TFTP: Disk full or allocation exceeded.\n");
                    break;
                case TFTP_ERROR_BADOP:
                    OSAL_PRINTF("TFTP: Illegal TFTP operation.\n");
                    break;
                case TFTP_ERROR_BADID:
                    OSAL_PRINTF("TFTP: Unknown transfer ID.\n");
                    break;
                case TFTP_ERROR_EXISTS:
                    OSAL_PRINTF("TFTP: File already exists.\n");
                    break;
                case TFTP_ERROR_NOUSER:
                    OSAL_PRINTF("TFTP: No such user.\n");
                    break;
                default:
                    OSAL_PRINTF("TFTP: unknown error (%d): %s.\n", \
                        tftpc_slot[pSlot->slot_id].errorcode, pData + 4);
                }

                switch (tftpc_slot[pSlot->slot_id].state)
                {
                case TFTPC_STATE_READ_ARP_WAIT_REPLY:
                case TFTPC_STATE_READ_WAIT_REPLY:
                case TFTPC_STATE_READ_WAIT_DATA:
                    tftpc_slot[pSlot->slot_id].timeout = TFTPC_TIMEOUT; /* Reset timeout */
                    tftpc_slot[pSlot->slot_id].status |= TFTPC_STATUS_COMPLETED;
                    break;

                default:
                    OSAL_PRINTF("%s():%d Error: TFTP_OPCODE_ERROR (state = %d)\n", __FILE__, (uint16)__LINE__, \
                        tftpc_slot[pSlot->slot_id].state);
                    break;
                }
            }
            break;

        case TFTP_OPCODE_OACK:
            {
                switch (tftpc_slot[pSlot->slot_id].state)
                {
                case TFTPC_STATE_READ_WAIT_REPLY:
                    tftpc_slot[pSlot->slot_id].timeout = TFTPC_TIMEOUT;
                    tftpc_slot[pSlot->slot_id].state = TFTPC_STATE_READ_WAIT_DATA;
                    tftpc_slot[pSlot->slot_id].cntBlock = 0;
                    tftpc_slot[pSlot->slot_id].cntSize = 0;
                    tftpc_reply_ack(pSlot->slot_id);
                    break;

                default:
                    OSAL_PRINTF("%s():%d Error: TFTP_OPCODE_OACK (state = %d)\n", __FILE__, (uint16)__LINE__, \
                        tftpc_slot[pSlot->slot_id].state);
                    break;
                }
            }
            break;

        default:
            OSAL_PRINTF("%s():%d TFTP OPCODE(%d) is Not support!\n", __FILE__, (uint16)__LINE__, opcode);
            break;
        }
    }
}


int32 tftpc_init(void) large
{
    int32 xdata i;

    DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    /* Initialize private struct */
    for (i=0; i<TFTPC_CLIENT_MAX; i++)
    {
        tftpc_slot[i].slot_id = i;
        tftpc_slot[i].socket_id = -1;
        tftpc_slot[i].state = TFTPC_STATE_NONE;
        tftpc_slot[i].status = TFTPC_STATUS_NONE;
    }

    return SUCCESS;
}

void tftpc_timer(uint32 milli_second) large
{
    static uint32 xdata timer_cnt_1sec = 0;  /* unit: millisecond */
    int32 xdata i;

    //DBG_LWPS_TFTPC_PRINTF("%s():%d milli_second = %d\n", __FILE__, (uint16)__LINE__, milli_second);

    /* update counter */
    timer_cnt_1sec += milli_second;

    /* one second */
    if (timer_cnt_1sec >= 1000) {
        timer_cnt_1sec -= 1000;

        //DBG_LWPS_TFTPC_PRINTF("%s():%d timer_cnt_1sec = %d\n", __FILE__, (uint16)__LINE__, timer_cnt_1sec);
        for (i=0; i<TFTPC_CLIENT_MAX; i++)
        {
            if ((tftpc_slot[i].status & TFTPC_STATUS_RUNNING) && \
                (tftpc_slot[i].timeout > 0))
            {
                tftpc_slot[i].timeout -= 1;
                DBG_LWPS_TFTPC_PRINTF("tftpc[%02d] timeout = %d\n", i, tftpc_slot[i].timeout);
                if(tftpc_slot[i].timeout == 0)
                {
                    sal_fwug_status_set(SAL_FWUG_STATUS_SERVER_NOT_FOUND);
                    tftpc_slot[i].slot_id = i;
                    tftpc_slot[i].socket_id = -1;
                    tftpc_slot[i].state = TFTPC_STATE_NONE;
                    tftpc_slot[i].status = TFTPC_STATUS_NONE;
                }
            }
        }
    }
}

#if 0
static int32 tftpc_send(void)
{
    return SUCCESS;
}
#endif

int32 tftpc_routine(void) large
{
    int32 xdata i;

    //DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    /* state-machine update */
    for (i=0; i<TFTPC_CLIENT_MAX; i++)
    {
        /* running? */
        if ((tftpc_slot[i].status & TFTPC_STATUS_RUNNING) &&
            !(tftpc_slot[i].status & TFTPC_STATUS_COMPLETED))
        {
            /* triger action */
            if (SUCCESS != tftpc_active(i))
            {
                //OSAL_PRINTF("%s():%d tftpc_action(%d) failed\n", __FILE__, (uint16)__LINE__, i);
                break;
            }
        }
    }

    return SUCCESS;
}

int32 tftpc_read(uint8 *pIp, uint8 *pFile, uint8 *pDstAddr, uint32 dstSize) large
{
    int32 xdata i;
    lwps_handler_t xdata tftpcHandler;

    /* find empty session for this request */
    for (i=0; i<TFTPC_CLIENT_MAX; i++)
    {
        if (tftpc_slot[i].socket_id < 0)    /* empty slot */
        {
            /* Alloc a socket for TFTP client */
            tftpcHandler.event_connect    = NULL;
            tftpcHandler.event_disconnect = NULL;
			DBG_LWPS_TFTPC_PRINTF("[%s][%d][tftpc_recv=%p]\n", __FILE__, (uint16)__LINE__, lwps_tftpc_recvfrom);
            tftpcHandler.event_recvfrom   = lwps_tftpc_recvfrom;
            DBG_LWPS_TFTPC_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

            tftpc_slot[i].socket_id = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, tftpcHandler);
            if (tftpc_slot[i].socket_id < 0)
            {
                OSAL_PRINTF("TFTP: Error - alloc socket failed!\n");

                return FAILED;
            }

            //str_to_ip((unsigned int *)tftpc_slot[i].server_ip.addr, pIp);

            tftpc_slot[i].server_ip.addr[0] = pIp[0];
            tftpc_slot[i].server_ip.addr[1] = pIp[1];
            tftpc_slot[i].server_ip.addr[2] = pIp[2];
            tftpc_slot[i].server_ip.addr[3] = pIp[3];

            memcpy(tftpc_slot[i].filename, pFile, sizeof(tftpc_slot[i].filename) - 1);
            tftpc_slot[i].filename[23] = '\0';
            tftpc_slot[i].pBufAddr = pDstAddr;
            tftpc_slot[i].bufSize = dstSize;
            tftpc_slot[i].state = TFTPC_STATE_READ_BEGIN;
            tftpc_slot[i].status = TFTPC_STATUS_RUNNING;
            tftpc_slot[i].timeout = TFTPC_TIMEOUT;
            tftpc_slot[i].blksize = TFTPC_BLKSIZE;
            tftpc_slot[i].retries = TFTPC_RETRIES;

            lwps_socket_setCookie(tftpc_slot[i].socket_id, (void *)&tftpc_slot[i]);

            return i;   /* success (i >= 0) */
        }
    }

    return -1;  /* failed */
}

#if 0
int32 tftpc_write(uint8 *pIp, uint8 *pFile, uint8 *pSrcAddr, uint32 srcSize) large
{
    int32 xdata tftpc_slot_id;
}

int32 tftpc_close(int32 slot_id) large
{
    if ((slot_id < 0) || (slot_id >= TFTPC_CLIENT_MAX))
        return FAILED;

    if (tftpc_slot[slot_id].socket_id < 0)
        return FAILED;

    /* close socket */
    lwps_close(tftpc_slot[slot_id].socket_id);

    tftpc_slot[slot_id].state = TFTPC_STATE_NONE;
    tftpc_slot[slot_id].status = TFTPC_STATUS_NONE;
    tftpc_slot[slot_id].socket_id = -1;

    return SUCCESS;
}
#endif

#if 0 /* tftpc_state function */
int32 tftpc_state(int32 slot_id)
{
    if ((slot_id < 0) && (slot_id >= TFTPC_CLIENT_MAX))
        return TFTPC_STATE_NONE;

    return tftpc_slot[slot_id].state;
}
#endif /* end of tftpc_state function */
#if 0
int32 tftpc_status(int32 slot_id) large
{
    if ((slot_id < 0) && (slot_id >= TFTPC_CLIENT_MAX))
        return TFTPC_STATE_NONE;

    return tftpc_slot[slot_id].status;
}
#endif
#if 0 /* tftpc_cntsize function */
uint32 tftpc_cntsize(int32 slot_id) large
{
    if ((slot_id < 0) && (slot_id >= TFTPC_CLIENT_MAX))
        return 0;

    return tftpc_slot[slot_id].cntSize;
}
#endif /* end of tftpc_cntsize function */

