#ifndef _RTL8367C_ASICDRV_IGMP_H_
#define _RTL8367C_ASICDRV_IGMP_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <rtl8367c_asicdrv.h>

#define RTL8367C_MAX_LEAVE_TIMER        (7)
#define RTL8367C_MAX_QUERY_INT          (0xFFFF)
#define RTL8367C_MAX_ROB_VAR            (7)

#define RTL8367C_IGMP_GOUP_NO           (256)
#define RTL8367C_IGMP_MAX_GOUP          (0xFF)
#define RTL8367C_IGMP_GRP_BLEN          (3)
#define RTL8367C_ROUTER_PORT_INVALID    (0xF)

enum RTL8367C_IGMPTABLE_FULL_OP
{
    TABLE_FULL_FORWARD = 0,
    TABLE_FULL_DROP,
    TABLE_FULL_TRAP,
    TABLE_FULL_OP_END
};

enum RTL8367C_CRC_ERR_OP
{
    CRC_ERR_DROP = 0,
    CRC_ERR_TRAP,
    CRC_ERR_FORWARD,
    CRC_ERR_OP_END
};

enum RTL8367C_IGMP_MLD_PROTOCOL_OP
{
    PROTOCOL_OP_ASIC = 0,
    PROTOCOL_OP_FLOOD,
    PROTOCOL_OP_TRAP,
    PROTOCOL_OP_DROP,
    PROTOCOL_OP_END
};

enum RTL8367C_IGMP_MLD_BYPASS_GROUP
{
    BYPASS_224_0_0_X = 0,
    BYPASS_224_0_1_X,
    BYPASS_239_255_255_X,
    BYPASS_IPV6_00XX,
    BYPASS_GROUP_END
};
#if 1
typedef struct
{
    uint32 p0_timer;
    uint32 p1_timer;
    uint32 p2_timer;
    uint32 p3_timer;
    uint32 p4_timer;
    uint32 p5_timer;
    uint32 p6_timer;
    uint32 p7_timer;
    uint32 p8_timer;
    uint32 p9_timer;
    uint32 p10_timer;
    uint32 report_supp_flag;

}rtl8367c_igmpgroup;
#endif
/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 76306 $
 * $Date: 2017-03-08 15:13:58 +0800 (?Ÿæ?ä¸? 08 ä¸‰æ? 2017) $
 *
 * Purpose : RTL8367C switch high-level API for RTL8367C
 * Feature : IGMP related functions
 *
 */
#include <rtl8367c_asicdrv_igmp.h>

ret_t rtl8367c_setAsicIgmp(uint32 enabled);
ret_t rtl8367c_getAsicIgmp(uint32 *pEnabled);
ret_t rtl8367c_setAsicIpMulticastVlanLeaky(uint32 port, uint32 enabled );
ret_t rtl8367c_getAsicIpMulticastVlanLeaky(uint32 port, uint32 *pEnabled );
ret_t rtl8367c_setAsicIGMPTableFullOP(uint32 operation);
ret_t rtl8367c_getAsicIGMPTableFullOP(uint32 *pOperation);
ret_t rtl8367c_setAsicIGMPCRCErrOP(uint32 operation);
ret_t rtl8367c_getAsicIGMPCRCErrOP(uint32 *pOperation);
ret_t rtl8367c_setAsicIGMPFastLeaveEn(uint32 enabled);
ret_t rtl8367c_getAsicIGMPFastLeaveEn(uint32 *pEnabled);
ret_t rtl8367c_setAsicIGMPLeaveTimer(uint32 leave_timer);
ret_t rtl8367c_getAsicIGMPLeaveTimer(uint32 *pLeave_timer);
ret_t rtl8367c_setAsicIGMPQueryInterval(uint32 interval);
ret_t rtl8367c_getAsicIGMPQueryInterval(uint32 *pInterval);
ret_t rtl8367c_setAsicIGMPRobVar(uint32 rob_var);
ret_t rtl8367c_getAsicIGMPRobVar(uint32 *pRob_var);
ret_t rtl8367c_setAsicIGMPStaticRouterPort(uint32 pmsk);
ret_t rtl8367c_getAsicIGMPStaticRouterPort(uint32 *pMsk);
ret_t rtl8367c_setAsicIGMPAllowDynamicRouterPort(uint32 pmsk);
ret_t rtl8367c_getAsicIGMPAllowDynamicRouterPort(uint32 *pPmsk);
ret_t rtl8367c_getAsicIGMPdynamicRouterPort1(uint32 *pPort, uint32 *pTimer);
ret_t rtl8367c_getAsicIGMPdynamicRouterPort2(uint32 *pPort, uint32 *pTimer);
ret_t rtl8367c_setAsicIGMPSuppression(uint32 report_supp_enabled, uint32 leave_supp_enabled);
ret_t rtl8367c_getAsicIGMPSuppression(uint32 *pReport_supp_enabled, uint32 *pLeave_supp_enabled);
ret_t rtl8367c_setAsicIGMPQueryRX(uint32 port, uint32 allow_query);
ret_t rtl8367c_getAsicIGMPQueryRX(uint32 port, uint32 *pAllow_query);
ret_t rtl8367c_setAsicIGMPReportRX(uint32 port, uint32 allow_report);
ret_t rtl8367c_getAsicIGMPReportRX(uint32 port, uint32 *pAllow_report);
ret_t rtl8367c_setAsicIGMPLeaveRX(uint32 port, uint32 allow_leave);
ret_t rtl8367c_getAsicIGMPLeaveRX(uint32 port, uint32 *pAllow_leave);
ret_t rtl8367c_setAsicIGMPMRPRX(uint32 port, uint32 allow_mrp);
ret_t rtl8367c_getAsicIGMPMRPRX(uint32 port, uint32 *pAllow_mrp);
ret_t rtl8367c_setAsicIGMPMcDataRX(uint32 port, uint32 allow_mcdata);
ret_t rtl8367c_getAsicIGMPMcDataRX(uint32 port, uint32 *pAllow_mcdata);
ret_t rtl8367c_setAsicIGMPv1Opeartion(uint32 port, uint32 igmpv1_op);
ret_t rtl8367c_getAsicIGMPv1Opeartion(uint32 port, uint32 *pIgmpv1_op);
ret_t rtl8367c_setAsicIGMPv2Opeartion(uint32 port, uint32 igmpv2_op);
ret_t rtl8367c_getAsicIGMPv2Opeartion(uint32 port, uint32 *pIgmpv2_op);
ret_t rtl8367c_setAsicIGMPv3Opeartion(uint32 port, uint32 igmpv3_op);
ret_t rtl8367c_getAsicIGMPv3Opeartion(uint32 port, uint32 *pIgmpv3_op);
ret_t rtl8367c_setAsicMLDv1Opeartion(uint32 port, uint32 mldv1_op);
ret_t rtl8367c_getAsicMLDv1Opeartion(uint32 port, uint32 *pMldv1_op);
ret_t rtl8367c_setAsicMLDv2Opeartion(uint32 port, uint32 mldv2_op);
ret_t rtl8367c_getAsicMLDv2Opeartion(uint32 port, uint32 *pMldv2_op);
ret_t rtl8367c_setAsicIGMPPortMAXGroup(uint32 port, uint32 max_group);
ret_t rtl8367c_getAsicIGMPPortMAXGroup(uint32 port, uint32 *pMax_group);
ret_t rtl8367c_getAsicIGMPPortCurrentGroup(uint32 port, uint32 *pCurrent_group);
ret_t rtl8367c_getAsicIGMPGroup(uint32 idx, uint32 *pValid, rtl8367c_igmpgroup *pGrp);
ret_t rtl8367c_setAsicIpMulticastPortIsoLeaky(uint32 port, uint32 enabled);
ret_t rtl8367c_getAsicIpMulticastPortIsoLeaky(uint32 port, uint32 *pEnabled);
ret_t rtl8367c_setAsicIGMPReportLeaveFlood(uint32 flood);
ret_t rtl8367c_getAsicIGMPReportLeaveFlood(uint32 *pFlood);
ret_t rtl8367c_setAsicIGMPDropLeaveZero(uint32 drop);
ret_t rtl8367c_getAsicIGMPDropLeaveZero(uint32 *pDrop);
ret_t rtl8367c_setAsicIGMPBypassStormCTRL(uint32 bypass);
ret_t rtl8367c_getAsicIGMPBypassStormCTRL(uint32 *pBypass);
ret_t rtl8367c_setAsicIGMPIsoLeaky(uint32 leaky);
ret_t rtl8367c_getAsicIGMPIsoLeaky(uint32 *pLeaky);
ret_t rtl8367c_setAsicIGMPVLANLeaky(uint32 leaky);
ret_t rtl8367c_getAsicIGMPVLANLeaky(uint32 *pLeaky);
ret_t rtl8367c_setAsicIGMPBypassGroup(uint32 bypassType, uint32 enabled);
ret_t rtl8367c_getAsicIGMPBypassGroup(uint32 bypassType, uint32 *pEnabled);

#endif /*#ifndef _RTL8367C_ASICDRV_IGMP_H_*/

