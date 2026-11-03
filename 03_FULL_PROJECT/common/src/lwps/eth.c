//#pragma STRING (far)
/*
 * Ethernet of Light-weight Protocol Stack
 */

#include <local/lwps_config.h>

#if LWPS_ENABLE


/*
 * Include Files
 */

#include <lwps/lwps.h>
#include <stdio.h>
#if LWPS_LLDP
#include <lldp/lldp.h>
#endif
#include <rtk_proto/rtk_proto.h>
#include <salnic.h>
#include <salvlan.h>
#if RTK_PROTO_RRCPV3
#include <rrcpv3.h>
#endif

/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
/* For Debug */
 
 
#if DBG_LWPS_ETH_ENABLE
//static void eth_hdr_dump(eth_hdr_t *pEthHdr)
// Ethernet Header Dump: Prints formatted Ethernet header information for debugging purposes
// Displays destination MAC, source MAC, and ethertype fields in readable format
void eth_hdr_dump(eth_hdr_t *pEthHdr)  large
{
    OSAL_PRINTF("-- [ETH HDR dump] -----------------------------\n");
    OSAL_PRINTF(" Dst-MAC: %02X:%02X:%02X:%02X:%02X:%02X \n",
        (uint16)pEthHdr->dst_mac.addr[0],
        (uint16)pEthHdr->dst_mac.addr[1],
        (uint16)pEthHdr->dst_mac.addr[2],
        (uint16)pEthHdr->dst_mac.addr[3],
        (uint16)pEthHdr->dst_mac.addr[4],
        (uint16)pEthHdr->dst_mac.addr[5]
        );
    OSAL_PRINTF(" Src-MAC: %02X:%02X:%02X:%02X:%02X:%02X \n",
        (uint16)pEthHdr->src_mac.addr[0],
        (uint16)pEthHdr->src_mac.addr[1],
        (uint16)pEthHdr->src_mac.addr[2],
        (uint16)pEthHdr->src_mac.addr[3],
        (uint16)pEthHdr->src_mac.addr[4],
        (uint16)pEthHdr->src_mac.addr[5]
        );
    OSAL_PRINTF(" Ethtype: 0x%04X\n", pEthHdr->ether_type);
    OSAL_PRINTF("-----------------------------------------------\n");
}
#endif /* end of DBG_LWPS_ETH_ENABLE */

// Ethernet Initialize: Initializes the Ethernet layer of the protocol stack
// Returns LWPS_OK on successful initialization
int32 eth_init(void)
{
    DBG_LWPS_ETH_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    return LWPS_OK;
}

// Ethernet Up: Processes incoming Ethernet frames and dispatches to appropriate protocol handlers
// Handles CPU tags, VLAN tags, and routes packets to IP, ARP, RSTP, or Realtek protocol handlers
int32 eth_up(pktBuf_t *pBuf)
{
    uint16    xdata eth_type;
    uint16    xdata *eth_type_ptr = NULL;
    uint8     xdata rtk_proto_id;
    eth_hdr_t xdata *pEthHdr;
    uint8     xdata tag_off = 0;
    ip_addr_t *sip;

    DBG_LWPS_ETH_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* DO NOT check NULL pointer */

    /* Basic process */
    pEthHdr = (eth_hdr_t *)pBuf->l2;

    if ( ETH_TYPE_REALTEK==(eth_type=ntohs(pEthHdr->ether_type)) )
    {
        /* cpu tag */
        if ( RTK_CPUTAG_PROTO_ID == *(pBuf->l2 + sizeof(eth_hdr_t)) )
        {
            pBuf->pkt_cputag = pBuf->l2 + sizeof(struct eth_addr_s)*2;
            eth_type_ptr =  (uint16*)(pBuf->pkt_cputag + CPU_TAG_SIZE);
            eth_type = ntohs( *eth_type_ptr );
			tag_off = CPU_TAG_SIZE;
       	}
#ifdef RTL8367C
       	else if( 0x23 == *(pBuf->l2 + sizeof(eth_hdr_t)) ) //0x23 is RLDP protocol identifier--> do not take action
       	{
            return LWPS_OK;
       	}
#endif       	
        else
        {
            pBuf->pkt_cputag = NULL;
        }
   	}
    else
    {
        pBuf->pkt_cputag = NULL;
    }
    sal_nic_rxPort_set(pBuf);

    if ( ETH_TYPE_VLAN == eth_type )
    {
        pBuf->pkt_vlantag = pBuf->l2 + sizeof(struct eth_addr_s)*2 + tag_off;
        eth_type_ptr =  (uint16*)(pBuf->pkt_vlantag + ETH_VLANTAG_LEN);
        eth_type = ntohs( *eth_type_ptr );		
		tag_off += ETH_VLANTAG_LEN;
    }
    else
    {
        pBuf->pkt_vlantag = NULL;
    }    
    sal_nic_rxVid_set(pBuf);

    if(tag_off)
    {
        int8 i;
        /* move DA, SA backward serveral bytes to overwrite CPU tag or VLAN tag */
        /* move last byte of SA first and first byte of DA last */
        for(i = ETH_HWADDR_LEN * 2 - 1; i >= 0; i--)
        {
            *(pBuf->l2 + i + tag_off) = *(pBuf->l2 + i);
        }

        pBuf->l2 += tag_off;
        pBuf->size -= tag_off;
        /* Since the tag have been removed, set this pointer to NULL */
        pBuf->pkt_vlantag = NULL;
    }


    switch ( eth_type )
    {
#if LWPS_IP
    case ETH_TYPE_IP:   /* 0x0800 */
        pBuf->l3fmt = L3FMT_IP;
       	pBuf->l3 = pBuf->l2 + sizeof(eth_hdr_t);

#if LWPS_ETHARP
        /* Update ARP entry */
		sip = (ip_addr_t *)&(((ip_hdr_t *)pBuf->l3)->src_ip);

	    if (((sip->addr[0] & netmask[0]) != (this_ip[0] & netmask[0])) ||
	         ((sip->addr[1] & netmask[1]) != (this_ip[1] & netmask[1])) ||
	         ((sip->addr[2] & netmask[2]) != (this_ip[2] & netmask[2])) ||
	         ((sip->addr[3] & netmask[3]) != (this_ip[3] & netmask[3])))
	    {	
	    	//different subnet
	    }
        else
	    {
	    	//only update ARP cache from same subnet, don't learn the routing packet from other subnet
            etharp_entry_update((ip_addr_t *)&(((ip_hdr_t *)pBuf->l3)->src_ip), &(((eth_hdr_t *)pBuf->l2)->src_mac));
	    }        
#endif /* end of LWPS_ETHARP */

        if (LWPS_OK != ip_up(pBuf))
        {
           return LWPS_ERR;
        }
        break;
#endif /* end of LWPS_IP */

#if LWPS_IPV6
    case ETH_TYPE_IPV6:
        pBuf->l3fmt = L3FMT_IPV6;
       	pBuf->l3 = pBuf->l2 + sizeof(eth_hdr_t);
        if (LWPS_OK != ipv6_up(pBuf))
        {
            return LWPS_ERR;
        }
        break;
#endif /* end of LWPS_IPV6 */

#if LWPS_RSTP
    case ETH_TYPE_STP_TCN:
    case ETH_TYPE_STP:
    case ETH_TYPE_RSTP:
        pBuf->l2fmt = L2FMT_LEN;
        pBuf->l3fmt = L3FMT_RSTP; 
        pBuf->l3 = pBuf->l2 + sizeof(eth_hdr_t);
        rstp_up(pBuf);/* LLC check in rstp_up */
        break;
#endif /* end of LWPS_RSTP */

#if LWPS_ETHARP
    case ETH_TYPE_ARP:  /* 0x0806 */
    	pBuf->l2fmt = L2FMT_ETHARP;
        pBuf->l3 = pBuf->l2 + sizeof(eth_hdr_t);        
        if (LWPS_OK != etharp_up(pBuf))
        {
            return LWPS_ERR;
        }
        break;
#endif /* end of LWPS_ETHARP */

#if LWPS_LLDP
        case ETH_TYPE_LLDP:
            //handleLLDPpkt(pBuf);
            break;
#endif

#if LWPS_REALTEK
    case ETH_TYPE_REALTEK:
		pBuf->l3 = pBuf->l2 + sizeof(eth_hdr_t);
		rtk_proto_id = *pBuf->l3;
        switch ( rtk_proto_id )
        {
#if RTK_PROTO_RRCPV1
        case RTK_PROTO_ID_RRCPV1:
			rrcpv1_up(pBuf);
			break;
#endif

#if RTK_PROTO_RRCPV3       
        case RTK_PROTO_ID_RRCPV3:
            rrcpv3_up(pBuf);
            break;
#endif

#if RTK_PROTO_RLPP
        case RTK_PROTO_ID_RLPP:
			rlpp_up(pBuf);
			break;
#endif

		default:
			DBG_LWPS_ETH_PRINTF(("%s(): %u - rtk_proto_id: %u\n", __FILE__, (uint16)__LINE__, (uint16)rtk_proto_id));
			break;
        }
	    break;
#endif /* end of LWPS_REALTEK */

    default:
        DBG_LWPS_ETH_PRINTF(("%s():%d - ethertype (%04X) is not supported!\n", __FILE__, (uint16)__LINE__, pEthHdr->ether_type));
        return LWPS_ERR_L2_NDEF;
    }

    return LWPS_OK;
}


#if LWPS_IP
// Ethernet Down Handle IP: Prepares IP packet for transmission by resolving destination MAC via ARP
// Determines if packet is broadcast, routed to gateway, or local, then finds corresponding ARP entry
static int32 eth_down_handle_ip(pktBuf_t *pBuf)
{
    eth_hdr_t  xdata *pEthHdr;
    eth_addr_t xdata DstEthAddr;
    ip_hdr_t   xdata *pIpHdr;
    ip_addr_t  xdata DstIpAddr;

    DBG_LWPS_ETH_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

#if LWPS_ETHARP

    pIpHdr = (ip_hdr_t *)pBuf->l3;
    /* Broadcast packet? */
    if ( (pIpHdr->dst_ip[0] == 255) &&
         (pIpHdr->dst_ip[1] == 255) &&
         (pIpHdr->dst_ip[2] == 255) &&
         (pIpHdr->dst_ip[3] == 255))
    {
        IPADDR_COPY(DstIpAddr.addr, pIpHdr->dst_ip);
    }
    /* Routing: to default gateway? */
    else if (  (pIpHdr->dst_ip[0] & netmask[0]) != (this_ip[0] & netmask[0]) ||
          (pIpHdr->dst_ip[1] & netmask[1]) != (this_ip[1] & netmask[1]) ||
          (pIpHdr->dst_ip[2] & netmask[2]) != (this_ip[2] & netmask[2]) ||
          (pIpHdr->dst_ip[3] & netmask[3]) != (this_ip[3] & netmask[3])    )
    {
        /* DIP and MyIP are not in the same subnet */
        IPADDR_COPY(DstIpAddr.addr, default_gateway.addr);
        gw_mac_need = 1;
    }
    else
    {
        IPADDR_COPY(DstIpAddr.addr, pIpHdr->dst_ip);
    }

    DBG_LWPS_ETH_PRINTF(("DstIpAddr is %d.%d.%d.%d \n", \
        (uint16)DstIpAddr.addr[0], \
        (uint16)DstIpAddr.addr[1], \
        (uint16)DstIpAddr.addr[2], \
        (uint16)DstIpAddr.addr[3]));

    /* Find ARP entry */
    if (LWPS_OK != etharp_entry_find(&DstIpAddr, &DstEthAddr))
    {
        /* [FIXME] send arp request */
        DBG_LWPS_ETH_PRINTF(("%s():%d - ETHARP not found (drop packet)!\n", __FILE__, (uint16)__LINE__));

        return LWPS_ERR;
    }

    /* Extend Buffer */
    pBuf->l2 = pBuf->l3 - sizeof(eth_hdr_t);
    pBuf->size += sizeof(eth_hdr_t);
    pEthHdr = (eth_hdr_t *)pBuf->l2;

    /* Ethernet Header Setting */
    ETHADDR_COPY(pEthHdr->dst_mac.addr, DstEthAddr.addr);
    ETHADDR_COPY(pEthHdr->src_mac.addr, this_mac);
    pEthHdr->ether_type = ETH_TYPE_IP;

#endif /* end of LWPS_ETHARP */
    //eth_hdr_dump(pEthHdr);

    return LWPS_OK;
}
#endif  /* LWPS_IP */

// Ethernet Down: Prepares packets for transmission by adding Ethernet headers and VLAN tags
// Handles IP packet routing, VLAN insertion, and padding for minimum frame size
int32 eth_down(pktBuf_t *pBuf)
{
	//tcp_hdr_t xdata *pTcpHdr_Xmit = pBuf->l4;
	//ip_hdr_t xdata *pIpHdr_Xmit = pBuf->l3;
	uint16 vid;
	eth_hdr_t  xdata *pEthHdr;
    uint16 i;
    uint16 padding;
    sal_enable_t vlanEnable;
    //uint8 protocolType;
    DBG_LWPS_ETH_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));

    /* Check parameter */
    if (pBuf == NULL)
    {
        return LWPS_ERR_NULLPOINT;
    }


    /* Insert Ethernet Header into the packet(pBuf) */
    switch (pBuf->l3fmt)
    {
    case L3FMT_NONE:
        switch (pBuf->l2fmt)
        {
        case L2FMT_NONE:
//            DBG_LWPS_ETH_PRINTF(("%s():%d - l2fmt (%d) and l3fmt (%d) is not handled!\n", __FILE__, (uint16)__LINE__, pBuf->l2fmt, pBuf->l3fmt));
            return LWPS_ERR;
            break;

        case L2FMT_ETHARP:
        case L2FMT_ETH:
            break;

        default:
//            DBG_LWPS_ETH_PRINTF(("%s():%d - l2fmt (%d) is not supported!\n", __FILE__, (uint16)__LINE__, pBuf->l2fmt));
            return LWPS_ERR;
            break;
        }
        break;

#if  LWPS_IP
    case L3FMT_IP:
        if (LWPS_OK != eth_down_handle_ip(pBuf))
        {
            DBG_LWPS_ETH_PRINTF(("%s():%d - eth_down_handle_ip failed!\n", __FILE__, (uint16)__LINE__));
            return LWPS_ERR;
        }
        
        break;
#endif  /* LWPS_IP */

    default:
        DBG_LWPS_ETH_PRINTF(("%s():%d - l3fmt (%d) is not supported!\n", __FILE__, (uint16)__LINE__, pBuf->l3fmt));
        return LWPS_ERR;
        break;
    }

        pBuf->pkt_head = pBuf->l2;
        pEthHdr = (eth_hdr_t *)pBuf->l2;
        //protocolType = *(pBuf->l2+ sizeof(eth_hdr_t));
        sal_vlan_enable_get(&vlanEnable);
    
        if ((0 == sal_nic_rxVid_get(&vid)) && vlanEnable && (pEthHdr->ether_type != 0x8899))
        {
            if(pBuf->l3fmt == L3FMT_IP) /* fix TCP WEB packet with size < 60 will cause FCS error */
            {
                if(pBuf->size < 60)
                    padding = 60 - (pBuf->size);
                else
                    padding = 0;
                        
                for( i = 0; i < MAC_ADDR_LEN * 2; i++)
                {
                    *(pBuf->l2 + i - 4 - padding) = *(pBuf->l2 + i);
                }
                if(padding != 0)
                {
                    for (i = ETH_HWADDR_LEN*2;i < (pBuf->size) ; i++)
                    {
                        *(pBuf->l2 + i - padding) = *(pBuf->l2 + i);  
                    }
                }
                    
                pBuf->l2 -= (4 + padding);
                pBuf->pkt_head = pBuf->l2;
                pEthHdr = (eth_hdr_t *)pBuf->l2;
                pBuf->size += (4 + padding);
        
                pEthHdr = (eth_hdr_t *)pBuf->pkt_head;
                pEthHdr->ether_type = ETH_TYPE_VLAN;
                pBuf->pkt_vlantag = pBuf->l2 + sizeof(struct eth_addr_s)*2;
                *(pBuf->pkt_vlantag + 2) = (uint8)(vid >> 8);
                *(pBuf->pkt_vlantag + 3) = (uint8)(vid&0xff);            

            }
            else
            {
                /* Fix arp reply packet FCS error */
                for( i = 0; i < MAC_ADDR_LEN * 2; i++)
                {
                    *(pBuf->l2 + i - 4) = *(pBuf->l2 + i);
                }
               
                pBuf->l2 -= 4;
                pBuf->pkt_head = pBuf->l2;
                //pEthHdr = (eth_hdr_t *)pBuf->l2;
                pBuf->size += 4;
        
                //pEthHdr = (eth_hdr_t *)pBuf->pkt_head;
                //pEthHdr->ether_type = ETH_TYPE_VLAN;
                pBuf->pkt_vlantag = pBuf->l2 + sizeof(struct eth_addr_s)*2;
                *(pBuf->pkt_vlantag + 0) = (uint8)(0x81);
                *(pBuf->pkt_vlantag + 1) = (uint8)(0x00);
                *(pBuf->pkt_vlantag + 2) = (uint8)(vid >> 8);
                *(pBuf->pkt_vlantag + 3) = (uint8)(vid&0xff);            
            }

    /*OSAL_PRINTF("\n");
        OSAL_PRINTF("Out:\n");
        for(i = 0; i < pBuf->size; i++)
        {
            if(i%16 ==0)
                OSAL_PRINTF("\n");
            OSAL_PRINTF("%-2.2X ",(uint16)(*(pBuf->l2 + i)));
            
    }*/
  //sal_nic_rxVid_clear();      
    }

    lwps_pbuf_tx(pBuf);

    return LWPS_OK;
}

#endif /* end of LWPS_ENABLE */