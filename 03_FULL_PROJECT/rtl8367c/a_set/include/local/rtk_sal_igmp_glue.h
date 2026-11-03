#ifndef __RTK_SAL_IGMP_GLUE_H__
#define __RTK_SAL_IGMP_GLUE_H__

#include <local/config.h>

#if LWPS_IGMP

#include <api/rtk_error.h>
#include <common_types.h>
#include <saltype.h>

extern rt_error_code_t rtk_sal_chipdep_igmp_init( void );
extern rt_error_code_t rtk_sal_get_portmask(void *cpu_tag, portmask_type *pm);
extern rt_error_code_t rtk_sal_send_pkt(pktBuf_t *pBuf, portmask_type pm) large;
extern rt_error_code_t rtk_sal_set_ip_multicast(ip_addr_t *dip, ip_addr_t *sip, rtk_fid_t fid, portmask_type pm) large;
extern rt_error_code_t rtk_sal_del_ip_multicast(ip_addr_t *dip, ip_addr_t *sip, rtk_fid_t fid) large;

#define RTK_SAL_SEND_PKT(pkt_s, member) rtk_sal_send_pkt((pkt_s), (member))


#define RTK_SAL_SET_IP_MULTICAST(dip, sip, fid, portmask) \
	do{rtk_sal_set_ip_multicast(dip, sip, fid, portmask);}while(0);

#define RTK_SAL_DEL_IP_MULTICAST(dip, sip, fid, data_struct) \
	do{rtk_sal_del_ip_multicast(dip, sip, fid);}while(0);

/* DIP only for IGMPv1/IGMPv2*/
#define RTK_SAL_SET_DIP_MULTICAST(dip, fid, portmask) \
	do{rtk_sal_set_ip_multicast(dip, NULL, fid, portmask);}while(0);

#define RTK_SAL_DEL_DIP_MULTICAST(dip, fid, data_struct) \
	do{rtk_sal_del_ip_multicast(dip, NULL, fid);}while(0);

#if CONFIG_IGMP_MLD_ENABLE
#define RTK_SAL_SET_IPV6_MULTICAST(dip, sip, fid, portmask) \
	do{rtk_sal_set_ipv6_multicast(dip, sip, fid, portmask);}while(0)

#define RTK_SAL_DEL_IPV6_MULTICAST(dip, sip, fid, data_struct) \
	do{rtk_sal_del_ipv6_multicast(dip, sip, fid);}while(0);

/* DIP only for IGMPv1/IGMPv2*/
#define RTK_SAL_SET_DIPV6_MULTICAST(dip, fid, portmask) \
	do{rtk_sal_set_ipv6_multicast(dip, NULL, fid, portmask);}while(0);

#define RTK_SAL_DEL_DIPV6_MULTICAST(dip, fid, data_struct) \
	do{rtk_sal_del_ipv6_multicast(dip, NULL, fid);}while(0);

extern rt_error_code_t rtk_sal_set_ipv6_multicast(ip_addr_t *dip, ip_addr_t *sip, rtk_fid_t fid, portmask_type pm) large;
extern rt_error_code_t rtk_sal_del_ipv6_multicast(ip_addr_t *dip, ip_addr_t *sip, rtk_fid_t fid) large;
#endif /* end of CONFIG_IGMP_MLD_ENABLE */


#if LWPS_IGMP_TRUNK_SUPPORT
/* FIXME:  include sal in the future */
#include <local/web_config.h>
#define MASK_BIT_LEN			32
//#define PHY_MASK_LEN			(MAX_PHY_N_CPU_PORT / MASK_BIT_LEN + 1)	/* array size of physical ulMask */
//#define LOGIC_MASK_LEN		    (MAX_LOGIC_PORT / MASK_BIT_LEN + 1)	/* array size of logical ulMask */

#define MAC_ADDR_LEN	        6


extern int32 sal_trunk_port_get(uint8 trkId, uint8 *pIs_lacp, sal_pPortmask_t *pPortMask, sal_pPortmask_t *pActMask);
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */


#endif /* end of CONFIG_IGMP */

#endif /* end of __RTK_SAL_CPUTAG_H__ */

