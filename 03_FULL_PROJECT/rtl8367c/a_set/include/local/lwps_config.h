#ifndef __LWPS_CONFIG_H__
#define __LWPS_CONFIG_H__

#include <local/config.h>

#define THIS_MAC                            0x00, 0x23, 0x79, 0x00, 0x23, 0x79
#define THIS_IP                             192, 168, 1, 1
#define DEFAULT_GATEWAY                     192, 168, 1, 254
#define NETMASK                             255, 255, 255, 0
#define SERVER_IP                           192, 168, 1, 20
#define LWPS_ETHARP_ARP_TABLE_SIZE          (8)
#define LWPS_ETHARP_ARP_MAXAGE              (255)
#define LWPS_ETHARP_ARP_MAXPENDING          (128)
#define LWPS_IP_VERSION_SWAP                (1)
#define LWPS_TCP_DATA_OFFSET_SWAP           (1)

#define LWPS_PER_PORT_MAC                   (1)   //total 1+ "# of ports" MACs

/* feature start */
#ifdef LWS_A_SET
#define LWPS_REALTEK                        (1)
#define LWPS_ETHARP                         (1)
#define LWPS_IP                             (1)
#define LWPS_IPV6                           (0)
#define LWPS_ICMP                           (1)
#define LWPS_IGMP                           (1)
#define LWPS_IGMP_TRUNK_SUPPORT             (1)
#define LWPS_IGMP_NO_ROUTER_DROP            (0)
#define LWPS_TCP                            (1)
#define LWPS_UDP                            (1)
#define LWPS_API                            (1)
#define LWPS_RSTP                           (0)
#define LWPS_DHCPC                          (1)
#define LWPS_SNTPC                          (0)
#define RTK_PROTO_RRCPV1                    (1)
#define RTK_PROTO_RLPP                      (1)
#define RTK_PROTO_RRCPV3R                   (0)
#define RTK_PROTO_RRCPV3                    (1)
#define LWPS_LLDP                           (0)
#else
#define LWPS_REALTEK                        (1)
#define LWPS_ETHARP                         (1)
#define LWPS_IP                             (1)
#define LWPS_IPV6                           (1)
#define LWPS_ICMP                           (1)
#define LWPS_IGMP                           (1)
#define LWPS_IGMP_TRUNK_SUPPORT             (1)
#define LWPS_IGMP_NO_ROUTER_DROP            (0)
#define LWPS_TCP                            (1)
#define LWPS_UDP                            (1)
#define LWPS_API                            (1)
#define LWPS_RSTP                           (1)
#define LWPS_DHCPC                          (1)
#define LWPS_SNTPC                          (1)
#define RTK_PROTO_RRCPV1                    (1)
#define RTK_PROTO_RLPP                      (1)
#define RTK_PROTO_RRCPV3R                   (0)
#define RTK_PROTO_RRCPV3                    (1)
#define LWPS_LLDP                           (1)
#endif /* end of else of LWPS_ENABLE */
/* feature end */


#ifndef LWIP_UNUSED_ARG
#define LWIP_UNUSED_ARG(x) (void)x
#endif /* LWIP_UNUSED_ARG */


#endif /* end of __LWPS_CONFIG_H__ */
