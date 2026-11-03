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

#if LWPS_TCP

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static tcp_pcb_t     xdata tcp_pcb_entry[TCP_CONNECTION_MAX];
static tcp_handler_t xdata _TcpHandler;
static char          xdata tcp_state_str[TCP_MAX_STATES][16] = { \
    "NONE", "LISTEN", "SYN-SENT", "SYN-RECEIVED", \
    "ESTABLISHED", "FIN-WAIT-1", "FIN-WAIT-2", \
    "CLOSE-WAIT", "CLOSING", "LAST-ACK", "TIME-WAIT", \
    "CLOSED" };
static ip_addr_t xdata ip_addr_empty = { { 0, 0, 0, 0 } };


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
#if DBG_LWPS_TCP_ENABLE
//static void tcp_hdr_dump(tcp_hdr_t *pTcpHdr)
void tcp_hdr_dump(tcp_hdr_t *pTcpHdr) large
{
    //DBG_LWPS_TCP_PRINTF(("%s():%d pTcpHdr = %p\n", __FILE__, (uint16)__LINE__, pTcpHdr));
    OSAL_PRINTF("-- [TCP HDR dump] -----------------------------\n");
    OSAL_PRINTF("   Src Port: 0x%04X (%d)\n", (uint16)pTcpHdr->src_port, (uint16)pTcpHdr->src_port);
    OSAL_PRINTF("   Dst Port: 0x%04X (%d)\n", (uint16)pTcpHdr->dst_port, (uint16)pTcpHdr->dst_port);
    OSAL_PRINTF(" Seq-Number: 0x%08llX\n", pTcpHdr->seq_num);
    OSAL_PRINTF(" Ack-Number: 0x%08llX\n", pTcpHdr->ack_num);
    OSAL_PRINTF(" DataOffset: 0x%01X (%d * 4 = %d Bytes)\n", (uint16)pTcpHdr->data_offset, \
        (uint16)pTcpHdr->data_offset, (uint16)(pTcpHdr->data_offset * 4) );
    OSAL_PRINTF("   Reserved: 0x%01X\n", (uint16)pTcpHdr->reserved);
    OSAL_PRINTF(" Ctrl-Flags: 0x%02X\n", (uint16)pTcpHdr->ctrl_flags);
    OSAL_PRINTF("             [%3s %3s %3s %3s %3s %3s %3s %3s]\n", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_CWR)? "CWR" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_ECE)? "ECE" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_URG)? "URG" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_ACK)? "ACK" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_PSH)? "PSH" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_RST)? "RST" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_SYN)? "SYN" : "   ", \
        (pTcpHdr->ctrl_flags & TCP_FLAG_FIN)? "FIN" : "   ");
    OSAL_PRINTF(" WindowSize: 0x%04X (%d)\n", (uint16)pTcpHdr->window_size, (uint16)pTcpHdr->window_size);
    OSAL_PRINTF("   Checksum: 0x%04X\n", (uint16)pTcpHdr->chksum);
    OSAL_PRINTF(" Urgent-Ptr: 0x%04X\n", (uint16)pTcpHdr->urgent_ptr);
    if ((pTcpHdr->data_offset * 4) > sizeof(tcp_hdr_t))
    {
        OSAL_PRINTF(" Option-Len: 0x%01X (%d Bytes)\n", \
            (uint16)((pTcpHdr->data_offset * 4) - sizeof(tcp_hdr_t)), \
            (uint16)((pTcpHdr->data_offset * 4) - sizeof(tcp_hdr_t)));
    }
    OSAL_PRINTF("-----------------------------------------------\n");
} /* end of tcp_hdr_dump */
#endif /* end of DBG_LWPS_TCP_ENABLE */

void tcp_state_update(uint16 pcbId, uint8 state)
{
	//DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

	/*t1=		tcp_pcb_entry[pcbId].remote_ip.addr[0];
     t2=   tcp_pcb_entry[pcbId].remote_ip.addr[1];
     t3=   tcp_pcb_entry[pcbId].remote_ip.addr[2];
       t4= tcp_pcb_entry[pcbId].remote_ip.addr[3];*/
  //     t5= tcp_pcb_entry[pcbId].remote_port;

    //printf(" <=> R#%d (Change)\n",  t5);
    //DBG_LWPS_TCP_PRINTF(("%s():%d tcp_state_update[%d]: %s => %s\n", __FILE__, (uint16)__LINE__, \
    //    (uint16)pcbId, tcp_state_str[tcp_pcb_entry[pcbId].state], tcp_state_str[state]));

    if (tcp_pcb_entry[pcbId].state == state)
        return;

    /* Event_Connect : others to ESTABLISHED */
    if ((tcp_pcb_entry[pcbId].state != TCP_STATE_ESTABLISHED) && (state == TCP_STATE_ESTABLISHED))
    {
    	//printf(("tcp_pcb_entry[pcbId].state != TCP_STATE_ESTABLISHED"));
		
        /* callback to handler (even_connect) */
        if (_TcpHandler.event_connect != NULL)
        {
        	//printf((" parent_id=%d, pcbId=%d, rport=%d\n", (tcp_pcb_entry[pcbId].parent_id), \
        	 //    (pcbId), \
        	 //    (tcp_pcb_entry[pcbId].remote_port)));
            _TcpHandler.event_connect(tcp_pcb_entry[pcbId].parent_id, pcbId, tcp_pcb_entry[pcbId].remote_ip, tcp_pcb_entry[pcbId].remote_port);
        }
    }
    /* Event_Disconnect : ESTABLISHED to others */
    else if ((tcp_pcb_entry[pcbId].state == TCP_STATE_ESTABLISHED) && (state != TCP_STATE_ESTABLISHED))
    {
        /* callback to handler (even_disconnect) */
        if (_TcpHandler.event_disconnect != NULL)
        {
            _TcpHandler.event_disconnect(pcbId);
        }
    }

    /* update state */
    tcp_pcb_entry[pcbId].state = state;
} /* end of tcp_state_update */

static int16 tcp_findPCB(
    ip_addr_t xdata local_ip,
    uint16    xdata local_port,
    ip_addr_t xdata remote_ip,
    uint16    xdata remote_port)
{
    uint16 xdata i;

	DBG_LWPS_TCP_PRINTF(("%s():%d, tcp_pcb_entry=0x%04X\n", __FILE__, (uint16)__LINE__, tcp_pcb_entry));
    for (i=0; i<TCP_CONNECTION_MAX; i++)
    {
    	DBG_LWPS_TCP_PRINTF(("%s():%d[tcp_pcb_entry[i].state=0x%02X]\n", __FILE__, (uint16)__LINE__, (uint16)tcp_pcb_entry[i].state));
        DBG_LWPS_TCP_PRINTF(("%s():%d TCP-PCB[%02d] L#%d.%d.%d.%d:%d <=> R#%d.%d.%d.%d:%d (%s)\n", \
            __FILE__, (uint16)__LINE__, (uint16)i, \
            (uint16)tcp_pcb_entry[i].local_ip.addr[0], \
            (uint16)tcp_pcb_entry[i].local_ip.addr[1], \
            (uint16)tcp_pcb_entry[i].local_ip.addr[2], \
            (uint16)tcp_pcb_entry[i].local_ip.addr[3], \
            (uint16)tcp_pcb_entry[i].local_port, \
            (uint16)tcp_pcb_entry[i].remote_ip.addr[0], \
            (uint16)tcp_pcb_entry[i].remote_ip.addr[1], \
            (uint16)tcp_pcb_entry[i].remote_ip.addr[2], \
            (uint16)tcp_pcb_entry[i].remote_ip.addr[3], \
            (uint16)tcp_pcb_entry[i].remote_port, \
            tcp_state_str[tcp_pcb_entry[i].state]));

        if (IS_IPADDR_EQUAL(tcp_pcb_entry[i].local_ip.addr, local_ip.addr) &&
            (tcp_pcb_entry[i].local_port == local_port) &&
            IS_IPADDR_EQUAL(tcp_pcb_entry[i].remote_ip.addr, remote_ip.addr) &&
            (tcp_pcb_entry[i].remote_port == remote_port))
        {
            DBG_LWPS_TCP_PRINTF(("%s():%d - Match PCB-ID = %d\n", __FILE__, (uint16)__LINE__, (uint16)i));

            return i;
        }
    }

    return -1;  /* Not Found */
}

static void tcp_delPCB(uint16 idx) large
{
    DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    if (idx >= TCP_CONNECTION_MAX)
    {
        DBG_LWPS_TCP_PRINTF(("%s():%d - idx (%lu) error\n", __FILE__, (uint16)__LINE__, idx));
        return;
    }

    tcp_pcb_entry[idx].local_ip = ip_addr_empty;
    tcp_pcb_entry[idx].remote_ip = ip_addr_empty;
    tcp_pcb_entry[idx].local_port = 0;
    tcp_pcb_entry[idx].remote_port = 0;
    //tcp_pcb_entry[idx].state = TCP_STATE_NONE;
    DBG_LWPS_TCP_PRINTF(("%s():%d, idx=0x%04X, &idx=0x%04X\n", __FILE__, (uint16)__LINE__, (uint16)idx,  (uint16)&idx));
    tcp_state_update(idx, TCP_STATE_NONE);

    tcp_pcb_entry[idx].local_seq = 0;
    tcp_pcb_entry[idx].remote_seq = 0;
    tcp_pcb_entry[idx].remote_mss = TCP_LOCAL_MSS;
    tcp_pcb_entry[idx].parent_id = idx; /* self */
    tcp_pcb_entry[idx].timer_alive = -1;
    tcp_pcb_entry[idx].timer_rest = -1;
}

void tcp_timer(void) large
{
    uint16 xdata i;

    //DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    for (i=0; i<TCP_CONNECTION_MAX; i++)
    {
        /* alive_time + 1, if alive >= 0 */
        if (tcp_pcb_entry[i].timer_alive >= 0) {
            tcp_pcb_entry[i].timer_alive++;
        }
        /* idle_time + 1, if idle >= 0 */
        if (tcp_pcb_entry[i].timer_rest >= 0) {
            tcp_pcb_entry[i].timer_rest++;
        }

        /* timeout? */
        if (tcp_pcb_entry[i].timer_rest > TCP_TIMER_REST_TIMEOUT) {
            DBG_LWPS_TCP_PRINTF(("%s():%d TCP PCB[%02d] Rest Timeout!\n", __FILE__, (uint16)__LINE__, i));
            tcp_delPCB(i);
        }
        else if ((tcp_pcb_entry[i].timer_rest > TCP_TIMER_REST_TIMEOUT_FAST) &&
                 ((tcp_pcb_entry[i].state == TCP_STATE_SYN_RECEIVED) || 
                  (tcp_pcb_entry[i].state == TCP_STATE_FIN_WAIT_1) ||
                  (tcp_pcb_entry[i].state == TCP_STATE_FIN_WAIT_2) ||
                  (tcp_pcb_entry[i].state == TCP_STATE_CLOSE_WAIT) ||
                  (tcp_pcb_entry[i].state == TCP_STATE_TIME_WAIT))) {
            //printf("TCP PCB[%02d] Rest-Fast Timeout!\n", i);
            tcp_delPCB(i);
        }
            
    }

    return;
}

int16 tcp_getPCB(uint16 idx, tcp_pcb_t **ppTcpPCB) large
{
    if (idx >= TCP_CONNECTION_MAX)
    {
        OSAL_PRINTF("%s():%d - idx (%lu) error\n", __FILE__, (uint16)__LINE__, idx);

        return LWPS_ERR;
    }
    if (NULL == ppTcpPCB)
    {
        OSAL_PRINTF("%s():%d - ppTcpPCB is NULL\n", __FILE__, (uint16)__LINE__);

        return LWPS_ERR_NULLPOINT;
    }

    /* Assign pointer */
    *ppTcpPCB = &tcp_pcb_entry[idx];

    return LWPS_OK;
}

int16 tcp_setHandler(tcp_handler_t *pTcpHandler) large
{
    if (NULL == pTcpHandler)
    {
        OSAL_PRINTF("%s():%d - pTcpHandler is NULL\n", __FILE__, (uint16)__LINE__);

        return LWPS_ERR_NULLPOINT;
    }

    _TcpHandler.event_connect = pTcpHandler->event_connect;
    _TcpHandler.event_disconnect = pTcpHandler->event_disconnect;
    _TcpHandler.event_recv = pTcpHandler->event_recv;

    return LWPS_OK;
}

int16 tcp_init(void) large
{
    uint16 xdata i;

    DBG_LWPS_TCP_PRINTF(("%s():%d, tcp_pcb_entry=0x%04X\n", __FILE__, (uint16)__LINE__, tcp_pcb_entry));

    /* Clear handler */
    _TcpHandler.event_connect = NULL;
    _TcpHandler.event_disconnect = NULL;
    _TcpHandler.event_recv = NULL;
    

    /* Reset PCB */
    for (i=0; i<TCP_CONNECTION_MAX; i++) {
        tcp_delPCB(i);
    }

    return LWPS_OK;
} /* end of tcp_init */

static int16 tcp_xmit(uint16 pcbId, uint8 *pData, uint16 length, uint8 ctrlFlags) large
{
    pktBuf_t  xdata pBuf_Xmit;
    tcp_hdr_t xdata *pTcpHdr_Xmit;
    tcp_hdr_opt_mss_t xdata *pTcpHdrMSS;
    uint8 xdata *pTcpSegData;

    DBG_LWPS_TCP_PRINTF(("%s():%d pcbId = %d, pData = 0x%04x, length = %d, ctrlFlags = %02x\n", \
        __FILE__, (uint16)__LINE__, (uint16)pcbId, (uint16)pData, (uint16)length, (uint16)ctrlFlags));

    /* [WARN] DO NOT check pcbId, because it's vaild likely */
#if 0
    if ((pData == NULL) && (length == 0)) {
        return LWPS_ERR_NULLPOINT;
    }
#endif

    /* [FIXME] fix length of pbuf note
     * <IP alignment> ETH (6+6+2) + VLAN (4) + IP (20) + TCP (20 + 4 [MSS])
     * DON'T have to consider RTK tag
     * 
     */
    #define TCP_XMIT_PBUF_SIZE  (14 + 4 + 20 + 24)

    if( 0==length && TCP_FLAG_ACK==ctrlFlags) /* for ack, payload==0 */
    {
        pBuf_Xmit.l4   = (uint8*)((&p_tx_buffer[TAG_SIZE+FRAME_INFO_SIZE]) + TCP_XMIT_PBUF_SIZE - (sizeof(tcp_hdr_t) + 4));    /* Max TCP Hdr size (+4 for MSS) */
    }
    else /* using rx buffer because p_tx_buffer is used by (*pData) */
    {
        /* pointer to L4 hdr */
        pBuf_Xmit.l4   = (uint8*)(pkt_head + TCP_XMIT_PBUF_SIZE - (sizeof(tcp_hdr_t) + 4));    /* Max TCP Hdr size (+4 for MSS) */
    }
    pBuf_Xmit.size = 0;

    /* Insert TCP HDR */
    pTcpHdr_Xmit = (tcp_hdr_t *)pBuf_Xmit.l4;
    pTcpHdr_Xmit->src_port = tcp_pcb_entry[pcbId].local_port;
    pTcpHdr_Xmit->dst_port = tcp_pcb_entry[pcbId].remote_port;
    pTcpHdr_Xmit->seq_num = tcp_pcb_entry[pcbId].local_seq;
    pTcpHdr_Xmit->ack_num = tcp_pcb_entry[pcbId].remote_seq;
	printf("local_seq %d\n",(uint16)tcp_pcb_entry[pcbId].local_seq);
	printf("remote_seq %d\n",(uint16)tcp_pcb_entry[pcbId].remote_seq);
    pTcpHdr_Xmit->data_offset = sizeof(tcp_hdr_t) / 4;
    pTcpHdr_Xmit->reserved = 0;
    pTcpHdr_Xmit->ctrl_flags = ctrlFlags;
    pTcpHdr_Xmit->window_size = TCP_WINDOW_SIZE;
    pTcpHdr_Xmit->chksum = 0;
    pTcpHdr_Xmit->urgent_ptr = 0;  /* Not used in LwPS */
    pBuf_Xmit.size += sizeof(tcp_hdr_t);
    /* MSS will be added when SYN flag */
    if (ctrlFlags & TCP_FLAG_SYN)
    {
        pTcpHdrMSS = (tcp_hdr_opt_mss_t *)(pBuf_Xmit.l4 + sizeof(tcp_hdr_t));
        pTcpHdr_Xmit->data_offset += 1;    /* extend size */
        
        pTcpHdrMSS->opt_kind = TCP_OPT_MSS;
        pTcpHdrMSS->opt_len = 4;
        pTcpHdrMSS->opt_mss = TCP_LOCAL_MSS;

        pBuf_Xmit.size += 4;

        pTcpSegData = pBuf_Xmit.l4 + sizeof(tcp_hdr_t) + 4;
    } else {
        pTcpSegData = pBuf_Xmit.l4 + sizeof(tcp_hdr_t);
    }

    /* TCP segment data */
    if (length > 0) {
        memcpy(pTcpSegData, pData, length);
        pBuf_Xmit.size += length;
    }

    //tcp_hdr_dump(pTcpHdr_Xmit);

    DBG_LWPS_TCP_PRINTF(("%s:%d, size = %d\n", __FILE__, (uint16)__LINE__, (uint16)pBuf_Xmit.size));

    pBuf_Xmit.l4fmt = L4FMT_TCP;

    if (LWPS_OK != ip_down(&pBuf_Xmit, &tcp_pcb_entry[pcbId].remote_ip)) {
        OSAL_PRINTF("%s():%d - ip_output failed!\n", __FILE__, (uint16)__LINE__);
        return LWPS_OK;                 /* DO NOT change the PCB entry */
    }

    /* update TCP PCB */
    if ((ctrlFlags & TCP_FLAG_SYN) || (ctrlFlags & TCP_FLAG_FIN)) {
        tcp_pcb_entry[pcbId].local_seq += 1;
    } else {
        tcp_pcb_entry[pcbId].local_seq += length;
    }

    return LWPS_OK;
} /* end of tcp_xmit */

#ifdef  LWPS_IP
static int16 tcp_up_handle_ip(pktBuf_t *pBuf) large
{
    tcp_hdr_t xdata *pTcpHdr = (tcp_hdr_t *)pBuf->l4;
    tcp_hdr_opt_mss_t xdata *pTcpHdrMSS = (tcp_hdr_opt_mss_t *)(pBuf->l4 + sizeof(tcp_hdr_t));
    ip_hdr_t xdata *pIpHdr = (ip_hdr_t *)pBuf->l3;
    int32 xdata pcbId;
    int32 xdata pcbId_empty;
    uint16 test;
    DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
    /* find entry of TCP PCB */
    if ((pcbId = tcp_findPCB(*(ip_addr_t *)(&pIpHdr->dst_ip), pTcpHdr->dst_port, *(ip_addr_t *)(&pIpHdr->src_ip), pTcpHdr->src_port)) < 0)
    {
        printf("- TCP PCB is NOT found!\n");

        if (pTcpHdr->ctrl_flags & TCP_FLAG_SYN)
        {
            /* try to find listening port (if SYN) */
            if ((pcbId = tcp_findPCB(ip_addr_empty, pTcpHdr->dst_port, ip_addr_empty, 0)) < 0)
            {
                printf("- TCP PCB (LISTEN) is NOT found!\n");
            
                /* [FIXME] reply ICMP to tell remote host no listener */
                //lwps_pbuf_free(pBuf);
            
                return LWPS_OK;
            }
            else
            {
                /* Check LISTEN State */
                if (tcp_pcb_entry[pcbId].state == TCP_STATE_LISTEN)
                {
                    /* create a new connection to wait for handler */
                    pcbId_empty = tcp_findPCB(ip_addr_empty, 0, ip_addr_empty, 0);
                    if (pcbId_empty < 0)
                    {
                       // printf("All TCP Connection is full!\n");
                        /* [FIXME] Empty entry is NOT found */
            
                        return LWPS_OK;
                    }
                    else
                    {
                        printf(" New TCP Connection, pcbId = %d\n", pcbId);            			
                        IPADDR_COPY(tcp_pcb_entry[pcbId_empty].local_ip.addr, pIpHdr->dst_ip);
                        tcp_pcb_entry[pcbId_empty].local_port = pTcpHdr->dst_port;
                        IPADDR_COPY(tcp_pcb_entry[pcbId_empty].remote_ip.addr, pIpHdr->src_ip);
                        tcp_pcb_entry[pcbId_empty].remote_port = pTcpHdr->src_port;
                        //tcp_pcb_entry[pcbId_empty].state = TCP_STATE_LISTEN;
                        tcp_state_update(pcbId_empty, TCP_STATE_LISTEN);
                        tcp_pcb_entry[pcbId_empty].parent_id = pcbId;
                        tcp_pcb_entry[pcbId_empty].timer_alive = 0;
                        tcp_pcb_entry[pcbId_empty].timer_rest = 0;
            
                        pcbId = pcbId_empty;
                    }
                }
                else
                {
                    /* [FIXME] Connection Not Found */
                    //lwps_pbuf_free(pBuf);
            
                    return LWPS_OK;
                }
            }
        }
        else
        {
            /* Connection is not found and FLAG is not SYN */
            //printf("- <warning> Connection is not found and FLAG is not SYN\n");
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
    }

    tcp_pcb_entry[pcbId].timer_rest = 0;

    /* find an exist connection */
	switch (tcp_pcb_entry[pcbId].state)
    {
    case TCP_STATE_LISTEN:
		printf("TCP_STATE_LISTEN\n");
        if (pTcpHdr->ctrl_flags & TCP_FLAG_SYN)
        {
            /* Update State */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_SYN_RECEIVED;
            tcp_state_update(pcbId, TCP_STATE_SYN_RECEIVED);

            tcp_pcb_entry[pcbId].local_seq = 0x10000000;
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num + 1; /* Relative Ack Number */

            /* Update MSS */
            /* [FIXME] Only if the first option is mss header */
            if (((pTcpHdr->data_offset * 4) > sizeof(tcp_hdr_t)) &&
                (pTcpHdrMSS->opt_kind == TCP_OPT_MSS) &&
                (pTcpHdrMSS->opt_len == 4))
            {
                tcp_pcb_entry[pcbId].remote_mss = pTcpHdrMSS->opt_mss;
            }
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_SYN | TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_SYN | TCP_FLAG_ACK);
        }
        break;

    case TCP_STATE_SYN_SENT:
		printf("TCP_STATE_SYN_SENT\n");
        /* Not Implmented */
        break;

    case TCP_STATE_SYN_RECEIVED:
		printf("TCP_STATE_SYN_RECEIVED\n");
		test=pTcpHdr->ctrl_flags;
    	printf("pTcpHdr->ctrl_flags=0x%02x\n", test);
        if (pTcpHdr->ctrl_flags & TCP_FLAG_ACK)
        {
        	//DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
            //tcp_pcb_entry[pcbId].state = TCP_STATE_ESTABLISHED;
            tcp_state_update(pcbId, TCP_STATE_ESTABLISHED);
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num;
        }
        break;

    case TCP_STATE_ESTABLISHED:
		printf("TCP_STATE_ESTABLISHED\n");
        if (tcp_pcb_entry[pcbId].remote_seq == pTcpHdr->seq_num)
        {
            //printf((" Update remote_seq, Old = 0x%4X\n", \
             //    tcp_pcb_entry[pcbId].remote_seq));
            printf("tcp_pcb_entry[pcbId].remote_seq == pTcpHdr->seq_num\n");
            /* Update remote_seq */
            //printf((" [size = 0x%04X][offset = 0x%04X]\n", \
            //    pBuf->size, pTcpHdr->data_offset * 4U));
            tcp_pcb_entry[pcbId].remote_seq += pBuf->size - (pBuf->l4 - pBuf->l2) - (pTcpHdr->data_offset * 4U);

            /* Reply an ACK packet */
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);

            /* Passive Close? */
            if (pTcpHdr->ctrl_flags & TCP_FLAG_FIN)
            {
                /* Skip CLOSE-WAIT state */
                tcp_state_update(pcbId, TCP_STATE_LAST_ACK);
                tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num;
                tcp_xmit(pcbId, NULL, 0, TCP_FLAG_FIN | TCP_FLAG_ACK);

                return LWPS_OK;
            }
            /* Receive Data? */

            if ((pBuf->size - (pTcpHdr->data_offset * 4)) > 0)
            {
                if (_TcpHandler.event_recv != NULL)
                {
                    _TcpHandler.event_recv(pcbId, pBuf->l4 + (pTcpHdr->data_offset * 4), \
    			        pBuf->size - (pBuf->l4 - pBuf->l2) - (pTcpHdr->data_offset * 4));
                }
            }

        }
        else
        {
           // printf(("- pcb->remote_seq (%lu) != pTcpHdr->seq_num (%lu)!\n", \
            //    tcp_pcb_entry[pcbId].remote_seq, pTcpHdr->seq_num));
              printf("tcp_pcb_entry[pcbId].remote_seq != pTcpHdr->seq_num\n");
            /* Sequence Nubmer is wrong -> reply an ACK packet */
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        

        break;

    case TCP_STATE_FIN_WAIT_1:
		printf("TCP_STATE_FIN_WAIT_1\n");
        if ((pTcpHdr->ctrl_flags & TCP_FLAG_FIN) && (pTcpHdr->ctrl_flags & TCP_FLAG_ACK))
        {
            //tcp_pcb_entry[pcbId].state = TCP_STATE_TIME_WAIT;
            /* jump to CLOSED state directly */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSED;
            tcp_state_update(pcbId, TCP_STATE_CLOSED);
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num + 1;
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);
            tcp_delPCB(pcbId);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        else if (pTcpHdr->ctrl_flags & TCP_FLAG_FIN)
        {
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSING;
            /* jump to CLOSED state directly */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSED;
            tcp_state_update(pcbId, TCP_STATE_CLOSED);
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num + 1;
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);
            tcp_delPCB(pcbId);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        else if (pTcpHdr->ctrl_flags & TCP_FLAG_ACK)
        {
            //tcp_pcb_entry[pcbId].state = TCP_STATE_FIN_WAIT_2;
            tcp_state_update(pcbId, TCP_STATE_FIN_WAIT_2);
        }
        break;

    case TCP_STATE_FIN_WAIT_2:
		printf("TCP_STATE_FIN_WAIT_2\n");
        if (pTcpHdr->ctrl_flags & TCP_FLAG_FIN)
        {
            //tcp_pcb_entry[pcbId].state = TCP_STATE_TIME_WAIT;
            /* jump to CLOSED state directly */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSED;
            tcp_state_update(pcbId, TCP_STATE_CLOSED);
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num + 1;
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);
            tcp_delPCB(pcbId);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        break;

    case TCP_STATE_CLOSE_WAIT:
		printf("TCP_STATE_CLOSE_WAIT\n");
        /* skip this state (from ESTABLISHED to LAST_ACK directly) */
        break;

    case TCP_STATE_CLOSING:
		printf("TCP_STATE_CLOSING\n");
        if (pTcpHdr->ctrl_flags & TCP_FLAG_ACK)
        {
            /* Active CLOSE */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_TIME_WAIT;
            /* jump to CLOSED state directly */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSED;
            tcp_state_update(pcbId, TCP_STATE_CLOSED);
            tcp_delPCB(pcbId);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        break;

    case TCP_STATE_LAST_ACK:
		printf("TCP_STATE_LAST_ACK\n");
        if (pTcpHdr->ctrl_flags & TCP_FLAG_ACK)
        {
            /* Passive CLOSE */
            //tcp_pcb_entry[pcbId].state = TCP_STATE_CLOSED;
            tcp_state_update(pcbId, TCP_STATE_CLOSED);
            tcp_pcb_entry[pcbId].remote_seq = pTcpHdr->seq_num;
            //tcp_up_handle_ip_reply(pBuf, pcbId, TCP_FLAG_ACK);
            tcp_xmit(pcbId, NULL, 0, TCP_FLAG_ACK);
            tcp_delPCB(pcbId);
            //lwps_pbuf_free(pBuf);

            return LWPS_OK;
        }
        break;

    case TCP_STATE_TIME_WAIT:
		//printf("TCP_STATE_TIME_WAIT\n");
        break;

    case TCP_STATE_CLOSED:
		//printf("TCP_STATE_CLOSED\n");
        break;

    default:
        break;
    }

    //lwps_pbuf_free(pBuf);

    return LWPS_OK;
} /* end of tcp_up_handle_ip */
#endif  /* LWPS_IP */

int16 tcp_up(pktBuf_t *pBuf) large
{
    tcp_hdr_t xdata *pTcpHdr = (tcp_hdr_t *)pBuf->l4;

    DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Dispatch */
    switch (pBuf->l3fmt)
    {
#ifdef  LWPS_IP
    case L3FMT_IP:
        if (LWPS_OK != tcp_up_handle_ip(pBuf))
        {
            DBG_LWPS_TCP_PRINTF(("%s():%d - tcp_up_handle_ip failed!\n", __FILE__, (uint16)__LINE__));
            return LWPS_ERR;
        }
        break;
#endif  /* LWPS_IP */

    default:
        DBG_LWPS_TCP_PRINTF(("%s():%d - l3fmt (%d) is NOT supported!\n", __FILE__, (uint16)__LINE__, pBuf->l3fmt));
        //lwps_pbuf_free(pBuf);
        break;
    }

	DBG_LWPS_TCP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
    return LWPS_OK;
} /* end of tcp_up */

int16 tcp_bind(uint16 pcbId, ip_addr_t ip, uint16 port) large
{
    DBG_LWPS_TCP_PRINTF(("%s():%d pcbId = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId));

    if (pcbId > TCP_CONNECTION_MAX)
    {
        OSAL_PRINTF("%s():%d - pcbId (%d) Error\n", __FILE__, (uint16)__LINE__, pcbId);

        return LWPS_ERR;
    }

    tcp_pcb_entry[pcbId].local_ip = ip;
    tcp_pcb_entry[pcbId].local_port = port;

    return LWPS_OK;
}

int16 tcp_listen(uint16 pcbId, uint16 backlog) large
{
    DBG_LWPS_TCP_PRINTF(("%s():%d pcbId = %d, backlog = %d\n", \
        __FILE__, (uint16)__LINE__, pcbId, backlog));

    if(backlog){}

    if (pcbId > TCP_CONNECTION_MAX)
    {
        DBG_LWPS_TCP_PRINTF(("%s():%d - pcbId (%d) Error\n", __FILE__, (uint16)__LINE__, pcbId));

        return LWPS_ERR;
    }

    //tcp_pcb_entry[pcbId].state = TCP_STATE_LISTEN;
    tcp_state_update(pcbId, TCP_STATE_LISTEN);

    return LWPS_OK;
}

/*
int16 tcp_connect(uint16 pcbId, ip_addr_t ip, uint16 port) large
{
    DBG_LWPS_TCP_PRINTF(("%():%d pcbId = %d, host = %d.%d.%d.%d:%d\n", \
        __FILE__, (uint16)__LINE__, \
        ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3], port));

    return LWPS_OK;
}
*/

int16 tcp_send(uint16 pcbId, uint8 *pData, uint16 length) large
{
#if 1
    int32 xdata i;

    for (i=0; i<length; i+=1460)
    {
        if (LWPS_OK != tcp_xmit(pcbId, pData + i, ((length - i) >= 1460)? 1460 : (length - i), TCP_FLAG_ACK))
        {
            return LWPS_ERR;
        }
    }

    return LWPS_OK;
#else
    return tcp_xmit(pcbId, pData, length, TCP_FLAG_ACK);
#endif
}

int16 tcp_close(uint16 pcbId) large
{
    DBG_LWPS_TCP_PRINTF(("%s():%d pcbId = %d\n", __FILE__, (uint16)__LINE__, pcbId));

    if (pcbId > TCP_CONNECTION_MAX)
    {
        DBG_LWPS_TCP_PRINTF(("%s():%d - pcbId (%d) Error\n",  __FILE__, (uint16)__LINE__, pcbId));
        return LWPS_ERR;
    }

    if (LWPS_OK != tcp_xmit(pcbId, NULL, 0, TCP_FLAG_FIN | TCP_FLAG_ACK))
    {
        OSAL_PRINTF("%s():%d - tcp_xmit failed\n", __FILE__, (uint16)__LINE__);
        return LWPS_ERR;
    }

    //tcp_pcb_entry[pcbId].state = TCP_STATE_FIN_WAIT_1;
    tcp_state_update(pcbId, TCP_STATE_FIN_WAIT_1);

#if 0   /* temporary solution */
    /* fix some browser compatible issue */
    {
        DBG_LWPS_TCP_PRINTF(("%s():%d FORCE CLOSE TCP Connection, pcbId = %d\n", __FILE__, (uint16)__LINE__, pcbId));
        tcp_delPCB(pcbId);
    }
#endif

    return LWPS_OK;
}

#endif /* end of LWPS_TCP */

