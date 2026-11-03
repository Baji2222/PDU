#pragma STRING(far)
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
 * Feature : GPIO
 *
 */

/* 
 * Include Files 
 */
#include <stdio.h>
 
#include <salgpio.h>
#include <salerror.h>
#include <rtl8367c_asicdrv_gpio.h>


/* Function Name:
 *      sal_gpio_init
 * Description:
 *      Initialize GPIO 
 * Input:
 *      gpioPin - pin of enabled GPIO
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_gpio_init(uint32 gpioPin)
{
    rtl8367c_setAsicGpioEn(gpioPin, SAL_ENABLED);

    return SAL_ERR_SUCCESS;
}

 
/* Function Name:
 *      sal_gpio_dir_set
 * Description:
 *      Set direction of GPIO
 * Input:
 *      gpio_index - index of GPIO
 *      gpio_dir   - direction of GPIO
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_NOT_SUPPORT
 * Note:
 *      None
 */
int32 sal_gpio_dir_set(sal_gpio_index_t gpio_index, sal_gpio_dir_t gpio_dir)
{
    uint8  dir;

    if(gpio_index >= SAL_GPIO_END || gpio_dir >= SAL_GPIO_DIR_END)
        return SAL_ERR_PARAMETER;

    if(SAL_GPIO_DIR_INPUT == gpio_dir)
        dir = 0;
    else if(SAL_GPIO_DIR_OUTPUT == gpio_dir)
        dir = 1;

    if(rtl8367c_setAsicGpioMode(gpio_index, dir) != SUCCESS)
        return SAL_ERR_NOT_SUPPORT;

    return SAL_ERR_SUCCESS;
}
 
 /* Function Name:
  *      sal_gpio_output_set
  * Description:
  *      Set output value of GPIO
  * Input:
  *      gpio_index - index of GPIO
  *      gpio_val   - value of output
  * Output:
  *      None
  * Return:
  *      SAL_ERR_SUCCESS
  *      SAL_ERR_PARAMETER
  *      SAL_ERR_NOT_SUPPORT
  * Note:
  *      None
  */
 int32 sal_gpio_output_set(sal_gpio_index_t gpio_index, uint8 gpio_val)
{
    uint32 gpio;
    
    if(gpio_index >= SAL_GPIO_END || gpio_val > 1)
         return SAL_ERR_PARAMETER;

    gpio = gpio_val;
    if(rtl8367c_setAsicGpioOutput(gpio_index, gpio_val) != SUCCESS)
        return SAL_ERR_NOT_SUPPORT;

    return SAL_ERR_SUCCESS;
}
 
 
 /* Function Name:
  *      sal_gpio_input_get
  * Description:
  *      Get input value of GPIO
  * Input:
  *      gpio_index - index of GPIO
  * Output:
  *      pGpio_val  - gpio value
  * Return:
  *      SAL_ERR_SUCCESS
  *      SAL_ERR_PARAMETER
  * Note:
  *      None
  */
int32 sal_gpio_input_get(sal_gpio_index_t gpio_index, uint8 *pGpio_val)
{
    uint32 gpio;
    if(gpio_index >= SAL_GPIO_END)
          return SAL_ERR_PARAMETER;

    if(rtl8367c_getAsicGpioInput(gpio_index, &gpio) != SUCCESS)
        return SAL_ERR_NOT_SUPPORT;

    *pGpio_val = (uint8)gpio;

    return SAL_ERR_SUCCESS;
}
 
