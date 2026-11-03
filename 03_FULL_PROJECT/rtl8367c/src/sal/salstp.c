#pragma STRING(far)
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
 * $Revision: 7005 $
 * $Date: 2009-11-13 15:15:59 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : Spanning Tree
 *
 */

/* 
 * Include Files 
 */
#include <stdio.h>
#include <string.h>

#include <salstp.h>
#include <salerror.h>
#include <salflash.h>
#include <saltrunk.h>
#include <salportmask.h>

#include <lwps/rstp.h>

#include <rtl8367c_asicdrv_rma.h>
#include <rtl8367c_asicdrv_vlan.h>

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */

 /* 
  * Macro Declaration 
  */
#define SAL_STP_BUPD_INDEX     0
 
 /* 
  * Function Declaration 
  */

#if SAL_RSTP
/* Function Name:
 *      sal_stp_state_set
 * Description:
 *      Set spanning tree state.
 * Input:
 *      enable - spanning tree state
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_state_set(sal_enable_t enable)
{
    st_desc_t stp_desc;
    
    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;
    
    stp_desc.target = RSTP_SET_DESC_FUNC_ENABLE;
        
    if(SAL_ENABLED == enable)
        stp_desc.value = ENABLE;
    else
        stp_desc.value = DISABLE;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;
    
    /* [FIXME] add flash store function */
    return SAL_ERR_SUCCESS;
}
  
/* Function Name:
 *      sal_stp_state_get
 * Description:
 *      Set spanning tree state.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the spanning tree state.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_state_get(sal_enable_t *pEnable)
{
    st_desc_t stp_desc;
        
    stp_desc.target = RSTP_GET_DESC_FUNC_ENABLE;

    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(stp_desc.value == 0)
        *pEnable = SAL_DISABLED;
    else
        *pEnable = SAL_ENABLED;

    return SAL_ERR_SUCCESS;
}
  
/* Function Name:
 *      sal_stp_version_set
 * Description:
 *      Set stp/rstp version.
 * Input:
 *      version - version of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 *      SAL_ERR_FAILED    - function call error.
 *      SAL_ERR_NOT_SUPPORT; 
 * Note:
 *      None
 */
int32 sal_stp_version_set(sal_stp_version_t version)
{
    st_desc_t stp_desc;

    if(version >= SAL_STP_VERSION_END)
        return SAL_ERR_PARAMETER;

    stp_desc.target = RSTP_SET_DESC_FORCE_VER;
    
    if(version == SAL_STP_VERSION_STP)
        stp_desc.value = 0;
    else
        stp_desc.value = RSTP_FORCE_PROTO_VERSION;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    return sal_flash_stp_version_set (version);     
}

  
/* Function Name:
 *      sal_stp_version_get
 * Description:
 *      Get stp/rstp version.
 * Input:
 *      None
 * Output:
 *      pStpVersion - pointer to version of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_version_get(sal_stp_version_t *pVersion)
{
    st_desc_t stp_desc;
            
    stp_desc.target = RSTP_GET_DESC_FORCE_VER;
    
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(stp_desc.value == RSTP_FORCE_PROTO_VERSION)
        *pVersion = SAL_STP_VERSION_RSTP;
    else
        *pVersion = SAL_STP_VERSION_STP;
    
    return SAL_ERR_SUCCESS;
}
  
/* Function Name:
 *      sal_stp_priority_set
 * Description:
 *      Set stp/rstp priority.
 * Input:
 *      priority - priority of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 *      SAL_ERR_FAILED    - function call error. 
 * Note:
 *      None
 */
int32 sal_stp_priority_set(sal_stp_pri_t priority)
{
    st_desc_t stp_desc;

    if(priority > SAL_STP_PRIORITY_MAX)
        return SAL_ERR_PARAMETER;
    
    stp_desc.target = RSTP_SET_DESC_BRIDGE_PRIORITY;
    stp_desc.value = priority;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;
    
    return sal_flash_stp_priority_set(priority);
}

  
/* Function Name:
 *      sal_stp_priority_get
 * Description:
 *      Get stp/rstp priority.
 * Input:
 *      None
 * Output:
 *      pPiority - pointer to priority of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_priority_get(sal_stp_pri_t *pPriority)
{
    st_desc_t stp_desc;
                
    stp_desc.target = RSTP_GET_DESC_BRIDGE_PRIORITY;
        
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    *pPriority = stp_desc.value;
        
    return SAL_ERR_SUCCESS;
}

  
/* Function Name:
 *      sal_stp_timePara_set
 * Description:
 *      Set maximum age, hello time, forward delay of stp/rstp.
 * Input:
 *      maxAge    - maximum age of stp/rstp.
 *      helloTime - hello time of stp/rstp.
 *      fwdDelay  - forward delay of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 *      SAL_ERR_FAILED    - function call error. 
 * Note:
 *      None
 */
int32 sal_stp_timePara_set(sal_stp_time_t maxAge, sal_stp_time_t helloTime, sal_stp_time_t fwdDelay)
{
    st_desc_t stp_desc;

    if(maxAge > SAL_STP_MAX_AGE_SEC_MAX || maxAge < SAL_STP_MAX_AGE_SEC_MIN ||
       helloTime > SAL_STP_HELLO_SEC_MAX || helloTime< SAL_STP_HELLO_SEC_MIN ||
       fwdDelay > SAL_STP_FWD_DELAY_SEC_MAX || fwdDelay < SAL_STP_FWD_DELAY_SEC_MIN
       )
        return SAL_ERR_PARAMETER;

    stp_desc.target = RSTP_SET_DESC_MAX_AGE;
    stp_desc.value  = (uint32)maxAge << 8;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    stp_desc.target = RSTP_SET_DESC_HELLO_TIME;
    stp_desc.value  = (uint32)helloTime << 8;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    stp_desc.target = RSTP_SET_DESC_FWD_DELAY;
    stp_desc.value  = (uint32)fwdDelay << 8;
    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    return sal_flash_stp_timePara_set(maxAge, helloTime, fwdDelay);
}
  
/* Function Name:
 *      sal_stp_timePara_get
 * Description:
 *      Get maximum age, hello time, forward delay of stp/rstp.
 * Input:
 *      None
 * Output:
 *      pMmaxAge   - pointer of maximum age of stp/rstp.
 *      pHelloTime - pointer of hello time of stp/rstp.
 *      pFwdDelay  - pointer of forward delay of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_timePara_get(sal_stp_time_t *pMaxAge, sal_stp_time_t *pHelloTime, sal_stp_time_t *pFwdDelay)
{
    st_desc_t stp_desc;

    stp_desc.target = RSTP_GET_DESC_MAX_AGE;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    *pMaxAge = (uint8)((stp_desc.value>>8) & 0xFF);

    stp_desc.target = RSTP_GET_DESC_HELLO_TIME;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    *pHelloTime= (uint8)((stp_desc.value>>8) & 0xFF);

    stp_desc.target = RSTP_GET_DESC_FWD_DELAY;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    *pFwdDelay = (uint8)((stp_desc.value>>8) & 0xFF);
            
    return SAL_ERR_SUCCESS;
}
  
/* Function Name:
 *      sal_stp_rootInfo_get
 * Description:
 *      Get root information of stp/rstp.
 * Input:
 *      None
 * Output:
 *      pRootInfo - pointer to root information.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 *      SAL_ERR_NULL_POINTER - null pointer
 * Note:
 *      None
 */
int32 sal_stp_rootInfo_get(sal_stp_rootInfo_t *pRootInfo)
{
    st_desc_t stp_desc;

    if(NULL == pRootInfo)
        return SAL_ERR_NULL_POINTER;
                        
    stp_desc.target = RSTP_GET_DESC_THIS_IS_ROOT;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(stp_desc.value == 1)
    {
        /* switch is root */    
        pRootInfo->rootPort_exist = 0;
    }
    else
    {
        /* switch is not root */        
        pRootInfo->rootPort_exist = 1;
    }
    
    stp_desc.target = RSTP_GET_DESC_ROOT_PORT;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootPort = (uint8)(stp_desc.value & 0xFF);


    stp_desc.target = RSTP_GET_DESC_ROOT_MAX_AGE;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootMaxAge = (uint8)((stp_desc.value>>8) & 0xFF);


    stp_desc.target = RSTP_GET_DESC_ROOT_HELLO_TIME;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootHelloTime = (uint8)((stp_desc.value>>8) & 0xFF);


    stp_desc.target = RSTP_GET_DESC_ROOT_FWD_DELAY;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootForwardDelay = (uint8)((stp_desc.value>>8) & 0xFF);


    stp_desc.target = RSTP_GET_DESC_ROOT_PRIORITY;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootPriority= stp_desc.value;


    stp_desc.target = RSTP_GET_DESC_ROOT_PATH_COST;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pRootInfo->rootPathCost = (uint16)(stp_desc.value & 0xFFFF);


    stp_desc.target = RSTP_GET_DESC_ROOT_MAC;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;
    memcpy(pRootInfo->rootMac.octet, stp_desc.mac, 6);
                
    return SAL_ERR_SUCCESS;
}
  
/* Function Name:
 *      sal_stp_portPara_set
 * Description:
 *      Set port parameter of stp/rstp.
 * Input:
 *      pPortPara - Pointer to port parameter of stp/rstp.
 *      port      - port id 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 *      SAL_ERR_NULL_POINTER - null pointer
 * Note:
 *      None
 */
int32 sal_stp_portPara_set(sal_port_t port, sal_stp_portPara_t *pPortPara)
{
    st_desc_t stp_desc;

    if(pPortPara->edge >= SAL_STP_PORT_EDGE_END ||
       pPortPara->p2p >= SAL_STP_PORT_P2P_MAX ||
       pPortPara->pathCost < SAL_STP_PORT_PATH_COST_MIN ||
       pPortPara->pathCost > SAL_STP_PORT_PATH_COST_MAX
       )
        return SAL_ERR_PARAMETER;

   if(NULL == pPortPara)
        return SAL_ERR_NULL_POINTER;

    stp_desc.pn = port;
    
    stp_desc.target = RSTP_SET_DESC_PORT_ADMIN_EDGE;
    if(SAL_STP_PORT_EDGE_TRUE == pPortPara->edge)
        stp_desc.value = TRUE;
    else
        stp_desc.value = FALSE;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    stp_desc.target = RSTP_SET_DESC_PORT_PRIORITY;
    stp_desc.value = pPortPara->priority;
    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    stp_desc.target = RSTP_SET_DESC_PORT_P2P;
    if(SAL_STP_PORT_P2P_FALSE == pPortPara->edge)
        stp_desc.value = RSTP_ADMIN_P2P_FORCEFALSE;
    else if(SAL_STP_PORT_P2P_TRUE == pPortPara->edge)
        stp_desc.value = RSTP_ADMIN_P2P_FORCETRUE;
    else
        stp_desc.value = RSTP_ADMIN_P2P_AUTO;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    stp_desc.target = RSTP_SET_DESC_PORT_ADMIN_RPC;
    stp_desc.value = pPortPara->pathCost;

    if(rstp_set(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    return sal_flash_stp_port_set(port, pPortPara);
}

/* Function Name:
 *      sal_stp_portPara_get
 * Description:
 *      Get port parameter of stp/rstp.
 * Input:
 *      port        - port id
 * Output:
 *      pPortPara - Pointer to port parameter of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_portPara_get(sal_port_t port, sal_stp_portPara_t *pPortPara)
{
    st_desc_t stp_desc;

    if(NULL == pPortPara)
        return SAL_ERR_NULL_POINTER;

    stp_desc.pn = port;
    
    stp_desc.target = RSTP_GET_DESC_PORT_EDGE;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(stp_desc.value & (1 << 2))
        pPortPara->edge = SAL_STP_PORT_EDGE_TRUE;
    else
        pPortPara->edge = SAL_STP_PORT_EDGE_FALSE;

    stp_desc.target = RSTP_GET_DESC_PORT_P2P;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(RSTP_ADMIN_P2P_FORCEFALSE == stp_desc.value)
        pPortPara->p2p = SAL_STP_PORT_P2P_FALSE;
    else if(RSTP_ADMIN_P2P_FORCETRUE == stp_desc.value)
        pPortPara->p2p = SAL_STP_PORT_P2P_TRUE;
    else
        pPortPara->p2p = SAL_STP_PORT_P2P_AUTO;
    

    stp_desc.target = RSTP_GET_DESC_PORT_PATH_COST;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pPortPara->pathCost= stp_desc.value;


    stp_desc.target = RSTP_GET_DESC_PORT_PRIORITY;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    pPortPara->priority= stp_desc.value;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_stp_portStatus_get
 * Description:
 *      Get port status of stp/rstp.
 * Input:
 *      port        - port id
 * Output:
 *      pPortStatus - Pointer to port status of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
int32 sal_stp_portStatus_get(sal_port_t port, sal_stp_portStatus_t *pPortStatus)
{
    st_desc_t stp_desc;
    uint32 state;
	if(NULL == pPortStatus)
        return SAL_ERR_NULL_POINTER;
    
    stp_desc.pn = port;

    stp_desc.target = RSTP_GET_DESC_PORT_REAL_EDGE;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    if(TRUE == stp_desc.value)
        pPortStatus->actualEdge = SAL_STP_PORT_EDGE_TRUE;
    else
        pPortStatus->actualEdge = SAL_STP_PORT_EDGE_FALSE;
   

    stp_desc.target = RSTP_GET_DESC_PORT_REAL_P2P;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;
    
    if(RSTP_ADMIN_P2P_FORCEFALSE == stp_desc.value)
        pPortStatus->actualP2p = SAL_STP_PORT_P2P_FALSE;
    else if(RSTP_ADMIN_P2P_FORCETRUE == stp_desc.value)
        pPortStatus->actualP2p = SAL_STP_PORT_P2P_TRUE;
    else
        pPortStatus->actualP2p = SAL_STP_PORT_P2P_AUTO;
    
    
    stp_desc.target = RSTP_GET_DESC_PORT_REAL_PATH_COST;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;
    pPortStatus->actualPathCost= stp_desc.value;
    rtl8367c_getAsicSpanningTreeStatus(port, 0, &state);
	pPortStatus->state=(uint16)state;
    stp_desc.target = RSTP_GET_DESC_PORT_ROLE;
    if(rstp_get(&stp_desc) != LWPS_OK)
        return SAL_ERR_FAILED;

    switch (stp_desc.value)
    {
    case RSTP_PORT_ROLE_DISABLE:
        pPortStatus->role = SAL_STP_PORT_ROLE_DISABLE;
        break;
    case RSTP_PORT_ROLE_ALTERNATE:
        pPortStatus->role = SAL_STP_PORT_ROLE_ALTERNATE;
        break;
    case RSTP_PORT_ROLE_BACKUP:
        pPortStatus->role = SAL_STP_PORT_ROLE_BACKUP;
        break;
    case RSTP_PORT_ROLE_DESIGNATED:
        pPortStatus->role = SAL_STP_PORT_ROLE_DESIGNATED;
        break;
    case RSTP_PORT_ROLE_ROOT:
        pPortStatus->role = SAL_STP_PORT_ROLE_ROOT;
        break;
    default:
        return SAL_ERR_FAILED;
    }
    
    return SAL_ERR_SUCCESS;

}
  
/* Function Name:
 *      sal_stp_chip_init
 * Description:
 *      Set initilization of stp protocol to chip.
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_stp_chip_init()
{
    rtl8367c_rma_t rma;

    memset(&rma, 0, sizeof(rtl8367c_rma_t));
    rma.operation = RMAOP_TRAP_TO_CPU;
    rtl8367c_setAsicRma(SAL_STP_BUPD_INDEX, &rma);

    return SAL_ERR_SUCCESS;
}

int32 sal_stp_config_restore(void)
{
    sal_stp_version_t   ver;
    sal_stp_pri_t       pri;
    sal_stp_time_t      age;
    sal_stp_time_t      hello;
    sal_stp_time_t      fwd_delay;
    sal_stp_portPara_t  port_cfg;

    sal_port_t      port;
    sal_lPortmask_t portmask;

    sal_flash_stp_version_get(&ver);
    sal_stp_version_set(ver);
    
    sal_flash_stp_priority_get(&pri);
    sal_stp_priority_set(pri);
    
    sal_flash_stp_timePara_get(&age, &hello, &fwd_delay);
    sal_stp_timePara_set(age, hello, fwd_delay);
   
    sal_trunk_logicPort_setAll(&portmask);
    for (port = 0; port < SAL_PORT_ID_CPU; port++)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portmask, port))
        {
            continue;
        }

        sal_flash_stp_port_get(port, &port_cfg);
        sal_stp_portPara_set(port, &port_cfg);
    }
    
    return SAL_ERR_SUCCESS;
}
#endif /* end of SAL_RSTP */

/* Function Name:
 *      sal_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stp_state - spanning tree state
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */


int32 sal_stp_mstpState_set(sal_stg_t msti, sal_port_t port, sal_stp_state_t stp_state)
{
    if(msti >= SAL_MSTI_MAX)
        return SAL_ERR_PARAMETER;

#if defined(LWS_8G_2F)
    if((port >= SAL_MAX_PHY_PORT) && (port <(SAL_PORT_ID_CPU - SAL_MAX_EXT_PORT)))
        return SAL_ERR_PARAMETER;

    if (port == EXT_PORT0)
        port = 8;
    if (port == EXT_PORT1)
        port = 9;
#elif defined(LWS_5G_1F)
    if((port > SAL_MAX_PHY_PORT) && (port <(SAL_PORT_ID_CPU - SAL_MAX_EXT_PORT)))
        return SAL_ERR_PARAMETER;

    if (port == EXT_PORT0)
        port = 6;
#else
    if(port >= SAL_MAX_PHY_PORT)
        return SAL_ERR_PARAMETER;
#endif

    return rtl8367c_setAsicSpanningTreeStatus(port, msti, stp_state);
}

