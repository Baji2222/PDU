#ifndef _RTL8367C_ASICDRV_MISC_H_
#define _RTL8367C_ASICDRV_MISC_H_

#include <rtl8367c_asicdrv.h>

extern ret_t rtl8367c_setAsicMacAddress(ether_addr_t mac);
extern ret_t rtl8367c_getAsicMacAddress(ether_addr_t *pMac);
extern ret_t rtl8367c_getAsicDebugInfo(uint32 port, uint32 *pDebugifo);
extern ret_t rtl8367c_setAsicPortJamMode(uint32 mode);
extern ret_t rtl8367c_getAsicPortJamMode(uint32* pMode);
extern ret_t rtl8367c_setAsicMaxLengthCfg(uint32 cfgId, uint32 maxLength);
extern ret_t rtl8367c_getAsicMaxLengthCfg(uint32 cfgId, uint32 *pMaxLength);
extern ret_t rtl8367c_setAsicMaxLength(uint32 port, uint32 type, uint32 cfgId);
extern ret_t rtl8367c_getAsicMaxLength(uint32 port, uint32 type, uint32 *pCfgId);

#endif /*_RTL8367C_ASICDRV_MISC_H_*/

