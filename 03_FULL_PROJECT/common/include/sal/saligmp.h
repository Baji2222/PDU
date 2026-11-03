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
 * Feature : IGMP Snooping
 *
 */
#ifndef __SAL_IGMP_H__
#define __SAL_IGMP_H__

#include <saltype.h>

typedef struct sal_igmp_entry_s
{
    sal_ipaddr_t    group_addr;
    sal_pPortmask_t fwd_portmask;
    uint16        vid;    
} sal_igmp_entry_t;

/* Function Name:
 *      sal_igmp_config_restore
 * Description:
 *      Restore igmp configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_config_restore(void);

/* Function Name:
 *      sal_igmp_enable_set
 * Description:
 *      Set (enable/disable) IGMP status to HW
 * Input:
 *      enable		- enable/disable
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_enable_set(uint32	enable);

/* Function Name:
 *      sal_igmp_enable_get
 * Description:
 *      Get (enable/disable) IGMP status from HW
 * Input:
 *      None
 * Output:
 *      pEnable		- enable/disable
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_enable_get(uint32	*pEnable);

/* Function Name:
 *      sal_igmp_configEnable_set
 * Description:
 *      Set (enable/disable) IGMP status to protocol module
 * Input:
 *      igmp		- enable/disable
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_configEnable_set(uint8 igmp);

/* Function Name:
 *      sal_igmp_configEnable_get
 * Description:
 *      Get (enable/disable) IGMP status from protocol module
 * Input:
 *      None
 * Output:
 *      pIgmp		- enable/disable
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_configEnable_get(uint8 *pIgmp);

/* Function Name:
 *      sal_igmp_nextEntry_get
 * Description:
 *      Get next valid IGMP entry.
 * Input:
 *      None
 * Output:
 *      pEntry  - pointer to next valid entry
 *      pIsTail - pointer to indicate whether next valid entry exists.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_nextEntry_get(sal_igmp_entry_t *pEntry, uint8 *pIsTail);

/* Function Name:
 *      sal_igmp_ipAddrEntry_add
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 *      pMember _ Pointer to the member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_ipAddrEntry_add(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid, sal_lPortmask_t *pMember);

/* Function Name:
 *      sal_igmp_ipAddrEntry_del
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_ipAddrEntry_del(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid);

/* Function Name:
 *      sal_igmp_ipv6AddrEntry_add
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 *      pMember _ Pointer to the member port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_ipv6AddrEntry_add(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid, sal_lPortmask_t *pMember);

/* Function Name:
 *      sal_igmp_ipv6AddrEntry_del
 * Description:
 *      Add an ip address to LUT
 * Input:
 *      dip     - Destination of IP address
 *      sip     - Source IP address
 *      fid     - FID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_ipv6AddrEntry_del(sal_ipaddr_t dip, sal_ipaddr_t sip, sal_fid_t fid);

/* Function Name:
 *      sal_igmp_chip_init
 * Description:
 *      Initialzie IGMP setting to chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_chip_init();

/* Function Name:
 *      sal_igmp_multicast_forward
 * Description:
 *      When IGMP is disabled, multicast packet should flooding
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_multicast_forward();

/* Function Name:
 *      sal_igmp_state_get
 * Description:
 *      Get IGMP enable state.
 * Input:
 *      None
 * Output:
 *      enable
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
 extern int32 sal_igmp_state_get(sal_enable_t *enable);

/* Function Name:
 *      sal_igmp_state_set
 * Description:
 *      Set IGMP enable state.
 * Input:
 *      enable
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_state_set(sal_enable_t enable);

/* Function Name:
 *      sal_igmp_staticRouter_get
 * Description:
 *      Get IGMP statistic router port configuration
 * Input:
 *      None
 * Output:
 *      pMask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_staticRouter_get(sal_lPortmask_t *pMask);

/* Function Name:
 *      sal_igmp_staticRouter_set
 * Description:
 *      Set IGMP statistic router port configuration
 * Input:
 *      mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_staticRouter_set(sal_lPortmask_t mask);

/* Function Name:
 *      sal_igmp_dynamicRouter_get
 * Description:
 *      Get current IGMP dynamic router port
 * Input:
 *      None
 * Output:
 *      pMask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_igmp_dynamicRouter_get(sal_lPortmask_t *pMask);

#endif/*__SAL_IGMP_H__*/

