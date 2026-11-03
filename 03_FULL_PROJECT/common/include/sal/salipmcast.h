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
 * Feature : IP multicast Table
 *
 */
#ifndef __SAL_IPMCAST_H__
#define __SAL_IPMCAST_H__
#include <rtk_type.h>
#include "lwps.h"

typedef enum portmaskOp_
{
    PORTMASK_ADD = 0,
    PORTMASK_DEL,
    PORTMASK_NOP
} portmaskOp_e;

/* Function Name:
 *      sal_ipMcast_add
 * Description:
 *      Add an IP multicast etry to HW
 * Input:
 *      vid		- vlan id
 *      portMask- port mask
 * Output:
 *      pGroupIp - multicast group IP
 *      pSip - source ip
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_ADDED
 * Note:
 *      None
 */
extern int32 sal_ipMcast_add (
	ip_addr_t *pGroupIp,
	ip_addr_t *pSip,
    uint32     vid,
	uint32 	portMask);

/* Function Name:
 *      sal_ipMcast_del
 * Description:
 *      Delete an IP multicast etry from HW
 * Input:
 *      vid		- vlan id
 * Output:
 *      pGroupIp - multicast group IP
 *      pSip - source ip
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_DELETED
 * Note:
 *      None
 */
extern int32 sal_ipMcast_del(
	ip_addr_t *pGroupIp,
	ip_addr_t *pSip,
    uint32     vid);

/* Function Name:
 *      sal_ipMcast_get
 * Description:
 *      Retrieve the portmask of an IP multicast etry from HW
 * Input:
 *      vid		- vlan id
 * Output:
 *      pGroupIp - multicast group IP
 *      pSip - source ip
 *      pPortMask - port mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_FOUND
 * Note:
 *      None
 */
extern int32 sal_ipMcast_get(
	ip_addr_t *pGroupIp,
	ip_addr_t *pSip,
    uint32     vid,
	uint32 	*pPortMask);

/* Function Name:
 *      sal_ipMcast_update
 * Description:
 *      Update an IP multicast etry present in HW
 * Input:
 *      vid		- vlan id
 *      portMask - port mask
 *      portOp - operation on port mask
 * Output:
 *      pGroupIp - multicast group IP
 *      pSip - source ip
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_ADDED
 *      SAL_ERR_IPMCAST_ENTRY_NOT_DELETED
 *      SAL_ERR_IPMCAST_ENTRY_NOT_FOUND
 * Note:
 *      None
 */
extern int32 sal_ipMcast_update(
	ip_addr_t *pGroupIp,
	ip_addr_t *pSip,
    uint32     vid,
	uint32 	portMask,
	uint32 portOp);

/* Function Name:
 *      sal_ipMcast_entry_getFirst
 * Description:
 *      Get first IP multicast etry from HW
 * Input:
 *      None
 * Output:
 *      pVid		- vlan id
 *      pPortmask - port mask
 *      pDip - multicast group IP
 *      pSip - source ip
 *      pIndex - index of the first entry
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_FOUND
 * Note:
 *      None
 */
extern int32 sal_ipMcast_entry_getFirst(
	uint32		*pIndex,
	uint32		*pDip,
	uint32		*pSip,
	uint32		*pVid,
	uint32		*pPortmask);

/* Function Name:
 *      sal_ipMcast_entry_getNext
 * Description:
 *      Get next valid IP multicast etry from HW specified by the index
 * Input:
 *      None
 * Output:
 *      pVid		- vlan id
 *      pPortmask - port mask
 *      pDip - multicast group IP
 *      pSip - source ip
 *      pIndex - index of the entry
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_FOUND
 * Note:
 *      None
 */
extern int32 sal_ipMcast_entry_getNext(
    uint32     *pIndex,
    uint32     *pDip,
    uint32     *pSip,
    uint32     *pVid,
    uint32     *pPortmask);

/* Function Name:
 *      sal_ipMcast_table_clear
 * Description:
 *      Clear IP multicast table from HW
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_IPMCAST_ENTRY_NOT_FOUND
 * Note:
 *      None
 */
extern int32 sal_ipMcast_table_clear(void);

#endif/* __SAL_IPMCAST_H__ */

