/*
 *  DHCP Client for LwPS
 */

#ifndef __LWPS_DHCPC_H__
#define __LWPS_DHCPC_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <saltype.h>

/*
 * Symbol Definition
 */
#define UDP_PORT_DHCP_SERVER    (67)
#define UDP_PORT_DHCP_CLIENT    (68)
#define DHCP_MAGIC_COOKIE_VALUE (0x63825363)
#define DHCPC_TIMEOUT           (5)
#define DHCPC_RETRIES           (5)
#define DHCPC_MAXTIME           (40)

typedef enum dhcp_op_e {
    DHCP_OP_BOOTREQUEST = 1,
    DHCP_OP_BOOTREPLY = 2,
} dhcp_opcode_t;
typedef enum dhcp_htype_e {
    DHCP_HTYPE_ETH = 1,                 /* Ethernet */
} dhcp_htype_t;
typedef enum dhcp_opt_e {               /* RFC 2132 */
    DHCP_OPT_PAD                = 0,    /* Pad */
    DHCP_OPT_SUBNETMASK         = 1,    /* Subnet Mask */
    DHCP_OPT_ROUTER             = 3,    /* Router */
    DHCP_OPT_IPADDRLEASETIME    = 51,   /* IP Address Lease Time */
    DHCP_OPT_MESSAGETYPE        = 53,   /* DHCP Message Type */
    DHCP_OPT_SERVERID           = 54,   /* Server Identifier */
    DHCP_OPT_CLIENTID           = 61,   /* Client Identifier */
    DHCP_OPT_AUTOCONF           = 116,  /* DHCP Auto-Configuration */
    DHCP_OPT_END                = 255   /* End */
} dhcp_opt_t;
typedef enum dhcp_msgtype_e {
    DHCP_MSGTYPE_DISCOVER       = 1,
    DHCP_MSGTYPE_OFFER          = 2,
    DHCP_MSGTYPE_REQUEST        = 3,
    DHCP_MSGTYPE_ACK            = 5,
} dhcp_msgtype_t;

typedef struct dhcp_hdr_s {
    uint8  op;                 /* Packet OP Code / Message Type */
    uint8  htype;              /* Hardware Address Type */
    uint8  hlen;               /* Hardware Address Length */
    uint8  hops;               /* Client sets to zero, optionally used by gateways */
    uint32 xid;                /* Transaction Id */
    uint16 secs;               /* Filled in by client, seconds elapsed since client started trying to boot */
    uint16 flags;              /* Flags */
    uint8  ciaddr[4];          /* Client IP Address, filled in by client inbootrequest if known */
    uint8  yiaddr[4];          /* `Your` (client) IP Address */
    uint8  siaddr[4];          /* Server IP Address, returned in bootreply by server */
    uint8  giaddr[4];          /* Gateway IP Address, used in optional cross-gateway booting */
    uint8  chaddr[16];         /* Client HW Address, filled in by client */
    uint8  sname[64];          /* (BOOTP legacy) */
    uint8  file[128];          /* (BOOTP legacy) */
    uint32 magic_cookie;       /* Magic Cookie (0x63825363) */
    //uint8  option;             /* Option(variable) */
} dhcp_hdr_t;


/*
 * Function Declaration
 */
extern int32 dhcpc_init(void);
extern int32 dhcpc_routine(void);
extern void dhcpc_timer(uint32 milli_second);
extern int32 dhcpc_setEnable(bool bEnable);
extern int32 dhcpc_setTimeout(uint32 timeout, sal_ipaddr_t ip, sal_ipaddr_t netmask, sal_ipaddr_t gateway);

#endif  /* __LWPS_DHCPC_H__ */


