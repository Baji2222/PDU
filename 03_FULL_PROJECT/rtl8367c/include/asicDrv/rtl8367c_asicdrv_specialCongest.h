#ifndef _RTL8367C_ASICDRV_SPECIALCONGEST_H_
#define _RTL8367C_ASICDRV_SPECIALCONGEST_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_SPECIALCONGEST_SUSTAIN_TIMERMAX 0xF

extern ret_t rtl8367c_setAsicSpecialCongestModeConfig(uint32 port, uint32 sustain);
extern ret_t rtl8367c_getAsicSpecialCongestModeConfig(uint32 port, uint32* pSustain);

extern ret_t rtl8367c_getAsicSpecialCongestModeTimer(uint32 port, uint32* pTimer);

#endif /*_RTL8367C_ASICDRV_SPECIALCONGEST_H_*/

