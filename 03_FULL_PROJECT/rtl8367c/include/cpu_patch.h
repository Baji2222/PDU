#ifndef __CPU_PATCH_H__
#define __CPU_PATCH_H__

/* ACL */
#define ACL_TEMPLATE_INDEX_DMAC2        0
#define ACL_TEMPLATE_INDEX_DMAC1        0
#define ACL_TEMPLATE_INDEX_DMAC0        0
#define ACL_TEMPLATE_INDEX_ETHERTYPE    0
#define ACL_TEMPLATE_INDEX_L4DPORT      0
#define ACL_TEMPLATE_INDEX_L4SPORT      0
#define ACL_FIELD_INDEX_DMAC2       0
#define ACL_FIELD_INDEX_DMAC1       1
#define ACL_FIELD_INDEX_DMAC0       2
#define ACL_FIELD_INDEX_ETHERTYPE   3
#define ACL_FIELD_INDEX_L4SPORT     4
#define ACL_FIELD_INDEX_L4DPORT     5

#define SYS_ALLOC_FWD_CPU_ACL       (0)
#define SYS_ALLOC_MIRROR_DHCP_ACL   (1)
#define SYS_ALLOC_MIRROR_ARP_ACL    (2)
#define SYS_ALLOC_TRAP_RTK_ACL      (32)

/* Meter */
#define CPU_RATE_CONTORL_METER_INDEX    0

/* VLAN */
#define DEFAULT_VLAN    1

/* QoS */
#define QUEUE_NUMBER    4
#define PRIORITY0_QID   0
#define PRIORITY1_QID   0
#define PRIORITY2_QID   1
#define PRIORITY3_QID   1
#define PRIORITY4_QID   2
#define PRIORITY5_QID   2
#define PRIORITY6_QID   3
#define PRIORITY7_QID   3

extern void project_init();

#endif

