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
 * Feature : LLDP (Link Layer Discovery Protocol)
 *
 */
#ifndef __SAL_LLDP_H__
#define __SAL_LLDP_H__

#include <saltype.h>

typedef uint16 sal_lldp_timer_t;
typedef uint8  sal_lldp_multi_t;

typedef enum sal_lldp_port_state_e
{
    SAL_LLDP_PORT_DISABLE = 0,
    SAL_LLDP_PORT_TXONLY,
    SAL_LLDP_PORT_RXONLY,
    SAL_LLDP_PORT_BOTH,
    SAL_LLDP_PORT_END
} sal_lldp_port_state_t;

typedef struct sal_lldp_remote_entry_s
{
    sal_port_t  local_port;
    uint8       chassis_id_subtype;
    uint8       *chassis_id;
    uint16      chassis_id_str_len;
    uint8       port_id_subtype;
    uint8       *port_id;
    uint16      port_id_str_len;
    uint8       ttl;
    uint8       *port_desc;
    uint16      port_desc_str_len;
    uint8       *sys_name;
    uint16      sys_name_str_len;
    uint8       *sys_desc;
    uint16      sys_desc_str_len;
    uint16      sys_capability;
    uint16      sys_capability_en;
    uint8*      manag_addr;
    uint8       manag_str_subtype;
} sal_lldp_remote_entry_t;

/* Function Name:
 *      sal_lldp_set
 * Description:
 *      Set enable/disable function of LLDP
 * Input:
 *      enable
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_lldp_set(sal_enable_t enable);

/* Function Name:
 *      sal_lldp_get
 * Description:
 *      Get enable/disable function of LLDP
 * Input:
 *      None
 * Output:
 *      pEnable
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_lldp_get(sal_enable_t *pEnable);

/* Function Name:
 *      sal_lldp_timer_set
 * Description:
 *      Set timer parameters for LLDP
 * Input:
 *      transIntvl    - Transmission Interval
 *      holdTimeMulti - Hold time multiplier
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_lldp_timer_set(sal_lldp_timer_t transIntvl, sal_lldp_multi_t holdTimeMulti);

/* Function Name:
 *      sal_lldp_timer_get
 * Description:
 *      Get timer parameters for LLDP
 * Input:
 *      None
 * Output:
 *      pTransIntvl    - Poniter to transmission Interval
 *      pHoldTimeMulti - Pointer tp hold time multiplier
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_lldp_timer_get(sal_lldp_timer_t *pTransIntvl, sal_lldp_multi_t *pHoldTimeMulti);

/* Function Name:
 *      sal_lldp_portState_set
 * Description:
 *      Set port state of LLDP
 * Input:
 *      port  - port ID
 *      state - state of port
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_lldp_portState_set(sal_port_t port, sal_lldp_port_state_t state);

/* Function Name:
 *      sal_lldp_portState_get
 * Description:
 *      Get port state of LLDP
 * Input:
 *      port   - port id
 * Output:
 *      pState - pointer to state of port
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_lldp_portState_get(sal_port_t port, sal_lldp_port_state_t *pState);

/* Function Name:
 *      sal_lldp_remoteEntryNumber_get
 * Description:
 *      Get entry number of remote
 * Input:
 *      None
 * Output:
 *      pNum - Pointer to remote entry number
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_lldp_remoteEntryNumber_get(uint8 *pNum);

/* Function Name:
 *      sal_lldp_remoteEntry_get
 * Description:
 *      Get remote entry
 * Input:
 *      index  - index of remote entry
 * Output:
 *      pEntry - pointer to remote entry
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_lldp_remoteEntry_get(uint8 index, sal_lldp_remote_entry_t *pEntry);

 /* Function Name:
  *      sal_lldp_config_restore
  * Description:
  *      Restore lldp configuration from flash to system
  * Input:
  *      None
  * Output:
  *      None
  * Return:
  *      SAL_ERR_SUCCESS
  * Note:
  *      None
  */
extern int32 sal_lldp_config_restore(void);

#endif/*__SAL_LLDP_H__*/

