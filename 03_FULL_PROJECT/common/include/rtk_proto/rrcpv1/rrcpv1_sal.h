#ifndef __RRCPV1_SAL_H__
#define __RRCPV1_SAL_H__

#include <common_types.h>
#include <rtk_proto/rtk_proto.h>
#include <rtk_proto/rtk_sal.h>

typedef enum rrcpv1_ip_addr_mode_e
{
    RRCPV1_IP_ADDR_MODE_STATIC_IP = 0,
    RRCPV1_IP_ADDR_MODE_DHCP		
} rrcpv1_ip_addr_mode_t;

extern int32 SalRrcpGetAdminPortMask(uint32 *pmsk);
extern int32 SalRrcpGetChipId(uint16 *chipId);
extern int32 SalRrcpGetVersionId(uint32 *version_id);
extern int32 SalRrcpGetGateway(uint32 *ipv4dfgw);
extern int32 SalRrcpSetGateway(uint32 ipv4dfgw);
extern int32 SalRrcpGetIp(uint32 *ipv4addr, uint32 *ipv4mask);
extern int32 SalRrcpSetIp(uint32 ipv4addr, uint32 ipv4mask);
extern int32 SalRrcpSetDhcp(uint8 dhcp);
extern int32 SalRrcpGetDhcp(uint8 *dhcp);
#endif

