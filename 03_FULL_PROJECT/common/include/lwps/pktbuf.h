/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : ICMP (Internet Control Message Protocol) of Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_PKTBUF_H__
#define	__LWPS_PKTBUF_H__

/*
 * Include Files 
 */

#include <common_types.h>

/*
 * Symbol Definition 
 */
typedef enum layer2_fmt_e {
    L2FMT_NONE,
    L2FMT_ETH,
    L2FMT_ETHARP,
    L2FMT_LEN
} layer2_fmt_t;

typedef enum layer3_fmt_e {
    L3FMT_NONE,
    L3FMT_IP,
    L3FMT_IPV6,
    L3FMT_RSTP  /* LLC+RSTP is in L3 in this system */
} layer3_fmt_t;

typedef enum layer4_fmt_e {
    L4FMT_NONE,
    L4FMT_ICMP,
    L4FMT_IGMP,
    L4FMT_TCP,
    L4FMT_UDP,
    L4FMT_ICMPV6
} layer4_fmt_t;

typedef enum chk_cal_e {
    CAL_L2_CHKSUM,
	CAL_L3_CHKSUM,
	CAL_L4_CHKSUM
} chk_cal_t;

typedef enum tx_pkt_mode_e {
    TX_DPORT_DEFAULT,        /* do not assign tx portmask */
	TX_DPORT_ASSIGN_PMSK     /* follow tx portmask */
} tx_pkt_mode_t;

typedef enum tx_pkt_vlan_tag_e {
	TX_VLAN_FOLLOW_ALE = 0,
	TX_VLAN_FOLLOW_RX,
	TX_VLAN_ASSIGN_VID	
} tx_pkt_vlan_tag_t;

typedef enum tx_pkt_trunk_e
{
    TX_TRUNK_REPRESENT_PORT = 0,
    TX_TRUNK_IGNORE
} tx_pkt_trunk_t;

typedef struct pktBuf_s {
    uint16 size;
    uint8 *frame_info;
    uint8 *pkt_cputag;
	uint8 *pkt_vlantag;
    uint8 *pkt_head;
    uint8 *l2;
    uint8 *l3;
    uint8 *l4;
    uint8 l2fmt;          /* Layer 2 Format */
    uint8 l3fmt;          /* Layer 3 Format */
    uint8 l4fmt;          /* Layer 4 Format */

    uint8  spa;             /* Rx port */

    uint8  tx_chksum_mode;  /* Check sum config for NIC tx function */
    uint8  tx_dport_mode;   /* Tx packet mode. please reference to tx_pkt_mode_t */
    uint32 tx_dst_pmsk;
    uint8  inner_tag_mode;  /* InnerTagMode of TX pkt */
    uint8  outer_tag_mode;  /* InnerTagMode of TX pkt */    
    uint32 inner_tag;       /* Inner tag of rx pkt / Inner tag of tx pkt */
    uint32 outer_tag;       /* Outer tag of rx pkt / Outer tag of tx pkt */    

    uint8  tx_trunk_support; /* Send pacekt to represent port in trunk or all member port */
} pktBuf_t;


#endif  /* __LWPS_PKTBUF_H__ */

