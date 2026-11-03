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
 * Feature : Trunk
 *
 */
#ifndef __SAL_TRUNK_H__
#define __SAL_TRUNK_H__

#include <saltype.h>
#include <salport.h>

/* Function Name:
 *      sal_trunk_config_restore
 * Description:
 *      Restore trunk configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_config_restore(void);


/* Function Name:
 *      sal_trunk_port_set
 * Description:
 *      Set the trunk member port mask
 * Input:
 *      trkId - Trunk ID
 *      is_lacp - static or lacp trunk
 *      pPortMask - Pointer to the member port mask
 *      pActMask - Pointer to lacp active member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_TRUNK_PORT_USED_BY_TRK
 *      SAL_ERR_TRUNK_PORT_VLAN_CONFLICT
 *      SAL_ERR_TRUNK_PORT_IN_MIRROR
 *      SAL_ERR_TRUNK_PORT_RATE_LIMITED
 *      SAL_ERR_TRUNK_PORT_NEGO
 *      SAL_ERR_TRUNK_PORT_SPEED
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_port_set(uint8 trkId, uint8 is_lacp, sal_pPortmask_t *pPortMask, sal_pPortmask_t *pActMask);


/* Function Name:
 *      sal_trunk_port_del
 * Description:
 *      Delete all the trunk member ports of a trunk
 * Input:
 *      trkId - Trunk ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_TRUNK_TRK_RANGE
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_port_del(uint8 trkId);


/* Function Name:
 *      sal_trunk_port_get
 * Description:
 *      Get the trunk member ports of a trunk
 * Input:
 *      trkId - Trunk ID
 * Output:
 *      pIs_lacp ¡V Pointer to the static/lacp trunk type
 *      pPortMask - Pointer to the trunk member port mask
 *      pActMask - Pointer to the trunk active port mask 
 * Return:
 *      SAL_ERR_TRUNK_TRK_RANGE
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_port_get(uint8 trkId, uint8 *pIs_lacp, sal_pPortmask_t *pPortMask, sal_pPortmask_t *pActMask);


/* Function Name:
 *      sal_trunk_logicId_toStr
 * Description:
 *      Translate the logical port ID to a logical port string, such as "Port 1", "CPU", or "Trunk 2"
 * Input:
 *      logicId - Logical port ID
 * Output:
 *      pLogicIdStr - Logical port ID string
 * Return:
 *      SAL_ERR_TRUNK_TRK_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_logicId_toStr(int8 *pLogicIdStr, sal_port_t logicId);


/* Function Name:
 *      sal_trunk_logicId_toPhysicId
 * Description:
 *      Translate the logical port ID to a physical port ID
 * Input:
 *      logicId - Logical port ID
 * Output:
 *      pPhysicId - Pointer to the physical port ID
 * Return:
 *      SAL_ERR_TRUNK_TRK_NO_EXIST
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS 
 * Note:
 *      None
 */
extern int32 sal_trunk_logicId_toPhysicId(sal_port_t *pPhysicId, sal_port_t logicId);


/* Function Name:
 *      sal_trunk_physicId_toLogicId
 * Description:
 *      Translate the physical port ID to a logical port ID
 * Input:
 *      physicId - Physical port ID
 * Output:
 *      pLogicId - Pointer to the logical port ID
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_physicId_toLogicId(sal_port_t *pLogicId, sal_port_t physicId);


/* Function Name:
 *      sal_trunk_logicMask_toPhysicMask
 * Description:
 *      Translate the logical port mask to a physical port mask
 * Input:
 *      pLPortMask - Pointer to the source logical port mask
 * Output:
 *      pPPortMask - Pointer to the target physical port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_logicMask_toPhysicMask(sal_pPortmask_t *pPPortMask, sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_trunk_physicMask_toLogicMask
 * Description:
 *      Translate the physical port mask to a logical port mask
 * Input:
 *      pPPortMask - Pointer to the source physical port mask
 * Output:
 *      pLPortMask - Pointer to the target logical port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_physicMask_toLogicMask(sal_lPortmask_t *pLPortMask, sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_trunk_logicId_toPhysicMask
 * Description:
 *      Translate the logical port ID to a phyical port mask
 * Input:
 *      logicId - The logical port ID
 * Output:
 *      pPPortMask - Pointer to the target physical port mask
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_logicId_toPhysicMask(sal_pPortmask_t *pPPortMask, sal_port_t logicId);


/* Function Name:
 *      sal_trunk_logicPort_setAll
 * Description:
 *      Set all the logical ports (without trunked ports) for a port mask
 * Input:
 *      None
 * Output:
 *      pLPortMask - Pointer to the logical port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_logicPort_setAll(sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_trunk_logicId_exist
 * Description:
 *      Check if a logical port ID is existed
 * Input:
 *      logicId - The logical port ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_trunk_logicId_exist(sal_port_t logicId);

/* Function Name:
 *      sal_trunk_representPort_get
 * Description:
 *      Get a represent port of the trunk of a physical port
 * Input:
 *      port - Physical port number
 * Output:
 *      pLport - Port number of the represent port.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_trunk_representPort_get(sal_port_t port, sal_port_t *pLport);

extern int32 sal_trunk_logicId_toStr_rlpp(int8 *pLogicIdStr, sal_port_t logicId);


#endif/*__SAL_TRUNK_H__*/

