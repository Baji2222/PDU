#pragma STRING(far)
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

#if LWPS_IP

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
static uint16    xdata ip_hdr_id_number = 0;
static ip_addr_t xdata ip_addr_bcast = { { 255, 255, 255, 255 } };


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
#if DBG_LWPS_IP_ENABLE
//static void ip_hdr_dump(ip_hdr_t *pIpHdr)
// IP Header Dump: Prints formatted IP header fields for debugging purposes
// Displays version, header length, TOS, length, ID, flags, TTL, protocol, checksum and addresses
void ip_hdr_dump(ip_hdr_t *pIpHdr)
{
    OSAL_PRINTF("-- [IP HDR dump] ------------------------------\n");
    OSAL_PRINTF("    Version: 0x%01X\n", (uint16)pIpHdr->version);
    OSAL_PRINTF("    Hdr-Len: 0x%01X\n", (uint16)pIpHdr->hdr_len);
    OSAL_PRINTF("        TOS: 0x%02X\n", (uint16)pIpHdr->tos);
    OSAL_PRINTF("     Length: 0x%04X (%d Bytes)\n", (uint16)pIpHdr->length, (uint16)pIpHdr->length);
    OSAL_PRINTF("  Id Number: 0x%04X (%d)\n", (uint16)pIpHdr->id_number, (uint16)pIpHdr->id_number);
    OSAL_PRINTF(" FragOffset: 0x%04X (%d)\n", (uint16)pIpHdr->frag_offset, (uint16)pIpHdr->frag_offset);
    OSAL_PRINTF("        TTL: 0x%02X (%d)\n", (uint16)pIpHdr->ttl, (uint16)pIpHdr->ttl);
    OSAL_PRINTF("      Proto: 0x%02X (%d)\n", (uint16)pIpHdr->proto, (uint16)pIpHdr->proto);
    OSAL_PRINTF("   Checksum: 0x%04X\n", (uint16)pIpHdr->chksum);
    OSAL_PRINTF("     Src-IP: %d.%d.%d.%d\n", \
        (uint16)pIpHdr->src_ip[0], \
        (uint16)pIpHdr->src_ip[1], \
        (uint16)pIpHdr->src_ip[2], \
        (uint16)pIpHdr->src_ip[3]);
    OSAL_PRINTF("     Dst-IP: %d.%d.%d.%d\n", \
        (uint16)pIpHdr->dst_ip[0], \
        (uint16)pIpHdr->dst_ip[1], \
        (uint16)pIpHdr->dst_ip[2], \
        (uint16)pIpHdr->dst_ip[3]);
    OSAL_PRINTF("-----------------------------------------------\n");
}
#endif /*DBG_LWPS_IP_ENABLE*/

// IP Initialize: Initializes the IP protocol layer
// Returns LWPS_OK on successful initialization
uint32 ip_init(void)
{
    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    return LWPS_OK;
}

// IP Up: Processes incoming IP packets and dispatches to appropriate protocol handlers
// Validates destination IP, then routes to ICMP, IGMP, TCP, or UDP based on protocol field
uint32 ip_up(pktBuf_t *pBuf)
{
    ip_hdr_t xdata *pIpHdr = (ip_hdr_t *)pBuf->l3;

    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

#if LWPS_ETHARP
    if ((!IS_IPADDR_EQUAL(pIpHdr->dst_ip, ip_addr_bcast.addr)) &&
        (!IS_IPADDR_EQUAL(pIpHdr->dst_ip, this_ip))            &&
        (0xE0!=(0xE0&pIpHdr->dst_ip[0]))
       )
    {
        DBG_LWPS_IP_PRINTF(("ip_up(): DIP(%d.%d.%d.%d) != MyIP(%d.%d.%d.%d) --> Drop!\n", \
            (uint16)pIpHdr->dst_ip[0], (uint16)pIpHdr->dst_ip[1], \
            (uint16)pIpHdr->dst_ip[2], (uint16)pIpHdr->dst_ip[3], \
            (uint16)this_ip[0],        (uint16)this_ip[1], \
            (uint16)this_ip[2],        (uint16)this_ip[3]));
            
        return LWPS_OK;
    }
#endif /* end of LWPS_ETHARP */

    /* Layer 4 protocol */
    pBuf->l4 = (pBuf->l3 + (pIpHdr->hdr_len)*4);

	ip_hdr_dump(pIpHdr);
	DBG_LWPS_IP_PRINTF(("%s:%d, size = %d\n", __FILE__, (uint16)__LINE__, (uint16)pBuf->size));

    switch (pIpHdr->proto) {
#if  LWPS_ICMP
    case IP_PROTO_ICMP: /* ICMP */
        pBuf->l4fmt = L4FMT_ICMP;
        if (LWPS_OK != icmp_up(pBuf)) {
            return LWPS_ERR;
        }
        break;
#endif  /* end of LWPS_ICMP */

#if  LWPS_IGMP
    case IP_PROTO_IGMP: /* IGMP */
        pBuf->l4fmt = L4FMT_IGMP;
        if (LWPS_OK != igmp_up(pBuf)) {
            return LWPS_ERR;
        }
        break;
#endif  /* end of LWPS_IGMP */

#if LWPS_TCP
    case IP_PROTO_TCP:  /* TCP */
        pBuf->l4fmt = L4FMT_TCP;
        if (LWPS_OK != tcp_up(pBuf)) {
            return LWPS_ERR;
        }
        break;
#endif  /* end of LWPS_TCP */

#if LWPS_UDP
    case IP_PROTO_UDP:  /* UDP */
        pBuf->l4fmt = L4FMT_UDP;
        if (LWPS_OK != udp_up(pBuf)) {
            return LWPS_ERR;
        }
        break;
#endif  /* end of LWPS_UDP */

    default:
        DBG_LWPS_IP_PRINTF(("ip_up(): IP proto (%d) is not supported!\n", pIpHdr->proto));
        break;
    }

	//ip_hdr_dump(pBuf->l3);
    return LWPS_OK;
}

#if LWPS_ICMP
// IP Down Handle ICMP: Prepares ICMP packet for transmission by adding IP header
// Sets IP header fields for ICMP protocol and copies source/destination addresses
static uint32 ip_down_handle_icmp(pktBuf_t *pBuf, ip_addr_t *pIpAddr)
{
    ip_hdr_t xdata *pIpHdr_Output;

    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Insert IP Header into the packet(pBuf) */
    pBuf->l3 = pBuf->l4 - sizeof(ip_hdr_t);
    pBuf->size += sizeof(ip_hdr_t);
    pIpHdr_Output = (ip_hdr_t *)pBuf->l3;

    /* IP Header Setting */
    pIpHdr_Output->length = pBuf->size;
    pIpHdr_Output->version = 4;
    pIpHdr_Output->hdr_len = 5;
    pIpHdr_Output->tos = 0;
    pIpHdr_Output->id_number = ip_hdr_id_number++;
    pIpHdr_Output->frag_offset = 0; /* Not support fragment packet */
    pIpHdr_Output->ttl = IP_HDR_DEFAULT_TTL;
    pIpHdr_Output->proto = IP_PROTO_ICMP;
    pIpHdr_Output->chksum = 0;  /* Always let NIC recalculate */
    IPADDR_COPY(pIpHdr_Output->src_ip, this_ip);
    IPADDR_COPY(pIpHdr_Output->dst_ip, pIpAddr->addr);

    //ip_hdr_dump(pIpHdr_Output);

    return LWPS_OK;
} /* end of ip_down_handle_icmp */
#endif  /* LWPS_ICMP */

#if LWPS_TCP
// IP Down Handle TCP: Prepares TCP packet for transmission by adding IP header
// Sets IP header fields for TCP protocol with DF flag for no fragmentation
static uint32 ip_down_handle_tcp(pktBuf_t *pBuf, ip_addr_t *pIpAddr)
{

    ip_hdr_t xdata *pIpHdr_Output;

    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));


    /* Insert IP Header into the packet(pBuf) */
    pBuf->l3 = pBuf->l4 - sizeof(ip_hdr_t);
    pBuf->size += sizeof(ip_hdr_t);
    pIpHdr_Output = (ip_hdr_t *)pBuf->l3;

    /* IP Header Setting */
    pIpHdr_Output->version = 4;
    pIpHdr_Output->hdr_len = 5;
    pIpHdr_Output->tos = 0;
    pIpHdr_Output->length = pBuf->size;
    pIpHdr_Output->id_number = ip_hdr_id_number++;
    pIpHdr_Output->frag_offset = 0 | IP_FLAG_DF; /* Not support fragment packet */
    pIpHdr_Output->ttl = IP_HDR_DEFAULT_TTL;
    pIpHdr_Output->proto = IP_PROTO_TCP;
    pIpHdr_Output->chksum = 0;  /* Always let NIC recalculate */
    IPADDR_COPY(pIpHdr_Output->src_ip, this_ip);   /* [FIXME] */
    IPADDR_COPY(pIpHdr_Output->dst_ip, pIpAddr->addr);

    //ip_hdr_dump(pIpHdr_Output);

    return LWPS_OK;
} /* end of ip_down_handle_tcp */
#endif  /* LWPS_TCP */

#if LWPS_UDP
// IP Down Handle UDP: Prepares UDP packet for transmission by adding IP header
// Sets IP header fields for UDP protocol with DF flag for no fragmentation
static uint32 ip_down_handle_udp(pktBuf_t *pBuf, ip_addr_t *pIpAddr)
{
    ip_hdr_t xdata *pIpHdr_Output;

    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Insert IP Header into the packet(pBuf) */
    pBuf->l3 = pBuf->l4 - sizeof(ip_hdr_t);
    pBuf->size += sizeof(ip_hdr_t);
    pIpHdr_Output = (ip_hdr_t *)pBuf->l3;
    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* IP Header Setting */
    pIpHdr_Output->version = 4;
    pIpHdr_Output->hdr_len = 5;
    pIpHdr_Output->tos = 0;
    pIpHdr_Output->length = pBuf->size;
    pIpHdr_Output->id_number = ip_hdr_id_number++;
    pIpHdr_Output->frag_offset = 0 | IP_FLAG_DF; /* Not support fragment packet */
    pIpHdr_Output->ttl = IP_HDR_DEFAULT_TTL;
    pIpHdr_Output->proto = IP_PROTO_UDP;
    pIpHdr_Output->chksum = 0;  /* Always let NIC recalculate */
    //pIpHdr_Output->src_ip = pIpHdr->dst_ip;
    //IPADDR_COPY(pIpHdr_Output->src_ip, pBuf->netif_tx->ip_addr.addr);
    IPADDR_COPY(pIpHdr_Output->src_ip, this_ip);    /* [FIXME] */
    //pIpHdr_Output->dst_ip = pIpHdr->src_ip;
    IPADDR_COPY(pIpHdr_Output->dst_ip, pIpAddr->addr);

    //ip_hdr_dump(pIpHdr_Output);

    return LWPS_OK;
} /* end of ip_down_handle_udp */
#endif  /* LWPS_UDP */

// IP Down: Prepares packets for transmission by adding IP headers based on layer 4 protocol
// Dispatches to protocol-specific handlers and forwards to Ethernet layer
uint32 ip_down(pktBuf_t *pBuf, ip_addr_t *pIpAddr)
{
    DBG_LWPS_IP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Check parameter */
    /*if ((pBuf == NULL) || (pIpAddr == NULL)) {
        DBG_LWPS_IP_PRINTF(("%s():%d - pBuf or pIpHdr is NULL\n", __FILE__, (uint16)__LINE__));
        return LWPS_ERR_NULLPOINT;
    }*/

    switch (pBuf->l4fmt) {
#if LWPS_ICMP
    case L4FMT_ICMP:
        if (LWPS_OK != ip_down_handle_icmp(pBuf, pIpAddr)) {
            return LWPS_OK;
        }
        break;
#endif  /* LWPS_ICMP */

#if LWPS_TCP
    case L4FMT_TCP:
        if (LWPS_OK != ip_down_handle_tcp(pBuf, pIpAddr)) {
            //lwps_pbuf_free(pBuf);
            return LWPS_OK;
        }
        break;
#endif  /* LWPS_TCP */

#if LWPS_UDP
    case L4FMT_UDP:
        if (LWPS_OK != ip_down_handle_udp(pBuf, pIpAddr)) {
            //lwps_pbuf_free(pBuf);
            return LWPS_OK;
        }
        break;
#endif  /* LWPS_UDP */

    default:
        DBG_LWPS_IP_PRINTF(("ip_down(): L4FMT (%d) is not supported!\n", (uint16)pBuf->l4fmt));
        return LWPS_ERR;
        break;
    }

    pBuf->l3fmt = L3FMT_IP;
    DBG_LWPS_IP_PRINTF(("%s:%d, size = %d\n", __FILE__, (uint16)__LINE__, (uint16)pBuf->size));
    if (LWPS_OK != eth_down(pBuf)) {
        //OSAL_PRINTF("%s():%d - lwps_pbuf_tx failed (drop packet)!\n", __FILE__, (uint16)__LINE__);
        //lwps_pbuf_free(pBuf);
        /* DO NOT return ERROR, Just release this packet */
    }

    return LWPS_OK;
} /* end of ip_down */

#endif /* end of LWPS_IP */