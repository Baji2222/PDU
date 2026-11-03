#ifndef _RTL8367C_ASICDRV_LED_H_
#define _RTL8367C_ASICDRV_LED_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_LEDGROUPNO                 3
#define RTL8367C_LEDGROUPMASK               0x7
#define RTL8367C_LED_FORCE_MODE_BASE        RTL8367C_REG_CPU_FORCE_LED0_CFG0
#define RTL8367C_LED_FORCE_CTRL             RTL8367C_REG_CPU_FORCE_LED_CFG

enum RTL8367C_LEDOP{

    LEDOP_SCAN0=0,
    LEDOP_SCAN1,
    LEDOP_PARALLEL,
    LEDOP_SERIAL,
    LEDOP_END,
};

enum RTL8367C_LEDSERACT{

    LEDSERACT_HIGH=0,
    LEDSERACT_LOW,
    LEDSERACT_MAX,
};

enum RTL8367C_LEDSER{

    LEDSER_16G=0,
    LEDSER_8G,
    LEDSER_MAX,
};

enum RTL8367C_LEDCONF{

    LEDCONF_LEDOFF=0,
    LEDCONF_DUPCOL,
    LEDCONF_LINK_ACT,
    LEDCONF_SPD1000,
    LEDCONF_SPD100,
    LEDCONF_SPD10,
    LEDCONF_SPD1000ACT,
    LEDCONF_SPD100ACT,
    LEDCONF_SPD10ACT,
    LEDCONF_SPD10010ACT,
    LEDCONF_LOOPDETECT,
    LEDCONF_EEE,
    LEDCONF_LINKRX,
    LEDCONF_LINKTX,
    LEDCONF_MASTER,
    LEDCONF_ACT,
    LEDCONF_END
};

enum RTL8367C_LEDBLINKRATE{

    LEDBLINKRATE_32MS=0,
    LEDBLINKRATE_64MS,
    LEDBLINKRATE_128MS,
    LEDBLINKRATE_256MS,
    LEDBLINKRATE_512MS,
    LEDBLINKRATE_1024MS,
    LEDBLINKRATE_48MS,
    LEDBLINKRATE_96MS,
    LEDBLINKRATE_END,
};

enum RTL8367C_LEDFORCEMODE{

    LEDFORCEMODE_NORMAL=0,
    LEDFORCEMODE_BLINK,
    LEDFORCEMODE_OFF,
    LEDFORCEMODE_ON,
    LEDFORCEMODE_END,
};

enum RTL8367C_LEDFORCERATE{

    LEDFORCERATE_512MS=0,
    LEDFORCERATE_1024MS,
    LEDFORCERATE_2048MS,
    LEDFORCERATE_NORMAL,
    LEDFORCERATE_END,

};

enum RTL8367C_LEDMODE
{
    RTL8367C_LED_MODE_0 = 0,
    RTL8367C_LED_MODE_1,
    RTL8367C_LED_MODE_2,
    RTL8367C_LED_MODE_3,
    RTL8367C_LED_MODE_END
};

extern ret_t rtl8367c_setAsicLedIndicateInfoConfig(uint32 ledno, uint32 config);
extern ret_t rtl8367c_getAsicLedIndicateInfoConfig(uint32 ledno, uint32* pConfig);
extern ret_t rtl8367c_setAsicForceLed(uint32 port, uint32 group, uint32 mode);
extern ret_t rtl8367c_getAsicForceLed(uint32 port, uint32 group, uint32* pMode);
extern ret_t rtl8367c_setAsicForceGroupLed(uint32 groupmask, uint32 mode);
extern ret_t rtl8367c_getAsicForceGroupLed(uint32* groupmask, uint32* pMode);
extern ret_t rtl8367c_setAsicLedBlinkRate(uint32 blinkRate);
extern ret_t rtl8367c_getAsicLedBlinkRate(uint32* pBlinkRate);
extern ret_t rtl8367c_setAsicLedForceBlinkRate(uint32 blinkRate);
extern ret_t rtl8367c_getAsicLedForceBlinkRate(uint32* pBlinkRate);
extern ret_t rtl8367c_setAsicLedGroupMode(uint32 mode);
extern ret_t rtl8367c_getAsicLedGroupMode(uint32* pMode);
extern ret_t rtl8367c_setAsicLedGroupEnable(uint32 group, uint32 portmask);
extern ret_t rtl8367c_getAsicLedGroupEnable(uint32 group, uint32 *portmask);
extern ret_t rtl8367c_setAsicLedOperationMode(uint32 mode);
extern ret_t rtl8367c_getAsicLedOperationMode(uint32 *mode);
extern ret_t rtl8367c_setAsicLedSerialModeConfig(uint32 active, uint32 serimode);
extern ret_t rtl8367c_getAsicLedSerialModeConfig(uint32 *active, uint32 *serimode);
extern ret_t rtl8367c_setAsicLedOutputEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicLedOutputEnable(uint32 *ptr_enabled);
extern ret_t rtl8367c_setAsicLedSerialOutput(uint32 output, uint32 pmask);
extern ret_t rtl8367c_getAsicLedSerialOutput(uint32 *pOutput, uint32 *pPmask);


#endif /*#ifndef _RTL8367C_ASICDRV_LED_H_*/

