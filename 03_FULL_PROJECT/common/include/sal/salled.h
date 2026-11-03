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
 * Feature : LED related
 *
 */
#ifndef __SAL_LED_H__
#define __SAL_LED_H__

#include <saltype.h>

typedef enum sal_led_force_e
{
    SAL_LED_FORCE_NORMAL = 0,
    SAL_LED_FORCE_BLINK,
    SAL_LED_FORCE_OFF,
    SAL_LED_FORCE_ON,
    SAL_LED_FORCE_END,
} sal_led_force_t;

typedef enum sal_led_mode_e
{
    SAL_LED_MODE_OFF,
    SAL_LED_MODE_DUPLEX_COL,
    SAL_LED_MODE_LINKACT,
    SAL_LED_MODE_SPD1000,
    SAL_LED_MODE_SPD100,
    SAL_LED_MODE_SPD10,
    SAL_LED_MODE_SPD1000_ACT,    
    SAL_LED_MODE_SPD100_ACT,        
    SAL_LED_MODE_SPD10_ACT,        
    SAL_LED_MODE_ACT,
    SAL_LED_MODE_SPD100_SPD10_ACT,
    SAL_LED_MODE_END
} sal_led_mode_t;

typedef enum sal_led_group_e
{
    SAL_LED_GROUP0,
    SAL_LED_GROUP1,
    SAL_LED_GROUP2,
    SAL_LED_GROUP_END
} sal_led_group_t;

typedef uint16 sal_led_blink_rate_t;

/* Function Name:
 *      sal_led_mode_set
 * Description:
 *      Set mode of LED.
 * Input:
 *      led  - led index.
 *      mode - force mode.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS;
 *      SAL_ERR_PARAMETER;
 * Note:
 *      None
 */
extern int32 sal_led_mode_set(sal_led_group_t group, sal_led_mode_t mode);


/* Function Name:
 *      sal_led_mode_get
 * Description:
 *      Get mode of LED.
 * Input:
 *      led   - led index.
 *      pMode - pointer to LED mode.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS;
 *      SAL_ERR_PARAMETER;
 *      SAL_ERR_FAILED; 
 * Note:
 *      None
 */
extern int32 sal_led_mode_get(sal_led_group_t group, sal_led_mode_t *pMode);

/* Function Name:
 *      sal_led_forceMode_set
 * Description:
 *      Set force mode of LED
 * Input:
 *      port  - port id
 *      mode  - force mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS;
 *      SAL_ERR_PARAMETER;
 * Note:
 *      None
 */
extern int32 sal_led_forceMode_set(sal_port_t port, sal_led_force_t mode);


/* Function Name:
 *      sal_led_blinkRate_set
 * Description:
 *      Set blink rate when port led is forced to blink
 * Input:
 *      rate - Blink rate. Unit: 512ms
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS;
 *      SAL_ERR_NOT_SUPPORT;
 * Note:
 *      None
 */
extern int32 sal_led_blinkRate_set(sal_led_blink_rate_t rate);

#endif /* end of __SAL_LED_H__ */

