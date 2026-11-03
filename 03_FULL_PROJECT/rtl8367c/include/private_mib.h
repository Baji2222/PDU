#ifndef __PRIVATE_MIB_H__
#define __PRIVATE_MIB_H__
#include <local/config.h>
#include <snmp/opt.h>

#if SNMP_PRIVATE_MIB



#define DBG_PRIVATE_MIB_ENABLE   (0)
#if DBG_PRIVATE_MIB_ENABLE
  #define DBG_PRIVATE_MIB_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_PRIVATE_MIB_ENABLE */
  #define DBG_PRIVATE_MIB_PRINTF(x)
#endif/* end of else of DBG_PRIVATE_MIB_ENABLE */

extern struct mib_array_node private_mib;


#else  /* else of SNMP_PRIVATE_MIB */
#endif /* end of else of SNMP_PRIVATE_MIB */


#endif /* end of __PRIVATE_MIB_H__ */

