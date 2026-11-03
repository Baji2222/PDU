/*
 * Copyright Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * Program  : IGMP snooping Database
 * Abstract : 
 * Author   : 
 * $Id: $
 */

#ifndef __IGMP_GROUP_DB_H__
#define __IGMP_GROUP_DB_H__

/*
 * Include Files
 */
#include <local/lwps_config.h>

#if LWPS_IGMP
#include <lwps/lwps.h>
#include <common_types.h>
#include <local/igmp_config.h>

/*
 * Symbol Definition
 */
#define IGMP_GRP_DB_IPV4               (4)
#define IGMP_GRP_DB_IPV6               (6)
#define IGMP_ADDR_PBUF                 (1<<0)
#define IGMP_ADDR_GROUP_ADDR           (1<<1)
#define IGMP_ADDR_GROUP_ADDRV6         (1<<2)

typedef enum igmp_print_type_e{
    IGMP_PRINT_TYPE_ROUTER       = (1<<0),
    IGMP_PRINT_TYPE_GROUP_MEMBER = (1<<1)
}igmp_print_type_t;


typedef struct ipaddr_group_list_s{
    portmask_type fwdPortMask[1];
    uint8         portTimer[COMBO_PORT_NUM];
    ip_addr_t     groupAddr;
    struct ipaddr_group_list_s *next;
    
#if CONFIG_IGMP_MLD_ENABLE
    uint8         ipvn;
#endif /* end of CONFIG_IGMP_MLD_ENABLE */
    uint16        vid;

}ipaddr_group_list_t;

typedef struct ipaddr_group_head_s{
    uint16               listNumber;
    ipaddr_group_list_t *item;
}ipaddr_group_head_t;


typedef struct igmp_snoopingRouters_s{
    portmask_type     router_portMask;
    portmask_type     routerStatic_portMask;
    uint8             portTimer[COMBO_PORT_NUM];
}igmp_snoopingRouters_t;

/*
 * Macro Definition
 */
#define CONFIG_IGMPDB_DEBUG    (0)
#if CONFIG_IGMPDB_DEBUG
  #define DBG_IGMPDB_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else
  #define DBG_IGMPDB_PRINTF(x)
#endif

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
extern int32 igmp_group_db_init(void);
extern int32 igmp_group_add_queryPort(portmask_type portmask);
extern int32 igmp_group_del_queryPort(int32 pn);
extern int32 igmp_group_linkStatus_change(portmask_type curr_pm, portmask_type prev_pm);
extern int32 igmp_group_get_queryPort(portmask_type *portmask);

extern int32 igmp_group_add_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode);
extern int32 igmp_group_del_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode);
extern int32 igmp_group_del_group(ip_addr_t *grupAddr, uint16 vid, ip_addr_t *sip);
extern int32 igmp_group_get_groupMember(ip_addr_t *grupAddr, portmask_type *portmask);
extern void  igmpdb_timer(uint8 pass_seconds);
extern portmask_type check_trunk_portmask(portmask_type portmask);
extern ipaddr_group_head_t* igmp_get_groupList(void);

#if CONFIG_IGMP_MLD_ENABLE
extern int32 mld_group_add_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode);
extern int32 mld_group_del_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode);
extern int32 mld_group_del_group(ip_addr_t *grupAddr, uint16 vid, ip_addr_t *sip);
#endif /* end of CONFIG_IGMP_MLD_ENABLE */

#else  /* else of LWPS_IGMP */

#define igmpdb_timer(x)

#endif /* end of else of LWPS_IGMP */

#endif /* end of __IGMP_GROUP_DB_H__ */

