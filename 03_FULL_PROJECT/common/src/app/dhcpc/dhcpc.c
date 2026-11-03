#pragma STRING(far)
/*
 *  DHCP Client for LwPS
 */


/*
 * Include Files
 */
#include <common_types.h>
#include <lwps/lwps_api.h>
#include <local/flash_config.h>
#include <salsys.h>
#include <dhcpc/dhcpc.h>

#define DBG_PRINT
#define INIT_IP_ADDR    0

#define IPARR_TO_IPVAR(ip_32t, ip_array)  do { \
    ip_32t = ((uint32)ip_array[0] << 24) | \
             ((uint32)ip_array[1] << 16) | \
             ((uint32)ip_array[2] << 8) | \
             ((uint32)ip_array[3]); \
    } while(0)

/*
 * Symbol Definition
 */
 typedef enum dhcpc_flag_e {
    DHCPC_FLAG_NONE                = 0x00000000,
    DHCPC_FLAG_SERVER_IP_BROADCAST = 0x00000001,
    DHCPC_FLAG_WITH_OPTION54       = 0x00000002,
} dhcpc_flag_t;
typedef enum dhcpc_state_e {
    DHCPC_STATE_NONE = 0,
    DHCPC_STATE_INITIALIZING,
    DHCPC_STATE_SELECTING,
    DHCPC_STATE_REQUESTING,
    DHCPC_STATE_BOUND,
    DHCPC_STATE_RENEWING,
    DHCPC_STATE_REBINDING,
} tftpc_state_t;
typedef enum dhcpc_status_e {
    DHCPC_STATUS_NONE       = 0x00000000,
    DHCPC_STATUS_RUNNING    = 0x00000001,
    DHCPC_STATUS_TIMEOUT    = 0x00000002,   
    DHCPC_STATUS_RETRY      = 0x00000004, /* retry will be dec 1 if timeout reset */
    DHCPC_STATUS_BOUND      = 0x00000010,
    DHCPC_STATUS_RENEW      = 0x00000020,
    DHCPC_STATUS_REBIND     = 0x00000040,
    DHCPC_STATUS_COMPLETED  = 0x00000080, /* mission completed */
} tftpc_status_t;
typedef struct dhcpc_slot_s {
    int32  socket_id;
    int32  state;
    int32  status;
    uint32 transaction_id;

    uint32 sec_uptime;
    uint16 sec_elapsed;
    int8   timeout;
    int8   retries;

    uint32 left_time;
    uint32 max_time;

    /* Bound */
    ip_addr_t server_ip;         /* DHCP deamon IP */
    ip_addr_t serverid;         /* Server Identifier */
    ip_addr_t clientid;         /* Client Identifier */
    ip_addr_t netmask;          /* Subnet Mask */
    ip_addr_t gateway;          /* Router */
    uint32 ipaddr_lease_time;  /* IP Address Lease Time (Unit: Secound) */
    uint32 renewal_time;       /* Renewal(T1) Time (Unit: Secound) */
    uint32 rebinding_time;     /* Rebinding(T2) Time (Unit: Secound) */
    uint32 offer_times;

    /* default IP */
    sal_ipaddr_t def_ipaddr;
    sal_ipaddr_t def_netmask;
    sal_ipaddr_t def_gateway;
} dhcpc_slot_t;

/*
 * Data Declaration
 */
static dhcpc_slot_t dhcpc_slot;


/*
 * Function Declaration
 */

// DHCP Header Dump: Prints formatted DHCP header and options for debugging
// Displays all DHCP fields including op, htype, xid, addresses, and options
static void dhcp_hdr_dump(dhcp_hdr_t *pDhcpHdr)
{
#if 0
    uint8 *pData;
    uint8 type, len;
    int32 i;
#endif
    DBG_PRINT(2, "-- [DHCP HDR dump] ----------------------------\n");
    DBG_PRINT(2, "         OP: 0x%02X (%d)\n", pDhcpHdr->op, pDhcpHdr->op);
    DBG_PRINT(2, "      HTYPE: 0x%02X (%d)\n", pDhcpHdr->htype, pDhcpHdr->htype);
    DBG_PRINT(2, "       HLEN: 0x%02X (%d)\n", pDhcpHdr->hlen, pDhcpHdr->hlen);
    DBG_PRINT(2, "       HOPS: 0x%02X (%d)\n", pDhcpHdr->hops, pDhcpHdr->hops);
    DBG_PRINT(2, "        XID: 0x%08X (%d)\n", pDhcpHdr->xid, pDhcpHdr->xid);
    DBG_PRINT(2, "       SECS: 0x%04X (%d)\n", pDhcpHdr->secs, pDhcpHdr->secs);
    DBG_PRINT(2, "      FLAGS: 0x%04X\n", pDhcpHdr->flags);
    DBG_PRINT(2, "     CIADDR: 0x%08X (%d.%d.%d.%d)\n", *(uint32 *)(pDhcpHdr->ciaddr), \
        pDhcpHdr->ciaddr[0], pDhcpHdr->ciaddr[1], pDhcpHdr->ciaddr[2], pDhcpHdr->ciaddr[3]);
    DBG_PRINT(2, "     YIADDR: 0x%08X (%d.%d.%d.%d)\n", *(uint32 *)(pDhcpHdr->yiaddr), \
        pDhcpHdr->yiaddr[0], pDhcpHdr->yiaddr[1], pDhcpHdr->yiaddr[2], pDhcpHdr->yiaddr[3]);
    DBG_PRINT(2, "     SIADDR: 0x%08X (%d.%d.%d.%d)\n", *(uint32 *)(pDhcpHdr->siaddr), \
        pDhcpHdr->siaddr[0], pDhcpHdr->siaddr[1], pDhcpHdr->siaddr[2], pDhcpHdr->siaddr[3]);
    DBG_PRINT(2, "     GIADDR: 0x%08X (%d.%d.%d.%d)\n", *(uint32 *)(pDhcpHdr->giaddr), \
        pDhcpHdr->giaddr[0], pDhcpHdr->giaddr[1], pDhcpHdr->giaddr[2], pDhcpHdr->giaddr[3]);
    DBG_PRINT(2, "     CHADDR: 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", \
        pDhcpHdr->chaddr[0], pDhcpHdr->chaddr[1], pDhcpHdr->chaddr[2], pDhcpHdr->chaddr[3],
        pDhcpHdr->chaddr[4], pDhcpHdr->chaddr[5], pDhcpHdr->chaddr[6], pDhcpHdr->chaddr[7],
        pDhcpHdr->chaddr[8], pDhcpHdr->chaddr[9], pDhcpHdr->chaddr[10], pDhcpHdr->chaddr[11],
        pDhcpHdr->chaddr[12], pDhcpHdr->chaddr[13], pDhcpHdr->chaddr[14], pDhcpHdr->chaddr[15]);
    DBG_PRINT(2, "  ... (192 octets of 0's. BOOTP legacy) ...\n");
    DBG_PRINT(2, "MagicCookie: 0x%08X\n", pDhcpHdr->magic_cookie);
#if 0
    DBG_PRINT(0, "***********************************************\n");
    pData = (uint8 *)&pDhcpHdr->op + sizeof(dhcp_hdr_t);
    do {
        type = *pData;
        if (type == DHCP_OPT_END) {
            len = 0;
        } else {
            len = *(pData + 1);
        }
        DBG_PRINT(0, "DHCP Option: (t=%03d, l=%03d) \n", type, len);
        for (i=0; i<len; i++) {
            if ((i & 0xF) == 0) DBG_PRINT(0, "[%02X] ");
            DBG_PRINT(0, "%02X ", *(pData + i));
            if ((i & 0xF) == 0xF) DBG_PRINT(0, "\n");
        }
        DBG_PRINT(0, "\n");
        pData += (2 + len);
    } while (type != DHCP_OPT_END);
#endif
    DBG_PRINT(2, "-----------------------------------------------\n");
}

// DHCP Send Discover: Sends DHCP DISCOVER message to find available DHCP servers
// Builds and transmits DHCPDISCOVER packet with client MAC address and options
static int32 dhcpc_send_dhcpdiscover(void)
{
    uint8 buf[384];
    dhcp_hdr_t *pDhcpHdr = (dhcp_hdr_t *)&buf[0];
    ip_addr_t dst_ipaddr;
	sal_mac_t mac;

    /* DHCP Header */
    memset(buf, 0, sizeof(buf));
    pDhcpHdr->op = DHCP_OP_BOOTREQUEST;
    pDhcpHdr->htype = DHCP_HTYPE_ETH;
    pDhcpHdr->hlen = 6; /* length of Ethernet MAC address */
    pDhcpHdr->hops = 0;
    pDhcpHdr->xid = dhcpc_slot.transaction_id;
    pDhcpHdr->secs = dhcpc_slot.sec_elapsed;
    pDhcpHdr->flags = 0x8000;   /* broadcast flag */
    //memset(pDhcpHdr->ciaddr, 0, sizeof(pDhcpHdr->ciaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->yiaddr, 0, sizeof(pDhcpHdr->yiaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->siaddr, 0, sizeof(pDhcpHdr->siaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->giaddr, 0, sizeof(pDhcpHdr->giaddr));  /* 0.0.0.0 */
	sal_sys_mac_get(&mac);
    memcpy(pDhcpHdr->chaddr, mac.octet, ETH_HWADDR_LEN);
    pDhcpHdr->magic_cookie = DHCP_MAGIC_COOKIE_VALUE;

    /* Options */
    /* Option: (t=53,l=1) DHCP Message Type = DHCP Discover */
    memcpy(&buf[0] + sizeof(dhcp_hdr_t), "\x35\x01\x01", 3);
    //*(uint8 *)(&buf[0] + sizeof(dhcp_hdr_t)) = DHCP_OPT_END;  
    /* Options */
    /* Option: (t=55,l=2) DHCP Message Type = DHCP Discover */
    /* End Option */
    memcpy(&buf[3] + sizeof(dhcp_hdr_t), "\x37\x02\x01\x03\xff", 5);
    dhcp_hdr_dump(pDhcpHdr);

    dst_ipaddr.addr[0] = 255;
    dst_ipaddr.addr[1] = 255;
    dst_ipaddr.addr[2] = 255;
    dst_ipaddr.addr[3] = 255;
    if (LWPS_OK != lwps_sendto(dhcpc_slot.socket_id, (uint8 *)pDhcpHdr, sizeof(dhcp_hdr_t) + 8, dst_ipaddr, UDP_PORT_DHCP_SERVER))
    {
        OSAL_PRINTF("%s():%d lwps_sendto failed!\n", __FILE__, (uint16)__LINE__);
        return FAILED;
    }

    return SUCCESS;
}

// DHCP Send Request: Sends DHCP REQUEST message to request offered IP address
// Builds and transmits DHCPREQUEST with server identifier and requested IP options
static int32 dhcpc_send_dhcprequest(dhcpc_flag_t flag)
{
    uint8 buf[384];
    dhcp_hdr_t *pDhcpHdr = (dhcp_hdr_t *)&buf[0];
    ip_addr_t dst_ipaddr;
	sal_mac_t mac;
    uint16 opt_len;     /* Length of Options */

    /* DHCP Header */
    memset(buf, 0, sizeof(buf));
    pDhcpHdr->op = DHCP_OP_BOOTREQUEST;
    pDhcpHdr->htype = DHCP_HTYPE_ETH;
    pDhcpHdr->hlen = 6; /* length of Ethernet MAC address */
    pDhcpHdr->hops = 0;
    pDhcpHdr->xid = dhcpc_slot.transaction_id;
    pDhcpHdr->secs = dhcpc_slot.sec_elapsed;
    pDhcpHdr->flags = 0x8000;   /* broadcast flag */
    //memset(pDhcpHdr->ciaddr, 0, sizeof(pDhcpHdr->ciaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->yiaddr, 0, sizeof(pDhcpHdr->yiaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->siaddr, 0, sizeof(pDhcpHdr->siaddr));  /* 0.0.0.0 */
    //memset(pDhcpHdr->giaddr, 0, sizeof(pDhcpHdr->giaddr));  /* 0.0.0.0 */
	sal_sys_mac_get(&mac);
    memcpy(pDhcpHdr->chaddr, mac.octet, ETH_HWADDR_LEN);
    pDhcpHdr->magic_cookie = DHCP_MAGIC_COOKIE_VALUE;

    /* Options */
    opt_len = 0;

    /* Option: (t=53,l=1) DHCP Message Type = DHCPREQUEST (3) */
    memcpy(&buf[0] + sizeof(dhcp_hdr_t), "\x35\x01\x03", 3);
    opt_len += 3;

    /* Option: (t=54,l=4) Server Identifier = xxx.xxx.xxx.xxx */
    if (flag & DHCPC_FLAG_WITH_OPTION54)
    {
        memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len, "\x36\x04", 2);
        memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len + 2, dhcpc_slot.serverid.addr, IP_ADDR_LEN);
        opt_len += (2 + IP_ADDR_LEN);
    }

    /* Option: (t=50,l=4) Requested IP Address = xxx.xxx.xxx.xxx */
    memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len , "\x32\x04", 2);
    memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len + 2, dhcpc_slot.clientid.addr, IP_ADDR_LEN);
    opt_len += (2 + IP_ADDR_LEN);

    /* Option: (t=55,l=2) Parameter Request List => request option 1 (Subnet Mask) and 3 (Router) */
    memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len, "\x37\x02\x01\x03", 4);
    opt_len += 4;

    /* End Option */
    memcpy(&buf[0] + sizeof(dhcp_hdr_t) + opt_len, "\xff", 1);
    opt_len += 1;

    dhcp_hdr_dump(pDhcpHdr);

    if(flag & DHCPC_FLAG_SERVER_IP_BROADCAST)
    {
    dst_ipaddr.addr[0] = 255;
    dst_ipaddr.addr[1] = 255;
    dst_ipaddr.addr[2] = 255;
    dst_ipaddr.addr[3] = 255;
    }
    else
    {
        dst_ipaddr.addr[0] = dhcpc_slot.server_ip.addr[0];
        dst_ipaddr.addr[1] = dhcpc_slot.server_ip.addr[1];
        dst_ipaddr.addr[2] = dhcpc_slot.server_ip.addr[2];
        dst_ipaddr.addr[3] = dhcpc_slot.server_ip.addr[3];
    }
    if (LWPS_OK != lwps_sendto(dhcpc_slot.socket_id, (uint8 *)pDhcpHdr, sizeof(dhcp_hdr_t) + (opt_len), dst_ipaddr, UDP_PORT_DHCP_SERVER))
    {
        OSAL_PRINTF("%s():%d lwps_sendto failed!\n", __FILE__, (uint16)__LINE__);
        return FAILED;
    }

    return SUCCESS;
}

// DHCP State Update: Manages DHCP state machine transitions and timeouts
// Handles INITIALIZING, SELECTING, REQUESTING, BOUND, RENEWING, REBINDING states
static int32 dhcpc_state_update(void)
{
    if (!(dhcpc_slot.status & DHCPC_STATUS_RUNNING))
    {
        return SUCCESS;
    }

    /* Timeout? */
    if (dhcpc_slot.timeout == 0)
    {
        /* retry? */
        if (dhcpc_slot.retries > 0) {
            dhcpc_slot.timeout = DHCPC_TIMEOUT;
            //dhcpc_slot.retries -= 1;  /* always retry */
            dhcpc_slot.status |= DHCPC_STATUS_RETRY;
        } else {
            /* really timeout */
            dhcpc_slot.status |= DHCPC_STATUS_TIMEOUT;
            //dhcpc_slot.status |= DHCPC_STATUS_FAILED;
            dhcpc_slot.status |= DHCPC_STATUS_COMPLETED;
            dhcpc_slot.status &= ~(DHCPC_STATUS_RUNNING);
            return FAILED;
        }
    }
    if (dhcpc_slot.left_time == 0UL)
    {
        /* really timeout */
        dhcpc_slot.status |= DHCPC_STATUS_TIMEOUT;
        //dhcpc_slot.status |= DHCPC_STATUS_FAILED;
        dhcpc_slot.status |= DHCPC_STATUS_COMPLETED;
        dhcpc_slot.status &= ~(DHCPC_STATUS_RUNNING);
        return FAILED;
    }

    switch (dhcpc_slot.state)
    {
    case DHCPC_STATE_INITIALIZING:
        {
            sal_sys_ip_set(INIT_IP_ADDR, INIT_IP_ADDR);
            sal_sys_gateway_set(INIT_IP_ADDR);

            dhcpc_slot.retries = DHCPC_RETRIES; /* always retry */
            dhcpc_slot.transaction_id += 1UL;
            dhcpc_slot.sec_uptime = 0UL;
            dhcpc_slot.sec_elapsed = 0;
            dhcpc_slot.offer_times = 0UL;

            if (SUCCESS == dhcpc_send_dhcpdiscover())
            {
                dhcpc_slot.timeout = DHCPC_TIMEOUT;
                dhcpc_slot.state = DHCPC_STATE_SELECTING;
            }
        }
        break;
    
    case DHCPC_STATE_SELECTING:
        {
            if (dhcpc_slot.status & DHCPC_STATUS_RETRY)
            {
                dhcpc_slot.state = DHCPC_STATE_INITIALIZING;
            }

            if (dhcpc_slot.offer_times > 0UL)
            {
                if (SUCCESS == dhcpc_send_dhcprequest(DHCPC_FLAG_SERVER_IP_BROADCAST | DHCPC_FLAG_WITH_OPTION54))
                {
                    dhcpc_slot.state = DHCPC_STATE_REQUESTING;
                    dhcpc_slot.timeout = DHCPC_TIMEOUT;
                }
            }
        }
        break;
    
    case DHCPC_STATE_REQUESTING:
        {
            if (dhcpc_slot.status & DHCPC_STATUS_RETRY)
            {
                dhcpc_slot.state = DHCPC_STATE_SELECTING;
            }
        }
        break;

    case DHCPC_STATE_BOUND:
        {
            /* disable timeout to keep-alive */
            dhcpc_slot.timeout = -1;

            /* Lease time 50% expired */
            if (dhcpc_slot.sec_uptime >= (dhcpc_slot.ipaddr_lease_time >> 1))
            {
                dhcpc_slot.transaction_id += 1UL;
                dhcpc_slot.sec_elapsed = 0;

                if (SUCCESS == dhcpc_send_dhcprequest(DHCPC_FLAG_NONE))
                {
                    dhcpc_slot.state = DHCPC_STATE_RENEWING;
                }
            }

            /* Update IP address */
            if (dhcpc_slot.status & (DHCPC_STATUS_BOUND | DHCPC_STATUS_RENEW | DHCPC_STATUS_REBIND))
            {
                sal_ipaddr_t ip_addr;
                sal_ipaddr_t ip_netmask;
                sal_ipaddr_t ip_gateway;                
                
                IPARR_TO_IPVAR(ip_addr, dhcpc_slot.clientid.addr);
                IPARR_TO_IPVAR(ip_netmask, dhcpc_slot.netmask.addr);                
                sal_sys_ip_set(ip_addr, ip_netmask);
                IPARR_TO_IPVAR(ip_gateway, dhcpc_slot.gateway.addr);
                sal_sys_gateway_set(ip_gateway);

                dhcpc_slot.status &= ~(DHCPC_STATUS_BOUND | DHCPC_STATUS_RENEW | DHCPC_STATUS_REBIND);
            }
        }
        break;

    case DHCPC_STATE_RENEWING:
        {
            /* Lease time 87.5% expired */
            if (dhcpc_slot.sec_uptime >= ((dhcpc_slot.ipaddr_lease_time * 7) >> 3))
            {
                dhcpc_slot.transaction_id += 1UL;
                dhcpc_slot.sec_elapsed = 0;

                if (SUCCESS == dhcpc_send_dhcprequest(DHCPC_FLAG_SERVER_IP_BROADCAST))
                {
                    dhcpc_slot.state = DHCPC_STATE_REBINDING;
                }
            }
        }
        break;

    case DHCPC_STATE_REBINDING:
        {
            /* Lease time expired */
            if (dhcpc_slot.sec_uptime >= dhcpc_slot.ipaddr_lease_time)
            {
                dhcpc_slot.state = DHCPC_STATE_INITIALIZING;
                dhcpc_slot.left_time = dhcpc_slot.max_time;
            }
        }
        break;

    default:
        break;
    }

    /* update status */
    dhcpc_slot.status &= ~(DHCPC_STATUS_RETRY);

    return SUCCESS;
}

// LWPS DHCP Receive From: Processes incoming DHCP responses from server
// Handles DHCPOFFER, DHCPACK messages and updates client state accordingly
void lwps_dhcpc_recvfrom(int32 socketId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant
{
    //tftpc_session_t *pSlot = (tftpc_session_t *)lwps_socket_getCookie(socketId);
    dhcp_hdr_t *pDhcpHdr = (dhcp_hdr_t *)pData;
    uint8 *pBuf;
    uint8 type, len;
    //int32 i;
    uint8 msgtype = 0;

    DBG_PRINT(2, "%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId);
    DBG_PRINT(2, "%s():%d remove_host = %d.%d.%d.%d:%d\n", __FILE__, (uint16)__LINE__, 
        ip.addr[0], ip.addr[1], ip.addr[2], ip.addr[3], port);

    memcpy(dhcpc_slot.server_ip.addr, &ip, IP_ADDR_LEN);

//    DBG_PRINT(2, "%s():%d lwps_socket_getCookie(socketId = %d) = pSlot = %p\n", __FILE__, (uint16)__LINE__, socketId, pSlot);

    DBG_PRINT(2, "%s():%d DHCP Rx dump\n", __FILE__, (uint16)__LINE__);
    dhcp_hdr_dump(pDhcpHdr);

    /* Parse the Header */
    if ((pDhcpHdr->op != DHCP_OP_BOOTREPLY) ||
        (pDhcpHdr->xid != dhcpc_slot.transaction_id))
    {
        OSAL_PRINTF("%s():%d DHCP OP != DHCP_OP_BOOTREPLY\n", __FILE__, (uint16)__LINE__);
        return;
    }
    memcpy(dhcpc_slot.clientid.addr, pDhcpHdr->yiaddr, IP_ADDR_LEN);

    /* Parse the Options */
    pBuf = (uint8 *)&pDhcpHdr->op + sizeof(dhcp_hdr_t);
    do {
        type = *pBuf;
        if (type == DHCP_OPT_END) {
            len = 0;
        } else {
            len = *(pBuf + 1);
        }
        DBG_PRINT(2, "DHCP Option: (t=%03d, l=%03d) \n", type, len);
        switch (type)
        {
        case DHCP_OPT_SUBNETMASK:
            if ((len % IP_ADDR_LEN) == 0)
            {
                memcpy(dhcpc_slot.netmask.addr, (pBuf + 2), IP_ADDR_LEN);
            }
            break;

        case DHCP_OPT_ROUTER:
            if ((len % IP_ADDR_LEN) == 0)
            {
                memcpy(dhcpc_slot.gateway.addr, (pBuf + 2), IP_ADDR_LEN);
            }
            break;

        case DHCP_OPT_IPADDRLEASETIME:
            if (len == sizeof(uint32))
            {
                memcpy(&dhcpc_slot.ipaddr_lease_time, (pBuf + 2), sizeof(uint32));
            }
            break;

        case DHCP_OPT_MESSAGETYPE:
            if (len == sizeof(uint8))
            {
                memcpy(&msgtype, (pBuf + 2), sizeof(uint8));
            }
            break;

        case DHCP_OPT_SERVERID:
            if ((len % IP_ADDR_LEN) == 0)
            {
                memcpy(dhcpc_slot.serverid.addr, (pBuf + 2), IP_ADDR_LEN);
            }
            break;

        default:
            break;
        }

        pBuf += (2 + len);
    } while ((type != DHCP_OPT_END) && (pBuf <= (pData + length)));

    /* Process */
    if (dhcpc_slot.status & DHCPC_STATUS_RUNNING)
    {
        switch (dhcpc_slot.state)
        {
        case DHCPC_STATE_SELECTING:
            if (msgtype == DHCP_MSGTYPE_OFFER)
            {
                dhcpc_slot.offer_times += 1UL;
            }
            break;

        case DHCPC_STATE_REQUESTING:
            if (msgtype == DHCP_MSGTYPE_ACK)
            {
                OSAL_PRINTF("DHCPC is updated. (lease time = %lu sec)\n", dhcpc_slot.ipaddr_lease_time);
                dhcpc_slot.status |= DHCPC_STATUS_BOUND;
                dhcpc_slot.sec_uptime = 0UL;
                dhcpc_slot.sec_elapsed = 0;
                dhcpc_slot.state = DHCPC_STATE_BOUND;
            }
            break;

        case DHCPC_STATE_RENEWING:
            if (msgtype == DHCP_MSGTYPE_ACK)
            {
                OSAL_PRINTF("DHCPC is renewed.\n");
                dhcpc_slot.status |= DHCPC_STATUS_RENEW;
                dhcpc_slot.sec_uptime = 0UL;
                dhcpc_slot.sec_elapsed = 0;
                dhcpc_slot.state = DHCPC_STATE_BOUND;
            }
            break;

        case DHCPC_STATE_REBINDING:
            if (msgtype == DHCP_MSGTYPE_ACK)
            {
                OSAL_PRINTF("DHCPC is rebinded.\n");
                dhcpc_slot.status |= DHCPC_STATUS_REBIND;
                dhcpc_slot.sec_uptime = 0UL;
                dhcpc_slot.sec_elapsed = 0;
                dhcpc_slot.state = DHCPC_STATE_BOUND;
            }
            break;

        default:
            break;
        }
    }

}

// DHCP Client Init: Initializes DHCP client data structures
// Sets default values for transaction ID, timeout, and default IP configuration
int32 dhcpc_init(void)
{
    DBG_PRINT(1, "%s():%d\n", __FILE__, (uint16)__LINE__);

    dhcpc_slot.socket_id = -1;
    dhcpc_slot.state = DHCPC_STATE_NONE;
    dhcpc_slot.transaction_id = 0x0c265401;
    dhcpc_slot.max_time = DHCPC_MAXTIME;
    dhcpc_slot.def_ipaddr = DFLT_SYS_IP_IP;
    dhcpc_slot.def_netmask = DFLT_SYS_IP_NETMASK;
    dhcpc_slot.def_gateway = DFLT_SYS_IP_GATEWAY;

    return SUCCESS;
}

// DHCP Client Routine: Main processing loop for DHCP client
// Creates socket, binds to DHCP client port, and executes state machine
int32 dhcpc_routine(void)
{
    lwps_handler_t dhcpcHandler;
    ip_addr_t empty_ip;

    DBG_PRINT(3, "%s():%d\n", __FILE__, (uint16)__LINE__);

    if (dhcpc_slot.status & DHCPC_STATUS_RUNNING)
    {
        if (dhcpc_slot.socket_id < 0)
        {
            /* Alloc a socket for DHCP client */
            dhcpcHandler.event_connect    = NULL;
            dhcpcHandler.event_disconnect = NULL;
            dhcpcHandler.event_recvfrom   = lwps_dhcpc_recvfrom;

            dhcpc_slot.socket_id = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, dhcpcHandler);
            if (dhcpc_slot.socket_id < 0)
            {
                OSAL_PRINTF("DHCPC: Error - alloc socket failed!\n");
                return FAILED;
            }

            empty_ip.addr[0] = 0;
            empty_ip.addr[1] = 0;
            empty_ip.addr[2] = 0;
            empty_ip.addr[3] = 0;
            if (LWPS_OK != lwps_bind(dhcpc_slot.socket_id, empty_ip, UDP_PORT_DHCP_CLIENT))
            {
                OSAL_PRINTF("DHCPC: Error - bind socket failed!\n");
                return FAILED;
            }

        }
    }
    else
    {
        /* release socket */
        if (dhcpc_slot.socket_id >= 0)
        {
            lwps_close(dhcpc_slot.socket_id);
            dhcpc_slot.socket_id = -1;
        }
    }

    /* failed handling */
    if (SUCCESS != dhcpc_state_update())
    {
        DBG_PRINT(0, "%s():%d using the default IP address\n", __FILE__, (uint16)__LINE__);
        sal_sys_ip_set(dhcpc_slot.def_ipaddr, dhcpc_slot.def_netmask);
        sal_sys_gateway_set(dhcpc_slot.def_gateway);
     }

    return SUCCESS;
}

// DHCP Client Timer: Periodic timer for DHCP client operations
// Updates elapsed time and manages timeout counters for DHCP state machine
void dhcpc_timer(uint32 milli_second)
{
    static uint32 timer_cnt_1sec = 0;  /* unit: millisecond */

    DBG_PRINT(3, "%s():%d milli_second = %d\n", __FILE__, (uint16)__LINE__, milli_second);

    /* update counter */
    timer_cnt_1sec += milli_second;

    /* one second */
    if (timer_cnt_1sec >= 1000UL) {
        timer_cnt_1sec -= 1000UL;

        if (dhcpc_slot.status & DHCPC_STATUS_RUNNING)
        {
            dhcpc_slot.sec_uptime += 1UL;
            dhcpc_slot.sec_elapsed += 1;
            DBG_PRINT(3, "dhcpc_slot.ipaddr_lease_time = %d, dhcpc_slot.sec_elapsed = %lu\n", \
                dhcpc_slot.ipaddr_lease_time, dhcpc_slot.sec_elapsed);

            if (dhcpc_slot.timeout > 0)
            {
                dhcpc_slot.timeout -= 1;
                dhcpc_slot.left_time -= 1UL;
                DBG_PRINT(2, "dhcpc[0] timeout = %d\n", dhcpc_slot.timeout);
            }
        }
    }
}

// DHCP Client Enable: Enables DHCP client functionality
// Starts DHCP process by setting initial state and running flag
static int32 dhcpc_enable(void)
{
    dhcpc_slot.timeout = DHCPC_TIMEOUT;
    dhcpc_slot.retries = DHCPC_RETRIES;
    dhcpc_slot.state = DHCPC_STATE_INITIALIZING;
    dhcpc_slot.status |= DHCPC_STATUS_RUNNING;
    dhcpc_slot.left_time = dhcpc_slot.max_time;

    return SUCCESS;
}

// DHCP Client Disable: Disables DHCP client functionality
// Stops DHCP process by clearing running flag
static int32 dhcpc_disable(void)
{
    dhcpc_slot.status &= ~(DHCPC_STATUS_RUNNING);

    return SUCCESS;
}

// DHCP Client Set Enable: Enables or disables DHCP client
// Public API to start or stop DHCP client operation
int32 dhcpc_setEnable(bool bEnable)
{
    if (ENABLE == bEnable)
    {
        OSAL_PRINTF("DHCPC is enabled.\n");
        dhcpc_enable();
    }
    else
    {
        OSAL_PRINTF("DHCPC is disabled.\n");
        dhcpc_disable();
    }

    return SUCCESS;
}
#if 0//NOT used
// DHCP Client Set Timeout: Sets timeout and default IP configuration
// Configures DHCP client timeout and fallback IP settings
int32 dhcpc_setTimeout(uint32 timeout, sal_ipaddr_t ipaddr, sal_ipaddr_t netmask, sal_ipaddr_t gateway)
{
    dhcpc_slot.max_time = timeout;
    dhcpc_slot.def_ipaddr = ipaddr;
    dhcpc_slot.def_netmask = netmask;
    dhcpc_slot.def_gateway = gateway;
    
    return SUCCESS;
}
#endif