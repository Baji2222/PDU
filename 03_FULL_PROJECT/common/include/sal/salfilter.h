/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 7005 $
 * $Date: 2009-11-13 15:15:59 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : ACL, Flow Table
 *
 */
#ifndef __SAL_FILTER_H__
#define __SAL_FILTER_H__

#include <saltype.h>


enum FLOWTBLE_DFT_RULE
{

    FLOWTBL_DFT_RULE_IDX_MAX
};

enum ACL_DFT_RULE
{
	ACL_DFT_RULE_IDX_ARP_CPY_TO_CPU,
#ifdef __RT_PROTOCOL__
 #ifdef __RRCP_HW_FORAWRD__
	ACL_DFT_RULE_IDX_RRCP_BROADCAST,
	ACL_DFT_RULE_IDX_RRCP_MYMAC,
	ACL_DFT_RULE_IDX_RRCP_HELLO_REPLY,	
 #else
	ACL_DFT_RULE_IDX_RTK_PACKET,
 #endif
#endif
    ACL_DFT_RULE_IDX_DHCP_BCAST_OFFER_ACK_CPY_TO_CPU,
	ACL_DFT_RULE_IDX_HTTP_PRI,
	ACL_DFT_RULE_IDX_RRCP_PRI,
    ACL_DFT_RULE_IDX_MAX
};

typedef enum sal_filter_flowTbl_actionGroup_e
{
	SAL_FLOWTBL_LOG = 0,
	SAL_FLOWTBL_ASSIGNVLAN,
	SAL_FLOWTBL_ACTMAX
}sal_filter_flowTbl_actionGroup_t;

typedef enum sal_filter_aclTbl_actionGroup_e
{
	SAL_ACL_PERMIT_DROP_REDIR = 0,
	SAL_ACL_MIRROR,
	SAL_ACL_LOG,
	SAL_ACL_RATELIMIT,
	SAL_ACL_NEWSVID,
	SAL_ACL_NEWCVID,
	SAL_ACL_ACTMAX
}sal_filter_aclTbl_actionGroup_t;

typedef enum sal_tcamActType_e
{
	SAL_ACTTYPE_PERMIT = 0,
	SAL_ACTTYPE_DROP,
	SAL_ACTTYPE_REDIRECT,
	SAL_ACTTYPE_COPYTOCPU,
	SAL_ACTTYPE_ACTMAX
}sal_tcamActType_t;

typedef struct sal_filter_flowTbl_s
{
    uint8  dmac[ETHER_ADDR_LEN];
    uint8  smac[ETHER_ADDR_LEN];
    uint8  ipproto;
    uint8  tos;
    uint8  slp:5;
    uint8  frametype:2;
    uint8  valid:1;    /* valid bit */
    uint8  pktspri:3;   
    uint8  pktstagif:1; 
    uint8  ipv4:1;
    uint8  ipv6:1;
    uint8  ipv6mld:1;
    uint16 ethertype;
    uint16 pktctagif:1;
    uint16 pktcpri:3;
    uint16 pktcvid:12;
    uint16 srcport;
    uint16 dstport;    
    uint32 sip;
    uint32 dip;
    uint32 flowlabel:20;
    uint32 pktsvid:12;
    
    /* care bits */
    uint8  care_dmac[ETHER_ADDR_LEN];
    uint8  care_smac[ETHER_ADDR_LEN];
    uint8  care_ipproto;
    uint8  care_tos;
    uint8  care_slp:5;
    uint8  care_frametype:2;
    uint8  care_pktspri:3;
    uint8  care_pktstagif:1;
    uint8  care_ipv4:1;
    uint8  care_ipv6:1;
    uint8  care_ipv6mld:1;
    uint16 care_ethertype;
    uint16 care_pktctagif:1;
    uint16 care_pktcpri:3;
    uint16 care_pktcvid:12;
    uint16 care_srcport;
    uint16 care_dstport;
    uint32 care_sip;
    uint32 care_dip;
    uint32 care_flowlabel:20;
    uint32 care_pktsvid:12;
} sal_filter_flowTbl_t;

typedef struct sal_filter_action_s
{
    uint8 actGroup:3;

    union
    {
        /* flow table/ingress ACL action */
        struct
        {
            uint8 logindex:7;
        } log;

        /* flow table action */
        struct
        {
            uint8  usepktspri:1;
            uint8  usepktctag:1;
            uint16 actvid:12;
            uint16 actpri:3;
        } assignVlan;

        /* ingress ACL action */
        struct
        {
            uint8 acttype:2;    /* permit(00)/drop(01)/redirect(10)/copytocpu(11) */
            uint8 portid:5;     /* for redirect only                              */
        } permit_drop_redirect;
        struct
        {
            uint8 mirrorsetid:1;
        } mirror;
        struct
        {
            uint8 rateindex:7;
        } ratelimit;
        struct
        {
            uint16 svid:12;
        } newsvid;
        struct
        {
            uint8  replacecvid:1;
            uint8  replacecpri:1;
            uint16 cvid:12;
            uint16 cpri:3;
        } newcvid;
    }un;

} sal_filter_action_t;

typedef struct sal_filter_igrAcl_s
{
    uint8  dmac[ETHER_ADDR_LEN];
    uint8  smac[ETHER_ADDR_LEN];
	uint8  ipproto;
	uint8  tos;
    uint8  tcpflag;
    uint8  slp:5;
    uint8  frametype:2;
    uint8  ipv4:1;
    uint16 ethertype;
    uint16 rvid:12;
    uint16 ipv6:1;
    uint16 ipv6mld:1;
    uint16 patternmatch:2;
    uint16 srcport;
    uint16 dstport;
    uint32 sip;
    uint32 dip;
    uint32 flowlabel:20;
    uint32 pktsvid:12;
    
    /* care bits */
    uint8  care_dmac[ETHER_ADDR_LEN];
    uint8  care_smac[ETHER_ADDR_LEN];
    uint8  care_ipproto;
    uint8  care_tos;
    uint8  care_tcpflag;
    uint8  care_slp:5;
    uint8  care_frametype:2;
    uint8  care_ipv4:1;
    uint16 care_ethertype;
    uint16 care_rvid:12;
    uint16 care_ipv6:1;
    uint16 care_ipv6mld:1;
    uint16 care_patternmatch:2;
    uint16 care_srcport;
    uint16 care_dstport;
    uint32 care_sip;
    uint32 care_dip;
    uint32 care_flowlabel:20;
    uint32 care_pktsvid:12;
    
    uint8  valid:1;    /* valid bit */
} sal_filter_igrAcl_t;


/* Function Name:
 *      sal_filter_init
 * Description:
 *      Initialize the PIE module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_UNKNOWN
 */
extern int32 sal_filter_init(void);


/* Function Name:
 *      sal_filter_pktPrior_mgnt
 * Description:
 *      Set the PIE module so that it assign the packet of interest the specific priority.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 */
extern int32 sal_filter_pktPrior_mgnt(void);


/* Function Name:
 *      sal_filter_arp_copyToCpu
 * Description:
 *      Set the PIE module so that it could copy the ARP packets to CPU.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 */
extern int32 sal_filter_arp_copyToCpu(void);


/* Function Name:
 *      sal_filter_rtkPkt_trap
 * Description:
 *      Set the PIE module so that it could trap the RTK packets to CPU.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 */
extern int32 sal_filter_rtkPkt_trap(void);


/* Function Name:
 *      sal_filter_dftRule_set
 * Description:
 *      Set the PIE module some default rules.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_filter_dftRule_set(void);


/* Function Name:
 *      sal_filter_flowtable_set
 * Description:
 *      Set one rule to flowtable.
 * Input:
 *      ruleId  - rule id
 *      pCfg    - pointer buffer of flow table data
 *      pAction - pointer buffer of flow table action
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 */
extern int32 sal_filter_flowtable_set(uint32 ruleId, sal_filter_flowTbl_t *pCfg, sal_filter_action_t *pAction);


/* Function Name:
 *      sal_filter_igrAcl_set
 * Description:
 *      Set one rule to ingress acl.
 * Input:
 *      ruleId  - rule id
 *      pCfg    - pointer buffer of ingress acl data
 *      pAction	- pointer buffer of ingress acl action
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 */
extern int32 sal_filter_igrAcl_set(uint32 ruleId, sal_filter_igrAcl_t *pCfg, sal_filter_action_t *pAction);
#endif/*__SAL_FILTER_H__*/

