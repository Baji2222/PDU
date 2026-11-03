#ifndef __LWPS_CONFIG_H__
#define __LWPS_CONFIG_H__

#include <local/config.h>

#define THIS_MAC                            0x00, 0x23, 0x79, 0x00, 0x23, 0x79
#define THIS_IP                             192, 168, 1, 1
#define DEFAULT_GATEWAY                     192, 168, 1, 254
#define NETMASK                             255, 255, 254, 0
#define SERVER_IP                           192, 168, 1, 2
#define LWPS_ETHARP_ARP_TABLE_SIZE          (4)
#define LWPS_ETHARP_ARP_MAXAGE              (255)
#define LWPS_ETHARP_ARP_MAXPENDING          (128)
#define LWPS_IP_VERSION_SWAP                (1)
#define LWPS_TCP_DATA_OFFSET_SWAP           (1)

#if LWPS_ENABLE
#define LWPS_REALTEK                        (1)
#define LWPS_ETHARP                         (1)
#define LWPS_IP                             (1)
#define LWPS_ICMP                           (1)
#define LWPS_IGMP                           (0)
#define LWPS_TCP                            (1)
#define LWPS_UDP                            (1)
#define LWPS_API                            (1)
#define RTK_PROTO_RRCPV1                    (0)
#define RTK_PROTO_RLPP                      (0)
#define LWPS_RSTP                           (0)
#define LWPS_IPV6                           (0)
#define LWPS_LLDP                           (0)
#else  /* else of LWPS_ENABLE */
#define LWPS_REALTEK                        (0)
#define LWPS_ETHARP                         (0)
#define LWPS_IP                             (0)
#define LWPS_ICMP                           (0)
#define LWPS_IGMP                           (1)
#define LWPS_TCP                            (0)
#define LWPS_UDP                            (0)
#define LWPS_API                            (0)
#define RTK_PROTO_RRCPV1                    (0)
#define LWPS_RSTP                           (0)
#define LWPS_IPV6                           (0)
#endif /* end of else of LWPS_ENABLE */

#ifndef LWIP_UNUSED_ARG
#define LWIP_UNUSED_ARG(x) (void)x
#endif /* LWIP_UNUSED_ARG */


#endif /* end of __LWPS_CONFIG_H__ */
