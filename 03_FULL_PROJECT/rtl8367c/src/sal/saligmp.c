/* 
 * Copyright (C) 2009 Realtek Semiconductor Corp.  
 * All Rights Reserved. 
 * 
 * This program is the proprietary software of Realtek Semiconductor 
 * Corporation and/or its licensors, and only be used, duplicated,  
 * modified or distributed under the authorized license from Realtek.  
 * 
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER  
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.  
 * 
 * $Revision: 6401 $ 
 * $Date: 2009-10-14 16:03:12 +0800 (Wed, 14 Oct 2009) $ 
 * 
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : System related
 * 
 */

/* 
 * Include Files 
 */
#include <saligmp.h>
#include <salerror.h>	
#include <salnic.h>
#include <lwps/pktbuf.h>
#include <lwps/igmp_group_db.h>
#include <rtl8367c_asicdrv_igmp.h>
#include <rtl8367c_asicdrv_unknownMulticast.h>
#include <rtl8367c_asicdrv_port.h>
#include <local/igmp_config.h>
#include <salflash.h>
#include <l2.h>
#include <leaky.h>
#include <igmp.h>
#include <saltrunk.h>
#include <salportmask.h>

#if SAL_IGMP

extern uint8 xdata _igmpStatus;
extern uint16 phyAllPortMask;

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */
static uint8 updateFlag;

/* 
 * Macro Declaration 
 */

/* 
 * Function Declaration 
 */

/* Function Name:
 *      sal_igmp_nextEntry_get
 * Description:
 *      Get next valid IGMP entry.
 * Input:
 *      None
 * Output:
 *      pEntry  - pointer to next valid entry
 *      pIsTail - pointer to indicate whether current node in linked list is tail.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
 int32 sal_igmp_nextEntry_get(sal_igmp_entry_t *pEntry, uint8 *pIsTail)
{
    static ipaddr_group_list_t *pCurrNode = NULL;
    ipaddr_group_head_t        *pList = NULL;

    /* get igmp entry linked list if listNumber = 0 */
    if(NULL == pCurrNode)
    {
        /* [FIXME] call get link list function */
        pList = igmp_get_groupList();
        pCurrNode = pList->item;
    }

    if( NULL==pList )
    {
        pCurrNode = pCurrNode->next;
    }

    if(NULL == pCurrNode)
    {
        *pIsTail = TRUE;
    }
    else
    {
        *pIsTail = FALSE;

        /*printf("                  [%d:%d:%d:%d]:0x%08lX\n", \
                (uint16)pCurrNode->groupAddr.addr[0], \ 
                (uint16)pCurrNode->groupAddr.addr[1], \
                (uint16)pCurrNode->groupAddr.addr[2], \
                (uint16)pCurrNode->groupAddr.addr[3], \
                (uint32)pCurrNode->fwdPortMask[0]);*/
        pEntry->fwd_portmask.bits[0] = pCurrNode->fwdPortMask[0];
        pEntry->group_addr = (((uint32)pCurrNode->groupAddr.addr[0]) << 24) |
                             (((uint32)pCurrNode->groupAddr.addr[1]) << 16) |
                             (((uint32)pCurrNode->groupAddr.addr[2]) << 8) |                             
                             (((uint32)pCurrNode->groupAddr.addr[3]));
        pEntry->vid = pCurrNode->vid;
    }

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_igmp_ipAddrEntry_add
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 *      pMember - Pointer to the member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_ipAddrEntry_add(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid, sal_lPortmask_t *pMember)
{
#if defined(CONFIG_IGMP_PURE_DUMB)    
    rtk_portmask_t mask;
#endif    
    int32          ret;
    rtk_l2_mcastAddr_t mcastAddr;

    memset(&mcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    mcastAddr.portmask.bits[0] = pMember->bits[0];

    /* if sip is not assigned, use L2 Multicast format to write LUT */
    if(NULL == sip)
    {
        mcastAddr.mac.octet[0] = 0x01;
        mcastAddr.mac.octet[1] = 0x00;
        mcastAddr.mac.octet[2] = 0x5E;
        mcastAddr.mac.octet[3] = (uint8)((dip & 0xFF0000) >> 16) & 0x7F;
        mcastAddr.mac.octet[4] = (uint8)((dip & 0xFF00) >> 8);
        mcastAddr.mac.octet[5] = (uint8)(dip & 0xFF);
        //if(fid == 1)
        //    mcastAddr.vid = 0;
        //else
            mcastAddr.vid = fid;
        mcastAddr.ivl = 1;
        ret = rtk_l2_mcastAddr_add(&mcastAddr);
        if(ret)
            return ret;
    }
#if defined(CONFIG_IGMP_PURE_DUMB)    
    else
    {
        ret = rtk_l2_ipMcastAddr_add(sip, dip, mask);
        if(ret)
            return ret;
    }
#endif /* end of CONFIG_IGMP_PURE_DUMB */
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_igmp_ipAddrEntry_del
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_ipAddrEntry_del(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid)
{
    rtk_l2_mcastAddr_t mcastAddr;

    memset(&mcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    /* if sip is not assigned, use L2 Multicast format to write LUT */
    if(0 == sip)
    {
        mcastAddr.mac.octet[0] = 0x01;
        mcastAddr.mac.octet[1] = 0x00;
        mcastAddr.mac.octet[2] = 0x5E;
        mcastAddr.mac.octet[3] = (uint8)((dip & 0xFF0000) >> 16) & 0x7F;
        mcastAddr.mac.octet[4] = (uint8)((dip & 0xFF00) >> 8);
        mcastAddr.mac.octet[5] = (uint8)(dip & 0xFF);
        //if(fid == 1)
        //    mcastAddr.vid = 0;
        //else
            mcastAddr.vid = fid;
        mcastAddr.ivl = 1;
        return rtk_l2_mcastAddr_del(&mcastAddr);
    }
#if defined(CONFIG_IGMP_PURE_DUMB)    
    else
    {
        return rtk_l2_ipMcastAddr_del(sip, dip);
    }
#endif /* end of CONFIG_IGMP_PURE_DUMB */    
    
    return SAL_ERR_SUCCESS;
}

#if CONFIG_IGMP_MLD_ENABLE

/* Function Name:
 *      sal_igmp_ipv6AddrEntry_add
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 *      pMember - Pointer to the member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_ipv6AddrEntry_add(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid, sal_lPortmask_t *pMember)
{
#if defined(CONFIG_IGMP_PURE_DUMB)    
    rtk_portmask_t mask;
#endif    
    rtk_l2_mcastAddr_t mcastAddr;

    memset(&mcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    mcastAddr.portmask.bits[0] = pMember->bits[0];
    
    /* if sip is not assigned, use L2 Multicast format to write LUT */
    if(0 == sip)
    {
        mcastAddr.mac.octet[0] = 0x33;
        mcastAddr.mac.octet[1] = 0x33;
        mcastAddr.mac.octet[2] = (uint8)((dip & 0xFF000000) >> 24);
        mcastAddr.mac.octet[3] = (uint8)((dip & 0xFF0000) >> 16);
        mcastAddr.mac.octet[4] = (uint8)((dip & 0xFF00) >> 8);
        mcastAddr.mac.octet[5] = (uint8)(dip & 0xFF);
        //if(fid == 1)
        //    mcastAddr.vid = 0;
        //else
            mcastAddr.vid = fid;
        mcastAddr.ivl = 1;
        return rtk_l2_mcastAddr_add(&mcastAddr);
    }
#if defined(CONFIG_IGMP_PURE_DUMB)    
    else
    {
        return rtk_l2_ipMcastAddr_add(sip, dip, mask);
    }
#endif /* end of CONFIG_IGMP_PURE_DUMB */    

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_igmp_ipv6AddrEntry_del
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_ipv6AddrEntry_del(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid)
{
    rtk_l2_mcastAddr_t mcastAddr;

    memset(&mcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    /* if sip is not assigned, use L2 Multicast format to write LUT */
    if(0 == sip)
    {
        mcastAddr.mac.octet[0] = 0x33;
        mcastAddr.mac.octet[1] = 0x33;
        mcastAddr.mac.octet[2] = (uint8)((dip & 0xFF000000) >> 24);
        mcastAddr.mac.octet[3] = (uint8)((dip & 0xFF0000) >> 16);
        mcastAddr.mac.octet[4] = (uint8)((dip & 0xFF00) >> 8);
        mcastAddr.mac.octet[5] = (uint8)(dip & 0xFF);
        //if(fid == 1)
        //    mcastAddr.vid = 0;
        //else
            mcastAddr.vid = fid;
        mcastAddr.ivl = 1;
        return rtk_l2_mcastAddr_del(&mcastAddr);
    }
#if defined(CONFIG_IGMP_PURE_DUMB)        
    else
    {
        return rtk_l2_ipMcastAddr_del(sip, dip);
    }
#endif /* end of CONFIG_IGMP_PURE_DUMB */    
    
    return SAL_ERR_SUCCESS;

}

#endif /* end of CONFIG_IGMP_MLD_ENABLE */

/* Function Name:
 *      sal_igmp_chip_init
 * Description:
 *      Initialzie IGMP setting to chip. (internal)
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_chip_init()
{
	sal_port_t          port, portLogic;
	sal_pPortmask_t	    portMask;
    sal_loop_port_status_t  loopStatus;	
    
    sal_trunk_logicPort_setAll(&portMask);
    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port))
        {
            sal_loop_portStatus_get(port, &loopStatus);
        	if (SAL_LOOP_STATUS_RLPP == loopStatus)
        	    continue;
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU);
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv2, IGMP_ACTION_TRAP2CPU);
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv3, IGMP_ACTION_TRAP2CPU);
#if CONFIG_IGMP_MLD_ENABLE        
            rtk_igmp_protocol_set(port, PROTOCOL_MLDv1, IGMP_ACTION_TRAP2CPU);
            rtk_igmp_protocol_set(port, PROTOCOL_MLDv2, IGMP_ACTION_TRAP2CPU);
#endif
#if defined(LWS_8G_2F)
            if(port == EXT_PORT0)
                portLogic = 8;
            else if(port == EXT_PORT1)
                portLogic = 9;
            else
                portLogic = port;
#elif defined(LWS_5G_1F)
            if(port == EXT_PORT0)
                portLogic = 6;
            else
                portLogic = port;
#else
            portLogic = port;
#endif
            //rtl8367c_setAsicIpMulticastVlanLeaky(i, TRUE);
            //rtl8367c_setAsicUnknownL2MulticastBehavior(portLogic, L2_UNKOWN_MULTICAST_DROP);
            rtl8367c_setAsicUnknownIPv4MulticastBehavior(portLogic, L3_UNKOWN_MULTICAST_DROP);
#if CONFIG_IGMP_MLD_ENABLE        
            rtl8367c_setAsicUnknownIPv6MulticastBehavior(portLogic, L3_UNKOWN_MULTICAST_DROP);
#endif
		}
	}
    
//    rtl8367c_setAsicIpMulticastVlanLeaky(SAL_PORT_ID_CPU, TRUE); //8051 also vlan leaky
    rtl8367c_setAsicPortUnknownMulticastFloodingPortmask(phyAllPortMask);
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_igmp_multicast_forward
 * Description:
 *      When IGMP is disabled, multicast packet should flooding
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_multicast_forward()
{
	sal_port_t          port, portLogic;
	sal_pPortmask_t	    portMask;
    sal_loop_port_status_t  loopStatus; 

    sal_trunk_logicPort_setAll(&portMask);
    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
    	sal_loop_portStatus_get(port, &loopStatus);
    	if (SAL_LOOP_STATUS_RLPP == loopStatus)
    	    continue;
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port))
        {
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv1, IGMP_ACTION_FORWARD);
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv2, IGMP_ACTION_FORWARD);
            rtk_igmp_protocol_set(port, PROTOCOL_IGMPv3, IGMP_ACTION_FORWARD);
#if CONFIG_IGMP_MLD_ENABLE        
            rtk_igmp_protocol_set(port, PROTOCOL_MLDv1, IGMP_ACTION_FORWARD);
            rtk_igmp_protocol_set(port, PROTOCOL_MLDv2, IGMP_ACTION_FORWARD);
#endif        
#if defined(LWS_8G_2F)
            if(port == EXT_PORT0)
                portLogic = 8;
            else if(port == EXT_PORT1)
                portLogic = 9;
            else
                portLogic = port;
#elif defined(LWS_5G_1F)
            if(port == EXT_PORT0)
                portLogic = 6;
            else
                portLogic = port;
#else
            portLogic = port;
#endif
            //rtl8367c_setAsicIpMulticastVlanLeaky(i, FALSE);
            //rtl8367c_setAsicUnknownL2MulticastBehavior(portLogic, L2_UNKOWN_MULTICAST_FLOODING);
            rtl8367c_setAsicUnknownIPv4MulticastBehavior(portLogic, L3_UNKOWN_MULTICAST_FLOODING);
#if CONFIG_IGMP_MLD_ENABLE        
            rtl8367c_setAsicUnknownIPv6MulticastBehavior(portLogic, L3_UNKOWN_MULTICAST_FLOODING);
#endif
        }
    }
//    rtl8367c_setAsicIpMulticastVlanLeaky(SAL_PORT_ID_CPU, FALSE); //8051
    rtl8367c_setAsicPortUnknownMulticastFloodingPortmask(phyAllPortMask);
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_igmp_state_get
 * Description:
 *      Get IGMP enable state.
 * Input:
 *      None
 * Output:
 *      enable
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
 int32 sal_igmp_state_get(sal_enable_t *enable)
{
    sal_enable_t enable_igmp;
    
    sal_flash_igmp_configEnable_get(&enable_igmp);
    *enable = enable_igmp;
    
    return SAL_ERR_SUCCESS;
}


extern ipaddr_group_head_t    xdata igmpGroupHead;
int32 igmp_group_del()
{
    uint16 i;
    uint16 group_size = igmpGroupHead.listNumber;
    ipaddr_group_list_t *iter_group = igmpGroupHead.item;
    ipaddr_group_list_t *iter_group_rm;

    /* remove query port */
    for (i=0; i<SAL_PORT_ID_CPU; i++)
    {
        igmp_group_del_queryPort(i);
    }

    /* Check memberPort timer */
    for(i=0; i<group_size; i++)
    {
        iter_group_rm = iter_group;
        iter_group = iter_group->next;
#if CONFIG_IGMP_MLD_ENABLE
        if(iter_group_rm->ipvn == IGMP_GRP_DB_IPV4)
            igmp_group_del_group( &(iter_group_rm->groupAddr), iter_group_rm->vid, NULL);
        else
            mld_group_del_group( &(iter_group_rm->groupAddr), iter_group_rm->vid, NULL);
#else
            igmp_group_del_group( &(iter_group_rm->groupAddr), iter_group_rm->vid, NULL);
#endif
        
    }
    
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_igmp_state_set
 * Description:
 *      Set IGMP enable state.
 * Input:
 *      enable
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
 int32 sal_igmp_state_set(sal_enable_t enable)
{
    
    if((enable == DISABLED) && (_igmpStatus == ENABLED))
    {
        igmp_group_del();
        igmp_group_db_init();
        sal_igmp_multicast_forward();
    }
    if((enable == ENABLED) && (_igmpStatus == DISABLED))
    {
        sal_igmp_chip_init();
        igmp_group_db_init();
    }
    _igmpStatus = enable;

    return sal_flash_igmp_configEnable_set(enable);
}

/* Function Name:
 *      sal_igmp_staticRouter_get
 * Description:
 *      Get IGMP statistic router port configuration
 * Input:
 *      None
 * Output:
 *      pMask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_staticRouter_get(sal_lPortmask_t *pMask)
{
    sal_lPortmask_t pm;

    if(pMask == NULL)
    {
        return LWPS_ERR_NULLPOINT;
    }

    sal_flash_igmp_staticRouter_get(&pm);
    *pMask = pm;
    
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_igmp_staticRouter_set
 * Description:
 *      Set IGMP statistic router port configuration
 * Input:
 *      mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_staticRouter_set(sal_lPortmask_t mask)
{
    extern int32 igmp_staticRouter_set(sal_lPortmask_t mask);

    igmp_staticRouter_set(mask);
    return sal_flash_igmp_staticRouter_set(mask);
}


/* Function Name:
 *      sal_igmp_dynamicRouter_get
 * Description:
 *      Get current IGMP dynamic router port
 * Input:
 *      None
 * Output:
 *      pMask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_igmp_dynamicRouter_get(sal_lPortmask_t *pMask)
{
    sal_lPortmask_t pm;
    extern int32 igmp_dynamicRouter_get(sal_lPortmask_t *mask);

    if(pMask == NULL)
    {
        return LWPS_ERR_NULLPOINT;
    }

    igmp_dynamicRouter_get(&pm);
    *pMask = pm;
    
    return SAL_ERR_SUCCESS;
}

int32 sal_igmp_config_restore(void)
{
    sal_enable_t enable;
    sal_lPortmask_t mask;

    /* Static router ports */
    sal_flash_igmp_staticRouter_get(&mask);
    sal_igmp_staticRouter_set(mask);

    sal_flash_igmp_configEnable_get(&enable);
    sal_igmp_state_set(enable);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_flash_igmp_init
 * Description:
 *      Initialize IGMP configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_igmp_init(void)
{
    sal_lPortmask_t mask = {0};

    /* IGMP state */
    sal_flash_igmp_configEnable_set(DISABLE);

    /* Static router ports */
    sal_flash_igmp_staticRouter_set(mask);

    return SAL_ERR_SUCCESS;
}
#endif

