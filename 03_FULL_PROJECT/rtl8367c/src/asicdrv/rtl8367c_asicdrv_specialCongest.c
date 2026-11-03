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
 * $Revision: 38651 $
 * $Date: 2013-04-17 14:32:56 +0800 (?Ÿæ?ä¸? 17 ?›æ? 2013) $
 *
 * Purpose : RTL8367C switch high-level API for RTL8367C
 * Feature : Special congestion related functions
 *
 */
#include <rtl8367c_asicdrv_specialCongest.h>
/* Function Name:
 *      rtl8367c_setAsicSpecialCongestModeConfig
 * Description:
 *      Set ASIC special congest mode configuration
 * Input:
 *      port 	- Physical port number (0~7)
 *      sustain - sustain timer (0-15)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_PORT_ID  	- Invalid port number
 *      RT_ERR_OUT_OF_RANGE - Input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicSpecialCongestModeConfig(uint32 port, uint32 sustain)
{
	if(port >= RTL8367C_PORTNO)
		return RT_ERR_PORT_ID;

    if(sustain > RTL8367C_SPECIALCONGEST_SUSTAIN_TIMERMAX)
        return RT_ERR_OUT_OF_RANGE;

	return rtl8367c_setAsicRegBits(RTL8367C_PORT_MISC_CFG_REG(port), RTL8367C_SPECIALCONGEST_SUSTAIN_TIMER_MASK, sustain);
}
#if 0 //NOT used
/* Function Name:
 *      rtl8367c_getAsicSpecialCongestModeConfig
 * Description:
 *      Get ASIC special congest mode configuration
 * Input:
 *      port 	- Physical port number (0~7)
 *      pSustain - sustain timer (0-15)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_PORT_ID  	- Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicSpecialCongestModeConfig(uint32 port, uint32 *pSustain)
{
	if(port >= RTL8367C_PORTNO)
		return RT_ERR_PORT_ID;

	return rtl8367c_getAsicRegBits(RTL8367C_PORT_MISC_CFG_REG(port), RTL8367C_SPECIALCONGEST_SUSTAIN_TIMER_MASK, pSustain);
}
/* Function Name:
 *      rtl8367c_getAsicSpecialCongestModeTimer
 * Description:
 *      Get ASIC special congest mode timer
 * Input:
 *      port 	- Physical port number (0~7)
 *      pTimer 	- time (0-15)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_PORT_ID  	- Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicSpecialCongestModeTimer(uint32 port, uint32* pTimer)
{
    if(port > RTL8367C_PORTNO)
        return RT_ERR_PORT_ID;

    return rtl8367c_getAsicRegBits(RTL8367C_PORT_SPECIAL_CONGEST_MODE_TIMER_REG(port), RTL8367C_PKTGEN_PORT0_TIMER_TIMER_MASK, pTimer);
}
#endif

