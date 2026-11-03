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
 * $Revision: 48989 $
 * $Date: 2014-07-01 15:45:24 +0800 (?Ÿæ?äº? 01 ä¸ƒæ? 2014) $
 *
 * Purpose : RTL8370 switch high-level API for RTL8367C
 * Feature :
 *
 */

#include <rtl8367c_asicdrv_eee.h>
#include <rtl8367c_asicdrv_phy.h>

/*
@func ret_t | rtl8367c_setAsicEee100M | Set eee force mode function enable/disable.
@parm uint32 | port | The port number.
@parm uint32 | enabled | 1: enabled, 0: disabled.
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_INPUT | Invalid input parameter.
@comm
    This API set the 100M EEE enable function.

*/
ret_t rtl8367c_setAsicEee100M(uint32 port, uint32 enable)
{
    rtk_api_ret_t   retVal;
    uint32      regData;

    if(port >= RTL8367C_PORTNO)
        return RT_ERR_PORT_ID;

    if (enable > 1UL)
        return RT_ERR_INPUT;

    if((retVal = rtl8367c_getAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, &regData)) != RT_ERR_OK)
        return retVal;

    if(enable)
        regData |= (0x0001 << 1UL);
    else
        regData &= ~(0x0001 << 1UL);

    if((retVal = rtl8367c_setAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}
#if 0//NOT used
/*
@func ret_t | rtl8367c_getAsicEee100M | Get 100M eee enable/disable.
@parm uint32 | port | The port number.
@parm uint32* | enabled | 1: enabled, 0: disabled.
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_INPUT | Invalid input parameter.
@comm
    This API get the 100M EEE function.
*/
ret_t rtl8367c_getAsicEee100M(uint32 port, uint32 *enable)
{
    rtk_api_ret_t   retVal;
    uint32      regData;

    if(port >= RTL8367C_PORTNO)
        return RT_ERR_PORT_ID;

    if((retVal = rtl8367c_getAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, &regData)) != RT_ERR_OK)
        return retVal;

    *enable = (regData & (0x0001 << 1UL)) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}
#endif

/*
@func ret_t | rtl8367c_setAsicEeeGiga | Set eee force mode function enable/disable.
@parm uint32 | port | The port number.
@parm uint32 | enabled | 1: enabled, 0: disabled.
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_INPUT | Invalid input parameter.
@comm
    This API set the 100M EEE enable function.

*/
ret_t rtl8367c_setAsicEeeGiga(uint32 port, uint32 enable)
{
    rtk_api_ret_t   retVal;
    uint32      regData;

    if(port >= RTL8367C_PORTNO)
        return RT_ERR_PORT_ID;

    if (enable > 1UL)
        return RT_ERR_INPUT;

    if((retVal = rtl8367c_getAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, &regData)) != RT_ERR_OK)
        return retVal;

    if(enable)
        regData |= (0x0001 << 2UL);
    else
        regData &= ~(0x0001 << 2UL);

    if((retVal = rtl8367c_setAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

#if 0//NOT used
/*
@func ret_t | rtl8367c_getAsicEeeGiga | Get 100M eee enable/disable.
@parm uint32 | port | The port number.
@parm uint32* | enabled | 1: enabled, 0: disabled.
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error. 
@rvalue RT_ERR_INPUT | Invalid input parameter.
@comm
    This API get the 100M EEE function.
*/
ret_t rtl8367c_getAsicEeeGiga(uint32 port, uint32 *enable)
{
    rtk_api_ret_t   retVal;
    uint32      regData;

    if(port >= RTL8367C_PORTNO)
        return RT_ERR_PORT_ID;

    if((retVal = rtl8367c_getAsicPHYOCPReg(port, EEE_OCP_PHY_ADDR, &regData)) != RT_ERR_OK)
        return retVal;

    *enable = (regData & (0x0001 << 2UL)) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}
#endif
