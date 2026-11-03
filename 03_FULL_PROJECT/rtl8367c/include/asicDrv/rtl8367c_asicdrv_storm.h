#ifndef _RTL8367C_ASICDRV_STORM_H_
#define _RTL8367C_ASICDRV_STORM_H_

#include <rtl8367c_asicdrv.h>

extern ret_t rtl8367c_setAsicStormFilterBroadcastEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterBroadcastEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterBroadcastMeter(uint32 port, uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterBroadcastMeter(uint32 port, uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterMulticastEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterMulticastEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterMulticastMeter(uint32 port, uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterMulticastMeter(uint32 port, uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterUnknownMulticastEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterUnknownMulticastEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterUnknownMulticastMeter(uint32 port, uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterUnknownMulticastMeter(uint32 port, uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterUnknownUnicastEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterUnknownUnicastEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterUnknownUnicastMeter(uint32 port, uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterUnknownUnicastMeter(uint32 port, uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterExtBroadcastMeter(uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterExtBroadcastMeter(uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterExtMulticastMeter(uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterExtMulticastMeter(uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterExtUnknownMulticastMeter(uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterExtUnknownMulticastMeter(uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterExtUnknownUnicastMeter(uint32 meter);
extern ret_t rtl8367c_getAsicStormFilterExtUnknownUnicastMeter(uint32 *pMeter);
extern ret_t rtl8367c_setAsicStormFilterExtBroadcastEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterExtBroadcastEnable(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterExtMulticastEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterExtMulticastEnable(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterExtUnknownMulticastEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterExtUnknownMulticastEnable(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterExtUnknownUnicastEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicStormFilterExtUnknownUnicastEnable(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicStormFilterExtEnablePortMask(uint32 portmask);
extern ret_t rtl8367c_getAsicStormFilterExtEnablePortMask(uint32 *pPortmask);


#endif /*_RTL8367C_ASICDRV_STORM_H_*/


