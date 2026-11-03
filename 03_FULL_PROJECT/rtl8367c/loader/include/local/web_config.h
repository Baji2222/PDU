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
#define PORT_PANEL_UPDATE           (1)    // 1: update port panel every 10 seconds, 0: disable update ability
#define WEB_AUTHORIZATION_NEEDED    (0)    /* 1: need authorization check */
#define __HTTP_STP__                (0)
#define __WEB_UPLOAD__            (1)


#ifdef STANDARD_MEM_LIB
  #error "STANDARD_MEM_LIB should not be defined"
#endif

#if defined(STANDARD_MEM_LIB)  
  #define WEB_MEMCPY(dst, src, len) memcpy((dst), (src), (len))  
  #define WEB_MEMCMP(dst, src, len)(dst, src, len) memcmp((dst), (src), (len))
  #define WEB_STRCMP(str1, str2) strcmp(str1, str2)
#else  /* defined(STANDARD_MEM_LIB) */
  extern void* nostarnard_memcpy(void *dest, const void *src, u16_t n);
  extern int   nostarnard_memcmp(const void *s1, const void *s2, u16_t n);
  extern int   nostarnard_strcmp(const char *str1, const char *str2);
  #define WEB_MEMCPY(dst, src, len) nostarnard_memcpy((dst), (src), (len))  
  #define WEB_MEMCMP(dst, src, len) nostarnard_memcmp((dst), (src), (len))
  #define WEB_STRCMP(str1, str2)  nostarnard_strcmp((str1),(str2))
#endif /* else of defined(STANDARD_MEM_LIB) */


#define MAX_PORT_ONBOARD	            (maxPortOnboard)
#define MAX_PORT_COMBO_ONBOARD	        (8)
#define MAX_TRK_PORT                    (maxTrkPortk)	/* number of trunk ports */
//#define RTK_TRUNK_GROUP_MAX             (rtkTrunkGroupMax)      // trunk group number
//#define PORT_SKIP_START                 (portSkipStart)
//#define PORT_SKIP_END                   (portSkipEnd)

#define MAX_CPU_PORT        1	/* number of cpu ports */

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
#define DEFAULT_VLAN            1

// temp configuration
#define MAX_MIRROR_GROUP		1
#define ETHER_ADDR_LEN          6
#define MAX_QUEUE_NUM           8       // total queue number in switch
#define MAX_LWS_QUEUE_NUM       4       // lws supported queue number
#define MAX_NUM_VLAN        4094
#define MAX_STATIC_VLAN     32
#define MAX_VLAN_NAME_LEN   17

#endif

