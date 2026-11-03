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
#ifndef __SAL_GPIO_H__
#define __SAL_GPIO_H__

#include <saltype.h>

typedef enum sal_gpio_index_e
{
    SAL_GPIO_ONCHIP_0 = 0,
    SAL_GPIO_ONCHIP_1,
    SAL_GPIO_ONCHIP_2,
    SAL_GPIO_ONCHIP_3,
    SAL_GPIO_ONCHIP_4,
    SAL_GPIO_ONCHIP_5,
    SAL_GPIO_ONCHIP_6,
    SAL_GPIO_ONCHIP_7,
    SAL_GPIO_ONCHIP_8,
    SAL_GPIO_ONCHIP_9,
    SAL_GPIO_ONCHIP_10,
    SAL_GPIO_ONCHIP_11,
    SAL_GPIO_ONCHIP_12,
    SAL_GPIO_ONCHIP_13,   
    SAL_GPIO_ONCHIP_14,
    SAL_GPIO_ONCHIP_15,
    SAL_GPIO_ONCHIP_16,
    SAL_GPIO_ONCHIP_17,
    SAL_GPIO_ONCHIP_18,
    SAL_GPIO_ONCHIP_19,
    SAL_GPIO_ONCHIP_20,
    SAL_GPIO_ONCHIP_21,
    SAL_GPIO_ONCHIP_22,
    SAL_GPIO_ONCHIP_23,
    SAL_GPIO_ONCHIP_24,
    SAL_GPIO_ONCHIP_25,
    SAL_GPIO_ONCHIP_26,
    SAL_GPIO_ONCHIP_27,
    SAL_GPIO_ONCHIP_28,
    SAL_GPIO_ONCHIP_29,
    SAL_GPIO_ONCHIP_30,
    SAL_GPIO_ONCHIP_31,
    SAL_GPIO_ONCHIP_32,
    SAL_GPIO_ONCHIP_33,
    SAL_GPIO_ONCHIP_34,
    SAL_GPIO_ONCHIP_35,
    SAL_GPIO_ONCHIP_36,
    SAL_GPIO_ONCHIP_37,
    SAL_GPIO_ONCHIP_38,
    SAL_GPIO_ONCHIP_39,
    SAL_GPIO_ONCHIP_40,
    SAL_GPIO_ONCHIP_41,
    SAL_GPIO_ONCHIP_42,
    SAL_GPIO_ONCHIP_43,
    SAL_GPIO_ONCHIP_44,
    SAL_GPIO_ONCHIP_45,
    SAL_GPIO_ONCHIP_46,
    SAL_GPIO_ONCHIP_47,
    SAL_GPIO_ONCHIP_48,
    SAL_GPIO_ONCHIP_49,
    SAL_GPIO_ONCHIP_50,
    SAL_GPIO_ONCHIP_51,
    SAL_GPIO_ONCHIP_52,
    SAL_GPIO_ONCHIP_53,
    SAL_GPIO_ONCHIP_54,
    SAL_GPIO_ONCHIP_55,
    SAL_GPIO_ONCHIP_56,
    SAL_GPIO_ONCHIP_57,
    SAL_GPIO_ONCHIP_58,
    SAL_GPIO_ONCHIP_59,
    SAL_GPIO_ONCHIP_60,
    SAL_GPIO_ONCHIP_61,
    SAL_GPIO_END
} sal_gpio_index_t;

typedef enum sal_gpio_dir_e
{
    SAL_GPIO_DIR_OUTPUT = 0,
    SAL_GPIO_DIR_INPUT,
    SAL_GPIO_DIR_END
} sal_gpio_dir_t;


/* Function Name:
 *      sal_gpio_init
 * Description:
 *      Initialize GPIO 
 * Input:
 *      mask - mask of enabled GPIO
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_gpio_init(uint32 mask);


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
 * Note:
 *      None
 */
extern int32 sal_gpio_dir_set(sal_gpio_index_t gpio_index, sal_gpio_dir_t gpio_dir);

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
 * Note:
 *      None
 */
extern int32 sal_gpio_output_set(sal_gpio_index_t gpio_index, uint8 gpio_val);


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
 *      SAL_ERR_NOT_SUPPORT
 * Note:
 *      None
 */
extern int32 sal_gpio_input_get(sal_gpio_index_t gpio_index, uint8 *pGpio_val);

#endif/*__SAL_GPIO_H__*/

