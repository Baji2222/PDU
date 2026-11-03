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
 * $Revision: 7002 $
 * $Date: 2009-11-13 14:40:24 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : Realtek Loop Detection and Prevention Protocol
 *
 */

/* 
 * Include Files 
 */
#include <stdlib.h>
#include <salflash.h>
#include <saltype.h>
#include <salloop.h>
#include <salerror.h>
#include <salsys.h>
#include <salacl.h>
#include <acl.h>
#include <rtk_proto/rlpp/rlpp.h>

#include <rtl8367c_asicdrv_rldp.h>
#include <rtl8367c_asicdrv_acl.h>
#ifdef RTL8367C
extern uint16 phyAllPortMask;
extern uint32 aclAllPortMask;
#endif

#if SAL_LOOP
/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */
static sal_loop_mode_t currentMode;

 /* 
  * Macro Declaration 
  */
 
 /* 
  * Function Declaration 
  */
 extern void setReg(uint16, uint16);
 extern uint16 getReg(uint16);

int32 sal_loop_portRange_check(sal_port_t port)
{
#if defined(LWS_8G_2F)
    if((port >= SAL_MAX_PHY_PORT) && (port <(SAL_PORT_ID_CPU - SAL_MAX_EXT_PORT)))
        return SAL_ERR_PARAMETER;
#elif defined(LWS_5G_1F)
    if((port > SAL_MAX_PHY_PORT) && (port <(SAL_PORT_ID_CPU - SAL_MAX_EXT_PORT)))
        return SAL_ERR_PARAMETER;
#else
    if(port >= SAL_MAX_PHY_PORT)
        return SAL_ERR_PARAMETER;
#endif
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_loop_mode_get
 * Description:
 *      Get loop detection/prevention mode
 * Input:
 *      pMode - Pointer to loop mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_loop_mode_get(sal_loop_mode_t *pMode)
{
    *pMode = currentMode;

    return SAL_ERR_SUCCESS;
}
#if 0//NOT used
void _spc_set(uint8 enable)
{
    uint16 i, tmp, addr;

    for(i = 0; i < SAL_MAX_PHY_PORT;i++)
    {
        addr = 0xe + i * 0x20;
        tmp = getReg(addr);
        tmp &= 0xFFF0;
        if(enable)
            tmp |= 2;      /* special congest timer: 2s */
                           /* otherwise, set congest timer as 0 */
        setReg(addr, tmp);                           
    }
}
#endif
#define RLDP_TX_PERIOD_MS    2000
#define RLDP_REMOVE_CNT      1000 
/* Function Name:
 *      sal_loop_mode_set
 * Description:
 *      Set loop detection/prevention mode
 * Input:
 *      mode - loop detection/prevention mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_loop_mode_set(sal_loop_mode_t mode)
{
    uint16 interval_time;
    uint32 recover_time;
    uint16 tmp;
    sal_mac_t mac;
    rtk_filter_cfg_t cfg;
    rtk_filter_action_t act;
    rtk_filter_field_t *field1;
    rtk_filter_number_t ruleNum;

    if( mode >= SAL_LOOP_MODE_END)
        return SAL_ERR_PARAMETER;

    currentMode = mode;

    switch(mode)
    {
    case SAL_LOOP_MODE_RLPP:
        rlpp_setState(TRUE);    
        rtl8367c_setAsicRldp(FALSE);
        /* Clear looped port mask */
        tmp = getReg(0x18e0);
        tmp |= 0x0010;
        setReg(0x18e0, tmp);
        tmp &= ~0x0010;
        setReg(0x18e0, tmp);

	    /* Add ACL rule 61 to trap RTK pakcets to cpu port */
        /* Add ACL rule 1 ARP+(DA=switch MAC)->redirect to 8051 with priority 1+ policing(meter 1)*/
    	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
        memset(&act, 0x00, sizeof(rtk_filter_action_t));
        field1= malloc(sizeof(rtk_filter_field_t));
        memset(field1, 0, sizeof(rtk_filter_field_t));

    	field1->fieldType = FILTER_FIELD_ETHERTYPE;
        field1->filter_pattern_union.etherType.dataType = FILTER_FIELD_DATA_MASK;
    	field1->filter_pattern_union.etherType.value       = 0x8899;
    	field1->filter_pattern_union.etherType.mask        =0xffff;
    	sal_filter_igrAcl_field_add(&cfg, field1);
        
        cfg.activeport.value.bits[0]    = aclAllPortMask;
        cfg.activeport.mask.bits[0]     = aclAllPortMask;
        cfg.invert = FALSE;

        /* Action :  */
        act.actEnable[FILTER_ENACT_TRAP_CPU] = TRUE;
        act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
        act.filterPriority = 1;
        act.actEnable[FILTER_ENACT_POLICING_0] = TRUE;
        act.filterPolicingIdx[0]=3;
    	sal_filter_igrAcl_cfg_add(61, &cfg, &act, &ruleNum);	

        free(field1);

        break;
    case SAL_LOOP_MODE_RLDP:
        rlpp_setState(FALSE);    
        rtl8367c_setAsicRldp(TRUE);
        
        /////////////////////////////
        /* enable rldp & change rldp to periodically mode */
        setReg(0x18e0, 0x4009);

        /* Configure the */
        sal_sys_mac_get(&mac);
        setReg(0x18e8, (mac.octet[4] << 8) | mac.octet[5]);
        setReg(0x18e9, (mac.octet[2] << 8) | mac.octet[3]);
        setReg(0x18ea, (mac.octet[0] << 8) | mac.octet[1]);

        /* set tx period */
        rlpp_interval_time_get(&interval_time);
        rlpp_recover_time_get(&recover_time);
        tmp = recover_time/interval_time;
        if(tmp == 0)
            tmp = 1;
        tmp = tmp | (tmp << 8);
        setReg(0x18e2, interval_time*1000);
        setReg(0x18e3, interval_time*1000);
        setReg(0x18e1, 0x0404);
        /////////////////////////////

	    /* Remove ACL rule */
        sal_filter_igrAcl_cfg_del(61);

        break;    
    case SAL_LOOP_MODE_DISABLE:
        rlpp_setState(FALSE);    
        rtl8367c_setAsicRldp(FALSE);
        /* Clear looped port mask */
        tmp = getReg(0x18e0);
        tmp |= 0x0010;
        setReg(0x18e0, tmp);
        tmp &= ~0x0010;
        setReg(0x18e0, tmp);

	    /* Remove ACL rule */
        sal_filter_igrAcl_cfg_del(61);

        break;    
    default:
        break;
    }
    
    sal_flash_loop_set(&mode);
    return SAL_ERR_SUCCESS;
}
 
 /* Function Name:
  *      sal_loop_portStatus_get
  * Description:
  *      Get loop status of a port
  * Input:
  *      None
  * Output:
  *      pStatus - Pointer to the loop status.
  * Return:
  *      SAL_ERR_PARAMETER
  *      SAL_ERR_SUCCESS
  * Note:
  *      None
  */
int32 sal_loop_portStatus_get(sal_port_t port, sal_loop_port_status_t *status)
{
    rlpp_port_state_t rlpp_state;
    uint16            rldp_loop_pmsk;
    sal_port_macStatus_t    mac_status;

    if(SAL_LOOP_MODE_RLPP == currentMode)
    {
        rlpp_getPortState(port, &rlpp_state);
        sal_port_macStatus_get(port, &mac_status);
        if(RLPP_FORWARDING_STATE == rlpp_state)
            *status = SAL_LOOP_STATUS_FREE;
        else
        {
            if(mac_status.linkup == TRUE)
            {
            *status = SAL_LOOP_STATUS_RLPP;
            }
            else
            {
                *status = SAL_LOOP_STATUS_FREE;
            }            
        }

        return SAL_ERR_SUCCESS;
    }
    else if(SAL_LOOP_MODE_RLDP == currentMode)
    {
        rldp_loop_pmsk = getReg(0x18f1);
        if(rldp_loop_pmsk & (1 << port))
            *status = SAL_LOOP_STATUS_RLDP;
        else
            *status = SAL_LOOP_STATUS_FREE;
    }
    else
        *status = SAL_LOOP_STATUS_FREE;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_loop_rlppRxPmsk_set
 * Description:
 *      Set ports that can receive RLPP packet
 * Input:
 *      pPmsk  - Ports that can receive RLPP packet
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_loop_rlppRxPmsk_set(sal_lPortmask_t *pPmsk)
{
    uint16 i;

    if(NULL == pPmsk)
        return SAL_ERR_NULL_POINTER;

    /* set ACL portmask as ports that are allowed to receive RLPP packet */
//    setReg(0x668, (uint16)(pPmsk->bits[0] & 0xFFFF));
    for(i = 0; i < SAL_PORT_ID_CPU;i++)
    {
        if(pPmsk->bits[0] & (1UL << i))
            sal_filter_igrAcl_state_set(i, SAL_ENABLED);
        else
            sal_filter_igrAcl_state_set(i, SAL_DISABLED);
    }

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_loop_rlppRxPmsk_get
 * Description:
 *      Get ports that can receive RLPP packet
 * Input:
 *      None
 * Output:
 *      pPmsk  - Ports that can receive RLPP packet
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_loop_rlppRxPmsk_get(sal_lPortmask_t *pPmsk)
{
    uint16 i;
    uint32 state;

    if(NULL == pPmsk)
        return SAL_ERR_NULL_POINTER;

    /* get ACL portmask as ports that are allowed to receive RLPP packet */
    pPmsk->bits[0] = 0;
    for(i = 0; i < SAL_PORT_ID_CPU;i++)
    {
        sal_filter_igrAcl_state_get(i, &state);
        if(state & 0x1)
        {
            pPmsk->bits[0] |= 1UL << i;
        }
    }
//	pPmsk->bits[0] = getReg(0x668);

    return SAL_ERR_SUCCESS;
}
 
/* Function Name:
 *      sal_loop_config_restore
 * Description:
 *      Restore loop detection/prevention configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
  */
int32 sal_loop_config_restore(void)
{
    sal_loop_mode_t mode;
    uint16 interval_time;
    uint32 recover_time;
    uint8 port;
    uint8 enable;

    sal_flash_loop_interval_time_get(&interval_time);
    sal_loop_interval_time_set(interval_time);
    sal_flash_loop_recover_time_get(&recover_time);
    sal_loop_recover_time_set(recover_time);

    sal_flash_loop_get(&mode);
    for (port = 0; port < SAL_PORT_ID_CPU; port++)
    {
        sal_flash_loop_port_enable_get(port,&enable);
#if defined(LWS_8G_2F)
        if ((port >= 8) && (port <= 15))
            continue;
#elif defined(LWS_5G_1F)
        if ((port >= 5) && (port <= 15))
            continue;
#else  
        if (port >= SAL_MAX_PHY_PORT)
            continue;
#endif
        sal_loop_port_enable_set((uint16)port,(uint16)enable);
    }
    sal_loop_mode_set(mode);
    
    return SAL_ERR_SUCCESS;    
}

int32 sal_loop_interval_time_get(uint16 *pTime)
{
    rlpp_interval_time_get(pTime);
    return SAL_ERR_SUCCESS;
}

int32 sal_loop_interval_time_set(uint16 time)
{
    rlpp_interval_time_set(time);
    sal_flash_loop_interval_time_set(time);
    return SAL_ERR_SUCCESS;
}

int32
sal_loop_recover_time_get(uint32 *pTime)
{
    rlpp_recover_time_get(pTime);
    return SAL_ERR_SUCCESS;
}

int32
sal_loop_recover_time_set(uint32 time)
{
    rlpp_recover_time_set(time);
    sal_flash_loop_recover_time_set(time);
    return SAL_ERR_SUCCESS;
}

int32 
sal_loop_port_enable_get(uint16 ucPortId, uint16 *pEnable)
{
    rlpp_port_enable_get(ucPortId, pEnable);
    return SAL_ERR_SUCCESS;
}

int32 
sal_loop_port_state_get(uint16 ucPortId, uint16 *pState)
{
    sal_port_macStatus_t    mac_status;

    sal_port_macStatus_get(ucPortId, &mac_status);
    if(mac_status.linkup == TRUE)
    {
    rlpp_get_portState(ucPortId, pState);
    }
    else
    {
        *pState = RLPP_FORWARDING_STATE;
    }
    return SAL_ERR_SUCCESS;
}

int32 
sal_loop_port_enable_set(uint16 ucPortId, uint16 enable)
{
    sal_loop_mode_t mode;
    rlpp_port_enable_set(ucPortId, enable);
    sal_flash_loop_port_enable_set((uint8)ucPortId, (uint8)enable);

    sal_loop_mode_get(&mode);
    if(SAL_LOOP_MODE_DISABLE != mode)
    {
    //    sal_filter_rtkPkt_trap(ucPortId, enable);
    }
    
    return SAL_ERR_SUCCESS;
}

#endif

