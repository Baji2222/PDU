#ifndef _RTL8367C_ASICDRV_TRUNKING_H_
#define _RTL8367C_ASICDRV_TRUNKING_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_MAX_TRUNK_GID              (2)
#define RTL8367C_TRUNKING_PORTNO            (4)
#define RTL8367C_TRUNKING1_PORTN0           (2)
#define RTL8367C_TRUNKING_HASHVALUE_MAX     (15)

extern ret_t rtl8367c_setAsicTrunkingGroup(uint32 group, uint32 portmask);
extern ret_t rtl8367c_getAsicTrunkingGroup(uint32 group, uint32* pPortmask);
extern ret_t rtl8367c_setAsicTrunkingFlood(uint32 enabled);
extern ret_t rtl8367c_getAsicTrunkingFlood(uint32* pEnabled);
extern ret_t rtl8367c_setAsicTrunkingHashSelect(uint32 hashsel);
extern ret_t rtl8367c_getAsicTrunkingHashSelect(uint32* pHashsel);

extern ret_t rtl8367c_getAsicQeueuEmptyStatus(uint32* pPortmask);

extern ret_t rtl8367c_setAsicTrunkingMode(uint32 mode);
extern ret_t rtl8367c_getAsicTrunkingMode(uint32* pMode);
extern ret_t rtl8367c_setAsicTrunkingFc(uint32 group, uint32 enabled);
extern ret_t rtl8367c_getAsicTrunkingFc(uint32 group, uint32* pEnabled);
extern ret_t rtl8367c_setAsicTrunkingHashTable(uint32 hashval, uint32 portId);
extern ret_t rtl8367c_getAsicTrunkingHashTable(uint32 hashval, uint32* pPortId);
extern ret_t rtl8367c_setAsicTrunkingHashTable1(uint32 hashval, uint32 portId);
extern ret_t rtl8367c_getAsicTrunkingHashTable1(uint32 hashval, uint32* pPortId);

#endif /*_RTL8367C_ASICDRV_TRUNKING_H_*/

