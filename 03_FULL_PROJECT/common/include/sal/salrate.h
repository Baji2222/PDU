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
 * Feature : Ingress and Egress Rate/Storm Control/Jumbo Frame
 *
 */
#ifndef __SAL_RATE_H__
#define __SAL_RATE_H__

#include <saltype.h>
#include <salport.h>

typedef enum sal_stormCtrl_type_e
{
	SAL_STORM_TYPE_UNKNOWN_UCAST = 0,
	SAL_STORM_TYPE_UNKNOWN_MCAST,
	SAL_STORM_TYPE_MCAST,
	SAL_STORM_TYPE_BCAST
} sal_stormCtrl_type_t;

typedef enum sal_jumbo_size_e
{
	SAL_JUMBO_FRAME_1522,
	SAL_JUMBO_FRAME_1536,
	SAL_JUMBO_FRAME_1552,
	SAL_JUMBO_FRAME_9216,
	SAL_JUMBO_FRAME_16k = SAL_JUMBO_FRAME_9216,
} sal_jumbo_size_t;


/* Function Name:
 *      sal_rate_config_restore
 * Description:
 *      Restore rate configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_rate_config_restore(void);

/* Function Name:
 *      sal_rate_stormControl_set
 * Description:
 *      Set storm control rate
 * Input:
 *      port    - port ID
 *      type    - storm type
 *      rate    - storm rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_STRM_CTRL_UNKNOWN_FILTER
 *      SAL_ERR_STRM_CTRL
 * Note:
 *      None
 */
extern int32 sal_rate_stormControl_set(
    sal_port_t port,
    sal_stormCtrl_type_t type,
    uint32 rate);

/* Function Name:
 *      sal_rate_stormControl_get
 * Description:
 *      Get storm control rate
 * Input:
 *      port    - port ID
 *      type    - storm type
 * Output:
 *      pRate   - storm rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_STRM_CTRL_UNKNOWN_FILTER
 *      SAL_ERR_STRM_CTRL
 * Note:
 *      None
 */
extern int32 sal_rate_stormControl_get(
    sal_port_t port,
    sal_stormCtrl_type_t type,
    uint32 *pRate);

/* Function Name:
 *      sal_rate_cpuEgrCtrl_set
 * Description:
 *      Set egress bandwidth control on CPU port
 * Input:
 *      enable  	- state of CPU port egress bandwidth control
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_cpuEgrCtrl_set(sal_enable_t enable);

/* Function Name:
 *      sal_rate_cpuEgrCtrl_get
 * Description:
 *      Get egress bandwidth control on CPU port
 * Input:
 *      None
 * Output:
 *      pEnable  	- state of CPU port egress bandwidth control
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_rate_cpuEgrCtrl_get(sal_enable_t *pEnable);

/* Function Name:
 *      sal_rate_igrIncludeIFG_set
 * Description:
 *      Set ingress bandwidth control to include IFG
 * Input:
 *      include	- include or exclude
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_igrIncludeIFG_set(uint8 include);

/* Function Name:
 *      sal_rate_igrIncludeIFG_get
 * Description:
 *      Get ingress bandwidth control IFG setting
 * Input:
 *      None
 * Output:
 *      pInclude	- include or exclude
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_igrIncludeIFG_get(uint8 *pInclude);

/* Function Name:
 *      sal_rate_egrIncludeIFG_set
 * Description:
 *      Set egress bandwidth control to include IFG
 * Input:
 *      include	- include or exclude
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_egrIncludeIFG_set(uint8 include);

/* Function Name:
 *      sal_rate_egrIncludeIFG_get
 * Description:
 *      Get egress bandwidth control IFG setting
 * Input:
 *      None
 * Output:
 *      pInclude	- include or exclude
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_egrIncludeIFG_get(uint8 *pInclude);

/* Function Name:
 *      sal_rate_igrBandwidthCtrl_set
 * Description:
 *      Set ingress bandwidth control
 * Input:
 *      port    - port ID
 *      rate    - Ingress rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_igrBandwidthCtrl_set(sal_port_t port, uint32 rate);

/* Function Name:
 *      sal_rate_igrBandwidthCtrl_get
 * Description:
 *      Get ingress bandwidth control
 * Input:
 *      port    - port ID
 * Output:
 *      pRate   - Ingress rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_igrBandwidthCtrl_get(sal_port_t port, uint32 *pRate);

#if defined(RLPP_PATCH2_8367N)
extern int32 sal_rate_igrBandwidthCtrl_set_old(sal_port_t port, uint32 rate);
extern int32 sal_rate_igrBandwidthCtrl_get_old(sal_port_t port, uint32 *pRate);
#endif

/* Function Name:
 *      sal_rate_egrBandwidthCtrl_set
 * Description:
 *      Set egress bandwidth control
 * Input:
 *      port    - port ID
 *      rate    - Egress rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_egrBandwidthCtrl_set(sal_port_t port, uint32 rate);

/* Function Name:
 *      sal_rate_egrBandwidthCtrl_get
 * Description:
 *      Get egress bandwidth control
 * Input:
 *      port    - port ID
 * Output:
 *      pRate   - Egress rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_egrBandwidthCtrl_get(sal_port_t port, uint32 *pRate);

/* Function Name:
 *      sal_rate_maxFrameSize_set
 * Description:
 *      Set the maximum frame size
 * Input:
 *      size	- Maximum frame size
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_JUMBO_FRAME_SIZE
 * Note:
 *      None
 */
extern int32 sal_rate_maxFrameSize_set(uint32	size);

/* Function Name:
 *      sal_rate_maxFrameSize_get
 * Description:
 *      Get the maximum frame size
 * Input:
 *      None
 * Output:
 *      pSize	- Maximum frame size
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_rate_maxFrameSize_get(uint32 *pSize);

#endif/*__SAL_RATE_H__*/

