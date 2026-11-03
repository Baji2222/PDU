#ifndef _RTL8367C_ASICDRV_FC_H_
#define _RTL8367C_ASICDRV_FC_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_PAGE_NUMBER    0x600


enum FLOW_CONTROL_TYPE
{
    FC_EGRESS = 0,
    FC_INGRESS,
};

enum FC_JUMBO_SIZE
{
    FC_JUMBO_SIZE_3K = 0,
    FC_JUMBO_SIZE_4K,
    FC_JUMBO_SIZE_6K,
    FC_JUMBO_SIZE_9K,
    FC_JUMBO_SIZE_END,

};


extern ret_t rtl8367c_setAsicFlowControlSelect(uint32 select);
extern ret_t rtl8367c_getAsicFlowControlSelect(uint32 *pSelect);
extern ret_t rtl8367c_setAsicFlowControlJumboMode(uint32 enabled);
extern ret_t rtl8367c_getAsicFlowControlJumboMode(uint32* pEnabled);
extern ret_t rtl8367c_setAsicFlowControlJumboModeSize(uint32 size);
extern ret_t rtl8367c_getAsicFlowControlJumboModeSize(uint32* pSize);
extern ret_t rtl8367c_setAsicFlowControlQueueEgressEnable(uint32 port, uint32 qid, uint32 enabled);
extern ret_t rtl8367c_getAsicFlowControlQueueEgressEnable(uint32 port, uint32 qid, uint32* pEnabled);
extern ret_t rtl8367c_setAsicFlowControlDropAll(uint32 dropall);
extern ret_t rtl8367c_getAsicFlowControlDropAll(uint32* pDropall);
extern ret_t rtl8367c_setAsicFlowControlPauseAllThreshold(uint32 threshold);
extern ret_t rtl8367c_getAsicFlowControlPauseAllThreshold(uint32 *pThreshold);
extern ret_t rtl8367c_setAsicFlowControlSystemThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSystemThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlSharedThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSharedThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortPrivateThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortPrivateThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlSystemDropThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSystemDropThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlSharedDropThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSharedDropThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortDropThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortDropThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortPrivateDropThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortPrivateDropThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlSystemJumboThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSystemJumboThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlSharedJumboThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlSharedJumboThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortJumboThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortJumboThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern ret_t rtl8367c_setAsicFlowControlPortPrivateJumboThreshold(uint32 onThreshold, uint32 offThreshold);
extern ret_t rtl8367c_getAsicFlowControlPortPrivateJumboThreshold(uint32 *pOnThreshold, uint32 *pOffThreshold);

extern ret_t rtl8367c_setAsicEgressFlowControlPortDropGap(uint32 gap);
extern ret_t rtl8367c_getAsicEgressFlowControlPortDropGap(uint32 *pGap);
extern ret_t rtl8367c_setAsicEgressFlowControlQueueDropGap(uint32 gap);
extern ret_t rtl8367c_getAsicEgressFlowControlQueueDropGap(uint32 *pGap);
extern ret_t rtl8367c_setAsicEgressFlowControlPortDropThreshold(uint32 port, uint32 threshold);
extern ret_t rtl8367c_getAsicEgressFlowControlPortDropThreshold(uint32 port, uint32 *pThreshold);
extern ret_t rtl8367c_setAsicEgressFlowControlQueueDropThreshold(uint32 qid, uint32 threshold);
extern ret_t rtl8367c_getAsicEgressFlowControlQueueDropThreshold(uint32 qid, uint32 *pThreshold);
extern ret_t rtl8367c_getAsicEgressQueueEmptyPortMask(uint32 *pPortmask);
extern ret_t rtl8367c_getAsicTotalPage(uint32 *pPageCount);
extern ret_t rtl8367c_getAsicPulbicPage(uint32 *pPageCount);
extern ret_t rtl8367c_getAsicMaxTotalPage(uint32 *pPageCount);
extern ret_t rtl8367c_getAsicMaxPulbicPage(uint32 *pPageCount);
extern ret_t rtl8367c_getAsicPortPage(uint32 port, uint32 *pPageCount);
extern ret_t rtl8367c_getAsicPortPageMax(uint32 port, uint32 *pPageCount);
extern ret_t rtl8367c_setAsicFlowControlEgressPortIndep(uint32 port, uint32 enable);
extern ret_t rtl8367c_getAsicFlowControlEgressPortIndep(uint32 port, uint32 *pEnable);

#endif /*_RTL8367C_ASICDRV_FC_H_*/

