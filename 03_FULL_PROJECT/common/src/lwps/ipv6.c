/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */
#pragma STRING (far)
#include <local/lwps_config.h>

#if LWPS_IPV6

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
static far const uint8 next_hdr_list[] = 
{
    IPV6_PROTO_HBHO,
    IPV6_PROTO_ROUTING,
    /*IPV6_PROTO_FRAG, */ /* Drop this packet when meet */
    /*IPV6_PROTO_AH,   */ /* Drop this packet when meet */
    /*IPV6_PROTO_ESP,  */ /* Drop this packet when meet */
    IPV6_PROTO_DO
};

static far const uint8 next_hdr_list_size = sizeof(next_hdr_list)/sizeof(uint8);


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
#if DBG_LWPS_IPV6_ENABLE
// IPv6 Header Dump: Prints formatted IPv6 header fields for debugging purposes
// Displays version, payload length, traffic class, next header, hop limit, and source/destination addresses
void ipv6_hdr_dump(ipv6_hdr_t *pIpv6Hdr)
{
    u8_t traffic;

    traffic = (pIpv6Hdr->trafC_1 << 4) | (pIpv6Hdr->trafC_2);
    OSAL_PRINTF("-- [IPV6 HDR dump] ------------------------------\n");
    OSAL_PRINTF("  Version: 0x%01X\n", (uint16)pIpv6Hdr->version);
    OSAL_PRINTF("   PL_LEN: 0x%04X\n", (uint16)pIpv6Hdr->payload_len);
    OSAL_PRINTF("  TRAFFIC: 0x%02bX\n", traffic);
    OSAL_PRINTF("  NextHdr: 0x%02bX\n", pIpv6Hdr->next_hdr);
    OSAL_PRINTF(" HopLimit: 0x%02bX\n", pIpv6Hdr->hop_limit);
    OSAL_PRINTF("   Src-IP: %02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX\n", \
        pIpv6Hdr->src_ip[0],  pIpv6Hdr->src_ip[1],  \
        pIpv6Hdr->src_ip[2],  pIpv6Hdr->src_ip[3],  \
        pIpv6Hdr->src_ip[4],  pIpv6Hdr->src_ip[5],  \
        pIpv6Hdr->src_ip[6],  pIpv6Hdr->src_ip[7],  \
        pIpv6Hdr->src_ip[8],  pIpv6Hdr->src_ip[9],  \
        pIpv6Hdr->src_ip[10], pIpv6Hdr->src_ip[11], \
        pIpv6Hdr->src_ip[12], pIpv6Hdr->src_ip[13], \
        pIpv6Hdr->src_ip[14], pIpv6Hdr->src_ip[15]     );
    OSAL_PRINTF("   Dst-IP: %02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX\n", \
        pIpv6Hdr->dst_ip[0],  pIpv6Hdr->dst_ip[1],  \
        pIpv6Hdr->dst_ip[2],  pIpv6Hdr->dst_ip[3],  \
        pIpv6Hdr->dst_ip[4],  pIpv6Hdr->dst_ip[5],  \
        pIpv6Hdr->dst_ip[6],  pIpv6Hdr->dst_ip[7],  \
        pIpv6Hdr->dst_ip[8],  pIpv6Hdr->dst_ip[9],  \
        pIpv6Hdr->dst_ip[10], pIpv6Hdr->dst_ip[11], \
        pIpv6Hdr->dst_ip[12], pIpv6Hdr->dst_ip[13], \
        pIpv6Hdr->dst_ip[14], pIpv6Hdr->dst_ip[15]     );
    OSAL_PRINTF("-----------------------------------------------\n");
}
#endif /*DBG_LWPS_IPV6_ENABLE*/

/*
uint32 ipv6_init(void)
{
    DBG_LWPS_IPV6_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    return LWPS_OK;
}
*/

// IPv6 Up: Processes incoming IPv6 packets and dispatches to appropriate protocol handlers
// Handles IPv6 extension headers and routes ICMPv6/MLD packets to mld_up handler
uint32 ipv6_up(pktBuf_t *pBuf)
{
    u8_t i, nxt_hdr;
    u8_t *pnxt_hdr;
    ipv6_hdr_t xdata *pIpv6Hdr = (ipv6_hdr_t *)pBuf->l3;

    DBG_LWPS_IPV6_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Layer 4 protocol */
    pBuf->l4 = (pBuf->l3 + 40);

	ipv6_hdr_dump(pIpv6Hdr);

    /* only MLD(ICMPv6) support */
    nxt_hdr = pIpv6Hdr->next_hdr;
    while( (IPV6_PROTO_ICMPV6!=nxt_hdr) )
    {
        for(i=0;i<next_hdr_list_size;i++)
        {
            if( next_hdr_list[i]==nxt_hdr )
            {
                pnxt_hdr = pBuf->l4;
                pBuf->l4 += ((pnxt_hdr[1]+1)*8);
                nxt_hdr = *pnxt_hdr;
                break;
            }
        }
        if( i==next_hdr_list_size )
        {
            return LWPS_OK;
        }
    }

    switch (nxt_hdr) {
#if CONFIG_IGMP_MLD_ENABLE
    case IPV6_PROTO_ICMPV6: /* ICMPv6 */
        DBG_LWPS_IPV6_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
        pBuf->l4fmt = L4FMT_ICMPV6;
        if (LWPS_OK != mld_up(pBuf)) {
            return LWPS_ERR;
        }
        break;
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

    default:
        DBG_LWPS_IPV6_PRINTF(("ipv6_up(): IPv6 next hdr (%d) is not supported!\n", pIpv6Hdr->next_hdr));
        break;
    }

	//ipv6_hdr_dump(pBuf->l3);
    return LWPS_OK;
}

#endif /* end of LWPS_IPV6 */