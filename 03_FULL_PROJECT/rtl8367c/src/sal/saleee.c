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

/* 
 * Include Files 
 */
#include <stdio.h>
#include <common.h>
#include <rtl8367c_asicdrv_eee.h>
#include <rtl8367c_asicdrv_phy.h>

#include <saleee.h>
#include <salerror.h>
#include <salflash.h>
#include <eee.h>

#if SAL_EEE

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */
static sal_enable_t eee_enable;

/* 
 * Macro Declaration 
 */
 
/* 
 * Function Declaration 
 */

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
int32 sal_eee_set(sal_enable_t enable)
{
    uint16 i;

    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;

    eee_enable = enable;

    rtk_eee_init();

    for(i = 0; i < (SAL_MAX_PHY_PORT - SAL_MAX_EXT_PORT);i++)
    {
        rtk_eee_portEnable_set(i, enable);
    }
    
    return sal_flash_eee_set(eee_enable);
}

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
//int32 sal_eee_get(sal_enable_t *pEnable)
//{
//    *pEnable = eee_enable;

//    return SAL_ERR_SUCCESS;
//}

 /* Function Name:
  *      sal_eee_config_restore
  * Description:
  *      Restore EEE configuration from flash to system
  * Input:
  *      None
  * Output:
  *      None
  * Return:
  *      SAL_ERR_SUCCESS
  * Note:
  *      None
  */
int32 sal_eee_config_restore(void)
{
    sal_enable_t eee;

    sal_flash_eee_get(&eee);
    sal_eee_set(eee);

    eee_enable = eee;

    return SAL_ERR_SUCCESS;    
}

#endif

