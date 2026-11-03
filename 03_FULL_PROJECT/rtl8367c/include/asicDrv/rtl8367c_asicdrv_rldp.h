#ifndef _RTL8367C_ASICDRV_RLDP_H_
#define _RTL8367C_ASICDRV_RLDP_H_

#include <rtl8367c_asicdrv.h>
#include <string.h>

extern ret_t rtl8367c_setAsicRldp(uint32 enabled);
extern ret_t rtl8367c_getAsicRldp(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicRldpEnable8051(uint32 enabled);
extern ret_t rtl8367c_getAsicRldpEnable8051(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicRldpCompareRandomNumber(uint32 enabled);
extern ret_t rtl8367c_getAsicRldpCompareRandomNumber(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicRldpIndicatorSource(uint32 src);
extern ret_t rtl8367c_getAsicRldpIndicatorSource(uint32 *pSrc);
extern ret_t rtl8367c_setAsicRldpCheckingStatePara(uint32 retryCount, uint32 retryPeriod);
extern ret_t rtl8367c_getAsicRldpCheckingStatePara(uint32 *pRetryCount, uint32 *pRetryPeriod);
extern ret_t rtl8367c_setAsicRldpLoopStatePara(uint32 retryCount, uint32 retryPeriod);
extern ret_t rtl8367c_getAsicRldpLoopStatePara(uint32 *pRetryCount, uint32 *pRetryPeriod);
extern ret_t rtl8367c_setAsicRldpTxPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicRldpTxPortmask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicRldpMagicNum(ether_addr_t seed);
extern ret_t rtl8367c_getAsicRldpMagicNum(ether_addr_t *pSeed);
extern ret_t rtl8367c_getAsicRldpLoopedPortmask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicRldp8051Portmask(uint32 portmask);
extern ret_t rtl8367c_getAsicRldp8051Portmask(uint32 *pPortmask);


extern ret_t rtl8367c_getAsicRldpRandomNumber(ether_addr_t *pRandNumber);
extern ret_t rtl8367c_getAsicRldpLoopedPortPair(uint32 port, uint32 *pLoopedPair);
extern ret_t rtl8367c_setAsicRlppTrap8051(uint32 enabled);
extern ret_t rtl8367c_getAsicRlppTrap8051(uint32 *pEnabled);

extern ret_t rtl8367c_setAsicRldpLeaveLoopedPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicRldpLeaveLoopedPortmask(uint32 *pPortmask);

extern ret_t rtl8367c_setAsicRldpEnterLoopedPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicRldpEnterLoopedPortmask(uint32 *pPortmask);

extern ret_t rtl8367c_setAsicRldpTriggerMode(uint32 enabled);
extern ret_t rtl8367c_getAsicRldpTriggerMode(uint32 *pEnabled);

#endif /*_RTL8367C_ASICDRV_RLDP_H_*/

