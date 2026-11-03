/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 48904 $
 * $Date: 2014-06-26 14:50:52 +0800 (?Ÿæ??? 26 ?­æ? 2014) $
 *
 * Purpose : RTL8367C switch high-level API for RTL8367C
 * Feature : Green ethernet related functions
 *
 */
#include <rtl8367c_asicdrv_gpio.h>
/* Function Name:
 *      rtl8367c_getAsicGpioInput
 * Description:
 *      Get gpio input
 * Input:
 *      gpioPin 		- GPIO pin number
 *      pInput 			- GPIO input
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicGpioInput(uint32 gpioPin, uint32* pGpioInput)
{
	if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_getAsicRegBit(RTL8367C_REG_GPIO_67C_I_X0 + (gpioPin / 16), (gpioPin % 16), pGpioInput);
}
/* Function Name:
 *      rtl8367c_setAsicGpioOutput
 * Description:
 *      Set gpio output
 * Input:
 *      gpioPin 		- GPIO pin number
 *      gpioOutput 		- GPIO output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicGpioOutput(uint32 gpioPin, uint32 gpioOutput)
{
	if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_setAsicRegBit(RTL8367C_REG_GPIO_67C_O_X0 + (gpioPin / 16), (gpioPin % 16), gpioOutput);
}
#if 0
/* Function Name:
 *      rtl8367c_getAsicGpioOutput
 * Description:
 *      Get gpio output
 * Input:
 *      gpioPin 		- GPIO pin number
 *      pGpioOutput 	- GPIO output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicGpioOutput(uint32 gpioPin, uint32* pGpioOutput)
{
	if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_getAsicRegBit(RTL8367C_REG_GPIO_67C_O_X0 + (gpioPin / 16), (gpioPin % 16), pGpioOutput);
}
#endif
/* Function Name:
 *      rtl8367c_setAsicGpioEn
 * Description:
 *      Set gpio control
 * Input:
 *      gpioPin 		- GPIO pin number
 *      gpioEn			- GPIO enabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicGpioEn(uint32 gpioPin, uint32 gpioEn)
{
	if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_setAsicRegBit(RTL8367C_REG_GPIO_MODE_67C_X0 + (gpioPin / 16), (gpioPin % 16), gpioEn);
}
#if 0
/* Function Name:
 *      rtl8367c_getAsicGpioEn
 * Description:
 *      Get gpio output
 * Input:
 *      gpioPin 		- GPIO pin number
 *      pGpioEn			- GPIO enabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicGpioEn(uint32 gpioPin, uint32* pGpioEn)
{
	if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_getAsicRegBit(RTL8367C_REG_GPIO_MODE_67C_X0 + (gpioPin / 16), (gpioPin % 16), pGpioEn);
}
#endif
/* Function Name:
 *      rtl8367c_setAsicGpioMode
 * Description:
 *      Set gpio control
 * Input:
 *      gpioPin 		- GPIO pin number
 *      mode			- 0: Input, 1:Output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicGpioMode(uint32 gpioPin, uint32 mode)
{
    if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(mode > 1)
        return RT_ERR_INPUT;

    return rtl8367c_setAsicRegBit(RTL8367C_REG_GPIO_67C_OE_X0 + (gpioPin / 16), (gpioPin % 16), mode);
}
#if 0
/* Function Name:
 *      rtl8367c_getAsicGpioMode
 * Description:
 *      Get gpio output
 * Input:
 *      gpioPin 		- GPIO pin number
 *      pMode			- 0: Input, 1:Output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicGpioMode(uint32 gpioPin, uint32 *pMode)
{
    if(gpioPin >= RTL8367C_GPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    if(pMode == NULL)
        return RT_ERR_NULL_POINTER;

    return rtl8367c_getAsicRegBit(RTL8367C_REG_GPIO_67C_OE_X0 + (gpioPin / 16), (gpioPin % 16), pMode);
}
/* Function Name:
 *      rtl8367c_setAsicGpioAclEnClear
 * Description:
 *      Set gpio control
 * Input:
 *      gpioPin 		- GPIO pin number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicGpioAclEnClear(uint32 gpioPin)
{
    ret_t retVal;

	if(gpioPin > RTL8367C_ACLGPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    /* Enable */
    if((retVal = rtl8367c_setAsicRegBit(RTL8367C_REG_EN_GPIO, gpioPin, 1)) != RT_ERR_OK)
        return retVal;

    /* ACL clear */
    if((retVal = rtl8367c_setAsicRegBit(RTL8367C_REG_ACL_GPIO, gpioPin, 1)) != RT_ERR_OK)
        return retVal;

    /* Control by asic */
    if((retVal = rtl8367c_setAsicRegBit(RTL8367C_REG_SEL_GPIO, gpioPin, 0)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367c_getAsicGpioAclEnClear
 * Description:
 *      Get gpio output
 * Input:
 *      gpioPin 		- GPIO pin number
 *      pGpioEn			- GPIO enabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicGpioAclEnClear(uint32 gpioPin, uint32* pGpioEn)
{
	if(gpioPin > RTL8367C_ACLGPIOPINNO)
		return RT_ERR_OUT_OF_RANGE;

    return rtl8367c_getAsicRegBit(RTL8367C_REG_ACL_GPIO, gpioPin, pGpioEn);
}
#endif
