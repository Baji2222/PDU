
#ifndef __WEB_VLAN_H__
#define __WEB_VLAN_H__

/* Header files */
#include <web/http_types.h>

/* External functions */

extern int32 get_vlan_static(struct http_request_s *pReq, uint16 usVlanId);
extern int32 get_vlan_portbased(struct http_request_s *pReq);
extern int32 get_vlan_ip(struct http_request_s *pReq);
extern int32 post_vlan_static (struct http_request_s *pReq);
extern int32 post_vlanstaticDelEntry (struct http_request_s *pReq);
extern int32 post_vlan_portbased (struct http_request_s *pReq);
extern int32 post_vlan_ip(struct http_request_s *pReq);

extern int add_gateway_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 gateway_ip,uint32 subnetmask) ;
extern int add_ip_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 ip_address,uint32 subnetmask);

#endif  /* __WEB_VLAN_H__ */
