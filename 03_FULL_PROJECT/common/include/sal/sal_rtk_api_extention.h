/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Prototype of additional rtk APIs which are needed by SAL module.
 *
 * Feature : Function prototype definition
 *
 */
 
#ifndef __SAL_RTK_API_EXTENTION_H__
#define	__SAL_RTK_API_EXTENTION_H__

#include <saltype.h>
#include <rtk_api/rtk_api.h>


#if SAL_RTL8316D
typedef enum rtk_qos_schedType_e
{
	SCHE_WFQ = 0,
	SCHE_WRR,
	SCHE_STRICT,
}rtk_qos_schedType_t;
extern rtk_api_ret_t rtk_qos_schedAlogrithm_get(uint8 port, rtk_qos_schedType_t* type);
extern rtk_api_ret_t rtk_qos_schedAlogrithm_set(uint8 port, rtk_qos_schedType_t type);

extern rtk_api_ret_t rtk_rate_igrBandwidthEnable_set(uint32 port,uint32 enable);
extern rtk_api_ret_t rtk_rate_igrBandwidthEnable_get(uint32 port,uint32 *enable);

extern rtk_api_ret_t rtk_rate_egrBandwidthEnable_set(uint32 port,uint32 enable);
extern rtk_api_ret_t rtk_rate_egrBandwidthEnable_get(uint32 port,uint32 *enable);

extern rtk_api_ret_t rtk_iso_mask_get(rtk_portmask_t *mask);
extern rtk_api_ret_t rtk_iso_mask_set(rtk_portmask_t mask);

extern rtk_api_ret_t rtk_vlan_destroy(rtk_vlan_t vid);
#endif

#if 0
typedef enum rtk_cable_channel_e {
    CHANNEL_A,
    CHANNEL_B, 
    CHANNEL_C,
    CHANNEL_D,
}rtk_cable_channel_t;

typedef enum rtk_cable_state_e {
    CABLE_OPEN,
    CABLE_SHORT,  
    CABLE_NORMAL,
}rtk_cable_state_t;

typedef struct rtk_cable_info_s {
    rtk_cable_state_t channel_a_status;
    rtk_cable_state_t channel_b_status;
    rtk_cable_state_t channel_c_status;
    rtk_cable_state_t channel_d_status;
    uint16 channel_a_len;
    uint16 channel_b_len;
    uint16 channel_c_len;
    uint16 channel_d_len;
}rtk_cable_info_t;

// cable test
extern int32 rtk_sal_cableTest_start(void);
extern int32 rtk_sal_cableTest_stop(void);
extern int32 rtk_sal_cableStat_get(rtk_port_t port, rtk_cable_info_t* stat);
#endif
#endif


