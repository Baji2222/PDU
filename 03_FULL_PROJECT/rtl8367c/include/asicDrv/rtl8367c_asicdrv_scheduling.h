#ifndef _RTL8367C_ASICDRV_SCHEDULING_H_
#define _RTL8367C_ASICDRV_SCHEDULING_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_QWEIGHTMAX    0x7F
#define RTL8367C_PORT_QUEUE_METER_INDEX_MAX    7

/* enum for queue type */
enum QUEUETYPE
{
    QTYPE_STRICT = 0,
    QTYPE_WFQ,
};
extern ret_t rtl8367c_setAsicLeakyBucketParameter(uint32 tick, uint32 token);
extern ret_t rtl8367c_getAsicLeakyBucketParameter(uint32 *tick, uint32 *token);
extern ret_t rtl8367c_setAsicAprMeter(uint32 port, uint32 qid, uint32 apridx);
extern ret_t rtl8367c_getAsicAprMeter(uint32 port, uint32 qid, uint32 *apridx);
extern ret_t rtl8367c_setAsicPprMeter(uint32 port, uint32 qid, uint32 ppridx);
extern ret_t rtl8367c_getAsicPprMeter(uint32 port, uint32 qid, uint32 *ppridx);
extern ret_t rtl8367c_setAsicAprEnable(uint32 port, uint32 aprEnable);
extern ret_t rtl8367c_getAsicAprEnable(uint32 port, uint32 *aprEnable);
extern ret_t rtl8367c_setAsicPprEnable(uint32 port, uint32 pprEnable);
extern ret_t rtl8367c_getAsicPprEnable(uint32 port, uint32 *pprEnable);

extern ret_t rtl8367c_setAsicWFQWeight(uint32, uint32 queueid, uint32 weight );
extern ret_t rtl8367c_getAsicWFQWeight(uint32, uint32 queueid, uint32 *weight );
extern ret_t rtl8367c_setAsicWFQBurstSize(uint32 burstsize);
extern ret_t rtl8367c_getAsicWFQBurstSize(uint32 *burstsize);

extern ret_t rtl8367c_setAsicQueueType(uint32 port, uint32 qid, uint32 queueType);
extern ret_t rtl8367c_getAsicQueueType(uint32 port, uint32 qid, uint32 *queueType);
extern ret_t rtl8367c_setAsicQueueRate(uint32 port, uint32 qid, uint32 ppridx, uint32 apridx );
extern ret_t rtl8367c_getAsicQueueRate(uint32 port, uint32 qid, uint32* ppridx, uint32* apridx );
extern ret_t rtl8367c_setAsicPortEgressRate(uint32 port, uint32 rate);
extern ret_t rtl8367c_getAsicPortEgressRate(uint32 port, uint32 *rate);
extern ret_t rtl8367c_setAsicPortEgressRateIfg(uint32 ifg);
extern ret_t rtl8367c_getAsicPortEgressRateIfg(uint32 *ifg);

#endif /*_RTL8367C_ASICDRV_SCHEDULING_H_*/

