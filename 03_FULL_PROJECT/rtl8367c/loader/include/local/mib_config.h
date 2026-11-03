#ifndef __MIB_CONFIG_H__
#define __MIB_CONFIG_H__

/*  Do NOT include this file directly
    Include this file via local/config.h 
 */

#include <local/config.h>

#if LWPS_SNMP

  #undef  MIB_ENABLE
  #define MIB_ENABLE                       (1)
  
  #if LWPS_SNMP_INTERFACE_GROUP
    #define MIB_INTERFACE_GROUP_ENABLE         (1)
  #else /* else of LWPS_SNMP_INTERFACE_GROUP */
    #define MIB_INTERFACE_GROUP_ENABLE         (0)
  #endif /* end of else of LWPS_SNMP_INTERFACE_GROUP */

  #if LWPS_SNMP_ETHERNET_LIKE
    #define MIB_ETHERNET_LIKE_ENABLE           (1)
  #else /* else of LWPS_SNMP_INTERFACE_GROUP */
    #define MIB_ETHERNET_LIKE_ENABLE         (0)
  #endif /* end of LWPS_SNMP_ETHERNET_LIKE */

  #if LWPS_SNMP_RMON
    #define MIB_RMON_ENABLE                    (1)
  #else /* else of LWPS_SNMP_INTERFACE_GROUP */
    #define MIB_RMON_ENABLE         (0)
  #endif /* end of LWPS_SNMP_RMON */

#elif MIB_ENABLE  /* else of LWPS_SNMP */

#define MIB_INTERFACE_GROUP_ENABLE         (1)
#define MIB_ETHERNET_LIKE_ENABLE           (1)
#define MIB_RMON_ENABLE                    (1)

#else /* else of MIB_ENABLE */

#define MIB_INTERFACE_GROUP_ENABLE         (0)
#define MIB_ETHERNET_LIKE_ENABLE           (0)
#define MIB_RMON_ENABLE                    (0)

#endif /* end of else of LWPS_SNMP */

#if MIB_ENABLE
#else  /* else of MIB_ENABLE */
#define mib_init()
#endif /* end of else of MIB_ENABLE */

#endif /* end of __SNMP_CONFIG_H__ */
