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
 * Feature : Realtek Green Ethernet
 *
 */

#ifndef __SAL_GREENETH_H__
#define __SAL_GREENETH_H__

#include <saltype.h>

/* Function Name:
 *      sal_greenEth_config_restore
 * Description:
 *      Restore Green Ethernet configuration
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_greenEth_config_restore();

/* Function Name:
 *      sal_greenEth_enable_set
 * Description:
 *      Enable/Disable Green Ethernet function
 * Input:
 *      enable - enable Green Ethernet function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_greenEth_enable_set(sal_enable_t enable);

/* Function Name:
 *      sal_greenEth_enable_get
 * Description:
 *      Get Green Ethernet function state
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to Green Ethernet function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
//extern int32 sal_greenEth_enable_get(sal_enable_t *pEnable);

#endif/*__SAL_GREENETH_H__*/



