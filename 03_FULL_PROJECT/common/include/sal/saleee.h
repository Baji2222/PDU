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
 * Feature : Realtek Ethernet Energy Efficient function
 *
 */

#ifndef __SAL_EEE_H__
#define __SAL_EEE_H__

#include <saltype.h>

/* Function Name:
 *      sal_eee_set
 * Description:
 *      Set EEE function
 * Input:
 *      enable - enable/disable EEE function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER 
 * Note:
 *      None
 */
extern int32 sal_eee_set(sal_enable_t enable);

/* Function Name:
 *      sal_eee_get
 * Description:
 *      Get EEE function
 * Input:
 *      pEnable - pointer to enable/disable EEE function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
//extern int32 sal_eee_get(sal_enable_t *pEnable);


 /* Function Name:
  *      sal_eee_config_restore
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
extern int32 sal_eee_config_restore(void);

#endif /* end of __SAL_EEE_H__ */

