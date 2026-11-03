#ifndef __RTK_SAL_MIB_H__
#define __RTK_SAL_MIB_H__

#include "local/config.h"
#include "common_types.h"
#include "api/rtk_error.h"

#if MIB_ENABLE
extern int16 rtk_stat_global_reset(void);
//extern int16 rtk_stat_port_reset(rtk_port_t port);
//extern int16 rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntr_idx, rtk_stat_counter_t *pCntr);
//extern int16 rtk_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs);

#define MIB_RESET() rtk_stat_global_reset()

#if MIB_INTERFACE_GROUP_ENABLE
extern u8_t get_IfInMulticastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfInBroadcastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfOutMulticastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfOutBroadcastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfHCInOctets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfHCInUcastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfHCOutOctets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_IfHCOutUcastPkts(u8_t interface, u8_t size, u8_t *ptr);

  
#define GET_IF_IN_MULTICAST_PKTS(if_nummber, size, ptr)          get_IfInMulticastPkts(if_nummber, size, ptr)
#define GET_IF_IN_BROADCAST_PKTS(if_nummber, size, ptr)          get_IfInBroadcastPkts(if_nummber, size, ptr)
#define GET_IF_OUT_MULTICAST_PKTS(if_nummber, size, ptr)         get_IfOutMulticastPkts(if_nummber, size, ptr)
#define GET_IF_OUT_BROADCAST_PKTS(if_nummber, size, ptr)         get_IfOutBroadcastPkts(if_nummber, size, ptr)
#define GET_IF_HC_IN_OCTETS(if_nummber, size, ptr)               get_IfHCInOctets(if_nummber, size, ptr)
#define GET_IF_HC_IN_UCAST_PKTS(if_nummber, size, ptr)           get_IfHCInUcastPkts(if_nummber, size, ptr)
#define GET_IF_HC_IN_MULTICAST_PKTS(if_nummber, size, ptr)       get_IfInMulticastPkts(if_nummber, size, ptr)
#define GET_IF_HC_IN_BROADCAST_PKTS(if_nummber, size, ptr)       get_IfInBroadcastPkts(if_nummber, size, ptr)
  
#define GET_IF_HC_OUT_OCTETS(if_nummber, size, ptr)              get_IfHCOutOctets(if_nummber, size, ptr)
#define GET_IF_HC_OUT_UCAST_PKTS(if_nummber, size, ptr)          get_IfHCOutUcastPkts(if_nummber, size, ptr)
#define GET_IF_HC_OUT_MULTICAST_PKTS(if_nummber, size, ptr)      get_IfOutMulticastPkts(if_nummber, size, ptr)
#define GET_IF_HC_OUT_BROADCAST_PKTS(if_nummber, size, ptr)      get_IfOutBroadcastPkts(if_nummber, size, ptr)

/*  For Example  
#define GET_IF_IN_MULTICAST_PKTS(if_nummber, size, ptr)          (*(ptr) = 2379)
#define GET_IF_IN_BROADCAST_PKTS(if_nummber, size, ptr)          (*(ptr) = 111)
#define GET_IF_OUT_MULTICAST_PKTS(if_nummber, size, ptr)         (*(ptr) = 9732) 
#define GET_IF_OUT_BROADCAST_PKTS(if_nummber, size, ptr)         (*(ptr) = 5106)
#define GET_IF_HC_IN_OCTETS(if_nummber, size, ptr)               do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_IN_UCAST_PKTS(if_nummber, size, ptr)           do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_IN_MULTICAST_PKTS(if_nummber, size, ptr)       do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_IN_BROADCAST_PKTS(if_nummber, size, ptr)       do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_OUT_OCTETS(if_nummber, size, ptr)              do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_OUT_UCAST_PKTS(if_nummber, size, ptr)          do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_OUT_MULTICAST_PKTS(if_nummber, size, ptr)      do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
#define GET_IF_HC_OUT_BROADCAST_PKTS(if_nummber, size, ptr)      do{ptr[0] = 1111; ptr[1] = 2222;}while(0)
//#define GET_IF_LINK_UPDOWN_TRAP_ENABLE(if_nummber, size, ptr)    (*(ptr) = 0)
#define GET_IF_HIGH_SPEED(if_nummber, size, ptr)                 (*(ptr) = 0)
#define GET_IF_PROMISCUOUS_MODE(if_nummber, size, ptr)           (*(ptr) = 0)
#define GET_IF_CONNECTOR_PRESENT(if_nummber, size, ptr)          (*(ptr) = 0)
#define GET_IF_ALIAS(if_nummber, size, ptr)                      (*(ptr) = 0)
#define GET_IF_COUNTER_DISCONTINUITY_TIME(if_nummber, size, ptr) (*(ptr) = 0)
  */
#else  /* else of MIB_INTERFACE_GROUP_ENABLE */
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
#endif /* end of else of MIB_INTERFACE_GROUP_ENABLE */

#if MIB_ETHERNET_LIKE_ENABLE

extern u8_t get_Dot3StatsFCSError(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsSingleCollisionFrames(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsMultipleCollisionFrames(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsDeferedTransmissions(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsLateCollisions(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsExcessiveCollisions(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsSymbolErrors(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3StatsDuplexStatus(u8_t *ptr);
extern u8_t get_Dot3ControlInUnknownOpcodes(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3InPauseFrames(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_Dot3OutPauseFrames(u8_t interface, u8_t size, u8_t *ptr);

#define SNMP_ETHERLINK_DUPLEX_STATUS_UNKNOW         (1UL)
#define SNMP_ETHERLINK_DUPLEX_STATUS_HALF           (2UL)
#define SNMP_ETHERLINK_DUPLEX_STATUS_FULL           (3UL)
#define SNMP_ETHERLINK_RATE_CONTROL_STATUS_OFF      (1UL)
#define SNMP_ETHERLINK_RATE_CONTROL_STATUS_ON       (2UL)
#define SNMP_ETHERLINK_RATE_CONTROL_STATUS_UNKNOWN  (3UL)

/* dot3StatsEntry */
#define GET_DOT3_STATS_ALIGNMENT_ERRORS(if_nummber, size, ptr)           \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)
		
#define GET_DOT3_STATS_FCS_ERRORS(if_nummber, size, ptr)                 \
	get_Dot3StatsFCSError(if_nummber, size, ptr)
		
#define GET_DOT3_STATS_SINGLE_COLLISION_FRAMES(if_nummber, size, ptr)    \
	get_Dot3StatsSingleCollisionFrames(if_nummber, size, ptr)
		
#define GET_DOT3_STATS_MULTIPLE_COLLISION_FRAMES(if_nummber, size, ptr)  \
	get_Dot3StatsMultipleCollisionFrames(if_nummber, size, ptr)
		
#define GET_DOT3_STATS_SQETEST_ERRORS(if_nummber, size, ptr)             \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)
		
#define GET_DOT3_STATS_DEFERED_TRANSMISSIONS(if_nummber, size, ptr)      \
	get_Dot3StatsDeferedTransmissions(if_nummber, size, ptr)
	
#define GET_DOT3_STATS_LATE_COLLISIONS(if_nummber, size, ptr)            \
	get_Dot3StatsLateCollisions(if_nummber, size, ptr)
	
#define GET_DOT3_STATS_EXCESSIVE_COLLISIONS(if_nummber, size, ptr)        \
	get_Dot3StatsExcessiveCollisions(if_nummber, size, ptr)

#define GET_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_nummber, size, ptr) \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)	
	
#define GET_DOT3_STATS_CARRIER_SENSE_ERRORS(if_nummber, size, ptr)       \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)

#define GET_DOT3_STATS_FRAME_TOO_LONGS(if_nummber, size, ptr)            \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)

#define GET_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_nummber, size, ptr)\
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)
	
#define GET_DOT3_STATS_ETHER_CHIPSET(if_nummber, size, ptr)              \
	do{;}while(0)

#define GET_DOT3_STATS_SYMBOL_ERRORS(if_nummber, size, ptr)              \
	get_Dot3StatsSymbolErrors(if_nummber, size, ptr)

#define GET_DOT3_STATS_RATE_CONTROL_ABILITY(if_nummber, size, ptr)       \
	do{                                                                  \
		u32_t *u32_ptr = (u32_t*)(ptr);                                  \
		*u32_ptr = 0;                                                    \
	}while(0)

#define GET_DOT3_STATS_RATE_CONTROL_STATUS(if_nummber, size, ptr)        \
	do{                                                                  \
		u32_t *u32_ptr = (u32_t*)(ptr);                                  \
		*u32_ptr = SNMP_ETHERLINK_RATE_CONTROL_STATUS_UNKNOWN;           \
	}while(0)


/* dot3CollEntry */
#define GET_DOT3_COLL_COUNT(if_nummber, size, ptr)                       \
	do{;}while(0)

#define GET_DOT3_COLL_FREQUENCIES(if_nummber, size, ptr)                 \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)


/* dot3ControlEntry */
#define GET_CONTROL_FUNCTIONS_SUPPORTED(if_nummber, size, ptr)           \
	do{;}while(0)

#define GET_CONTROL_UNKNOWN_OPCODES(if_nummber, size, ptr)               \
	get_Dot3ControlInUnknownOpcodes(if_nummber, size, ptr) 

#define GET_HC_CONTROL_UNKNOWN_OPCODES(if_nummber, size, ptr)            \
	get_Dot3ControlInUnknownOpcodes(if_nummber, size, ptr)


/* dot3PauseEntry */
#define GET_IN_PAUSE_FRAMES(if_nummber, size, ptr)                       \
	get_Dot3InPauseFrames(if_nummber, size, ptr)  

#define GET_OUT_PAUSE_FRAMES(if_nummber, size, ptr)                      \
	get_Dot3OutPauseFrames(if_nummber, size, ptr)

#define GET_HC_IN_PAUSE_FRAMES(if_nummber, size, ptr)                    \
	get_Dot3InPauseFrames(if_nummber, size, ptr)

#define GET_HC_OUT_PAUSE_FRAMES(if_nummber, size, ptr)                   \
	get_Dot3OutPauseFrames(if_nummber, size, ptr)

/* dot3HCStatsEntry */
#define GET_DOT3_HC_STATS_ALIGNMENT_ERRORS(if_nummber, size, ptr)        \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)
		
#define GET_DOT3_HC_STATS_FCS_ERRORS(if_nummber, size, ptr)              \
	get_Dot3StatsFCSError(if_nummber, size, ptr)

#define GET_DOT3_HC_STATS_INTERNAL_MAC_TRANSMIT_ERRORS(if_nummber, size, ptr)             \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)	

#define GET_DOT3_HC_STATS_FRAME_TOO_LONGS(if_nummber, size, ptr)         \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)

#define GET_DOT3_HC_STATS_INTERNAL_MAC_RECEIVE_ERRORS(if_nummber, size, ptr)             \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)

#define GET_DOT3_HC_STATS_SYMBOL_ERRORS(if_nummber, size, ptr)           \
	get_Dot3StatsSymbolErrors(if_nummber, size, ptr)

#else /* else of MIB_ETHERNET_LIKE_ENABLE */

#endif /* end of else of MIB_ETHERNET_LIKE_ENABLE */

#if MIB_RMON_ENABLE
extern u8_t get_EtherStatsOctets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherDropEvents(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsBroadcastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsMulticastPkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsUndersizePkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsOversizePkts(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsFragments(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsJabbers(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsCollisions(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts64Octets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts65To127Octets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts128To255Octets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts256To511Octets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts512To1023Octets(u8_t interface, u8_t size, u8_t *ptr);
extern u8_t get_EtherStatsPkts1024To1518Octets(u8_t interface, u8_t size, u8_t *ptr);


#define GET_ETHER_STATS_DROP_EVENTS(if_nummber, size, ptr)         \
	get_EtherDropEvents(if_nummber, size, ptr)

#define GET_ETHER_STATS_OCTETS(if_nummber, size, ptr)         \
	get_EtherStatsOctets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts(if_nummber, size, ptr)

#define GET_ETHER_STATS_BROADCAST_PKTS(if_nummber, size, ptr)         \
	get_EtherStatsBroadcastPkts(if_nummber, size, ptr)

#define GET_ETHER_STATS_MULTICAST_PKTS(if_nummber, size, ptr)         \
	get_EtherStatsMulticastPkts(if_nummber, size, ptr)

#define GET_ETHER_STATS_CRC_ALIGN_ERRORS(if_nummber, size, ptr)         \
	do{ u16_t i;                                                         \
		u8_t *u8_ptr = (u8_t*)(ptr);                                     \
		for(i=0;i<size;i++){ u8_ptr[i] = 0;}                             \
	}while(0)

#define GET_ETHER_STATS_UNDERSIZE_PKTS(if_nummber, size, ptr)         \
	get_EtherStatsUndersizePkts(if_nummber, size, ptr)

#define GET_ETHER_STATS_OVERSIZE_PKTS(if_nummber, size, ptr)         \
	get_EtherStatsOversizePkts(if_nummber, size, ptr)

#define GET_ETHER_STATS_FRAGMENTS(if_nummber, size, ptr)         \
	get_EtherStatsFragments(if_nummber, size, ptr)

#define GET_ETHER_STATS_JABBERS(if_nummber, size, ptr)         \
	get_EtherStatsJabbers(if_nummber, size, ptr)

#define GET_ETHER_STATS_COLLISIONS(if_nummber, size, ptr)         \
	get_EtherStatsCollisions(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_64_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts64Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_65TO127_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts65To127Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_128TO255_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts128To255Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_256TO511_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts256To511Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_512TO1023_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts512To1023Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_PKTS_1024TO1518_OCTECTS(if_nummber, size, ptr)         \
	get_EtherStatsPkts1024To1518Octets(if_nummber, size, ptr)

#define GET_ETHER_STATS_OWNER(if_nummber, size, ptr)         \
	do{;}while(0)

#define GET_ETHER_STATS_STATUS(if_nummber, size, ptr)         \
	do{;}while(0)

#else /* else of MIB_RMON_ENABLE */
#endif /* end of else of MIB_RMON_ENABLE */

#else  /* else of MIB_ENABLE */
#define MIB_RESET()
#endif /* end of else of MIB_ENABLE */

#endif /* end of __RTK_SAL_MIB_H__ */
