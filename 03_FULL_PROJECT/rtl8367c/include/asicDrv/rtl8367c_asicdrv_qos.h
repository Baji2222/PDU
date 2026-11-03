#ifndef _RTL8367C_ASICDRV_QOS_H_
#define _RTL8367C_ASICDRV_QOS_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_DECISIONPRIMAX    0xFF

/* enum Priority Selection Types */
enum PRIDECISION
{
    PRIDEC_PORT = 0,
    PRIDEC_ACL,
    PRIDEC_DSCP,
    PRIDEC_1Q,
    PRIDEC_1AD,
    PRIDEC_CVLAN,
    PRIDEC_DA,
    PRIDEC_SA,
    PRIDEC_END,
};

/* enum Priority Selection Index */
enum RTL8367C_PRIDEC_TABLE
{
    PRIDEC_IDX0 = 0,
    PRIDEC_IDX1,
    PRIDEC_IDX_END,
};

enum RTL8367C_DOT1P_PRISEL
{
    DOT1P_PRISEL_USER =  0,
    DOT1P_PRISEL_TAG,
    DOT1P_PRISEL_END
};

enum RTL8367C_DSCP_PRISEL
{
    DSCP_PRISEL_INTERNAL =  0,
    DSCP_PRISEL_DSCP,
    DSCP_PRISEL_USER ,
    DSCP_PRISEL_END
};


extern ret_t rtl8367c_setAsicRemarkingDot1pAbility(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicRemarkingDot1pAbility(uint32 port, uint32* pEnabled);
extern ret_t rtl8367c_setAsicRemarkingDot1pParameter(uint32 priority, uint32 newPriority );
extern ret_t rtl8367c_getAsicRemarkingDot1pParameter(uint32 priority, uint32 *pNewPriority );
extern  ret_t rtl8367c_setAsicRemarkingDot1pSrc(uint32 type);
extern  ret_t rtl8367c_getAsicRemarkingDot1pSrc(uint32 *pType);
extern ret_t rtl8367c_setAsicRemarkingDscpAbility(uint32 enabled);
extern ret_t rtl8367c_getAsicRemarkingDscpAbility(uint32* pEnabled);
extern ret_t rtl8367c_setAsicRemarkingDscpParameter(uint32 priority, uint32 newDscp );
extern ret_t rtl8367c_getAsicRemarkingDscpParameter(uint32 priority, uint32* pNewDscp );

extern ret_t rtl8367c_setAsicPriorityDot1qRemapping(uint32 srcpriority, uint32 priority );
extern ret_t rtl8367c_getAsicPriorityDot1qRemapping(uint32 srcpriority, uint32 *pPriority );
extern ret_t rtl8367c_setAsicPriorityDscpBased(uint32 dscp, uint32 priority );
extern ret_t rtl8367c_getAsicPriorityDscpBased(uint32 dscp, uint32 *pPriority );
extern ret_t rtl8367c_setAsicPriorityPortBased(uint32 port, uint32 priority );
extern ret_t rtl8367c_getAsicPriorityPortBased(uint32 port, uint32 *pPriority );
extern ret_t rtl8367c_setAsicPriorityDecision(uint32 index, uint32 prisrc, uint32 decisionPri);
extern ret_t rtl8367c_getAsicPriorityDecision(uint32 index, uint32 prisrc, uint32* pDecisionPri);
extern ret_t rtl8367c_setAsicPriorityToQIDMappingTable(uint32 qnum, uint32 priority, uint32 qid );
extern ret_t rtl8367c_getAsicPriorityToQIDMappingTable(uint32 qnum, uint32 priority, uint32* pQid);
extern ret_t rtl8367c_setAsicOutputQueueMappingIndex(uint32 port, uint32 qnum );
extern ret_t rtl8367c_getAsicOutputQueueMappingIndex(uint32 port, uint32 *pQnum );

extern ret_t rtl8367c_setAsicRemarkingDscpSrc(uint32 type);
extern ret_t rtl8367c_getAsicRemarkingDscpSrc(uint32 *pType);
extern ret_t rtl8367c_setAsicRemarkingDscp2Dscp(uint32 dscp, uint32 rmkDscp);
extern ret_t rtl8367c_getAsicRemarkingDscp2Dscp(uint32 dscp, uint32 *pRmkDscp);

extern ret_t rtl8367c_setAsicPortPriorityDecisionIndex(uint32 port, uint32 index );
extern ret_t rtl8367c_getAsicPortPriorityDecisionIndex(uint32 port, uint32 *pIndex );

#endif /*#ifndef _RTL8367C_ASICDRV_QOS_H_*/

