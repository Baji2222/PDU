#ifndef __IGMP_CONFIG_H__
#define __IGMP_CONFIG_H__

#include <local/config.h>

#define IGMPDB_CHECK_PERIOD     (4)

#define ROBUSTNESS_VAR          (2)
#define QUERY_INTERVAL          (125)      /* Seconds */
#define RESPONSE_INTERVAL       (10)       /* Seconds */
#define RESPONSE_INTERVAL_VALUE (RESPONSE_INTERVAL*10)   /* 0.1 Seconds */
#define IGMP_LEAVE_TIME         ((ROBUSTNESS_VAR+IGMPDB_CHECK_PERIOD-1)/IGMPDB_CHECK_PERIOD)         /* Seconds */
#define IGMP_GROUP_MEM_INTERVAL ((ROBUSTNESS_VAR*QUERY_INTERVAL+RESPONSE_INTERVAL)/IGMPDB_CHECK_PERIOD)      /* Seconds */
#define V1_ROUTER_PRESENT_TIME  (400/IGMPDB_CHECK_PERIOD)      /* URI, Seconds */




#define MAX_GROUP_SIZE          (128UL)

#define CONFIG_IGMPV3_ENABLE    (0)
#define CONFIG_IGMPV3_MAX_SIP   (2)

#endif /* end of __IGMP_CONFIG_H__ */

