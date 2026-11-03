/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file provides configuration for web
*$Id: web_config.h,v 1.0 2009/07/27 08:03:40 zhenyang_jin Exp $
*/

#ifndef __WEB_CONFIG_H__
#define __WEB_CONFIG_H__

#include <common_types.h>
#include <local/sal_config.h>

extern uint8 maxPortOnboard;
extern uint8 maxTrkPortk;
//extern uint8 rtkTrunkGroupMax;
//extern uint8 portSkipStart;
//extern uint8 portSkipEnd;

//#define RTK_API_LWPS_BASE
#define PORT_PANEL_UPDATE   (1)            /* 1: update port panel every 10 seconds, 0: disable update ability */
#define WEB_AUTHORIZATION_NEEDED    (1)    /* 1: need authorization check */

/* Feature Start */
#define __WEB_SERVICE_VLAN__        SAL_SERVICE_VLAN
#define __WEB_IGMP__                SAL_IGMP
#define __WEB_LLDP_ENABLE__         SAL_LLDP
#define __WEB_STP__                 SAL_RSTP
#define __WEB_EEE_ENABLE__          SAL_EEE
#define __WEB_LOOP__                SAL_LOOP
#define __WEB_SNTP__                SAL_SNTP
#define __WEB_DSCP__                SAL_DSCP_REMARKING
#define __WEB_GREENETH__            SAL_GREENETH
#define __WEB_CABLE_DIAGNOSTIC__        SAL_RTCT
/* this macro should be defined as 1 when http upgrade
   or configuration upload is required */
#define __WEB_UPLOAD__                  (1)
#define __WEB_HTTP_UPGRADE__            (0)
#define __WEB_TFTP_UPGRADE__            (0)
#define __WEB_CFG_BACKUP__              SAL_CFG_BACK
#define __WEB_RMON_COUNTER_QUERY__      (0)
#define __WEB_SNMP__                    SAL_SNMP
#define __WEB_SYSLOG__	                SAL_SYSLOG
/* Feature End */


#define MAX_CPU_PORT        1	/* number of cpu ports */


#define MAX_PORT_ONBOARD	            (maxPortOnboard)
#define MAX_TRK_PORT                    (maxTrkPortk)	/* number of trunk ports */
//#define RTK_TRUNK_GROUP_MAX             (rtkTrunkGroupMax)      // trunk group number
//#define PORT_SKIP_START                 (portSkipStart)
//#define PORT_SKIP_END                   (portSkipEnd)

#define MAX_PORT_MEGA		0
//#define MAX_PORT_GIGA		MAX_PORT_ONBOARD



#define MAX_LOGIC_COMBO_PORT		(11)

// user account
#define FLSH_USR_USERNAME_LEN       20
#define FLSH_USR_PASSWORD_LEN       20

// link aggregation
#define RTK_TRUNK_PORT_NUM_PER_GRP  4      // port number per group

// rate
#define RATE_GRANULARITY_IN_KB  (8UL)

// storm control
#define STRM_CTRL_BLOCK         0               
#define STRM_CTRL_OFF           (0x1FFFF * 8)   

// bandwidth control
#define BWCTRL_DISABLE_THRES    (0x1FFFF * 8)

// forwarding table 
#define MAX_STATIC_MAC          32
#define MAX_MAC_SRAM_ENTRY      (8*1024)
#define MAX_MAC_CAM_ENTRY       64
#define MAX_MAC_HASH_INDEX      4096
#define MAX_MAC_HASH_WAY        4

// VLAN
#define MAX_VLAN_MASK           RTK_MAX_PORT_MASK  

// temp configuration
#define MAX_MIRROR_GROUP		1
#define ETHER_ADDR_LEN          6
#define MAX_QUEUE_NUM           8       // total queue number in switch
#define MAX_LWS_QUEUE_NUM       4       // lws supported queue number
#define MAX_NUM_VLAN        4094
#define MAX_STATIC_VLAN     32
#define MAX_VLAN_NAME_LEN   17
#define MAX_STATIC_SVLAN    8
#define MAX_SVLAN_NAME_LEN   17
#define MAX_LOGIC_ID_LEN    8

#endif

