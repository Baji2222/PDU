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
 * Feature : VLAN
 *
 */
#ifndef __SAL_VLAN_H__
#define __SAL_VLAN_H__

//#include <salflash.h>
#include <saltype.h>

/* tagged mode of VLAN - reference realtek private specification */
typedef enum sal_vlan_tagMode_e
{    
    SAL_VLAN_TAG_MODE_ORIGINAL = 0,
    SAL_VLAN_TAG_MODE_KEEP_FORMAT,
    SAL_VLAN_TAG_MODE_PRI,
    SAL_VLAN_TAG_MODE_END
} sal_vlan_tagMode_t;

/* acceptable frame type of 802.1Q and 802.1ad */
typedef enum sal_vlan_acceptFrameType_e
{    
    SAL_ACCEPT_FRAME_TYPE_ALL = 0,          /* untagged, priority-tagged and tagged */
    SAL_ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged                               */
    SAL_ACCEPT_FRAME_TYPE_UNTAG_ONLY,       /* untagged and priority-tagged         */
    SAL_ACCEPT_FRAME_TYPE_PRITAG_AND_TAG,   /* priority-tagged and tagged           */
    SAL_ACCEPT_FRAME_TYPE_END
} sal_vlan_acceptFrameType_t;

typedef struct sal_vlan_entry_e
{
    sal_vlan_t      vlan_id;
    uint8           name[SAL_MAX_VLAN_NAME_LEN];
    sal_pPortmask_t p_portMask;
    sal_pPortmask_t p_tagMask;
    uint8           valid;     /* 1: valid, 0: invalid */
} sal_vlan_entry_t;

/* Function Name:
 *      sal_vlan_config_restore
 * Description:
 *      Restore VLAN configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_config_restore(void);


/* Function Name:
 *      sal_vlan_enable_set
 * Description:
 *      Set the VLAN enable status
 * Input:
 *      enable - VLAN state
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      
 * Note:
 *      None
 */
extern int32 sal_vlan_enable_set(sal_enable_t enable);


/* Function Name:
 *      sal_vlan_enable_get
 * Description:
 *      Get the VLAN enable status
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the VLAN enalbe status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_enable_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_vlan_sttcEntry_create
 * Description:
 *      Create a static VLAN
 * Input:
 *      vlan_id - VLAN ID
 *      pName  - string of VLAN name
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_INPUT_STR_LEN
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_ID_INVALID
 *      SAL_ERR_VLAN_FULL
 *      SAL_ERR_VLAN_EXIST
 * Note:
 *      None
 */
extern int32 sal_vlan_sttcEntry_create(sal_vlan_t vlan_id, int8 *pName);


/* Function Name:
 *      sal_vlan_sttcEntry_set
 * Description:
 *      Set the member ports and tagged ports of a static VLAN
 * Input:
 *      vlan_id     - VLAN ID
 *      pPortMask   - VLAN member port mask
 *      pTagMask    - VLAN tag port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_ID_INVALID
 *      SAL_ERR_VLAN_FULL
 *      SAL_ERR_VLAN_NOT_EXIST
 *      SAL_ERR_VLAN_INS_VLAN_ENTRY
 *      SAL_ERR_SUCCESS
 *      
 * Note:
 *      None
 */
extern int32 sal_vlan_sttcEntry_set(sal_vlan_t vlan_id, sal_lPortmask_t *pPortMask, sal_lPortmask_t *pTagMask);


/* Function Name:
 *      sal_vlan_name_change
 * Description:
 *      Change the name of the VLAN
 * Input:
 *      vlan_id - VLAN ID
 *      pName  - string of VLAN name
 * Output:
 *      None
 * Return:
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_ID_INVALID
 *      SAL_ERR_VLAN_FULL
 *      SAL_ERR_VLAN_NOT_EXIST
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_name_change(sal_vlan_t vlan_id, int8 *pName);


/* Function Name:
 *      sal_vlan_sttcEntry_get
 * Description:
 *      Get the member port mask and tag port mask a static VLAN entry
 * Input:
 *      vlan_id     - VLAN ID
 * Output:
 *      pPortMask   - pointer to the VLAN member port mask
 *      pTagMask    - pointer to the VLAN tag port mask
 * Return:
 *      SAL_ERR_VLAN_NOT_EXIST
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_sttcEntry_get(sal_vlan_t vlan_id, sal_lPortmask_t *pPortMask, sal_lPortmask_t *pTagMask);


/* Function Name:
 *      sal_vlan_sttcEntry_remove
 * Description:
 *      Remove a static VLAN entry
 * Input:
 *      vlan_id - VLAN ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_EXIST_PVID
 *      SAL_ERR_VLAN_NOT_EXIST
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_VLAN_DEL_VLAN_ENTRY
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_sttcEntry_remove(sal_vlan_t vlan_id);


/* Function Name:
 *      sal_vlan_sttcEntry_add
 * Description:
 *      Add a static VLAN entry
 * Input:
 *      sal_vlan_t vlan_id  - VLAN ID*
 *      pName               - pointer to the VLAN name
 *      pPortMask           - pointer to the VLAN member portmask
 *      pTagMask            - pointer to the tagged portmask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_EXIST_PVID
 *      SAL_ERR_VLAN_NOT_EXIST
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_VLAN_DEL_VLAN_ENTRY
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_sttcEntry_add(sal_vlan_t vlan_id, int8 *pName, sal_lPortmask_t *pPortMask, sal_lPortmask_t *pTagMask);


/* Function Name:
 *      sal_vlan_entry_get
 * Description:
 *      Get the member port mask, tag port mask, FID, multiple spanning tree ID of VLAN entry
 * Input:
 *      vlan_id - VLAN ID
 * Output:
 *      pPortMask   - pointer to the VLAN member port mask
 *      pTagMask    - pointer to the VLAN tag port mask
 *      pFId        - pointer to the FID
 *		pMsti       - pointer to the multiple spanning tree ID
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_INPUT_STR_LEN
 *      SAL_ERR_VLAN_DISABLE
 *      SAL_ERR_VLAN_ID_INVALID
 *      SAL_ERR_VLAN_FULL
 *      SAL_ERR_VLAN_EXIST
 * Note:
 *      None
 */
extern int32 sal_vlan_entry_get(sal_vlan_t vlan_id, sal_lPortmask_t *pstLPortMask, sal_lPortmask_t *stLTagMask, sal_fid_t *pFId, sal_stg_t *pMsti);


/* Function Name:
 *      sal_vlan_db_insert
 * Description:
 *      Insert a VLAN to sorted local database for faster search
 * Input:
 *      vlan_id         - VLAN ID
 *      sorted_slot_idx - index of an array that stores sorted database slot index (sorted by stored VLAN ID)
 *      slot_idx        - index of database slot to store the VLAN
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_vlan_db_insert(sal_vlan_t vlan_id, uint16 sorted_slot_idx, uint16 slot_idx);


/* Function Name:
 *      sal_vlan_db_search
 * Description:
 *      Search the local database for a specific VLAN
 * Input:
 *      vlan_id - VLAN ID
 * Output:
 *      pSorted_slot_idx 	- pointer to the index of an array that stores sorted database slot index
 *      pSlot_idx 			- pointer to the index of database slot that stores the VLAN
 * Return:
 *      TRUE - The specific VLAN exist
 *      FALSE - The specific VLAN does not exist
 * Note:
 *      None
 */
extern int32 sal_vlan_db_search(sal_vlan_t vlan_id, uint16 *pSorted_slot_idx, uint16 *pSlot_idx);

/* Function Name:
 *      sal_vlan_db_del
 * Description:
 *      Delete a VLAN in sorted local database 
 * Input:
 *      sorted_slot_idx - index of an array that stores sorted database slot index (sorted by stored VLAN ID)
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_vlan_db_del(uint16 sorted_slot_idx);

/* Function Name:
 *      sal_vlan_portPvid_set
 * Description:
 *      Set the PVID of a specific port
 * Input:
 *      port - port id
 *      vlan_id - port VLAN ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_portPvid_set(sal_port_t port, sal_vlan_t vlan_id);


/* Function Name:
 *      sal_vlan_portPvid_get
 * Description:
 *      Get the PVID of a specific port
 * Input:
 *      port        - port ID
 * Output:
 *      pVlan_id    - pointer to the port VLAN ID
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_portPvid_get(sal_port_t port, sal_vlan_t *pVlan_id);


/* Function Name:
 *      sal_vlan_tagMode_set
 * Description:
 *      Set the VLAN tag mode of a specific port
 * Input:
 *      port - port ID
 *      mode - VLAN mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_tagMode_set(sal_port_t port, sal_vlan_tagMode_t mode);


/* Function Name:
 *      sal_vlan_tagMode_get
 * Description:
 *      Get the VLAN tag mode of a specific port
 * Input:
 *      port  - port ID
 * Output:
 *      pMode - pointer to the VLAN mode
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_tagMode_get(sal_port_t port, sal_vlan_tagMode_t *pMode);

/* Function Name:
 *      sal_vlan_port_AcceptFrameType_set
 * Description:
 *      Set the VLAN accept frame type of a specific port
 * Input:
 *      port  - port ID
 *      type  - VLAN accept frame type
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_port_AcceptFrameType_set(sal_port_t port, sal_vlan_acceptFrameType_t type);


/* Function Name:
 *      sal_vlan_port_AcceptFrameType_get
 * Description:
 *      Get the VLAN accept frame type of a specific port
 * Input:
 *      port  - port ID
 * Output:
 *      pType - pointer to the VLAN accept frame type
 * Return:
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_vlan_port_AcceptFrameType_get(sal_port_t port, sal_vlan_acceptFrameType_t *pType);

#if 0
/* Function Name:
 *      sal_vlan_index_entry_set
 * Description:
 *      Set vlan entry by entry. Setting will be saved to chip, software configuration or both.
 *      Please reference to sal_entry_action_t in saltype.h
 * Input:
 *      index - VLAN entry index (0~31)
 *      pEntry - Pointer to the VLAN entry.
 *      action - Save operation.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      
 * Note:
 *      None
 */
extern int32 sal_vlan_index_entry_set(uint16 index, sal_flash_vlanEntry_t *pEntry, sal_entry_action_t action);

/* Function Name:
 *      sal_vlan_index_entry_get
 * Description:
 *      Get vlan entry by entry. Setting will be saved to chip, software configuration or both.
 * Input:
 *      index - VLAN entry index (0~31)
 *      pEntry - Pointer to the VLAN entry.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      
 * Note:
 *      None
 */
extern int32 sal_vlan_index_entry_get(int16 index, sal_flash_vlanEntry_t *pEntry);
#endif

#ifdef __PRIVATE_VLAN__
/* Function Name:
 *      sal_vlan_privateEnable_set
 * Description:
 *      Set the Private-VLAN enable state
 * Input:
 *      enable - Private-VLAN status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      
 * Note:
 *      None
 */
extern int32 sal_vlan_privateEnable_set(sal_enable_t enable);
#endif /* __PRIVATE_VLAN__ */

extern int32 sal_vlan_index_entry_set(int16 index, sal_vlan_entry_t *pEntry, sal_entry_action_t action); 
extern int32 sal_vlan_index_entry_get(int16 index, sal_vlan_entry_t *pEntry);

#endif/*__SAL_VLAN_H__*/

