#ifndef _RTL8367C_ASICDRV_MIRROR_H_
#define _RTL8367C_ASICDRV_MIRROR_H_

#include <rtl8367c_asicdrv.h>

extern ret_t rtl8367c_setAsicPortMirror(uint32 source, uint32 monitor);
extern ret_t rtl8367c_getAsicPortMirror(uint32 *pSource, uint32 *pMonitor);
extern ret_t rtl8367c_setAsicPortMirrorRxFunction(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorRxFunction(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorTxFunction(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorTxFunction(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorIsolation(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorIsolation(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorPriority(uint32 priority);
extern ret_t rtl8367c_getAsicPortMirrorPriority(uint32* pPriority);
extern ret_t rtl8367c_setAsicPortMirrorMask(uint32 SourcePortmask);
extern ret_t rtl8367c_getAsicPortMirrorMask(uint32 *pSourcePortmask);
extern ret_t rtl8367c_setAsicPortMirrorVlanRxLeaky(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorVlanRxLeaky(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorVlanTxLeaky(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorVlanTxLeaky(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorIsolationRxLeaky(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorIsolationRxLeaky(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorIsolationTxLeaky(uint32 enabled);
extern ret_t rtl8367c_getAsicPortMirrorIsolationTxLeaky(uint32* pEnabled);
extern ret_t rtl8367c_setAsicPortMirrorRealKeep(uint32 mode);
extern ret_t rtl8367c_getAsicPortMirrorRealKeep(uint32* pMode);
extern ret_t rtl8367c_setAsicPortMirrorOverride(uint32 rxMirror, uint32 txMirror, uint32 aclMirror);
extern ret_t rtl8367c_getAsicPortMirrorOverride(uint32 *pRxMirror, uint32 *pTxMirror, uint32 *pAclMirror);

#endif /*#ifndef _RTL8367C_ASICDRV_MIRROR_H_*/

