#ifndef __SNMP_CONFIG_H__
#define __SNMP_CONFIG_H__

#include <local/lwps_config.h>

/* SNMP Configuration */
#if LWPS_SNMP
#define LWPS_SNMP_IFTABLE			          		(1)
#define LWPS_SNMP_INTERFACE_GROUP           (1)
#define LWPS_SNMP_ETHERNET_LIKE             (1)
#define LWPS_SNMP_RMON                      (1)
#define LWPS_SNMP_BRIDGE_MIB                (1)
#define LWPS_SNMP_OAM_MIB                   (0)
#else  /* else of LWPS_SNMP */
#define LWPS_SNMP_INTERFACE_GROUP           (0)
#define LWPS_SNMP_ETHERNET_LIKE             (0)
#define LWPS_SNMP_RMON                      (0)
#define LWPS_SNMP_BRIDGE_MIB                (0)
#define LWPS_SNMP_OAM_MIB                   (0)
#endif /* end of else of LWPS_SNMP */
#define LWIP_NOASSERT                       (0x1) /* defined or undef */
#define LWPS_SNMP_PRIVATE_MIB               (0)

#define LWPS_SNMP_TRAP_ENABLE               (1)
#define LWPS_SNMP_TRAP_IP                   192, 168, 1, 1
#define LWPS_SNMP_TRAP_TESTTIME             (20)

/* FIXME: all interfaces have the same address length */
#define CONFIG_INTERFACE_ADDR_LEN           (4)
#define CONFIG_SYSTEM_NAME_LEN              (32)
#define CONFIG_SYSTEM_CONTACT_LEN           (32)
#define CONFIG_SYSTEM_LOCATION_LEN          (32)
#define CONFIG_SNMP_IF_ALIAS_LENGTH         (16)



#if LWPS_SNMP
#else
  #define snmp_init()
#endif /* end of LWPS_SNMP==0 */

#endif /* end of __SNMP_CONFIG_H__ */

