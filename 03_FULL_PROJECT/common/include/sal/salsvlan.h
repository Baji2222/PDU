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
 * $Revision: 6226 $
 * $Date: 2009-10-01 18:07:30 +0800 (Thu, 01 Oct 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : SVLAN
 *
 */
#ifndef __SALSVLAN_H__
#define __SALSVLAN_H__

#include <saltype.h>

/* 
 * Symbol Definition
 */

typedef struct sal_svlan_entry_e
{
    sal_vlan_t      vlan_id;
    uint8           name[SAL_MAX_SVLAN_NAME_LEN];
    sal_pPortmask_t p_portMask;
    uint8           valid;     /* 1: valid, 0: invalid */
} sal_svlan_entry_t;


/* Function Name:
 *      sal_svlan_config_restore
 * Description:
 *      Restore SVLAN configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_config_restore(void);

/* Function Name:
 *      sal_svlan_tpid_set
 * Description:
 *      Set system TPID of SVLAN
 * Input:
 *      tpid - TPID of SVLAN
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_tpid_set(sal_vlan_t tpid);

/* Function Name:
 *      sal_svlan_tpid_get
 * Description:
 *      Get system TPID of SVLAN
 * Input:
 *      None
 * Output:
 *      pTpid - Pointer to system TPID
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_tpid_get(sal_vlan_t *pTpid);

/* Function Name:
 *      sal_svlan_servicePort_set
 * Description:
 *      Set service ports
 * Input:
 *      pPortMask - Pointer to the service port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_servicePort_set(sal_pPortmask_t *pPortMask);

/* Function Name:
 *      sal_svlan_servicePort_get
 * Description:
 *      Get service ports
 * Input:
 *      None
 * Output:
 *      pPortMask - Pointer to the service port mask
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_servicePort_get(sal_pPortmask_t *pPortMask);

/* Function Name:
 *      sal_svlan_name_change
 * Description:
 *      Change the name of the service VLAN
 * Input:
 *      svlan_id - Service VLAN ID
 *      pcName  - string of service VLAN name
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_name_change(sal_vlan_t svlan_id, uint8 *pcName);


/* Function Name:
 *      sal_svlan_sttcEntry_create
 * Description:
 *      Create a static VLAN
 * Input:
 *      svlan_id - service VLAN ID
 *      pcName   - string of service VLAN name
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_sttcEntry_create(sal_vlan_t svlan_id, uint8 *pcName);


/* Function Name:
 *      sal_svlan_sttcEntry_add
 * Description:
 *      Add a static S-VLAN entry
 * Input:
 *      svlan_id            - S-VLAN ID*
 *      pName               - pointer to the S-VLAN name
 *      pPortMask           - pointer to the S-VLAN member portmask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_SVLAN_TABLE_FULL
 *      SAL_ERR_VLAN_ID_INVALID
 * Note:
 */
extern int32 sal_svlan_sttcEntry_add(
    sal_vlan_t svlan_id, 
    int8       *pName, 
    sal_lPortmask_t *pPortMask
    );     


/* Function Name:
 *      sal_svlan_sttcEntry_set
 * Description:
 *      Set the member ports and tagged ports of a static Service VLAN
 * Input:
 *      svlan_id    - Service VLAN ID
 *      pPortMask   - Service VLAN member port mask
 *      pTagMask    - Service VLAN tag port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_sttcEntry_set(sal_vlan_t svlan_id, sal_lPortmask_t *pPortMask);


/* Function Name:
 *      sal_svlan_sttcEntry_remove
 * Description:
 *      Remove a static service VLAN entry
 * Input:
 *      svlan_id - Service VLAN ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_sttcEntry_remove(sal_vlan_t svlan_id);


/* Function Name:
 *      sal_svlan_sttcEntry_get
 * Description:
 *      Get the member port mask and tag port mask a static service VLAN entry
 * Input:
 *      svlan_id     - Service VLAN ID
 * Output:
 *      pPortMask   - pointer to the service VLAN member port mask
 *      pTagMask    - pointer to the service VLAN tag port mask
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_sttcEntry_get(sal_vlan_t svlan_id, sal_lPortmask_t *pPortMask);

/* Function Name:
 *      sal_svlan_defaultSvid_set
 * Description:
 *      Set the default SVID for uplink packets
 * Input:
 *      svlan_id    - Service VLAN ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_defaultSvid_set(sal_vlan_t svlan_id);

/* Function Name:
 *      sal_svlan_defaultSvid_get
 * Description:
 *      Get the default SVID for uplink packets
 * Input:
 *      none
 * Output:
 *      pSvlan_id    - Pointer to the default service VID
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_svlan_defaultSvid_get(sal_vlan_t *pSvlan_id);

/* Function Name:
 *      sal_svlan_index_entry_get
 * Description:
 *      Get SVLAN entry by table index
 * Input:
 *      index - SVLAN index
 *      pEntry - Pointer to the SVLAN entry
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_svlan_index_entry_get(uint16 index, sal_svlan_entry_t *pEntry);

/* Function Name:
 *      sal_svlan_index_entry_set
 * Description:
 *      Set SVLAN entry by table index
 * Input:
 *      index - SVLAN index
 *      action - Command type. Used to indicate save to chip or configuration or both. Please reference to sal_entry_action_t.
 * Output:
 *      pEntry - Pointer to the SLVAN entry
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_svlan_index_entry_set(uint16 index, sal_svlan_entry_t *pEntry, sal_entry_action_t action); 

#endif

