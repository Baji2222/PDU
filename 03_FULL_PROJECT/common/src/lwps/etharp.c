#pragma STRING (far)
/*
 * ARP (Address Resolution Protocol) of Light-weight Protocol Stack
 */

#include <local/lwps_config.h>
#include <local/snmp_config.h>

#if LWPS_ETHARP

/*
 * Include Files
 */

#include <lwps/lwps.h>
#include <stdio.h>

#if LWPS_SNMP_TRAP_ENABLE
#include <salsnmp.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration 
 */
static eth_addr_t xdata eth_addr_empty = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
static eth_addr_t xdata eth_addr_bcast = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
static ip_addr_t xdata ip_addr_bcast = { { 255, 255, 255, 255 } };
static volatile etharp_entry_t xdata arp_table[LWPS_ETHARP_ARP_TABLE_SIZE];
int gw_mac_need = 0;


/*
 * Macro Definition
 */
 
 
/*
 * Function Declaration
 */
#if DBG_LWPS_ETHARP_ENABLE
static void etharp_hdr_dump(etharp_hdr_t *pEthArpHdr) large
{
    OSAL_PRINTF("-- [ETH ARP HDR dump] -------------------------\n");
    OSAL_PRINTF("------------\n");
    OSAL_PRINTF("    HW type: %04X \n", pEthArpHdr->hw_type);
    OSAL_PRINTF(" Proto Type: %04X \n", pEthArpHdr->proto_type);
    OSAL_PRINTF("      H-Len: %02X \n", (uint16)pEthArpHdr->hlen);
    OSAL_PRINTF("      P-Len: %02X \n", (uint16)pEthArpHdr->plen);
    OSAL_PRINTF("    OP Code: %04X \n", pEthArpHdr->opcode);
    OSAL_PRINTF(" Sender MAC: %02X:%02X:%02X:%02X:%02X:%02X \n",
        (uint16)pEthArpHdr->sender_hwaddr.addr[0],
        (uint16)pEthArpHdr->sender_hwaddr.addr[1],
        (uint16)pEthArpHdr->sender_hwaddr.addr[2],
        (uint16)pEthArpHdr->sender_hwaddr.addr[3],
        (uint16)pEthArpHdr->sender_hwaddr.addr[4],
        (uint16)pEthArpHdr->sender_hwaddr.addr[5]
        );
    OSAL_PRINTF("  Sender IP: %d.%d.%d.%d \n",
        (uint16)pEthArpHdr->sender_ipaddr.addr[0],
        (uint16)pEthArpHdr->sender_ipaddr.addr[1],
        (uint16)pEthArpHdr->sender_ipaddr.addr[2],
        (uint16)pEthArpHdr->sender_ipaddr.addr[3]
        );
    OSAL_PRINTF(" Target MAC: %02X:%02X:%02X:%02X:%02X:%02X \n",
        (uint16)pEthArpHdr->target_hwaddr.addr[0],
        (uint16)pEthArpHdr->target_hwaddr.addr[1],
        (uint16)pEthArpHdr->target_hwaddr.addr[2],
        (uint16)pEthArpHdr->target_hwaddr.addr[3],
        (uint16)pEthArpHdr->target_hwaddr.addr[4],
        (uint16)pEthArpHdr->target_hwaddr.addr[5]
        );
    OSAL_PRINTF("  Target IP: %d.%d.%d.%d \n",
        (uint16)pEthArpHdr->target_ipaddr.addr[0],
        (uint16)pEthArpHdr->target_ipaddr.addr[1],
        (uint16)pEthArpHdr->target_ipaddr.addr[2],
        (uint16)pEthArpHdr->target_ipaddr.addr[3]
        );
    OSAL_PRINTF("-----------------------------------------------\n");
}
#endif /* end of DBG_LWPS_ETHARP_ENABLE */

int32 etharp_init(void) large
{
    uint8 xdata i;

    DBG_LWPS_ETHARP_PRINTF("%s(): %d\n", __FILE__, (uint16)__LINE__);

    gw_mac_need = 1;   /* try to ask the ARP request once when initialize */

    /* Clear ARP entries */
    for (i=0; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
        arp_table[i].state = ETHARP_STATE_EMPTY;
        arp_table[i].ctime = 0;
    }

    return LWPS_OK;
}

void etharp_timer(void) large
{
    uint8 i;
	static uint8 gateway_timeout = LWPS_ETHARP_GATEWAY_QUERY_TIME;
	eth_addr_t GatewayEthAddr;
#if LWPS_SNMP_TRAP_ENABLE
    ip_addr_t xdata snmp_trap_dip;
	sal_ipaddr_t     sal_ip;
    eth_addr_t snmpTrapEthAddr;
#endif /* end of LWPS_SNMP_TRAP_ENABLE */
    static uint8 cnt = 0;    

    /*DBG_LWPS_ETHARP_PRINTF("%s(): %d\n", __FILE__, (uint16)__LINE__);*/

    cnt++;

    /* Remove expired entries from the ARP table */
    for (i=0; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
        if( ETHARP_STATE_EMPTY!=arp_table[i].state )
        {
        	if(arp_table[i].ctime < LWPS_ETHARP_ARP_MAXAGE)
            	arp_table[i].ctime++;
        }
        else
        {
            continue;
        }

        /* expired entry? */
        if ((arp_table[i].state == ETHARP_STATE_STABLE) && \
            (arp_table[i].ctime >= LWPS_ETHARP_ARP_MAXAGE))
        {
            DBG_LWPS_ETHARP_PRINTF("%s(): %d - expired stable entry\n", __FILE__, (uint16)__LINE__);
            arp_table[i].state = ETHARP_STATE_EXPIRED;
        }
        else if (arp_table[i].state == ETHARP_STATE_PENDING)
        {
            /* entry unresolved/pending for too long? */
            if (arp_table[i].ctime >= LWPS_ETHARP_ARP_MAXPENDING)
            {
                DBG_LWPS_ETHARP_PRINTF("%s(): %d - expired pending entry\n", __FILE__, (uint16)__LINE__);
                arp_table[i].state = ETHARP_STATE_EXPIRED;
            }
        }

        /* clean up entries that have just been expired */
        if (arp_table[i].state == ETHARP_STATE_EXPIRED)
        {
            /* recycle entry for re-use */
            arp_table[i].state = ETHARP_STATE_EMPTY;
            arp_table[i].ctime = 0;
        }
    }

    if(cnt == 60)
    {
        cnt = 0;
        if(!(default_gateway.addr[0]==0 && 
			default_gateway.addr[1]==0 &&
			default_gateway.addr[2]==0 && 
			default_gateway.addr[3]==0))
        {
            etharp_request(default_gateway);
        }
    }

	if ( gateway_timeout>=LWPS_ETHARP_GATEWAY_QUERY_TIME )
	{
		/*Stop ARP request if ip=0.0.0.0*/
		/* Check Gateway ARP entry (Periodically update gateway's MAC) */
		if ( !(default_gateway.addr[0]==0 && 
				default_gateway.addr[1]==0 &&
				default_gateway.addr[2]==0 && 
				default_gateway.addr[3]==0) && 
				(gw_mac_need) &&
				(LWPS_OK != etharp_entry_find( &default_gateway, &GatewayEthAddr)))
		{
            gw_mac_need = 0;    /* avoid to enter again */
            
			/* send arp request to gateway */
			etharp_request(default_gateway);
		}
#if LWPS_SNMP_TRAP_ENABLE
        sal_snmp_trapIp_get(&sal_ip);
        snmp_trap_dip.addr[0] = (uint8)((sal_ip & 0xFF000000) >> 24);
        snmp_trap_dip.addr[1] = (uint8)((sal_ip & 0xFF0000)   >> 16);            
        snmp_trap_dip.addr[2] = (uint8)((sal_ip & 0xFF00)     >> 8);                    
        snmp_trap_dip.addr[3] = (uint8) (sal_ip & 0xFF);

        /* Check SNMP ARP entry (Periodically update SNMP trap dip's MAC) */
        if (  (!IS_IPADDR_EQUAL(default_gateway.addr, snmp_trap_dip.addr))        &&
              ((snmp_trap_dip.addr[0] & netmask[0]) == (this_ip[0] & netmask[0])) &&
              ((snmp_trap_dip.addr[1] & netmask[1]) == (this_ip[1] & netmask[1])) &&
              ((snmp_trap_dip.addr[2] & netmask[2]) == (this_ip[2] & netmask[2])) &&
              ((snmp_trap_dip.addr[3] & netmask[3]) == (this_ip[3] & netmask[3]))    )
        {
            /* DIP and MyIP are in the same subnet */
            if ( (snmp_trap_dip.addr[0]!=0 && snmp_trap_dip.addr[1]!=0  && \
		          snmp_trap_dip.addr[2]!=0 && snmp_trap_dip.addr[3]!=0) && \
		        LWPS_OK != etharp_entry_find( &snmp_trap_dip, &snmpTrapEthAddr) )
		    {
			    /* send arp request to gateway */
			    etharp_request(snmp_trap_dip);
		    }
        }
		
#endif /* end of LWPS_SNMP_TRAP_ENABLE */

		gateway_timeout = 0;
	}
	gateway_timeout++;
}

int32 etharp_entry_update(ip_addr_t *pIpAddr, eth_addr_t *pEthAddr) large
{
    uint8 xdata i;
    uint8 xdata max_ctime_entry_id;
    uint8 xdata max_ctime_val;
    int16 xdata empty_entry_id;

    /*DBG_LWPS_ETHARP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);*/

    if ((pIpAddr == NULL) || (pEthAddr == NULL))
    {
        OSAL_PRINTF("%s():%d - pIpAddr or pEthAddr is NULL\n", __FILE__, (uint16)__LINE__);
        return LWPS_ERR_NULLPOINT;
    }

    /* scan the ARP table */
    empty_entry_id = -1;
    max_ctime_entry_id = 0;
    max_ctime_val = arp_table[0].ctime;
    for (i=0; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
    	/*reserved entry 0 for Gateway ARP*/
        /*if (arp_table[i].state == ETHARP_STATE_EMPTY)*/
        if (arp_table[i].state == ETHARP_STATE_EMPTY && i != 0)
        {
        	if ( -1==empty_entry_id ){
            	empty_entry_id = i;
        	}
            continue;
        }

        if (arp_table[i].ctime > max_ctime_val)
        {
            max_ctime_entry_id = i;
            max_ctime_val = arp_table[i].ctime;
        }

        if (IS_IPADDR_EQUAL(arp_table[i].ipaddr.addr, pIpAddr->addr) &&
            IS_ETHADDR_EQUAL(arp_table[i].ethaddr.addr, pEthAddr->addr))
        {
            DBG_LWPS_ETHARP_PRINTF("%s():%d - update ARP entry (already exist)\n", __FILE__, (uint16)__LINE__);
            arp_table[i].ctime = 0; /* recharge */
			/*fixed with same IP/MAC can't return to STABLE*/
			arp_table[i].state = ETHARP_STATE_PENDING; 
            return LWPS_OK;
        }
    }

    /* Not found -> save entry */
    DBG_LWPS_ETHARP_PRINTF("%s():%d - update ARP entry (add or replace)\n", __FILE__, (uint16)__LINE__);
	if ( -1==empty_entry_id )
	{
		empty_entry_id = max_ctime_entry_id;
	}

	/*forced gateway MAC to ARP entry 0 and will replace ARP entry 0*/
	if ( default_gateway.addr[0] == pIpAddr->addr[0] && 
					default_gateway.addr[1]== pIpAddr->addr[1] &&
					default_gateway.addr[2]== pIpAddr->addr[2] && 
					default_gateway.addr[3]== pIpAddr->addr[3] )
	{
		empty_entry_id = 0;
	}
	else if(empty_entry_id == 0)
	{
		empty_entry_id ++;
	}

	IPADDR_COPY(arp_table[empty_entry_id].ipaddr.addr, pIpAddr->addr);
    ETHADDR_COPY(arp_table[empty_entry_id].ethaddr.addr, pEthAddr->addr);
    arp_table[empty_entry_id].state = ETHARP_STATE_PENDING;
    arp_table[empty_entry_id].ctime = 0;

    return LWPS_OK;
}

int32 etharp_entry_find(ip_addr_t *pIpAddr, eth_addr_t *pEthAddr) large
{
    uint8 i;

    //OSAL_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    /* Scan ARP entries */
    for (i=0; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
        if ((arp_table[i].state != ETHARP_STATE_EMPTY) &&
            (IS_IPADDR_EQUAL(arp_table[i].ipaddr.addr, pIpAddr->addr)))
        {
            /* Update entry */
            arp_table[i].state = ETHARP_STATE_STABLE;

            /* Output */
            ETHADDR_COPY(pEthAddr->addr, arp_table[i].ethaddr.addr);

            return LWPS_OK;
        }
    }

    /* Broadcast? */
    if (IS_IPADDR_EQUAL(ip_addr_bcast.addr, pIpAddr->addr))
    {
        ETHADDR_COPY(pEthAddr->addr, eth_addr_bcast.addr);
        return LWPS_OK;
    }

    return LWPS_ERR;
}

static int32 etharp_up_handle_arp_request(pktBuf_t *pBuf) large
{
    etharp_hdr_t xdata *pEthArpHdr = (etharp_hdr_t *)pBuf->l3;
    etharp_hdr_t xdata *pEthArpHdr_Reply;
	eth_addr_t   tmp_target_hwaddr;
	ip_addr_t    tmp_target_ipaddr;
	
    DBG_LWPS_ETHARP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

    if ((pEthArpHdr->hw_type != ETHARP_HWTYPE_ETHERNET) || \
        (pEthArpHdr->proto_type != ETHARP_PROTOTYPE_IP))
    {
        OSAL_PRINTF("%s():%d ETHARP HW (%d) Proto (%d) is NOT supported!\n", \
            __FILE__, (uint16)__LINE__, pEthArpHdr->hw_type, pEthArpHdr->proto_type);

        return LWPS_OK;
    }
    
    pEthArpHdr_Reply = (etharp_hdr_t *)pBuf->l3;
    ETHADDR_COPY(tmp_target_hwaddr.addr,      pEthArpHdr_Reply->sender_hwaddr.addr);
	IPADDR_COPY (tmp_target_ipaddr.addr,      pEthArpHdr_Reply->sender_ipaddr.addr);
   	pEthArpHdr_Reply = (etharp_hdr_t *)(pBuf->l2 + sizeof(eth_hdr_t));

   	pBuf->size = pBuf->size - 4;


    /* Ethernet Header Setting */
    ETHADDR_COPY( (uint8)pBuf->l2, (pBuf->l2 + sizeof(eth_addr_t)) );
    ETHADDR_COPY( (uint8)(pBuf->l2 + sizeof(eth_addr_t)), this_mac); /* [FIXME] */
    (uint16)(pBuf->l2 + sizeof(eth_addr_t)*2) = ETH_TYPE_ARP;
    
    /* ETH ARP HDR setting */
    pEthArpHdr_Reply->hw_type    = ETHARP_HWTYPE_ETHERNET;
    pEthArpHdr_Reply->proto_type = ETHARP_PROTOTYPE_IP;
    pEthArpHdr_Reply->hlen       = ETH_HWADDR_LEN;
    pEthArpHdr_Reply->plen       = IP_ADDR_LEN;
    pEthArpHdr_Reply->opcode     = ETHARP_OPCODE_ARP_REPLY;
    ETHADDR_COPY(pEthArpHdr_Reply->target_hwaddr.addr,      tmp_target_hwaddr.addr);
    IPADDR_COPY (pEthArpHdr_Reply->target_ipaddr.addr,      tmp_target_ipaddr.addr);
    ETHADDR_COPY(pEthArpHdr_Reply->sender_hwaddr.addr, this_mac);
    IPADDR_COPY (pEthArpHdr_Reply->sender_ipaddr.addr, this_ip);
    
    pBuf->l3fmt = L3FMT_NONE;
    
    /*etharp_hdr_dump(pEthArpHdr_Reply);*/

    if (LWPS_OK != eth_down(pBuf))
    {
        OSAL_PRINTF("%s():%d - eth_output failed!\n", __FILE__, (uint16)__LINE__);
    }

    return LWPS_OK;
}

int32 etharp_up(pktBuf_t *pBuf) large
{
    etharp_hdr_t xdata *pEthArpHdr = (etharp_hdr_t *)pBuf->l3;

    DBG_LWPS_ETHARP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

#if 0
    /* Check IP address */
    if (!IS_IPADDR_EQUAL(pEthArpHdr->target_ipaddr.addr, this_ip))
    {
        /* Drop packet and return directly */
        OSAL_PRINTF("%s():%d - TargetIP != MyIP --> Drop!\n", __FILE__, (uint16)__LINE__);
        return LWPS_OK;
    }
#endif

    switch (pEthArpHdr->opcode) {
    case ETHARP_OPCODE_ARP_REQUEST:
        /* [FIXME] it does NOT check the validity of sender's MAC/IP addr */
        etharp_entry_update(&pEthArpHdr->sender_ipaddr, &pEthArpHdr->sender_hwaddr);
		if ( IS_IPADDR_EQUAL(pEthArpHdr->target_ipaddr.addr, this_ip) ){
        	if (LWPS_OK != etharp_up_handle_arp_request(pBuf)) {
            	OSAL_PRINTF("%s():%d - etharp_up_handle_arp_request failed!\n", __FILE__, (uint16)__LINE__);
        	}
        }
        break;

    case ETHARP_OPCODE_ARP_REPLY:
        /*OSAL_PRINTF("%s():%d - Receive ARP reply!\n", __FILE__, (uint16)__LINE__);*/
        etharp_entry_update(&pEthArpHdr->sender_ipaddr, &pEthArpHdr->sender_hwaddr);
        break;
        
    default:
        OSAL_PRINTF("%s():%d - ETHARP opcode (%d) is not supported!\n", __FILE__, (uint16)__LINE__, pEthArpHdr->opcode);
        break;
    }

    return LWPS_OK;
}

/* ARP related API */
int32 etharp_check(ip_addr_t ip) large
{
    uint8 xdata i;

    /*DBG_LWPS_ETHARP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);*/

    if (  (ip.addr[0] & netmask[0]) != (this_ip[0] & netmask[0]) ||
          (ip.addr[1] & netmask[1]) != (this_ip[1] & netmask[1]) ||
          (ip.addr[2] & netmask[2]) != (this_ip[2] & netmask[2]) ||
          (ip.addr[3] & netmask[3]) != (this_ip[3] & netmask[3])    )
    {
        return LWPS_OK;
    }

    /* Scan ARP entries */
    for (i=0; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
        if ((arp_table[i].state != ETHARP_STATE_EMPTY) &&
            (IS_IPADDR_EQUAL(arp_table[i].ipaddr.addr, ip.addr)))
        {
            return LWPS_OK;
        }
    }

    return LWPS_ERR_NOTEXIST;
}

int32 etharp_request(ip_addr_t ip) large
{
	uint8 xdata i;
	pktBuf_t *pBuf_Request;
	etharp_hdr_t *pEthArpHdr_Request;
    DBG_LWPS_ETHARP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);

#if 1
    /* Scan ARP entries */
    for (i=1; i<LWPS_ETHARP_ARP_TABLE_SIZE; i++)
    {
        if ((arp_table[i].state != ETHARP_STATE_EMPTY) &&
            (IS_IPADDR_EQUAL(arp_table[i].ipaddr.addr, ip.addr)))
        {
            return LWPS_OK;
        }
    }
#endif

    /* Send ARP request packet if this entry is not exist */
    /*
    if (LWPS_OK != lwps_pbuf_alloc(sizeof(struct etharp_hdr_s), 0, &pBuf_Request))
    {
        OSAL_PRINTF("%s():%d - pBuf alloc failed!\n", __FILE__, (uint16)__LINE__);
        return LWPS_ERR;
    }
    */

	pBuf_Request = &pktbuf;
	pEthArpHdr_Request = (etharp_hdr_t *)(pkt_head + sizeof(eth_hdr_t));
	pBuf_Request->size = sizeof(etharp_hdr_t)+ sizeof(eth_hdr_t);
	pBuf_Request->l2 = (uint8*)pkt_head;


	/* Ethernet Header Setting */
    ETHADDR_COPY( pBuf_Request->l2, eth_addr_bcast.addr );
    ETHADDR_COPY( (pBuf_Request->l2 + sizeof(eth_addr_t)), this_mac); /* [FIXME] */
    
    (uint16)(pBuf_Request->l2 + sizeof(eth_addr_t)*2) = ETH_TYPE_ARP;	

    /* ETH ARP HDR setting */
    pEthArpHdr_Request->hw_type = ETHARP_HWTYPE_ETHERNET;
    pEthArpHdr_Request->proto_type = ETHARP_PROTOTYPE_IP;
    pEthArpHdr_Request->hlen = ETH_HWADDR_LEN;
    pEthArpHdr_Request->plen = IP_ADDR_LEN;
    pEthArpHdr_Request->opcode = ETHARP_OPCODE_ARP_REQUEST;
    ETHADDR_COPY((pEthArpHdr_Request->sender_hwaddr.addr), this_mac);
    /*IPADDR_COPY (pEthArpHdr_Request->sender_ipaddr.addr, this_ip);*/
    pEthArpHdr_Request->sender_ipaddr.addr[0] = this_ip[0];
	pEthArpHdr_Request->sender_ipaddr.addr[1] = this_ip[1];
	pEthArpHdr_Request->sender_ipaddr.addr[2] = this_ip[2];
	pEthArpHdr_Request->sender_ipaddr.addr[3] = this_ip[3];
    pEthArpHdr_Request->target_hwaddr = eth_addr_empty;
    IPADDR_COPY ((pEthArpHdr_Request->target_ipaddr.addr), ip.addr);
#if 0
	etharp_hdr_dump(pEthArpHdr_Request);
#endif
    pBuf_Request->l3fmt = L3FMT_NONE;
    pBuf_Request->l2fmt = L2FMT_ETHARP;
    if (LWPS_OK != eth_down(pBuf_Request))
    {
        OSAL_PRINTF("%s():%d - eth_output failed!\n", __FILE__, (uint16)__LINE__);
        //lwps_pbuf_free(pBuf_Request);
        return LWPS_ERR;
    }

    return LWPS_OK;
}

#endif /* end of LWPS_ETHARP */

