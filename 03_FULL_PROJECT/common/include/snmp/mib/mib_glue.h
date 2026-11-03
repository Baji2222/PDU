#ifndef __MIB_GLUE_H__
#define __MIB_GLUE_H__

#if MIB_ENABLE
#define MIB_RESET() rtk_stat_global_reset()

extern int32 mib_portCounter_get(sal_stat_port_type_t, sal_port_t, uint8, uint8*);

#if MIB_INTERFACE_GROUP_ENABLE
#define GET_IF_IN_MULTICAST_PKTS(if_nummber, size, ptr)          mib_portCounter_get(SAL_IN_MCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_IN_BROADCAST_PKTS(if_nummber, size, ptr)          mib_portCounter_get(SAL_IN_BCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_OUT_MULTICAST_PKTS(if_nummber, size, ptr)         mib_portCounter_get(SAL_OUT_MCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_OUT_BROADCAST_PKTS(if_nummber, size, ptr)         mib_portCounter_get(SAL_OUT_BCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_IN_OCTETS(if_nummber, size, ptr)               mib_portCounter_get(SAL_IF_OUT_OCTETS_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_IN_UCAST_PKTS(if_nummber, size, ptr)           mib_portCounter_get(SAL_IN_UCAST_PKTS_CNT_INDEX,, if_nummber, size, ptr)
#define GET_IF_HC_IN_MULTICAST_PKTS(if_nummber, size, ptr)       mib_portCounter_get(SAL_IN_MCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_IN_BROADCAST_PKTS(if_nummber, size, ptr)       mib_portCounter_get(SAL_IN_BCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)

#define GET_IF_HC_OUT_OCTETS(if_nummber, size, ptr)              mib_portCounter_get(SAL_OUT_OCTETS_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_OUT_UCAST_PKTS(if_nummber, size, ptr)          mib_portCounter_get(SAL_OUT_UCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_OUT_MULTICAST_PKTS(if_nummber, size, ptr)      mib_portCounter_get(SAL_OUT_MCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)
#define GET_IF_HC_OUT_BROADCAST_PKTS(if_nummber, size, ptr)      mib_portCounter_get(SAL_OUT_BCAST_PKTS_CNT_INDEX, if_nummber, size, ptr)

#else /* else of #if MIB_INTERFACE_GROUP_ENABLE */
#define GET_IF_IN_MULTICAST_PKTS(if_nummber, size, ptr)
#define GET_IF_IN_BROADCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_OUT_MULTICAST_PKTS(if_nummber, size, ptr)
#define GET_IF_OUT_BROADCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_IN_OCTETS(if_nummber, size, ptr)
#define GET_IF_HC_IN_UCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_IN_MULTICAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_IN_BROADCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_OUT_OCTETS(if_nummber, size, ptr)
#define GET_IF_HC_OUT_UCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_OUT_MULTICAST_PKTS(if_nummber, size, ptr)
#define GET_IF_HC_OUT_BROADCAST_PKTS(if_nummber, size, ptr)
#define GET_IF_LINK_UPDOWN_TRAP_ENABLE(if_nummber, size, ptr)
#define GET_IF_HIGH_SPEED(if_nummber, size, ptr)
#define GET_IF_PROMISCUOUS_MODE(if_nummber, size, ptr)
#define GET_IF_CONNECTOR_PRESENT(if_nummber, size, ptr)
#define GET_IF_ALIAS(if_nummber, size, ptr)
#define GET_IF_COUNTER_DISCONTINUITY_TIME(if_nummber, size, ptr) 
#endif /* end of MIB_INTERFACE_GROUP_ENABLE */

#if MIB_ETHERNET_LIKE_ENABLE
#define GET_DOT3_STATS_ALIGNMENT_ERRORS(if_nummber, size, ptr)           mib_portCounter_get(SAL_DOT3_STATS_ALIGNMENT_ERRORS_INDEX, if_nummber, size, ptr)
#define GET_DOT3_STATS_FCS_ERRORS(if_nummber, size, ptr)                 mib_portCounter_get(SAL_DOT3_STATS_FCS_ERRORS_INDEX, if_nummber, size, ptr)
#define GET_DOT3_STATS_SINGLE_COLLISION_FRAMES(if_nummber, size, ptr)    mib_portCounter_get(SAL_DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX, if_nummber, size, ptr)
#define GET_DOT3_STATS_MULTIPLE_COLLISION_FRAMES(if_nummber, size, ptr)
#endif

#else /* else of if MIB_ENABLE */
#endif /* end of MIB_ENABLE */

#endif /* end of __MIB_GLUE_H__ */

