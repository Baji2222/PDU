#pragma STRING(far)
/*
* Copyright Realtek Semiconductor Corporation, 2008
* All rights reserved.
*
* Program  : ICMP (Internet Control Message Protocol) of Light-weight Protocol Stack
* Abstract : 
* Author   : 
* $Id: $
*/

#include <local/lwps_config.h>

#if LWPS_ICMP

/*
 * Include Files
 */

#include <lwps/lwps.h>
#include <stdio.h>
#include <string.h>

#include <local/sal_config.h>

/*
 * Symbol Definition
 */



/*
 * Function Declaration
 */
#if DBG_LWPS_ICMP_ENABLE
// ICMP Header Dump: Prints formatted ICMP header fields for debugging purposes
// Displays type, code, checksum, identifier, and sequence number fields
static void icmp_hdr_dump(icmp_hdr_t *pIcmpHdr) large
{
    DBG_LWPS_ICMP_PRINTF("-- [ICMP HDR dump] ----------------------------\n");
    DBG_LWPS_ICMP_PRINTF("       Type: 0x%02X (%d)\n", (uint16)pIcmpHdr->type, (uint16)pIcmpHdr->type);
    DBG_LWPS_ICMP_PRINTF("       Code: 0x%02X (%d)\n", (uint16)pIcmpHdr->cod, (uint16)pIcmpHdr->cod);
    DBG_LWPS_ICMP_PRINTF("   Checksum: 0x%04X\n", (uint16)pIcmpHdr->chksum);
    DBG_LWPS_ICMP_PRINTF(" Identifier: 0x%04X\n", (uint16)pIcmpHdr->identifier);
    DBG_LWPS_ICMP_PRINTF(" Seq-Number: 0x%04X (%d)\n", (uint16)pIcmpHdr->seq_number, (uint16)pIcmpHdr->seq_number);
    DBG_LWPS_ICMP_PRINTF("-----------------------------------------------\n");
}
#endif /* end of DBG_LWPS_ICMP_ENABLE */

// ICMP Initialize: Initializes the ICMP protocol layer
// Returns LWPS_OK on successful initialization
int32 icmp_init(void) large
{
    DBG_LWPS_ICMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    return LWPS_OK;
}

// ICMP Up Handle Echo Request: Processes ICMP echo request (ping) and sends echo reply
// Swaps IP addresses, sets reply type, and transmits response back to sender
static int32 icmp_up_handle_echo_request(pktBuf_t xdata *pBuf) large
{
    ip_hdr_t   xdata *pIpHdr         = (ip_hdr_t *)pBuf->l3;  /* IP only */
    icmp_hdr_t xdata *pIcmpHdr       = (icmp_hdr_t *)pBuf->l4;
    ip_addr_t  xdata DstIpAddr;
    pktBuf_t   xdata *pBuf_Reply     = pBuf;
    icmp_hdr_t xdata *pIcmpHdr_Reply = (icmp_hdr_t *)pBuf->l4;

    DBG_LWPS_ICMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    /* ICMP HDR setting */
    pIcmpHdr_Reply->type = ICMP_TYPE_ECHOREPLY;
    pIcmpHdr_Reply->cod = 0;
    pIcmpHdr_Reply->chksum = 0;
    pIcmpHdr_Reply->identifier = pIcmpHdr->identifier;
    pIcmpHdr_Reply->seq_number = pIcmpHdr->seq_number;

    //icmp_hdr_dump(pIcmpHdr_Reply);

    /* IP information */
    IPADDR_COPY( (DstIpAddr.addr), pIpHdr->src_ip);
    /* Reply the padding */
           
    //pBuf_Reply->length += (pBuf->length - sizeof(icmp_hdr_t));


    pBuf_Reply->l4fmt = L4FMT_ICMP;
	pBuf_Reply->size -= sizeof(eth_hdr_t);//pre-subtract

    if (LWPS_OK != ip_down(pBuf_Reply, &DstIpAddr)) {
        OSAL_PRINTF("%s():%d - ip_output failed!\n", __FILE__, (uint16)__LINE__);
    }
    
    //icmp_hdr_dump(pIcmpHdr_Reply);

    return LWPS_OK;
}

// ICMP Up Handle Echo Reply: Handles incoming ICMP echo reply packets
// Currently placeholder for processing ping responses (not fully implemented)
static int32 icmp_up_handle_echo_reply(pktBuf_t *pBuf) large
{
	/* kill compiler warning message */
	uint8 xdata tmp = pBuf->l2fmt;

    DBG_LWPS_ICMP_PRINTF("%s():%d - [FIXME] ECHO reply \n", __FILE__, (uint16)__LINE__);

    return LWPS_OK;
}

// ICMP Up: Dispatches incoming ICMP packets to appropriate handlers
// Routes echo requests to reply handler and echo replies to reply handler
int32 icmp_up(pktBuf_t *pBuf) large
{
    icmp_hdr_t xdata *pIcmpHdr = (icmp_hdr_t *)pBuf->l4;

    DBG_LWPS_ICMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    //icmp_hdr_dump(pIcmpHdr);

    switch (pIcmpHdr->type) {
    case ICMP_TYPE_ECHO: 
        return icmp_up_handle_echo_request(pBuf);
        break;

    case ICMP_TYPE_ECHOREPLY:
        return icmp_up_handle_echo_reply(pBuf);
        break;

    default:
        DBG_LWPS_ICMP_PRINTF("icmp_up() - Not support icmp type %d \n", pIcmpHdr->type);
        //lwps_pbuf_free(pBuf);
        break;
    }

	
    return LWPS_OK;
}

#endif /* end of LWPS_ICMP */