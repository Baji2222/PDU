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
 * Feature : L2 Address Table
 *
 */
#ifndef __SALL2_H__
#define __SALL2_H__
#include <saltype.h>

typedef enum sal_l2_type_e
{
	SAL_L2_UCAST = 0,
	SAL_L2_MCAST,
	SAL_L2_IP_MCAST,
	SAL_L2_MAC_FILTER
} sal_l2_type_t;

typedef enum sal_l2_clr_e
{
	SAL_L2_CLR_ALL,
	SAL_L2_CLR_STATIC,
	SAL_L2_CLR_DYNAMIC
} sal_l2_clr_t;

/* l2 limit learning count action */
typedef enum sal_l2_limitLearnCntAction_e
{
    SAL_LIMIT_LEARN_CNT_ACTION_DROP = 0,
    SAL_LIMIT_LEARN_CNT_ACTION_FORWARD,
    SAL_LIMIT_LEARN_CNT_ACTION_TO_CPU,
    SAL_LIMIT_LEARN_CNT_ACTION_END
} sal_l2_limitLearnCntAction_t;

typedef struct sal_l2_ucastAddr_s
{
    sal_mac_t       mac;
    sal_vlan_t      vid;
    uint8           isStatic;
    sal_port_t      srcPort;
    uint8           saBlock;
    uint8           daBlock;
} sal_l2_ucastAddr_t;

typedef struct sal_l2_sttcEntry_s {
    sal_mac_t       mac;
    sal_vlan_t      fid;
    uint8           auth;
    uint8           saBlock;
    uint8           daBlock;
    sal_pPortmask_t pPortMask;
    uint8           valid;     /* 1: valid, 0: invalid */
} sal_l2_sttcEntry_t;

/* Function Name:
 *      sal_l2_config_restore
 * Description:
 *      Restore L2 configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_l2_config_restore (void);

/* Function Name:
 *      sal_l2_table_clear
 * Description:
 *      Clear the l2 address table based on specified type
 * Input:
 *      type	-	L2 entry type
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_table_clear (sal_l2_clr_t type);

/* Function Name:
 *      sal_l2_entry_add
 * Description:
 *      Add a unicast mac entry
 * Input:
 *      pMac        -	MAC address
 *      vid         -	VLAN ID
 *      is_static   -	static or dynamic entry
 *      is_auth     -	authenticated MAC address
 *      sa_block    -	source MAC filter
 *      da_block    -	destination MAC filter
 *      pPortMask   -	member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_entry_add(
    sal_mac_t *pMac,
    sal_vlan_t vid,
    uint8 is_static,
    uint8 is_auth,
    uint8 sa_block,
    uint8 da_block,
    sal_pPortmask_t *pPortMask);


/* Function Name:
 *      sal_l2_entry_del
 * Description:
 *      Delete a L2 MAC entry
 * Input:
 *      pMac        -   MAC address
 *      vid     -   VLAN ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NOT_SUPPORT
 * Note:
 *      None
 */
extern int32 sal_l2_entry_del(sal_mac_t *	pMac, sal_vlan_t vid);

/* Function Name:
 *      sal_l2_sramEntry_get
 * Description:
 *      Get a L2 entry from SRAM by Index
 * Input:
 *      index		-	entry index
 * Output:
 *      pMac		-	MAC address
 *      pVid		-	VLAN ID
 *      pStatic	-	static or dynamic entry
 *      pType		-	L2 unicast or L2 multicast or IP multicast
 *      pPortMask	-	member port mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_MAC_ENTRY_INVALID
 * Note:
 *      None
 */
extern int32 sal_l2_sramEntry_get (
    uint32 index,
    sal_mac_t  *pMac,
    sal_vlan_t *pVid,
    uint8   *pStatic,
    sal_l2_type_t   *pType,
    sal_pPortmask_t *pPortMask);

/* Function Name:
 *      sal_l2_addr_get
 * Description:
 *      Get a L2 entry from ASIC with adress being L2 unicast.
 * Input:
 *      pMac        -   MAC address
 *      vid         -   VLAN ID 
 * Output:
 *      pEntry      -   Pointer to the L2 entry.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_MAC_ENTRY_NOT_FOUND 
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_l2_addr_get (
    sal_mac_t           *pMac,
    sal_vlan_t          vid,
    sal_l2_ucastAddr_t  *pEntry
);

/* Function Name:
 *      sal_l2_dlfPortMsk_set
 * Description:
 *      Set the flooding port mask for destination lookup fail packet
 * Input:
 *      None
 * Output:
 *      pPortMask     -   flooding port mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_UNKNOWN
 * Note:
 *      None
 */
extern int32 sal_l2_dlfPortMsk_set(sal_pPortmask_t *pPortMask);

/* Function Name:
 *      sal_l2_entry_get
 * Description:
 *      Get L2 entry from configuration by index.
 * Input:
 *      index - Index of L2 entry in configuration
 * Output:
 *      pEntry - Pointer to the entry.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_UNKNOWN
 * Note:
 *      None
 */
extern int32 sal_l2_entry_get(uint16 index, sal_l2_sttcEntry_t *pEntry);

/* Function Name:
 *      sal_l2_entry_set
 * Description:
 *      Set L2 entry from configuration by index.
 * Input:
 *      index - Index of L2 entry in configuration
 *      pEntry - Pointer to the entry. 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_UNKNOWN
 * Note:
 *      None
 */
extern int32 sal_l2_entry_set(uint16 index, sal_l2_sttcEntry_t *pEntry, sal_entry_action_t action);

/* Function Name:
 *      sal_l2_limitLearningCnt_set
 * Description:
 *      Set the allowed learned count of port.
 * Input:
 *      port     - Port id
 *      mac_cnt  - Pointer to allowed learned count 
 * Output:
        None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_limitLearningCnt_set(sal_port_t port, uint32 mac_cnt);

/* Function Name:
 *      sal_l2_limitLearningCnt_get
 * Description:
 *      Get the allowed learned count of port.
 * Input:
 *      port     - Port id
 * Output:
 *      pMac_cnt - Pointer to allowed learned count
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_limitLearningCnt_get(sal_port_t port, uint32 *pMac_cnt);

/* Function Name:
 *      sal_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when learned count is over.
 * Input:
 *      port    - port id
 *      pAction - pointer to the action
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_limitLearningCntAction_set(sal_port_t port, sal_l2_limitLearnCntAction_t action);

/* Function Name:
 *      sal_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when learned count is over.
 * Input:
 *      port    - port id
 * Output:
 *      pAction - pointer to the action
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_limitLearningCntAction_get(sal_port_t port, sal_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      sal_l2_learnedEntry_flush
 * Description:
 *      Flush leanred entry of a port
 * Input:
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_l2_learnedEntry_flush(sal_port_t port);


#endif/*__SALL2_H__*/

