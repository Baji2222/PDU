#ifndef __RRCPV1_H__
#define __RRCPV1_H__

#include <common_types.h>
#include <rtk_proto/rtk_proto.h>
#include <lwps/lwps.h>

#define RRCPV1_PKT_LEN    (60)

typedef enum rrcpv1_opcode_e
{
    RRCPV1_OPCODE_HELLO = 0,
    RRCPV1_OPCODE_GET = 1,
    RRCPV1_OPCODE_SET = 2
} rrcpv1_opcode_t;

typedef enum rrcpv1_ip_addr_mode_e
{
    RRCPV1_IP_ADDR_MODE_STATIC_IP = 0,
    RRCPV1_IP_ADDR_MODE_DHCP        
} rrcpv1_ip_addr_mode_t;

typedef struct rrcp_mac_info_s {
    uint8  mac[MAC_ADDR_LEN];
    uint8  sport;
    uint8  timestamp;
} rrcp_mac_info_t;

typedef struct rrcpv1_payload_ip_s
{
    uint8               ipSet;
    uint8               addrMode;
    uint32              ip;
    uint32              subNetMask;
    uint32              gateway;
} rrcpv1_payload_ip_t;

typedef struct rrcpv1_payload_td_s 
{
    uint8               down_link_port;
    uint8               up_link_port;

    struct eth_addr_s   uplink_mac;
    uint16              chip_id;
    uint32              vender_id;
} rrcpv1_payload_td_t;

typedef union rrcpv1_payload_u {
    rrcpv1_payload_ip_t rrcpv1_ip;
    rrcpv1_payload_td_t rrcpv1_td;	
} rrcpv1_payload_t;

typedef struct rrcpv1_hdr_s {
    uint8               proto_id;	
	uint8               opcode    :6;
	uint8               cpu_flag  :1;
	uint8               reply_flag:1;
    uint16              auth_key;

    rrcpv1_payload_t  rrcpv1_payload;

} rrcpv1_hdr_t;

extern int32 rrcpv1_init(void);
extern int32 rrcpv1_up(pktBuf_t *);

#define CONFIG_RRCPV1_DEBUG    (0)
#if CONFIG_RRCPV1_DEBUG
  #define DBG_RRCPV1_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else
  #define DBG_RRCPV1_PRINTF(x)
#endif

#endif

