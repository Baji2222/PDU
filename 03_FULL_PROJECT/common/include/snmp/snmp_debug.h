#ifndef __SNMP_DEBUG_H
#define __SNMP_DEBUG_H

#define CONFIG_SNMP_DEBUG       (0)

#if CONFIG_SNMP_DEBUG
  #define DBG_SNMP_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else
  #define DBG_SNMP_PRINTF(x)
#endif

#define LWIP_ASSERT(x,y) 

#endif /* end of __SNMP_DEBUG_H */