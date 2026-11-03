#ifndef _RTL8367C_ASICDRV_GREEN_H_
#define _RTL8367C_ASICDRV_GREEN_H_

#include <rtl8367c_asicdrv.h>
#include <rtl8367c_asicdrv_phy.h>

#define PHY_POWERSAVING_REG                         24

extern ret_t rtl8367c_setAsicGreenTrafficType(uint32 priority, uint32 traffictype);
extern ret_t rtl8367c_getAsicGreenTrafficType(uint32 priority, uint32* pTraffictype);
extern ret_t rtl8367c_getAsicGreenPortPage(uint32 port, uint32* pPage);
extern ret_t rtl8367c_getAsicGreenHighPriorityTraffic(uint32 port, uint32* pIndicator);
extern ret_t rtl8367c_setAsicGreenHighPriorityTraffic(uint32 port);
extern ret_t rtl8367c_setAsicGreenEthernet(uint32 port, uint32 green);
extern ret_t rtl8367c_getAsicGreenEthernet(uint32 port, uint32* green);
extern ret_t rtl8367c_setAsicPowerSaving(uint32 phy, uint32 enable);
extern ret_t rtl8367c_getAsicPowerSaving(uint32 phy, uint32* enable);
#endif /*#ifndef _RTL8367C_ASICDRV_GREEN_H_*/

