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
#ifndef __SAL_STP_H__
#define __SAL_STP_H__

#include <saltype.h>

#define SAL_STP_PRIORITY_SCALE      4096
#define SAL_STP_PRIORITY_MAX        65535
#define SAL_STP_MAX_AGE_SEC_MAX     40
#define SAL_STP_MAX_AGE_SEC_MIN     6
#define SAL_STP_HELLO_SEC_MAX       10
#define SAL_STP_HELLO_SEC_MIN       1
#define SAL_STP_FWD_DELAY_SEC_MAX   30
#define SAL_STP_FWD_DELAY_SEC_MIN   4
#define SAL_STP_PORT_PATH_COST_MAX  200000000UL
#define SAL_STP_PORT_PATH_COST_MIN  0UL

typedef uint8   sal_stp_time_t;     /* unit: second */
typedef uint32  sal_stp_pri_t;
typedef uint32  sal_stp_pathCost_t;

typedef enum sal_stp_portEdge_e
{
    SAL_STP_PORT_EDGE_FALSE = 0,
    SAL_STP_PORT_EDGE_TRUE,
    SAL_STP_PORT_EDGE_END
} sal_stp_portEdge_t;

typedef enum sal_stp_state_e
{
    SAL_STP_STATE_DISABLED = 0,
    SAL_STP_STATE_BLOCKING,
    SAL_STP_STATE_LEARNING,
    SAL_STP_STATE_FORWARDING,
    SAL_STP_STATE_END
} sal_stp_state_t;

typedef enum sal_stp_version_e
{
    SAL_STP_VERSION_STP = 0,
    SAL_STP_VERSION_RSTP,
    SAL_STP_VERSION_END
} sal_stp_version_t;

typedef enum sal_stp_portP2p_e
{
    SAL_STP_PORT_P2P_AUTO = 0,
    SAL_STP_PORT_P2P_TRUE,
    SAL_STP_PORT_P2P_FALSE,
    SAL_STP_PORT_P2P_MAX
} sal_stp_portP2p_t;

typedef enum sal_stp_portRole_s
{
    SAL_STP_PORT_ROLE_DISABLE = 0,
    SAL_STP_PORT_ROLE_ROOT,
    SAL_STP_PORT_ROLE_DESIGNATED,
    SAL_STP_PORT_ROLE_ALTERNATE,
    SAL_STP_PORT_ROLE_BACKUP,
    SAL_STP_PORT_ROLE_MAX
} sal_stp_portRole_t;

typedef struct sal_stp_rootInfo_s
{
    uint8              rootPort_exist; /* 0: not exist; 1: exist */
    sal_port_t         rootPort;
    sal_stp_time_t     rootMaxAge;
    sal_stp_time_t     rootHelloTime;
    sal_stp_time_t     rootForwardDelay;
    sal_stp_pri_t      rootPriority;
    sal_stp_pathCost_t rootPathCost;    
    sal_mac_t          rootMac;
} sal_stp_rootInfo_t;    

typedef struct sal_stp_portPara_s
{
    sal_stp_pathCost_t pathCost;
    sal_stp_pri_t      priority;    
    sal_stp_portP2p_t  p2p;
    sal_stp_portEdge_t edge;    
} sal_stp_portPara_t;    

typedef struct sal_stp_portStatus_s
{
    sal_stp_state_t    state;
    sal_stp_portRole_t role;
    sal_stp_pathCost_t actualPathCost;
    sal_stp_portP2p_t  actualP2p;
    sal_stp_portEdge_t actualEdge;
} sal_stp_portStatus_t;    

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
extern int32 sal_stp_state_set(sal_enable_t enable);

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
extern int32 sal_stp_state_get(sal_enable_t *pEnable);

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
 * Note:
 *      None
 */
extern int32 sal_stp_version_set(sal_stp_version_t version);

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
extern int32 sal_stp_version_get(sal_stp_version_t *pVersion);

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
 * Note:
 *      None
 */
extern int32 sal_stp_priority_set(sal_stp_pri_t priority);

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
extern int32 sal_stp_priority_get(sal_stp_pri_t *pPriority);

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
 * Note:
 *      None
 */
extern int32 sal_stp_timePara_set(sal_stp_time_t maxAge, sal_stp_time_t helloTime, sal_stp_time_t fwdDelay);

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
extern int32 sal_stp_timePara_get(sal_stp_time_t *pMaxAge, sal_stp_time_t *pHelloTime, sal_stp_time_t *pFwdDelay);

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
 * Note:
 *      None
 */
extern int32 sal_stp_rootInfo_get(sal_stp_rootInfo_t *pRootInfo);

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
 * Note:
 *      None
 */
extern int32 sal_stp_portPara_set(sal_port_t port, sal_stp_portPara_t *pPortPara);

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
extern int32 sal_stp_portPara_get(sal_port_t port, sal_stp_portPara_t *pPortPara);

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
extern int32 sal_stp_portStatus_get(sal_port_t port, sal_stp_portStatus_t *pPortStatus);

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
extern int32 sal_stp_mstpState_set(sal_stg_t msti, sal_port_t port, sal_stp_state_t stp_state);


/* Function Name:
 *      sal_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStp_state - pointer buffer of spanning tree state
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_stp_mstpState_get(sal_stg_t msti, sal_port_t port, sal_stp_state_t *pStp_state);

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
extern int32 sal_stp_chip_init();

/* Function Name:
 *      sal_stp_config_restore
 * Description:
 *      Restore spanning tree configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_stp_config_restore();

#endif/* __SAL_STP_H__ */

