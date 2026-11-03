/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 7005 $
 * $Date: 2009-11-13 15:15:59 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : MIB counter
 *
 */
#ifndef __SAL_MIB_H__
#define __SAL_MIB_H__

#include <saltype.h>

/*
 * Symbol Definition
 */

/* global statistic counter index */
typedef enum sal_stat_global_type_e
{
    SAL_DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX = 0,
    SAL_DOT1D_TP_PORT_IN_DISCARDS_INDEX,
    SAL_IF_OUT_UCAST_PKTS_INDEX,
    SAL_IF_OUT_MCAST_PKTS_INDEX,
    SAL_IF_OUT_BCAST_PKTS_INDEX,
    SAL_EGR_LACK_RESOURCE_DROP_INDEX, /* 5 */
    SAL_MIB_GLOBAL_CNTR_END
}sal_stat_global_type_t;

/* port statistic counter index */
typedef enum sal_stat_port_type_e
{
    SAL_IF_OUT_OCTETS_INDEX = 0,
    SAL_IF_IN_OCTETS_INDEX,
    SAL_IF_IN_PKTS_INDEX,
    SAL_DOT3_OUT_PAUSE_FRAMES_INDEX,
    SAL_DOT3_OUT_PAUSE_ON_FRAMES_INDEX,
    SAL_DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX, /* 5 */
    SAL_DOT3_STATS_LATE_COLLISIONS_INDEX,
    SAL_DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX,
    SAL_DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX,
    SAL_DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX,
    SAL_DOT3_CTRL_IN_UNKNOWN_OPCODES_INDEX, /* 10 */
    SAL_DOT3_IN_PAUSE_FRAMES_INDEX,
    SAL_DOT3_STATS_SYMBOL_ERRORS_INDEX,
    SAL_DOT3_STATS_FCS_ERRORS_INDEX,
    SAL_ETHER_STATS_JABBERS_INDEX,
    SAL_ETHER_STATS_COLLISIONS_INDEX, /* 15 */
    SAL_ETHER_STATS_MCAST_PKTS_INDEX,
    SAL_ETHER_STATS_BCAST_PKTS_INDEX,
    SAL_ETHER_STATS_FRAGMENTS_INDEX,
    SAL_ETHER_STATS_PKTS_64OCTETS_INDEX,
    SAL_ETHER_STATS_PKTS_65TO127OCTETS_INDEX, /* 20 */
    SAL_ETHER_STATS_PKTS_128TO255OCTETS_INDEX,
    SAL_ETHER_STATS_PKTS_256TO511OCTETS_INDEX,
    SAL_ETHER_STATS_PKTS_512TO1023OCTETS_INDEX,
    SAL_ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX,
    SAL_ETHER_STATS_OVERSIZE_PKTS_INDEX, /* 25 */
    SAL_ETHER_STATS_OCTETS_INDEX,
    SAL_ETHER_STATS_UNDER_SIZE_PKTS_INDEX,
    SAL_IGR_LACK_PKT_BUF_DROP_INDEX,
    SAL_FLOWCTRL_ON_DROP_PKT_CNT_INDEX,
    SAL_TX_CRC_CHECK_FAIL_CNT_INDEX, /* 30 */
    SAL_SMART_TRIGGER_HIT0_INDEX,
    SAL_SMART_TRIGGER_HIT1_INDEX,
    SAL_OUT_UCAST_PKTS_CNT_INDEX,
    SAL_OUT_MCAST_PKTS_CNT_INDEX,
    SAL_OUT_BCAST_PKTS_CNT_INDEX, /* 35 */
    SAL_IN_UCAST_PKTS_CNT_INDEX,
    SAL_IN_MCAST_PKTS_CNT_INDEX,
    SAL_IN_BCAST_PKTS_CNT_INDEX,
    SAL_DOT3_STATS_DUPLEX_STATUS_INDEX,
    SAL_DOT3_STATS_ALIGNMENT_ERRORS_INDEX, /* 40 */
    SAL_DOT3_STATS_SQETEST_ERRORS_INDEX,
    SAL_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS_INDEX,
    SAL_DOT3_STATS_CARRIER_SENSE_ERRORS_INDEX,
    SAL_DOT3_STATS_FRAME_TOO_LONGS_INDEX,
    SAL_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS_INDEX, /* 45 */
    SAL_DOT3_STATS_ETHER_CHIPSET_INDEX,
    SAL_DOT3_STATS_RATE_CONTROL_ABILITY_INDEX,
    SAL_DOT3_STATS_RATE_CONTROL_STATUS_INDEX,
    SAL_DOT3_COLL_COUNT_INDEX,
    SAL_DOT3_COLL_FREQUENCIES_INDEX, /* 50 */
    SAL_DOT3_OAM_FRAMES_TX_INDEX,
    SAL_DOT3_OAM_FRAMES_RX_INDEX,
    SAL_DOT1D_HC_OUT_FRAMS_INDEX,
    SAL_DOT1D_HC_IN_FRAMS_INDEX,
    SAL_DOT1D_HC_IN_DISCARDS_INDEX, /* 55 */
    SAL_CONTROL_FUNCTIONS_SUPPORTED_INDEX,
    SAL_ETHER_STATS_DROP_EVENTS_INDEX,
    SAL_ETHER_STATS_PKTS_INDEX,
    SAL_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,
    SAL_ETHER_STATS_OWNER_INDEX, /* 60 */
    SAL_ETHER_STATS_STATUS_INDEX,
    SAL_MIB_PORT_CNTR_END
}sal_stat_port_type_t;

/* debug statistic counter index */
typedef enum sal_stat_debug_type_e
{
	SAL_ACL_RATE_LIMIT_DROP_PKTS_CNT_INDEX = 0,
	SAL_ACL_DROP_PKTS_CNT_INDEX,
	SAL_MSTP_DST_DROP_PKTS_CNT_INDEX,
	SAL_MSTP_SRC_DROP_PKTS_CNT_INDEX,
	SAL_VLAN_INGRESS_FILTER_PKTS_CNT_INDEX,
	SAL_VLAN_ACCEPT_TYPE_FILTER_PKTS_CNT_INDEX, /* 5 */
	SAL_VLAN_EGRESS_FILTER_PKTS_CNT_INDEX,
	SAL_VLAN_ERROR_DROP_PKTS_CNT_INDEX,
	SAL_DA_BLOCK_PKTS_CNT_INDEX,
	SAL_SA_BLOCK_PKTS_CNT_INDEX,
	SAL_L2_STORM_FILTER_PKTS_CNT_INDEX, /* 10 */
	SAL_LIMITED_L2_ENTRY_FULL_DROP_PKTS_CNT_INDEX,
	SAL_DOT1X_SPORT_UNAUTH_PKTS_CNT_INDEX,
	SAL_DOT1X_SMAC_UNAUTH_PKTS_CNT_INDEX,
	SAL_DOT1X_DPORT_UNAUTH_PKTS_CNT_INDEX,
	SAL_DOT1X_DMAC_UNAUTH_PKTS_CNT_INDEX, /* 15 */
	SAL_PORT_ISOLATION_DROP_PKTS_CNT_INDEX,
	SAL_RMA_DROP_PKTS_CNT_INDEX,
	SAL_SOFT_CONGEST_DROP_CNT_INDEX,
	SAL_ETHER_STATS_OQ_DROP_INDEX,
	SAL_SVLAN_SEARCH_NO_HIT_INDEX, /* 20 */
	SAL_SVLAN_INGRESS_FILTER_DROP_PKTS_CNT_INDEX,
	SAL_ETHER_STATS_OP_DROP_INDEX,
	SAL_SVLAN_ACCEPT_TYPE_DROP_PKTS_CNT_INDEX,
	SAL_LINK_DOWN_DROP_PKTS_CNT_INDEX,
	SAL_LOCAL_TRAFFIC_DROP_PKTS_CNT_INDEX, /* 25 */
	SAL_CPU_EXCESS_1600B_DROP_PKTS_CNT_INDEX,
	SAL_PACKET_BUF_PAGE_AGING_OUT_CNT_INDEX,
	SAL_MIB_DEBUG_CNTR_END
}sal_stat_debug_type_t;




/* Function Name:
 *      sal_mib_debug_get
 * Description:
 *      Get specified global debug counter.
 * Input:
 *      cntrIdx - specified debug counter index
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_debug_get(sal_stat_debug_type_t cntrIdx, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_debug_reset
 * Description:
 *      Reset the debug counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_mib_debug_reset(void);


/* Function Name:
 *      sal_mib_global_get
 * Description:
 *      Get specified global counter.
 * Input:
 *      cntrIdx - specified global counter index
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_global_get(sal_stat_global_type_t cntrIdx, uint32 *pCntH, uint32 *pCntL);



/* Function Name:
 *      sal_mib_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_mib_global_reset(void);


/* Function Name:
 *      sal_mib_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port        - port id
 *      cntr_idx    - specified port counter index
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_port_get(sal_port_t port, sal_stat_port_type_t cntrIdx, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_port_reset
 * Description:
 *      Reset the specified port counters.
 * Input:
 *      port 		- port id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_mib_port_reset(sal_port_t port);


/* Function Name:
 *      sal_mib_rxGoodPkt_get
 * Description:
 *      Get the number of rx good packets of the specified port.
 * Input:
 *      port 		- port id
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_rxGoodPkt_get(sal_port_t port, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_rxBadPkt_get
 * Description:
 *      Get the number of rx bad packets of the specified port.
 * Input:
 *      port 		- port id
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_rxBadPkt_get(sal_port_t port, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_txGoodPkt_get
 * Description:
 *      Get the number of tx good packets of the specified port.
 * Input:
 *      port 		- port id
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_txGoodPkt_get(sal_port_t port, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_txBadPkt_get
 * Description:
 *      Get the number of tx bad packets of the specified port.
 * Input:
 *      port 		- port id
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_txBadPkt_get(sal_port_t port, uint32 *pCntH, uint32 *pCntL);


/* Function Name:
 *      sal_mib_dropPkt_get
 * Description:
 *      Get the number of drop packets of the specified port.
 * Input:
 *      port 		- port id
 * Output:
 *      pCntH		- MSB of the counter value
 *      pCntL		- LSB of the counter value
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
extern int32 sal_mib_dropPkt_get(sal_port_t port, uint32 *pCntH, uint32 *pCntL);

#endif /* end of __SAL_MIB_H__ */

