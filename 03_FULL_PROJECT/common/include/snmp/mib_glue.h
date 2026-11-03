#ifndef __MIB_GLUE_H__
#define __MIB_GLUE_H__

#include <common_types.h>
#include <local/mib_config.h>
#include <salmib.h>

#if MIB_ENABLE

#define MIB_RESET() rtk_stat_global_reset()

extern int32 mib_glue_portCounter_get(sal_stat_port_type_t mibIndex, u8_t interface, u8_t size, u8_t *ptr);
//extern mib_glue_gobalCounter_get(sal_stat_global_type_t mibName, u8_t size, u8_t *ptr);

#if MIB_INTERFACE_GROUP_ENABLE
  #define GET_IF_IN_MULTICAST_PKTS(if_number, size, ptr)          mib_glue_portCounter_get(SAL_ETHER_STATS_MCAST_PKTS_INDEX, if_number, size, ptr);
  #define GET_IF_IN_BROADCAST_PKTS(if_number, size, ptr)          mib_glue_portCounter_get(SAL_ETHER_STATS_BCAST_PKTS_INDEX, if_number, size, ptr);
  #define GET_IF_OUT_MULTICAST_PKTS(if_number, size, ptr)         mib_glue_portCounter_get(SAL_OUT_MCAST_PKTS_CNT_INDEX, if_number, size, ptr);
  #define GET_IF_OUT_BROADCAST_PKTS(if_number, size, ptr)         mib_glue_portCounter_get(SAL_OUT_BCAST_PKTS_CNT_INDEX, if_number, size, ptr);
  #define GET_IF_HC_IN_OCTETS(if_number, size, ptr)               mib_glue_portCounter_get(SAL_IF_IN_OCTETS_INDEX, if_number, size, ptr);
  #define GET_IF_HC_IN_UCAST_PKTS(if_number, size, ptr)           mib_glue_portCounter_get(SAL_IN_UCAST_PKTS_CNT_INDEX, if_number, size, ptr);
  #define GET_IF_HC_IN_MULTICAST_PKTS(if_number, size, ptr)       mib_glue_portCounter_get(SAL_ETHER_STATS_MCAST_PKTS_INDEX, if_number, size, ptr);
  #define GET_IF_HC_IN_BROADCAST_PKTS(if_number, size, ptr)       mib_glue_portCounter_get(SAL_ETHER_STATS_BCAST_PKTS_INDEX, if_number, size, ptr);
  
  #define GET_IF_HC_OUT_OCTETS(if_number, size, ptr)              mib_glue_portCounter_get(SAL_IF_OUT_OCTETS_INDEX, if_number, size, ptr);
  #define GET_IF_HC_OUT_UCAST_PKTS(if_number, size, ptr)          mib_glue_portCounter_get(SAL_OUT_UCAST_PKTS_CNT_INDEX, if_number, size, ptr);
  #define GET_IF_HC_OUT_MULTICAST_PKTS(if_number, size, ptr)      mib_glue_portCounter_get(SAL_OUT_MCAST_PKTS_CNT_INDEX, if_number, size, ptr);
  #define GET_IF_HC_OUT_BROADCAST_PKTS(if_number, size, ptr)      mib_glue_portCounter_get(SAL_OUT_BCAST_PKTS_CNT_INDEX, if_number, size, ptr);

  #define GET_DOT1D_IN_DISCARDS_PKTS(if_number, size, ptr)           mib_glue_portCounter_get(SAL_DOT1D_HC_IN_DISCARDS_INDEX, if_number, size, ptr);
  
#else
  #define GET_IF_IN_MULTICAST_PKTS(if_number, size, ptr)
  #define GET_IF_IN_BROADCAST_PKTS(if_number, size, ptr)
  #define GET_IF_OUT_MULTICAST_PKTS(if_number, size, ptr)
  #define GET_IF_OUT_BROADCAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_IN_OCTETS(if_number, size, ptr)
  #define GET_IF_HC_IN_UCAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_IN_MULTICAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_IN_BROADCAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_OUT_OCTETS(if_number, size, ptr)
  #define GET_IF_HC_OUT_UCAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_OUT_MULTICAST_PKTS(if_number, size, ptr)
  #define GET_IF_HC_OUT_BROADCAST_PKTS(if_number, size, ptr)
  #define GET_IF_LINK_UPDOWN_TRAP_ENABLE(if_number, size, ptr)
  #define GET_IF_HIGH_SPEED(if_number, size, ptr)
  #define GET_IF_PROMISCUOUS_MODE(if_number, size, ptr)
  #define GET_IF_CONNECTOR_PRESENT(if_number, size, ptr)
  #define GET_IF_ALIAS(if_number, size, ptr)
  #define GET_IF_COUNTER_DISCONTINUITY_TIME(if_number, size, ptr) 
   #define GET_DOT1D_IN_DISCARDS_PKTS(if_number, size, ptr) 
#endif /* end of MIB_INTERFACE_GROUP_ENABLE */

#if MIB_ETHERNET_LIKE_ENABLE
  #define GET_DOT3_STATS_ALIGNMENT_ERRORS(if_number, size, ptr)                   mib_glue_portCounter_get(SAL_DOT3_STATS_ALIGNMENT_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_FCS_ERRORS(if_number, size, ptr)                         mib_glue_portCounter_get(SAL_DOT3_STATS_FCS_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_SINGLE_COLLISION_FRAMES(if_number, size, ptr)            mib_glue_portCounter_get(SAL_DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_MULTIPLE_COLLISION_FRAMES(if_number, size, ptr)          mib_glue_portCounter_get(SAL_DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_SQETEST_ERRORS(if_number, size, ptr)                     mib_glue_portCounter_get(SAL_DOT3_STATS_SQETEST_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_DEFERED_TRANSMISSIONS(if_number, size, ptr)              mib_glue_portCounter_get(SAL_DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_LATE_COLLISIONS(if_number, size, ptr)                    mib_glue_portCounter_get(SAL_DOT3_STATS_LATE_COLLISIONS_INDEX, if_number, size, ptr); 
  #define GET_DOT3_STATS_EXCESSIVE_COLLISIONS(if_number, size, ptr)               mib_glue_portCounter_get(SAL_DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_number, size, ptr)       mib_glue_portCounter_get(SAL_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_CARRIER_SENSE_ERRORS(if_number, size, ptr)               mib_glue_portCounter_get(SAL_DOT3_STATS_CARRIER_SENSE_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_FRAME_TOO_LONGS(if_number, size, ptr)                    mib_glue_portCounter_get(SAL_DOT3_STATS_FRAME_TOO_LONGS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_number, size, ptr)        mib_glue_portCounter_get(SAL_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_ETHER_CHIPSET(if_number, size, ptr)                      mib_glue_portCounter_get(SAL_DOT3_STATS_ETHER_CHIPSET_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_SYMBOL_ERRORS(if_number, size, ptr)                      mib_glue_portCounter_get(SAL_DOT3_STATS_SYMBOL_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_RATE_CONTROL_ABILITY(if_number, size, ptr)               mib_glue_portCounter_get(SAL_DOT3_STATS_RATE_CONTROL_ABILITY_INDEX, if_number, size, ptr);
  #define GET_DOT3_STATS_RATE_CONTROL_STATUS(if_number, size, ptr)                mib_glue_portCounter_get(SAL_DOT3_STATS_RATE_CONTROL_STATUS_INDEX, if_number, size, ptr);
  #define GET_DOT3_COLL_COUNT(if_number, size, ptr)                               mib_glue_portCounter_get(SAL_DOT3_COLL_COUNT_INDEX, if_number, size, ptr);
  #define GET_DOT3_COLL_FREQUENCIES(if_number, size, ptr)                         mib_glue_portCounter_get(SAL_DOT3_COLL_FREQUENCIES_INDEX, if_number, size, ptr);
  #define GET_CONTROL_FUNCTIONS_SUPPORTED(if_number, size, ptr)                   mib_glue_portCounter_get(SAL_CONTROL_FUNCTIONS_SUPPORTED_INDEX, if_number, size, ptr);
  #define GET_CONTROL_UNKNOWN_OPCODES(if_number, size, ptr)                       mib_glue_portCounter_get(SAL_DOT3_CTRL_IN_UNKNOWN_OPCODES_INDEX, if_number, size, ptr);
  #define GET_HC_CONTROL_UNKNOWN_OPCODES(if_number, size, ptr)                    mib_glue_portCounter_get(SAL_DOT3_CTRL_IN_UNKNOWN_OPCODES_INDEX, if_number, size, ptr);
  #define GET_IN_PAUSE_FRAMES(if_number, size, ptr)                               mib_glue_portCounter_get(SAL_DOT3_IN_PAUSE_FRAMES_INDEX, if_number, size, ptr);
  #define GET_OUT_PAUSE_FRAMES(if_number, size, ptr)                              mib_glue_portCounter_get(SAL_DOT3_OUT_PAUSE_FRAMES_INDEX, if_number, size, ptr);
  #define GET_HC_IN_PAUSE_FRAMES(if_number, size, ptr)                            mib_glue_portCounter_get(SAL_DOT3_IN_PAUSE_FRAMES_INDEX, if_number, size, ptr);
  #define GET_HC_OUT_PAUSE_FRAMES(if_number, size, ptr)                           mib_glue_portCounter_get(SAL_DOT3_OUT_PAUSE_FRAMES_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_ALIGNMENT_ERRORS(if_number, size, ptr)                mib_glue_portCounter_get(SAL_DOT3_STATS_ALIGNMENT_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_FCS_ERRORS(if_number, size, ptr)                      mib_glue_portCounter_get(SAL_DOT3_STATS_FCS_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_number, size, ptr)    mib_glue_portCounter_get(SAL_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_FRAME_TOO_LONGS(if_number, size, ptr)                 mib_glue_portCounter_get(SAL_DOT3_STATS_FRAME_TOO_LONGS_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_number, size, ptr)     mib_glue_portCounter_get(SAL_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS_INDEX, if_number, size, ptr);
  #define GET_DOT3_HC_STATS_SYMBOL_ERRORS(if_number, size, ptr)                   mib_glue_portCounter_get(SAL_DOT3_STATS_SYMBOL_ERRORS_INDEX, if_number, size, ptr);
  
#else  /* else of MIB_ETHERNET_LIKE_ENABLE */
  #define GET_DOT3_STATS_ALIGNMENT_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_FCS_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_ALIGNMENT_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_FCS_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_SINGLE_COLLISION_FRAMES(if_number, size, ptr)
  #define GET_DOT3_STATS_MULTIPLE_COLLISION_FRAMES(if_number, size, ptr)
  #define GET_DOT3_STATS_SQETEST_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_DEFERED_TRANSMISSIONS(if_number, size, ptr)
  #define GET_DOT3_STATS_LATE_COLLISIONS(if_number, size, ptr)
  #define GET_DOT3_STATS_EXCESSIVE_COLLISIONS(if_number, size, ptr)
  #define GET_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_CARRIER_SENSE_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_FRAME_TOO_LONGS(if_number, size, ptr)
  #define GET_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_ETHER_CHIPSET(if_number, size, ptr)
  #define GET_DOT3_STATS_SYMBOL_ERRORS(if_number, size, ptr)
  #define GET_DOT3_STATS_RATE_CONTROL_ABILITY(if_number, size, ptr)
  #define GET_DOT3_STATS_RATE_CONTROL_STATUS(if_number, size, ptr)
  #define GET_DOT3_COLL_COUNT(if_number, size, ptr)
  #define GET_DOT3_COLL_FREQUENCIES(if_number, size, ptr)
  #define GET_CONTROL_FUNCTIONS_SUPPORTED(if_number, size, ptr)
  #define GET_CONTROL_UNKNOWN_OPCODES(if_number, size, ptr)
  #define GET_HC_CONTROL_UNKNOWN_OPCODES(if_number, size, ptr)
  #define GET_IN_PAUSE_FRAMES(if_number, size, ptr)
  #define GET_OUT_PAUSE_FRAMES(if_number, size, ptr)
  #define GET_HC_IN_PAUSE_FRAMES(if_number, size, ptr)
  #define GET_HC_OUT_PAUSE_FRAMES(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_ALIGNMENT_ERRORS(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_FCS_ERRORS(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_FRAME_TOO_LONGS(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_number, size, ptr)
  #define GET_DOT3_HC_STATS_SYMBOL_ERRORS(if_number, size, ptr)
#endif /* end of MIB_ETHERNET_LIKE_ENABLE */

#if MIB_RMON_ENABLE
  #define GET_ETHER_STATS_DROP_EVENTS(if_number, size, ptr)                   mib_glue_portCounter_get(SAL_ETHER_STATS_DROP_EVENTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_OCTETS(if_number, size, ptr)                        mib_glue_portCounter_get(SAL_ETHER_STATS_OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS(if_number, size, ptr)                          mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_BROADCAST_PKTS(if_number, size, ptr)                mib_glue_portCounter_get(SAL_ETHER_STATS_BCAST_PKTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_MULTICAST_PKTS(if_number, size, ptr)                mib_glue_portCounter_get(SAL_ETHER_STATS_MCAST_PKTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_CRC_ALIGN_ERRORS(if_number, size, ptr)              mib_glue_portCounter_get(SAL_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_UNDERSIZE_PKTS(if_number, size, ptr)                mib_glue_portCounter_get(SAL_ETHER_STATS_UNDER_SIZE_PKTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_OVERSIZE_PKTS(if_number, size, ptr)                 mib_glue_portCounter_get(SAL_ETHER_STATS_OVERSIZE_PKTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_FRAGMENTS(if_number, size, ptr)                     mib_glue_portCounter_get(SAL_ETHER_STATS_FRAGMENTS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_JABBERS(if_number, size, ptr)                       mib_glue_portCounter_get(SAL_ETHER_STATS_JABBERS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_COLLISIONS(if_number, size, ptr)                    mib_glue_portCounter_get(SAL_ETHER_STATS_COLLISIONS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_64_OCTECTS(if_number, size, ptr)               mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_64OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_65TO127_OCTECTS(if_number, size, ptr)          mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_65TO127OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_128TO255_OCTECTS(if_number, size, ptr)         mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_128TO255OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_256TO511_OCTECTS(if_number, size, ptr)         mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_256TO511OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_512TO1023_OCTECTS(if_number, size, ptr)        mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_512TO1023OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_PKTS_1024TO1518_OCTECTS(if_number, size, ptr)       mib_glue_portCounter_get(SAL_ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX, if_number, size, ptr);
  #define GET_ETHER_STATS_OWNER(if_number, size, ptr)                         mib_glue_portCounter_get(SAL_ETHER_STATS_OWNER, if_number, size, ptr);
  #define GET_ETHER_STATS_STATUS(if_number, size, ptr)                        mib_glue_portCounter_get(SAL_DOT3_STATS_SYMBOL_ERRORS_INDEX, if_number, size, ptr);
#else /* else of MIB_RMON_ENABLE */
  #define GET_ETHER_STATS_DROP_EVENTS(if_number, size, ptr)
  #define GET_ETHER_STATS_OCTETS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS(if_number, size, ptr)
  #define GET_ETHER_STATS_BROADCAST_PKTS(if_number, size, ptr)
  #define GET_ETHER_STATS_MULTICAST_PKTS(if_number, size, ptr)
  #define GET_ETHER_STATS_CRC_ALIGN_ERRORS(if_number, size, ptr)
  #define GET_ETHER_STATS_UNDERSIZE_PKTS(if_number, size, ptr)
  #define GET_ETHER_STATS_OVERSIZE_PKTS(if_number, size, ptr)
  #define GET_ETHER_STATS_FRAGMENTS(if_number, size, ptr)
  #define GET_ETHER_STATS_JABBERS(if_number, size, ptr)
  #define GET_ETHER_STATS_COLLISIONS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_64_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_65TO127_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_128TO255_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_256TO511_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_512TO1023_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_PKTS_1024TO1518_OCTECTS(if_number, size, ptr)
  #define GET_ETHER_STATS_OWNER(if_number, size, ptr)
  #define GET_ETHER_STATS_STATUS(if_number, size, ptr)
#endif /* end of MIB_RMON_ENABLE */

#if MIB_BRIDGE_ENABLE
#define GET_DOT1D_TPHCPORT_IN_FRAMES(if_nummber, size, ptr)  mib_glue_portCounter_get(SAL_DOT1D_HC_IN_FRAMS_INDEX, if_nummber, size, ptr);
#define GET_DOT1D_TPHCPORT_OUT_FRAMES(if_nummber, size, ptr)  mib_glue_portCounter_get(SAL_DOT1D_HC_OUT_FRAMS_INDEX, if_nummber, size, ptr);
#define GET_DOT1D_TPHCPORT_IN_DISCARDS(if_nummber, size, ptr) mib_glue_portCounter_get(SAL_DOT1D_HC_IN_DISCARDS_INDEX, if_nummber, size, ptr);
#else
#define GET_DOT1D_TPHCPORT_IN_FRAMES(if_nummber, size, ptr) 
#define GET_DOT1D_TPHCPORT_OUT_FRAMES(if_nummber, size, ptr) 
#define GET_DOT1D_TPHCPORT_IN_DISCARDS(if_nummber, size, ptr)
#endif

#else /* else of MIB_ENABLE */
  #define MIB_RESET()
#endif /* end of MIB_ENABLE */

#endif /* end of __MIB_GLUE_H__ */

