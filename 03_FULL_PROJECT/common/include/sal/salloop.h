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
#ifndef __SAL_LOOP_H__
#define __SAL_LOOP_H__

#include <saltype.h>

#if SAL_LOOP

typedef enum sal_loop_mode_e
{
    SAL_LOOP_MODE_DISABLE = 0,
    SAL_LOOP_MODE_RLPP,
    SAL_LOOP_MODE_RLDP,
    SAL_LOOP_MODE_STP,
    SAL_LOOP_MODE_END
} sal_loop_mode_t;

typedef enum sal_loop_port_status_e
{
    SAL_LOOP_STATUS_FREE = 0,
    SAL_LOOP_STATUS_RLPP,
    SAL_LOOP_STATUS_RLDP,
    SAL_LOOP_END
} sal_loop_port_status_t;

typedef enum sal_rlpp_port_state_e
{
    SAL_RLPP_DISABLE_STATE = 0,
    SAL_RLPP_BLOCKING_STATE,
    SAL_RLPP_LISTENING_STATE,
    SAL_RLPP_FORWARDING_STATE
} sal_rlpp_port_state_t;


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
extern int32 sal_loop_mode_get(sal_loop_mode_t *pMode);

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
extern int32 sal_loop_mode_set(sal_loop_mode_t mode);

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
extern int32 sal_loop_portStatus_get(sal_port_t port, sal_loop_port_status_t *pStatus);

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
extern int32 sal_loop_rlppRxPmsk_set(sal_lPortmask_t *pPmsk);

/* Function Name:
 *      sal_loop_rlppRxPmsk_get
 * Description:
 *      Get ports that can receive RLPP packet
 * Input:
 *      None.
 * Output:
 *      pPmsk  - Ports that can receive RLPP packet
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_rlppRxPmsk_get(sal_lPortmask_t *pPmsk);

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
extern int32 sal_loop_config_restore(void);


/* Function Name:
 *      sal_loop_rlppLoopRemove_set
 * Description:

 *      Set action when root is removed.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_loop_rlppRootRemove_set(void);

/* Function Name:
 *      sal_loop_interval_time_get
 * Description:
 *      Get loop interval time
 * Input:
 *      None.
 * Output:
 *      pTime  - RLPP interval time
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_interval_time_get(uint16 *pTime);

/* Function Name:
 *      sal_loop_interval_time_set
 * Description:
 *      Set loop interval time
 * Input:
 *      Time  - RLPP interval time
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_interval_time_set(uint16 Time);

/* Function Name:
 *      sal_loop_recover_time_get
 * Description:
 *      Get loop recover time
 * Input:
 *      None.
 * Output:
 *      pTime  - RLPP recover time
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_recover_time_get(uint32 *pTime);

/* Function Name:
 *      sal_loop_recover_time_set
 * Description:
 *      Set loop recover time
 * Input:
 *      Time  - RLPP recover time
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_recover_time_set(uint32 Time);

/* Function Name:
 *      sal_loop_port_enable_set
 * Description:
 *      enable/disable per port loop function
 * Input:
 *      PortId    - port ID
 *      enable  - port status
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_port_enable_set(uint16 PortId, uint16 enable);

/* Function Name:
 *      sal_loop_port_enable_get
 * Description:
 *      Get  port loop function status
 * Input:
 *      PortId    - port ID
 * Output:
 *      pEnable  - port status
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_port_enable_get(uint16 ucPortId, uint16 *pEnable);

/* Function Name:
 *      sal_loop_port_state_get
 * Description:
 *      Get  port loop port status
 * Input:
 *      PortId    - port ID
 * Output:
 *      pState  - port state
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_loop_port_state_get(uint16 ucPortId, uint16 *pState);

extern int32 sal_loop_portRange_check(sal_port_t port);

#endif

#endif/*__SAL_LOOP_H__*/
