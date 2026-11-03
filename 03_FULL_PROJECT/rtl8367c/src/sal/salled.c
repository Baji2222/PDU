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
 * Feature : Realtek Loop Detection and Prevention Protocol
 *
 */

/* 
 * Include Files 
 */
#include <salerror.h>
#include <salled.h>

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */

 /* 
  * Macro Declaration 
  */
#define CHIP_FORCE_LED0_REG(port)       (0x1B08 + (port >> 3))
#define CHIP_FORCE_LED1_REG(port)       (0x1B0A + (port >> 3))
#define CHIP_FORCE_LED2_REG(port)       (0x1B0C + (port >> 3))
#define CHIP_FORCE_LED_OFF(port)        ((port & 7) * 2)
#define CHIP_FORCE_LED_MSK(port)        (3 << CHIP_FORCE_LED_OFF(port))
#define CHIP_FORCE_LED_BLINK_RATE_REG   (0x1B02)
#define CHIP_FORCE_LED_BLINK_RATE_OFF   (4)
#define CHIP_FORCE_LED_BLINK_RATE_MSK   (3 << CHIP_FORCE_LED_BLINK_RATE_OFF)

#define BLINK_RATE_MAX_IN_MS            (2048)
 
 /* 
  * Function Declaration 
  */

extern void setReg(uint16, uint16);
extern uint16 getReg(uint16);
#if 0//NOT used
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
int32 sal_led_mode_set(sal_led_group_t group, sal_led_mode_t mode)
{
    uint16 ledValue, off, tmp;

    if(group >= SAL_LED_GROUP_END || mode >= SAL_LED_MODE_END)
        return SAL_ERR_PARAMETER;

    switch(mode)
    {
    case SAL_LED_MODE_OFF:
        ledValue = 0;
        break;
    case SAL_LED_MODE_DUPLEX_COL:
        ledValue = 1;
        break;
    case SAL_LED_MODE_LINKACT:
        ledValue = 2;
        break;      
    case SAL_LED_MODE_SPD1000:
        ledValue = 3;
        break;        
    case SAL_LED_MODE_SPD100:
        ledValue = 4;
        break;      
    case SAL_LED_MODE_SPD10:
        ledValue = 5;
        break;      
    case SAL_LED_MODE_SPD1000_ACT:
        ledValue = 6;
        break;      
    case SAL_LED_MODE_SPD100_ACT:
        ledValue = 7;
        break;      
    case SAL_LED_MODE_SPD10_ACT:
        ledValue = 8;
        break;      
    case SAL_LED_MODE_ACT:
        ledValue = 15;
        break;        
    case SAL_LED_MODE_SPD100_SPD10_ACT:
        ledValue = 9;
        break;
    default:
        return SAL_ERR_PARAMETER;
    }

    switch(group)
    {
    case SAL_LED_GROUP0:
        off = 0;
        break;
    case SAL_LED_GROUP1:
        off = 4;
        break;    
    case SAL_LED_GROUP2:
        off = 8;
        break; 
    default:
        return SAL_ERR_PARAMETER;
    }

    tmp = getReg(0x1b03);
    tmp &= ~(0xF << off);
    tmp |= (ledValue << off);
    setReg(0x1b03, tmp);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_led_mode_get(sal_led_group_t group, sal_led_mode_t *pMode)
{
    uint16 ledValue, off, tmp;
    
    if(group >= SAL_LED_GROUP_END)
        return SAL_ERR_PARAMETER;

    switch(group)
    {
    case SAL_LED_GROUP0:
        off = 0;
        break;
    case SAL_LED_GROUP1:
        off = 4;
        break;    
    case SAL_LED_GROUP2:
        off = 8;
        break; 
    default:
        return SAL_ERR_PARAMETER;
    }

    tmp = getReg(0x1b03);
    ledValue = tmp & (0xF << off);

    switch(ledValue)
    {
    case 0:
        *pMode = SAL_LED_MODE_OFF;
        break;
    case 1:
        *pMode = SAL_LED_MODE_DUPLEX_COL;
        break;        
    case 2:
        *pMode = SAL_LED_MODE_LINKACT;
        break;        
    case 3:
        *pMode = SAL_LED_MODE_SPD1000;
        break;        
    case 4:
        *pMode = SAL_LED_MODE_SPD100;
        break;        
    case 5:
        *pMode = SAL_LED_MODE_SPD10;
        break;        
    case 6:
        *pMode = SAL_LED_MODE_SPD1000_ACT;
        break;        
    case 7:
        *pMode = SAL_LED_MODE_SPD100;
        break;        
    case 8:
        *pMode = SAL_LED_MODE_SPD10_ACT;
        break;        
    case 9:
        *pMode = SAL_LED_MODE_SPD100_SPD10_ACT;
        break;        
    case 15:
        *pMode = SAL_LED_MODE_ACT;
        break;        
    default:
        /* exception */
        return SAL_ERR_FAILED;
    }

    return SAL_ERR_SUCCESS;
}
#endif

/* Function Name:
 *      sal_led_forceMode_set
 * Description:
 *      Set force mode of LED
 * Input:
 *      port - port id
 *      mode - force mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS;
 *      SAL_ERR_PARAMETER;
 * Note:
 *      None
 */
int32 sal_led_forceMode_set(sal_port_t port, sal_led_force_t mode)
{
    uint16 regAddr, i, tmp;

    if(port >= SAL_PORT_ID_CPU || mode >= SAL_LED_FORCE_END)
        return SAL_ERR_PARAMETER;

#if defined(LWS_8G_2F)
    if (port == EXT_PORT0)
        port = 8;
    if (port == EXT_PORT1)
        port = 9;                
#elif defined(LWS_5G_1F)
    if (port == EXT_PORT0)
        port = 6;        
#else            
#endif

    for(i = 0; i < 3;i++)
    {
        regAddr = CHIP_FORCE_LED0_REG(port) + i * 2;
        tmp = getReg(regAddr);
        tmp &= ~(CHIP_FORCE_LED_MSK(port));
        tmp |= (mode << CHIP_FORCE_LED_OFF(port));
        setReg(regAddr, tmp);
    }

    return SAL_ERR_SUCCESS;
}

#if 0
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
int32 sal_led_blinkRate_set(sal_led_blink_rate_t rate)
{
    uint16 tmp;

    if(rate > (BLINK_RATE_MAX_IN_MS / 512))
        return SAL_ERR_NOT_SUPPORT;

    tmp = getReg(CHIP_FORCE_LED_BLINK_RATE_REG);
    tmp &= ~(CHIP_FORCE_LED_BLINK_RATE_MSK);
    tmp |= (rate << CHIP_FORCE_LED_BLINK_RATE_OFF);
    setReg(CHIP_FORCE_LED_BLINK_RATE_REG, tmp);

    return SAL_ERR_SUCCESS;
}
#endif

