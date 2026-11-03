#ifndef __MIB_H__
#define __MIB_H__

#include "local/config.h"
#include "common_types.h"

#if LWPS_SNMP

#if LWPS_SNMP_ETHERNET_LIKE
#include "snmp/mib/EthernetLike.h"
#else
#define MIB_TRANSMISSION_ID
#define MIB_TRANSMISSION_HANDLER
#endif /* end of LWPS_SNMP_ETHERNET_LIKE */

#if LWPS_SNMP_RMON
#include "snmp/mib/RMON.h"
#else
#define MIB_RMON_ID
#define MIB_RMON_HANDLER
#endif /* end of LWPS_SNMP_RMON */

#if LWPS_SNMP_INTERFACE_GROUP
#include "snmp/mib/InterfaceGroup.h"
#else
#define MIB_INTERFACE_GROUP_ID
#define MIB_INTERFACE_GROUP_HANDLER
#endif /* end of LWPS_SNMP_RMON */

#if LWPS_SNMP_BRIDGE_MIB
#include "snmp/mib/BridgeMib.h"
#else
#define MIB_BRIDGE_ID
#define MIB_BRIDGE_HANDLER
#endif

#if LWPS_SNMP_OAM_MIB
#include "snmp/mib/OAMMib.h"
#else
#define MIB_OAM_ID
#define MIB_OAM_HANDLER
#endif

#define RMON_HIS_BUCKET_GRANT  2
#define SNMP_ENTERPRISE_ID     63663        /*OPL */
#define SNMP_SYSOBJID_LEN 7
#define SNMP_SYSOBJID {1, 3, 6, 1, 4, 1, SNMP_ENTERPRISE_ID}

extern void objectidncpy(s32_t * dst, s32_t * src, u8_t n);
extern void uncpy(u32_t * dst, u32_t * src, u8_t n);
extern void ocstrncpy(u8_t * dst, u8_t * src, u8_t n);
extern void snmp_iptooid(struct ip_addr_s *ip, s32_t * ident);
extern void snmp_oidtoip(s32_t * ident, struct ip_addr_s *ip);


//extern far const struct mib_array_node internet;
/*mib2.c functions*/
extern void mib_get_object_def(struct mib_node *mn, u8_t ident_len, s32_t * ident, struct obj_def *od);
extern void mib_get_value(struct mib_node *mn, struct obj_def *od, u16_t len, void *value);
extern u8_t mib_set_test(struct mib_node *mn, struct obj_def *od, u16_t len, void *value);
extern void mib_set_value(struct mib_node *mn, struct obj_def *od, u16_t len, void *value);
extern void mib_init();
extern void snmp_inc_system_info( void );

#endif /* end of LWPS_SNMP */

#endif /* end of __MIB_H__ */