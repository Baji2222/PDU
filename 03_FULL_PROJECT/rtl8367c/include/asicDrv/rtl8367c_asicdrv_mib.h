#ifndef _RTL8367C_ASICDRV_MIB_H_
#define _RTL8367C_ASICDRV_MIB_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_MIB_PORT_OFFSET                (0x7C)
#define RTL8367C_MIB_LEARNENTRYDISCARD_OFFSET   (0x420)

#define RTL8367C_MAX_LOG_CNT_NUM                (32)
#define RTL8367C_MIB_MAX_LOG_CNT_IDX            (RTL8367C_MAX_LOG_CNT_NUM - 1)
#define RTL8367C_MIB_LOG_CNT_OFFSET             (0x3E0)
#define RTL8367C_MIB_MAX_LOG_MODE_IDX           (16-1)

typedef enum RTL8367C_MIBCOUNTER_E{

    /* RX */
    ifInOctets = 0,

    dot3StatsFCSErrors,
    dot3StatsSymbolErrors,
    dot3InPauseFrames,
    dot3ControlInUnknownOpcodes,

    etherStatsFragments,
    etherStatsJabbers,
    ifInUcastPkts,
    etherStatsDropEvents,

    ifInMulticastPkts,
    ifInBroadcastPkts,
    inMldChecksumError,
    inIgmpChecksumError,
    inMldSpecificQuery,
    inMldGeneralQuery,
    inIgmpSpecificQuery,
    inIgmpGeneralQuery,
    inMldLeaves,
    inIgmpLeaves,

    /* TX/RX */
    etherStatsOctets,

    etherStatsUnderSizePkts,
    etherOversizeStats,
    etherStatsPkts64Octets,
    etherStatsPkts65to127Octets,
    etherStatsPkts128to255Octets,
    etherStatsPkts256to511Octets,
    etherStatsPkts512to1023Octets,
    etherStatsPkts1024to1518Octets,

    /* TX */
    ifOutOctets,

    dot3StatsSingleCollisionFrames,
    dot3StatMultipleCollisionFrames,
    dot3sDeferredTransmissions,
    dot3StatsLateCollisions,
    etherStatsCollisions,
    dot3StatsExcessiveCollisions,
    dot3OutPauseFrames,
    ifOutDiscards,

    /* ALE */
    dot1dTpPortInDiscards,
    ifOutUcastPkts,
    ifOutMulticastPkts,
    ifOutBroadcastPkts,
    outOampduPkts,
    inOampduPkts,

    inIgmpJoinsSuccess,
    inIgmpJoinsFail,
    inMldJoinsSuccess,
    inMldJoinsFail,
    inReportSuppressionDrop,
    inLeaveSuppressionDrop,
    outIgmpReports,
    outIgmpLeaves,
    outIgmpGeneralQuery,
    outIgmpSpecificQuery,
    outMldReports,
    outMldLeaves,
    outMldGeneralQuery,
    outMldSpecificQuery,
    inKnownMulticastPkts,

    /*Device only */
    dot1dTpLearnedEntryDiscards,
    RTL8367C_MIBS_NUMBER,

}RTL8367C_MIBCOUNTER;


extern ret_t rtl8367c_setAsicMIBsCounterReset(uint32 greset, uint32 qmreset, uint32 pmask);
extern ret_t rtl8367c_getAsicMIBsCounter(uint32 port, RTL8367C_MIBCOUNTER mibIdx, uint32* pcounterH, uint32* pcounterL);
extern ret_t rtl8367c_getAsicMIBsLogCounter(uint32 index, uint32 *pCounter);
extern ret_t rtl8367c_getAsicMIBsControl(uint32* pMask);

extern ret_t rtl8367c_setAsicMIBsResetValue(uint32 value);
extern ret_t rtl8367c_getAsicMIBsResetValue(uint32* value);

extern ret_t rtl8367c_setAsicMIBsUsageMode(uint32 mode);
extern ret_t rtl8367c_getAsicMIBsUsageMode(uint32* pMode);
extern ret_t rtl8367c_setAsicMIBsTimer(uint32 timer);
extern ret_t rtl8367c_getAsicMIBsTimer(uint32* pTimer);
extern ret_t rtl8367c_setAsicMIBsLoggingMode(uint32 index, uint32 mode);
extern ret_t rtl8367c_getAsicMIBsLoggingMode(uint32 index, uint32* pMode);
extern ret_t rtl8367c_setAsicMIBsLoggingType(uint32 index, uint32 type);
extern ret_t rtl8367c_getAsicMIBsLoggingType(uint32 index, uint32* pType);
extern ret_t rtl8367c_setAsicMIBsResetLoggingCounter(uint32 index);
extern ret_t rtl8367c_setAsicMIBsLength(uint32 txLengthMode, uint32 rxLengthMode);
extern ret_t rtl8367c_getAsicMIBsLength(uint32 *pTxLengthMode, uint32 *pRxLengthMode);

#endif /*#ifndef _RTL8367C_ASICDRV_MIB_H_*/

