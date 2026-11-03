#ifndef __IGMP_CONFIG_H__
#define __IGMP_CONFIG_H__

#include <local/config.h>
extern uint16 default_vlan_fid;
#define IGMPDB_CHECK_PERIOD     (4)

#define ROBUSTNESS_VAR          (2)
#define QUERY_INTERVAL          (125)      /* Seconds */
#define RESPONSE_INTERVAL       (10)       /* Seconds */
#define RESPONSE_INTERVAL_VALUE (RESPONSE_INTERVAL*10)   /* 0.1 Seconds */
#define IGMP_LEAVE_TIME         ((ROBUSTNESS_VAR+IGMPDB_CHECK_PERIOD-1)/IGMPDB_CHECK_PERIOD)         /* Seconds */
#define IGMP_GROUP_MEM_INTERVAL ((ROBUSTNESS_VAR*QUERY_INTERVAL+RESPONSE_INTERVAL)/IGMPDB_CHECK_PERIOD)      /* Seconds */
#define V1_ROUTER_PRESENT_TIME  (400/IGMPDB_CHECK_PERIOD)      /* URI, Seconds */

#define CONFIG_IGMP_DFT_FID     (default_vlan_fid)
#define CONFIG_IGMP_MLD_ENABLE  (0)

#define MAX_GROUP_SIZE          (128UL)

#define CONFIG_IGMPV3_MAX_SIP   (2)

#if defined(STANDARD_MEM_LIB)
  #define IGMP_MEMCPY(dst, src, len) memcpy((dst), (src), (len))
  #define IGMP_MEMCMP(dst, src, len) memcmp((dst), (src), (len))
#else  /* defined(STANDARD_MEM_LIB) */
#endif /* else of defined(STANDARD_MEM_LIB) */


#endif /* end of __IGMP_CONFIG_H__ */

