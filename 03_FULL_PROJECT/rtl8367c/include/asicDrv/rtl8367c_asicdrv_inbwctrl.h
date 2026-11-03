#ifndef _RTL8367C_ASICDRV_INBWCTRL_H_
#define _RTL8367C_ASICDRV_INBWCTRL_H_

#include <rtl8367c_asicdrv.h>

extern ret_t rtl8367c_setAsicPortIngressBandwidth(uint32 port, uint32 bandwidth, uint32 preifg, uint32 enableFC);
extern ret_t rtl8367c_getAsicPortIngressBandwidth(uint32 port, uint32* pBandwidth, uint32* pPreifg, uint32* pEnableFC );
extern ret_t rtl8367c_setAsicPortIngressBandwidthBypass(uint32 enabled);
extern ret_t rtl8367c_getAsicPortIngressBandwidthBypass(uint32* pEnabled);


#endif /*_RTL8367C_ASICDRV_INBWCTRL_H_*/

