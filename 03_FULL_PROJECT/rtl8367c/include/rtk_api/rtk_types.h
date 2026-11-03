#ifndef _RTL8367C_TYPES_H_
#define _RTL8367C_TYPES_H_

#ifdef EMBEDDED_SUPPORT

#include <common_types.h>

#else

#include <stdio.h>

typedef unsigned long long      uint64;
typedef long long               int64;
typedef unsigned int            uint32;
typedef int                     int32;
typedef unsigned short          uint16;
typedef short                   int16;
typedef unsigned char           uint8;
typedef char                    int8;
#endif

#define CONST_T     const

#ifndef _RTL_TYPES_H


typedef uint32          		ipaddr_t;
typedef uint32					memaddr;

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN		6
#endif

typedef struct ether_addr_s {
	uint8 octet[ETHER_ADDR_LEN];
} ether_addr_t;

#define RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST    1

#define RTK_MAX_NUM_OF_PORT                         8
#define RTK_PORT_ID_MAX                             (RTK_MAX_NUM_OF_PORT-1)
#define RTK_PHY_ID_MAX                              (RTK_MAX_NUM_OF_PORT-4)
#define RTK_MAX_PORT_MASK                           0xFF

#define RTK_WHOLE_SYSTEM                            0xFF

typedef struct rtk_portmask_s
{
    uint32  bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rtk_portmask_t;

typedef enum rtk_enable_e
{
    DISABLED = 0,
    ENABLED,
    RTK_ENABLE_END
} rtk_enable_t;

/* ethernet address type */
typedef struct  rtk_mac_s
{
    uint8 octet[ETHER_ADDR_LEN];
} rtk_mac_t;

typedef uint32  rtk_pri_t;      /* priority vlaue */
typedef uint32  rtk_qid_t;      /* queue id type */
typedef uint32  rtk_data_t;
typedef uint32  rtk_dscp_t;     /* dscp vlaue */
typedef uint32  rtk_fid_t;      /* filter id type */
typedef uint32  rtk_vlan_t;     /* vlan id type */
typedef uint32  rtk_mac_cnt_t;  /* MAC count type  */
typedef uint32  rtk_meter_id_t; /* meter id type  */
typedef uint32  rtk_rate_t;     /* rate type  */

typedef enum rtk_port_e
{
    UTP_PORT0 = 0,
    UTP_PORT1,
    UTP_PORT2,
    UTP_PORT3,
    UTP_PORT4,
    UTP_PORT5,
    UTP_PORT6,
    UTP_PORT7,

    EXT_PORT0 = 16,
    EXT_PORT1,
    EXT_PORT2,

    UNDEFINE_PORT = 30,
    RTK_PORT_MAX = 31
} rtk_port_t;

#ifdef __KERNEL__
#define rtlglue_printf printk
#else
#define rtlglue_printf printf
#endif
#define PRINT           rtlglue_printf
#endif /*_RTL_TYPES_H*/

/* type abstraction */
#ifdef EMBEDDED_SUPPORT

typedef int16                   rtk_api_ret_t;
typedef int16                   ret_t;
typedef uint32                  rtk_u_long_t;

#else

typedef int32                   rtk_api_ret_t;
typedef int32                   ret_t;
typedef uint64                  rtk_u_long_t;

#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define CONST           const
#endif /* _RTL8367C_TYPES_H_ */
