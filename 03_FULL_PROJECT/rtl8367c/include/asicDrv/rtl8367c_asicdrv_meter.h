#ifndef _RTL8367C_ASICDRV_METER_H_
#define _RTL8367C_ASICDRV_METER_H_

#include <rtl8367c_asicdrv.h>


extern ret_t rtl8367c_setAsicShareMeter(uint32 index, uint32 rate, uint32 ifg);
extern ret_t rtl8367c_getAsicShareMeter(uint32 index, uint32 *pRate, uint32 *pIfg);
extern ret_t rtl8367c_setAsicShareMeterBucketSize(uint32 index, uint32 lbThreshold);
extern ret_t rtl8367c_getAsicShareMeterBucketSize(uint32 index, uint32 *pLbThreshold);
extern ret_t rtl8367c_setAsicShareMeterType(uint32 index, uint32 type);
extern ret_t rtl8367c_getAsicShareMeterType(uint32 index, uint32 *pType);
extern ret_t rtl8367c_setAsicMeterExceedStatus(uint32 index);
extern ret_t rtl8367c_getAsicMeterExceedStatus(uint32 index, uint32* pStatus);

#endif /*_RTL8367C_ASICDRV_FC_H_*/

