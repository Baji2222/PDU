#pragma STRING (far)
#include <local/lwps_config.h>

#if LWPS_IGMP
#include <lwps/lwps.h>
#include <common.h>
#include <lwps/igmp_snooping.h>
#include <lwps/igmp_group_db.h>
#include <saligmp.h>
#include <salnic.h>
#include <salvlan.h>
#include <salportmask.h>
#include <salloop.h>
#ifdef RTL8367C
#include <vlan.h>
#else
#include <rtk_api_ext.h>
#endif

// Remove RLPP Block Port: Removes ports in RLPP block or disable state from port mask
// Filters out ports that are blocked or disabled by RLPP loop protection
void remove_rlpp_block_port(portmask_type *pmask)
{
    uint16 port, loopStatus;
    sal_lPortmask_t         allPortMask;

    sal_lPortMask_addAll(&allPortMask);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&allPortMask, port))
        {
            continue;
        }

        sal_loop_port_state_get(port, &loopStatus);
        if((loopStatus == SAL_RLPP_DISABLE_STATE) || (loopStatus == SAL_RLPP_BLOCKING_STATE))
        {
            allPortMask.bits[0] &= ~(1UL << port);
        }
    }
    *pmask = allPortMask.bits[0];
}

#if LWPS_IGMP_TRUNK_SUPPORT

#define IGMP_UPDATE_PM(trunk_pm, target_pm, source_pm)   \ 
    do{                                                  \
        (trunk_pm) = check_trunk_portmask(source_pm);    \
        if( (trunk_pm) )                                 \
        {                                                \
            (target_pm) &= ~(trunk_pm);                  \
        }                                                \
        else                                             \
        {                                                \
            (target_pm) &= ~(source_pm);                 \
        }                                                \
}while(0)

#define IGMP_SEND_PKT(trunk_pm, source_pm, pBuf)         \
    do{                                                  \
        (trunk_pm) = check_trunk_portmask(source_pm);    \
        if( (trunk_pm) )                                 \
        {                                                \
            RTK_SAL_SEND_PKT(pBuf, (~trunk_pm) );        \
        }                                                \
        else                                             \
        {                                                \
            RTK_SAL_SEND_PKT(pBuf, (~source_pm) );       \
        }                                                \
}while(0)


#else  /* else of LWPS_IGMP_TRUNK_SUPPORT */

#define IGMP_UPDATE_PM(trunk_pm, target_pm, source_pm)   \
    do{                                                  \
        (target_pm) &= ~(source_pm);                     \
}while(0)

#define IGMP_SEND_PKT(trunk_pm, source_pm, pBuf)         \
    do{                                                  \
        RTK_SAL_SEND_PKT(pBuf, (~source_pm) );           \
}while(0)

#endif /* end of else of LWPS_IGMP_TRUNK_SUPPORT */


#if CONFIG_IGMP_MLD_ENABLE

#define IGMP_MLD_ADD_GROUP_MEMBER(pBuf, pm, vid, mode)        \
do{                                                      \
    if( L4FMT_ICMPV6 == pBuf->l4fmt )                    \
    {                                                    \
        mld_group_add_groupMember(pBuf, pm, vid, mode)        \
    }                                                    \
    else                                                 \
    {                                                    \
        igmp_group_add_groupMember(pBuf, pm, vid, mode);      \
    }                                                    \
}while(0)

#else  /* else of CONFIG_IGMP_MLD_ENABLE */

#define IGMP_MLD_ADD_GROUP_MEMBER(pBuf, pm, vid, mode)        \
    igmp_group_add_groupMember(pBuf, pm, vid, mode)

#endif /* end of else of CONFIG_IGMP_MLD_ENABLE */


/*
 * Symbol Definition
 */

uint8 xdata _igmpStatus   = DISABLE;
static uint8 xdata st_vlan_leaky = DISABLE;
far const ip_addr_t dip_query = {{224, 0, 0, 1}};
far const ip_addr_t dip_leave = {{224, 0, 0, 2}};
far const ip_addr_t proxy_query = {{0, 0, 0, 0}};
#if CONFIG_IGMP_MLD_ENABLE
far const ipv6_addr_t V6_genQueryGrpAddr = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
far const ipv6_addr_t V6_doneAddr        = {{0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x02}};
far const ipv6_addr_t V6_genQueryAddr    = {{0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01}};
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

static uint8 xdata igmp_version[COMBO_PORT_NUM];


/*
 * Function Declaration
 */
#if DBG_LWPS_IGMP_ENABLE
// IGMP Header Dump: Prints formatted IGMP/IGMPv3 header fields for debugging
// Displays type, checksum, group address, and record information for IGMPv3 reports
static void igmp_hdr_dump(igmp_hdr_t *pIgmpHdr) large
{
    igmpv3Report_t *igmpv3_hdr = (igmpv3Report_t *)pIgmpHdr;
    groupRecord_t  *iter;
    ip_addr_t  *iter_ipaddr;
    uint16 i, j;

    if ( IGMPV3_TYPE_MEMBERSHIP_REPORT==pIgmpHdr->type )
    {
        DBG_LWPS_IGMP_PRINTF("-- [IGMPv3 Report HDR dump] ----------------------------\n");
           DBG_LWPS_IGMP_PRINTF("          Type: 0x%02X\n", (uint16)igmpv3_hdr->type);
        DBG_LWPS_IGMP_PRINTF("      Checksum: 0x%04X\n", (uint16)igmpv3_hdr->checksum);
        DBG_LWPS_IGMP_PRINTF("  numOfRecords: 0x%04X\n", (uint16)igmpv3_hdr->numOfRecords);
        iter = &igmpv3_hdr->recordList;
        for(i=0; i<igmpv3_hdr->numOfRecords; i++)
        {
            DBG_LWPS_IGMP_PRINTF("    RecType[%d]: 0x%02X\n", i, (uint16)iter[i].type);
            DBG_LWPS_IGMP_PRINTF("     auxLen[%d]: 0x%04X\n", i, (uint16)iter[i].auxLen);
            DBG_LWPS_IGMP_PRINTF("   numOfSrc[%d]: 0x%04X\n", i, (uint16)iter[i].numOfSrc);

            iter_ipaddr = &(iter[i].groupAddr);
            DBG_LWPS_IGMP_PRINTF("  groupAddr[%d]: %d.%d.%d.%d\n", i, \
                (uint16)iter_ipaddr->addr[0], (uint16)iter_ipaddr->addr[1],\
                (uint16)iter_ipaddr->addr[2], (uint16)iter_ipaddr->addr[3]);
            
            iter_ipaddr = &(iter[i].srcList);
            for(j=0; j<iter[i].numOfSrc; j++)
            {
                DBG_LWPS_IGMP_PRINTF("    srcAddr[%d]: %d.%d.%d.%d\n", i,      \
                    (uint16)iter_ipaddr->addr[0], (uint16)iter_ipaddr->addr[1],\
                    (uint16)iter_ipaddr->addr[2], (uint16)iter_ipaddr->addr[3]);
            }
        }
        DBG_LWPS_IGMP_PRINTF("-----------------------------------------------\n");
    }
    else if ( IGMPV3_TYPE_MEMBERSHIP_REPORT==pIgmpHdr->type )
    {
    }
    else
    {
        DBG_LWPS_IGMP_PRINTF("-- [IGMP HDR dump] ----------------------------\n");
           DBG_LWPS_IGMP_PRINTF("       Type: 0x%02X\n", (uint16)pIgmpHdr->type);
        DBG_LWPS_IGMP_PRINTF("   RespTime: 0x%02X (%d)\n", (uint16)pIgmpHdr->maxRespTime, (uint16)pIgmpHdr->maxRespTime);
        DBG_LWPS_IGMP_PRINTF("   Checksum: 0x%04X\n", (uint16)pIgmpHdr->checksum);
        DBG_LWPS_IGMP_PRINTF("  GroupAddr: 0x%08lX\n", pIgmpHdr->groupAddr);
        DBG_LWPS_IGMP_PRINTF("-----------------------------------------------\n");
    }
}
#endif /* end of DBG_LWPS_ICMP_ENABLE */

#if CONFIG_IGMP_MLD_ENABLE&&DBG_LWPS_MLD_ENABLE
// MLD Header Dump: Prints formatted MLD (Multicast Listener Discovery) header for debugging
// Displays type, code, checksum, response delay, and IPv6 group address
static void mld_hdr_dump(igmp_mld_hdr_t *pMldHdr) large
{
    DBG_LWPS_MLD_PRINTF("-- [MLD HDR dump] ----------------------------\n");
    DBG_LWPS_MLD_PRINTF("       Type: 0x%02bX (%d)\n", pMldHdr->type, (uint16)pMldHdr->type);
    DBG_LWPS_MLD_PRINTF("       Code: 0x%02bX\n", pMldHdr->cod);
    DBG_LWPS_MLD_PRINTF("   Checksum: 0x%04X\n",  pMldHdr->checksum);
    DBG_LWPS_MLD_PRINTF("  RespDelay: 0x%04X\n",  pMldHdr->maxRespDelay);
    DBG_LWPS_MLD_PRINTF("  GroupAddr: %02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX:%02bX%02bX\n", \
            pMldHdr->groupAddr.addr[0], pMldHdr->groupAddr.addr[1], pMldHdr->groupAddr.addr[2], pMldHdr->groupAddr.addr[3], \
            pMldHdr->groupAddr.addr[4], pMldHdr->groupAddr.addr[5], pMldHdr->groupAddr.addr[6], pMldHdr->groupAddr.addr[7], \
            pMldHdr->groupAddr.addr[8], pMldHdr->groupAddr.addr[9], pMldHdr->groupAddr.addr[10], pMldHdr->groupAddr.addr[11], \
            pMldHdr->groupAddr.addr[12], pMldHdr->groupAddr.addr[13], pMldHdr->groupAddr.addr[14], pMldHdr->groupAddr.addr[15]);
    DBG_LWPS_MLD_PRINTF("-----------------------------------------------\n");
}
#endif /* end of CONFIG_IGMP_MLD_ENABLE&&DBG_LWPS_MLD_ENABLE */

// Get VLAN Mask: Retrieves the port mask for a specific VLAN
// Reads VLAN configuration and returns member port mask
void _get_vlan_mask(portmask_type *pmask)
{
    uint16 vid;
#ifdef RTL8367C    
    rtk_vlan_cfg_t vlanCfg;
#else
    rtk_portmask_t  member_mask;
    rtk_portmask_t  untag_mask;
    rtk_fid_t       fid;
#endif
    
    sal_nic_rxVid_get(&vid);
    if(vid == SAL_NIC_NULL_VID)
    {
        *pmask = 0;
    }
    else
    {
#ifdef RTL8367C    
        memset(&vlanCfg, 0, sizeof(vlanCfg));
        rtk_vlan_get(vid, &vlanCfg);    
        *pmask = vlanCfg.mbr.bits[0];
#else
        rtk_vlan_get(vid, &member_mask, &untag_mask, &fid);
        *pmask = member_mask.bits[0];
#endif        
    }

}

// IGMP Initialize: Initializes IGMP snooping protocol and related databases
// Enables IGMP, initializes group database, and calls chip initialization
int32 igmp_init(void) large
{
    _igmpStatus = ENABLE;
    st_vlan_leaky = ENABLE;
	sal_igmp_chip_init();

    igmp_group_db_init();

#if 0 /* Vic Test */
	{
		int32 retval;
		uint8 i, j;
		ip_addr_t grupAddr;

		grupAddr.addr[0] = 225;
		grupAddr.addr[1] = 3;
		grupAddr.addr[2] = 3;
		grupAddr.addr[3] = 1;
		for (i=0; i<3; i++, grupAddr.addr[3]+=1){
			for (j=0; j<INTERFACE_NUMBER;j++){
				if( LWPS_ERR==(retval=igmp_group_add_groupMember(&grupAddr, (1<<j), 0)) ){
					break;
				}
			}
			/*if( LWPS_ERR==retval ){
				//printf("[%s][%d][i=%d]\n", __FILE__, (uint16)__LINE__, i);
				//printf("[%s][%d][j=%d]\n", __FILE__, (uint16)__LINE__, j);
				//printf("[%s][%d][Leave]\n", __FILE__, (uint16)__LINE__);
				//print_igmp_all(IGMP_PRINT_TYPE_GROUP_MEMBER);
				break;
			}*/
		}
	}
#endif

    return LWPS_OK;
}

// IGMP Up Handle Membership Query: Processes IGMP membership query messages
// Adds source port to query ports and forwards query to all ports in VLAN
static int32 igmp_up_handle_membership_query(pktBuf_t *pBuf) large
{
    sal_port_t    spa;
    portmask_type pm;
    igmp_hdr_t *pIgmpHdr = (igmp_hdr_t *)pBuf->l4;
    ip_hdr_t xdata *pIpHdr = (ip_hdr_t *)pBuf->l3;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
#if CONFIG_IGMP_MLD_ENABLE
    igmp_mld_hdr_t *pMldHdr = (igmp_mld_hdr_t *)pBuf->l4;
    ipv6_hdr_t *pIpv6Hdr = (ipv6_hdr_t *)pBuf->l3;
#endif /* end of CONFIG_IGMP_MLD_ENABLE */
    portmask_type pmask, rlppmask;
    uint16 vid;

    /* Get Source Port */
    sal_nic_rxPort_get((void*)pBuf, &spa);
    pm = 1UL << spa;

    sal_nic_rxVid_get(&vid);

#if CONFIG_IGMP_MLD_ENABLE
    if( L4FMT_ICMPV6 == pBuf->l4fmt )
    {
        if ( IGMP_MEMCMP(pMldHdr->groupAddr.addr, V6_genQueryGrpAddr.addr, IPV6_ADDR_LEN) )
        {
            /* Add Source Port to query pport */
            igmp_group_add_queryPort(pm);
        }
    }
    else   
#endif /* end of CONFIG_IGMP_MLD_ENABLE */
    {
        if ( !IS_IPADDR_EQUAL(pIpHdr->src_ip, proxy_query.addr) )
        {
            /* Add Source Port to query pport */
            igmp_group_add_queryPort(pm);
        }
    }
    pmask = 0;
    _get_vlan_mask(&pmask);
    pBuf->tx_dst_pmsk = 0;
    remove_rlpp_block_port(&rlppmask);
    pBuf->tx_dport_mode    = TX_DPORT_ASSIGN_PMSK;
    pBuf->tx_dst_pmsk      = (~pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
    pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
    pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
    pBuf->inner_tag = vid;
    sal_nic_tx(pBuf);

#if LWPS_IGMP_TRUNK_SUPPORT
    /* kill warning message */
    trunk_portmask = 0;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    
    return LWPS_OK;
}

// IGMPv1/v2 Up Handle Membership Report: Processes IGMPv1/v2 membership report messages
// Adds source port to group membership and forwards report to query ports
static int32 igmpv12_up_handle_membership_report(pktBuf_t *pBuf) large
{
    sal_port_t    spa;
    portmask_type pm, router_pm;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    uint16 i;
    portmask_type pmask, rlppmask;
    uint16 vid;
    
    /* Get Source Port */
	sal_nic_rxPort_get((void*)pBuf, &spa);
 	pm = 1UL << spa;
  
    sal_nic_rxVid_get(&vid);

    /* Add Source Port to specific group */
    if( L3FMT_IP== pBuf->l3fmt)
    {
        igmp_group_add_groupMember(pBuf, pm, vid, IGMP_ADDR_PBUF);
    }
#if CONFIG_IGMP_MLD_ENABLE
    else
    {
        mld_group_add_groupMember(pBuf, pm, vid, IGMP_ADDR_PBUF);
    }
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

    /* Send report to all query port */
    igmp_group_get_queryPort(&router_pm);

    IGMP_UPDATE_PM(trunk_portmask, router_pm, pm);
    pmask = 0;
    _get_vlan_mask(&pmask);
    pBuf->tx_dst_pmsk = 0;
    remove_rlpp_block_port(&rlppmask);
    if( 0UL!=router_pm )
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = (router_pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
         sal_nic_tx(pBuf);
    }
    else
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = 0;
        for(i = 0; i < SAL_PORT_ID_CPU; i++)
        {
            if(i != spa)
                pBuf->tx_dst_pmsk |= 1UL<< i;
        }
        pBuf->tx_dst_pmsk = (pBuf->tx_dst_pmsk)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
         sal_nic_tx(pBuf);
    }    
    return LWPS_OK;
}

// IGMPv2 Up Handle Membership Leave: Processes IGMPv2 leave group messages
// Removes source port from group and forwards leave to query ports
static int32 igmpv2_up_handle_membership_leave(pktBuf_t *pBuf) large
{
    sal_port_t    spa;
    portmask_type pm, router_pm;
    igmp_hdr_t *pIgmpHdr = (igmp_hdr_t *)pBuf->l4;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    uint16 i;
    portmask_type pmask, rlppmask;
    uint16 vid;

    /* Get Source Port */
    sal_nic_rxPort_get((void*)pBuf, &spa);
    pm = 1UL << spa;

    sal_nic_rxVid_get(&vid);

    /* Add Source Port to specific group */
    if( L3FMT_IP== pBuf->l3fmt)
    {
        igmp_group_del_groupMember(pBuf, pm, vid, IGMP_ADDR_PBUF);
    }
#if CONFIG_IGMP_MLD_ENABLE
    else
    {
        mld_group_del_groupMember(pBuf, pm, vid, IGMP_ADDR_PBUF);
    }
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

    /* Send report to all query port */
    igmp_group_get_queryPort(&router_pm);

    IGMP_UPDATE_PM(trunk_portmask, router_pm, pm);
    pmask = 0;
    _get_vlan_mask(&pmask);
    pBuf->tx_dst_pmsk = 0;
    remove_rlpp_block_port(&rlppmask);    
    if( 0UL!=router_pm )
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = (router_pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
        sal_nic_tx(pBuf);
    }
    else
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = 0;
        for(i = 0; i < SAL_PORT_ID_CPU; i++)
        {
            if(i != spa)
                pBuf->tx_dst_pmsk |= 1UL<< i;
        }
        pBuf->tx_dst_pmsk = (pBuf->tx_dst_pmsk)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
        sal_nic_tx(pBuf);
    }    
    return LWPS_OK;
}

// IGMPv3 Up Handle Membership Report: Processes IGMPv3 membership report messages
// Handles include/exclude modes, adds/removes group members, and forwards to query ports
static int32 igmpv3_up_handle_membership_report(pktBuf_t *pBuf) large
{
    sal_port_t    spa;
    portmask_type pm, router_pm;
    igmpv3Report_t *igmpv3_hdr = (igmpv3Report_t *)pBuf->l4;
    groupRecord_t  *iter;
    ip_addr_t  *iter_ipaddr;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    uint16 i;
    portmask_type pmask, rlppmask;
    uint16 vid;

    /* Get Source Port */
    sal_nic_rxPort_get((void*)pBuf, &spa);
    pm = 1UL << spa;    

    sal_nic_rxVid_get(&vid);

    iter = &igmpv3_hdr->recordList;
    for(i=0; i<igmpv3_hdr->numOfRecords; i++)
    {
        iter_ipaddr = &(iter[i].groupAddr);
        
        /* Add Source Port to specific group */
        if( (CHANGE_TO_EXCLUDE_MODE==iter[i].type || MODE_IS_EXCLUDE==iter[i].type) )
        {
            if( L3FMT_IP== pBuf->l3fmt)
            {
                igmp_group_add_groupMember(iter_ipaddr, pm, vid, IGMP_ADDR_GROUP_ADDR);
            }
        }
        else if( (CHANGE_TO_INCLUDE_MODE==iter[i].type) || \
                 (MODE_IS_INCLUDE==iter[i].type)             )
        {

            if ( 0==iter[i].numOfSrc )
            {
                /* Support include "empty sip" mode, Leave All */
                if( L3FMT_IP== pBuf->l3fmt)
                {
                    igmp_group_del_groupMember(iter_ipaddr, pm, vid, IGMP_ADDR_GROUP_ADDR);
                }
            }
            else
            {
                if( L3FMT_IP== pBuf->l3fmt)
                {
                    igmp_group_add_groupMember(iter_ipaddr, pm, vid, IGMP_ADDR_GROUP_ADDR);
                }
            }
        }
        /* not support
        else if( BLOCK_OLD_SOURCES==iter[i].type )
        {
        }
        else if( ALLOW_NEW_SOURCES==iter[i].type )
        {
        }*/
    }

    /* Send report to all query port */
    igmp_group_get_queryPort(&router_pm);

    IGMP_UPDATE_PM(trunk_portmask, router_pm, pm);
    pmask = 0;
    _get_vlan_mask(&pmask);
    pBuf->tx_dst_pmsk = 0;
    remove_rlpp_block_port(&rlppmask);
    if( 0UL!=router_pm )
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = (router_pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
        sal_nic_tx(pBuf);
    }
    else
    {
        pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
        pBuf->tx_dport_mode = TX_DPORT_ASSIGN_PMSK;
        pBuf->tx_dst_pmsk = 0;
        for(i = 0; i < SAL_PORT_ID_CPU; i++)
        {
            if(i != spa)
                pBuf->tx_dst_pmsk |= 1UL<< i;
        }
        pBuf->tx_dst_pmsk = (pBuf->tx_dst_pmsk)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
        pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
        pBuf->inner_tag = vid;
        sal_nic_tx(pBuf);
    }    
    return LWPS_OK;
}

#if CONFIG_IGMP_MLD_ENABLE
// MLD Up: Processes MLD (Multicast Listener Discovery) messages for IPv6
// Handles MLD queries, reports, and done messages for IPv6 multicast
int32 mld_up(pktBuf_t *pBuf) large
{
    sal_port_t     spa;
    igmp_mld_hdr_t *pmldHdr = (igmp_mld_hdr_t *)pBuf->l4;
    ipv6_hdr_t *pIpv6Hdr = (ipv6_hdr_t *)pBuf->l3;
    u8_t       *v6Addr;

    portmask_type pm;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    portmask_type pmask, rlppmask;
    uint16 vid;

    mld_hdr_dump(pmldHdr);

    v6Addr   = pIpv6Hdr->dst_ip;

    switch (pmldHdr->type)
    {
        case MLDV1_TYPE_MEMBERSHIP_QUERY:

            if( 0==IGMP_MEMCMP(pmldHdr->groupAddr.addr, V6_genQueryGrpAddr.addr, 16) )/* General Query */
            {
                if ( 0!=IGMP_MEMCMP(v6Addr, V6_genQueryAddr.addr, 16) )
                {
                    DBG_LWPS_IGMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
                    goto mld_up_exit;
                }
            }

            return igmp_up_handle_membership_query(pBuf);

        case MLDV1_TYPE_MEMBERSHIP_REPORT:

            DBG_LWPS_IGMP_PRINTF("%s():%d[MLDV1_TYPE_MEMBERSHIP_REPORT]\n", __FILE__, (uint16)__LINE__);
            
            return igmpv12_up_handle_membership_report(pBuf);
            break;

        case MLDV1_TYPE_MEMBERSHIP_DONE:

            if( 0!=IGMP_MEMCMP(v6Addr, V6_doneAddr.addr, 16) )
            {
                DBG_LWPS_IGMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
                goto mld_up_exit;
            }
            
            return igmpv2_up_handle_membership_leave(pBuf);
            break;

        default:
            DBG_LWPS_IGMP_PRINTF("mld_up() - Not support mld type 0x%bX \n", pmldHdr->type);
            break;
    }

mld_up_exit:
    
    sal_nic_rxPort_get((void*)pBuf, &spa);
    pm = 1UL << spa;    
    
    sal_nic_rxVid_get(&vid);
    pmask = 0;
    remove_rlpp_block_port(&rlppmask);
    _get_vlan_mask(&pmask);
    pBuf->tx_dst_pmsk = 0;
    pBuf->tx_dport_mode    = TX_DPORT_ASSIGN_PMSK;
    pBuf->tx_dst_pmsk      = (~pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
    pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
    pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
    pBuf->inner_tag = vid;
    sal_nic_tx(pBuf);

#if LWPS_IGMP_TRUNK_SUPPORT
    /* kill warning message */    
    trunk_portmask = 0;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */

    return LWPS_OK;
}
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

// IGMP Up: Main handler for incoming IGMP packets (v1, v2, v3)
// Dispatches queries, reports, and leaves to appropriate handlers based on type
int32 igmp_up(pktBuf_t *pBuf) large
{
    sal_port_t spa;
    igmp_hdr_t xdata *pIgmpHdr = (igmp_hdr_t *)pBuf->l4;
    ip_hdr_t xdata *pIpHdr = (ip_hdr_t *)pBuf->l3;
    uint16 pl_len;   /* payload length */
    portmask_type pm;
#if LWPS_IGMP_TRUNK_SUPPORT
    portmask_type trunk_portmask;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
	sal_enable_t enable;
    portmask_type pmask, rlppmask;
    uint16 vid;
#ifdef RTL8367C    
    rtk_vlan_cfg_t vlanCfg;
#else
    rtk_portmask_t  member_mask;
    rtk_portmask_t  untag_mask;
    rtk_fid_t       fid;    
#endif    

    sal_igmp_state_get(&enable);
    if(enable == DISABLE)
    {
        return LWPS_OK;
    }


    sal_nic_rxPort_get((void*)pBuf, &spa);

    sal_nic_rxVid_get(&vid);
#if 0    
    sal_vlan_entry_get(vid, &LPortMask, &LTagMask, &FId, &Msti);

    if(!((LPortMask.bits[0]) & (1 << spa)))
    {
        return LWPS_OK;
    }
#else    
#ifdef RTL8367C    
    memset(&vlanCfg, 0, sizeof(vlanCfg));
    rtk_vlan_get(vid, &vlanCfg);    

    if(!((vlanCfg.mbr.bits[0]) & (1UL << spa)))
    {
        return LWPS_OK;
    }
#else 
    rtk_vlan_get(vid, &member_mask, &untag_mask, &fid);
    if(!((member_mask.bits[0]) & (1UL << spa)))
    {
        return LWPS_OK;
    }    
#endif    
#endif
    //DBG_LWPS_IGMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
    igmp_hdr_dump(pIgmpHdr);

    if( 224==pIgmpHdr->groupAddr.addr[0] &&
          0==pIgmpHdr->groupAddr.addr[1] &&
          0==pIgmpHdr->groupAddr.addr[2])
    {
        DBG_LWPS_IGMP_PRINTF("%s():%d\n", __FILE__, (uint16)__LINE__);
        goto igmp_up_exit;
    }

    switch (pIgmpHdr->type)
    {
        case IGMP_TYPE_MEMBERSHIP_QUERY:
            pl_len = pIpHdr->length - (pIpHdr->hdr_len*4);
            
            if ( 8==pl_len && (!IS_IPADDR_EQUAL(pIpHdr->dst_ip, dip_query.addr)) )
            {
                break;
            }

            return igmp_up_handle_membership_query(pBuf);

        case IGMPV1_TYPE_MEMBERSHIP_REPORT:
        case IGMPV2_TYPE_MEMBERSHIP_REPORT:

            DBG_LWPS_IGMP_PRINTF("%s():%d[IGMPV12_TYPE_MEMBERSHIP_REPORT]\n", __FILE__, (uint16)__LINE__);
            
            return igmpv12_up_handle_membership_report(pBuf);
            break;

        case IGMPV2_TYPE_MEMBERSHIP_LEAVE:

            if ( !IS_IPADDR_EQUAL(pIpHdr->dst_ip, dip_leave.addr) )
            {
                break;
            }
            
            return igmpv2_up_handle_membership_leave(pBuf);
            break;


        case IGMPV3_TYPE_MEMBERSHIP_REPORT:
            return igmpv3_up_handle_membership_report(pBuf);
            break;

        default:
            DBG_LWPS_IGMP_PRINTF("igmp_up() - Not support igmp type 0x%bX \n", pIgmpHdr->type);
            break;
    }

igmp_up_exit:
    
    sal_nic_rxPort_get((void*)pBuf, &spa);
    pm = 1UL << spa;    

    remove_rlpp_block_port(&rlppmask);
    _get_vlan_mask(&pmask);
    pBuf->tx_dport_mode    = TX_DPORT_ASSIGN_PMSK;
    pBuf->tx_dst_pmsk      = (~pm)&(pmask)&(~CPU_PORT_PORTMASK)&(rlppmask);
    pBuf->tx_trunk_support = TX_TRUNK_REPRESENT_PORT;
    pBuf->inner_tag_mode = TX_VLAN_ASSIGN_VID;
    pBuf->inner_tag = vid;
    sal_nic_tx(pBuf);

#if LWPS_IGMP_TRUNK_SUPPORT
    /* kill warning message */
    trunk_portmask = 0;
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */

    return LWPS_OK;
}

#endif /* end of __IGMP_SNOOPING_H__ */