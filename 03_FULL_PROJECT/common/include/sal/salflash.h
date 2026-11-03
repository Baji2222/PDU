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
 * $Revision: 7020 $
 * $Date: 2009-11-13 18:39:41 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : Flash driver
 *
 */
#ifndef __SAL_FLASH_H__
#define __SAL_FLASH_H__

#include <local/sal_config.h>

#include <saltype.h>
#include <salvlan.h>
#include <salqos.h>
#include <salloop.h>
#include <salrate.h>
#include <sallldp.h>
#include <config.h>

#if SAL_RSTP
  #include <salstp.h>
#endif

/* -------------------- Signature Definitions -------------------------- */
#define FLSH_SIGNATURE_WORD 0x23792379
#define FLSH_SIGNATURE_STR  "\x23\x79\x23\x79"

typedef struct sal_flash_beginMark_s{
    uint32 signature;
} sal_flash_beginMark_t;

/* -------------------- Marking Definitions ---------------------------- */

#define FLSH_MARK_INIT_NO	0
#define FLSH_MARK_INIT_OK	7777777

typedef struct sal_flash_dirtyMark_s{
	uint32	initialized;
} sal_flash_dirtyMark_t;

/* -------------------- System IP Definitions -------------------------- */

#define FLSH_SYS_BOOTCMD_LEN    100

typedef struct sal_flash_sysInfo_s{
	uint8	dhcp;
	uint32	ip;
	uint32	netmask;
	uint32	gateway;
	sal_mac_t mac;
#ifdef __BOOT_COMMAND__
	int8  bootCmd[FLSH_SYS_BOOTCMD_LEN];
#endif
} sal_flash_sysInfo_t;

/* -------------------- User Account Definitions -------------------------- */

#define SAL_FLSH_USR_USERNAME_LEN   20
#define SAL_FLSH_USR_PASSWORD_LEN   20

typedef struct {
	uint8 username[SAL_FLSH_USR_USERNAME_LEN + 1];
    uint8 password[SAL_FLSH_USR_PASSWORD_LEN + 1];
} sal_flash_account_t;

/* -------------------- Forwarding Definitions ------------------------- */

typedef struct sal_flash_storm_s {
	uint32 bCastRate;           /* Broadcast Rate */
	uint32 mCastRate;           /* Multicast Rate */
	uint32 uKnwnUcastRate;      /* Unknown Unicast Rate */
	uint32 uKnwnMcastRate;      /* Unknown Multicast Rate */
} sal_flash_storm_t;

typedef struct sal_flash_mirror_s {
	sal_pPortmask_t		mirredPortMask;     /* Mirrored port mask */
	sal_port_t          mirrPort;           /* Mirroring Port */
	uint8				mode;
} sal_flash_mirror_t;

typedef struct {
	sal_flash_storm_t	stormCtrl[COMBO_PORT_NUM];
	sal_flash_mirror_t	mirror[SAL_MAX_MIRROR_GROUP];
	uint16              frameSize;
#if 1
	sal_enable_t        cpuEgrCtrl;
	uint8		        igrIncludeIFG;
	uint8		        egrIncludeIFG;
#endif    
} sal_flash_fwd_t;

typedef enum sal_flash_configValidity_e
{
    SAL_FLASH_CONFIG_VALID = 0,
    SAL_FLASH_CONFIG_INVALID,
    SAL_FLASH_CONFIG_END
} sal_flash_configValidity_t;

/* -------------------- Port Definitions ------------------------------- */

typedef struct {
	sal_enable_t    enable[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   AutoNegotiation[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   Half_10[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   Full_10[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   Half_100[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   Full_100[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   Full_1000[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   FC[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   AsyFC[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint32  igrRate[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint32  egrRate[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint16  securityNum[SAL_MAX_PHY_N_CPU_COMBO_PORT];
	uint8   securityAct[SAL_MAX_PHY_N_CPU_COMBO_PORT];
    sal_pPortmask_t PortIsolationMask[SAL_MAX_PHY_N_CPU_COMBO_PORT];
#if SAL_RTL8316D
    uint8	IngrsEbl[SAL_MAX_PHY_N_CPU_COMBO_PORT];
    uint8	EgrsEbl[SAL_MAX_PHY_N_CPU_COMBO_PORT];
    sal_pPortmask_t iso_mask;
#endif
    uint32  entry_limits[SAL_MAX_PHY_N_CPU_COMBO_PORT];  /* mac constraint */
    uint8   entry_action;  /* mac constraint action*/
} sal_flash_port_t;

/* -------------------- L2 Definitions -------------------------- */

typedef struct sal_flash_l2_sttcEntry_s {
	sal_mac_t		mac;
	sal_vlan_t		fid;
	uint8	        auth;
	uint8	        saBlock;
	uint8	        daBlock;
	//sal_lPortmask_t	lPortMask;
	sal_pPortmask_t	pPortMask;
	uint8           valid;     /* 1: valid, 0: invalid */
} sal_flash_l2_sttcEntry_t;

/* -------------------- Trunking Definitions --------------------------- */

typedef struct {
	sal_pPortmask_t	portMask;
	sal_pPortmask_t	actMask;
	uint8 paraMsk;
} sal_flash_trunk_group_t;

typedef struct {
	sal_flash_trunk_group_t	flashTrunkGroup[SAL_MAX_STATIC_COMBO_TRUNK];
} sal_flash_trunk_t;

/* -------------------- VLAN Definitions ------------------------------- */

typedef struct sal_flash_vlan_s
{
	sal_enable_t                enable;
    sal_vlan_tagMode_t          mode[COMBO_PORT_NUM];
	sal_vlan_t                  pvid[COMBO_PORT_NUM];
	sal_vlan_acceptFrameType_t  accpetType[COMBO_PORT_NUM];
} sal_flash_vlan_t;

typedef struct sal_flash_vlanDb_e
{
	int16	sortedSlot[SAL_MAX_STATIC_VLAN+1];
	int16	slotVid[SAL_MAX_STATIC_VLAN+1];
} sal_flash_vlanDb_t;

typedef struct sal_flash_vlanEntry_e
{
	sal_vlan_t      vlan_id;
	uint8           name[SAL_MAX_VLAN_NAME_LEN];
	sal_pPortmask_t p_portMask;
	sal_pPortmask_t p_tagMask;
	uint8           valid;     /* 1: valid, 0: invalid */
} sal_flash_vlanEntry_t;


/* -------------------- Service VLAN Definitions ------------------------ */
#if SAL_SERVICE_VLAN

typedef struct sal_flash_svlanDb_e
{
	int16	sortedSlot[SAL_MAX_STATIC_SVLAN+1];
	int16	slotVid[SAL_MAX_STATIC_SVLAN+1];
} sal_flash_svlanDb_t;

typedef struct sal_flash_svlanEntry_e
{
	sal_vlan_t      vlan_id;
	uint8           name[SAL_MAX_SVLAN_NAME_LEN];
	sal_pPortmask_t p_portMask;
	uint8           valid;     /* 1: valid, 0: invalid */
} sal_flash_svlanEntry_t;

#endif

/* -------------------- QoS Definitions ------------------------------- */

typedef struct sal_flash_qos_port_info_s
{
	sal_pri_t  pri;  /* Port-based priority */
	uint8	scheAlg;   
	uint8	numOfQ;    
	uint8	weight[SAL_MAX_NUM_OF_QUEUE];  
} sal_flash_qos_port_info_t;

typedef struct sal_qos_port_table_s
{
    sal_flash_qos_port_info_t  qosPort[SAL_MAX_PHY_N_CPU_COMBO_PORT];
} sal_flash_qos_port_table_t;

#if defined(INTERNAL_PRIORITY)
typedef struct sal_flash_qos_pri_selet_s
{
    uint32 port_pri;
    uint32 dot1q_pri;
    uint32 acl_pri;
    uint32 dscp_pri;
    uint16 cvlan_pri;
    uint32 svlan_pri;
    uint32 dmac_pri;
    uint32 smac_pri;
} sal_flash_qos_pri_selet_t;

typedef struct sal_flash_qos_dscp_remap_s
{
    sal_pri_t intPri[SAL_MAX_VALUE_OF_DSCP];
} sal_flash_qos_dscp_remap_t;


typedef struct sal_flash_qos_pri2queue_s
{
    uint32 pri2queue[SAL_MAX_NUM_OF_PRIORITY];
} sal_flash_qos_pri2queue_t;

typedef struct sal_qos_table_s
{
    sal_flash_qos_pri_selet_t  priSelection;
    sal_flash_qos_dscp_remap_t dscpRemaping;
    sal_flash_qos_pri2queue_t  pri2queue;
} sal_flash_qos_table_t;
#endif

typedef struct sal_flash_qos_dscp_remarking_s
{
    uint8 dscp_enable;
    uint8 dscp[SAL_MAX_NUM_OF_QUEUE];
} sal_flash_qos_dscp_remarking_t;

/* -------------------- LOOP Definitions ------------------------------- */
#if SAL_LOOP
typedef struct sal_flash_loop_s
{				  
    sal_loop_mode_t loopMode;
    uint16      interval_time;
    uint32      recover_time;
    uint32      rlpp_en_pmsk;
} sal_flash_loop_t;
#endif /* __SAL_LOOP__ */

#if SAL_EEE
typedef struct sal_flash_eee_s
{
    sal_enable_t enable;
} sal_flash_eee_t;
#endif

#if SAL_GREENETH
typedef struct sal_flash_greeneth_s
{
    sal_enable_t enable;
} sal_flash_greeneth_t;
#endif

#if SAL_LLDP
typedef struct sal_flash_lldp_s
{
    sal_enable_t            enable;
    sal_lldp_timer_t        txIntvl;
    sal_lldp_multi_t        holdMulti;
    sal_lldp_port_state_t   state[COMBO_PORT_NUM];
} sal_flash_lldp_t;
#endif

/* -------------------- Spanning Tree Definitions ------------------------------- */
#if SAL_RSTP
typedef struct sal_flash_stp_s
{
    sal_stp_version_t   stp_ver;
    sal_stp_pri_t       stp_pri;
    sal_stp_time_t      stp_age;
    sal_stp_time_t      stp_hello;
    sal_stp_time_t      stp_fwd;
    sal_stp_portPara_t  stp_port[SAL_MAX_PHY_N_CPU_COMBO_PORT];
}sal_flash_stp_t;
#endif

/* -------------------- RRCP Definitions ------------------------------- */
#if SAL_RRCPv1
typedef struct {
	uint8 ucRrcpVlanLeakyEbl;
	uint8 ucRrcpPortBasedVlanEbl;
	uint32	ulRrcpAdminPortMask;
	uint32 ulRrcpAuthPortMask;
} TstFlshRrcp;
#endif

#if SAL_RRCPv1
typedef struct {	
	uint32 ulTxVid;
    uint8 ucTxPrio;
    uint8 ucAsicReplyMode;
    uint8 ucForwardMode;
} TstFlshRrcpTxTag;
#endif

#if SAL_RRCPv3
typedef struct {
	uint32	ulSecMsk;
	uint32	ulAbsTrustMsk;
	uint32	ulPrivateKey;
} TstFlshRRCPv3;
#endif

#if SAL_SNMP
typedef struct {
    char            description[SAL_SNMP_SYS_INFO_STR_LEN];
    char            location[SAL_SNMP_SYS_INFO_STR_LEN];
    char            contact[SAL_SNMP_SYS_INFO_STR_LEN];
} sal_flash_sysInfo_ext_t;

typedef struct {
    bool                    enable;
    char                    read_community[SAL_SNMP_COMMUNITY_STR_LEN + 1];
    char                    write_community[SAL_SNMP_COMMUNITY_STR_LEN + 1];    
    sal_ipaddr_t            trapIp;
    sal_flash_sysInfo_ext_t extSysinfo;
} sal_flash_snmp_t;
#endif

#ifdef SAL_SYSLOG
typedef struct sal_flash_syslog_e{
    sal_enable_t            enable;
//    bool                    reserved;
	uint16					sysLogServerPort;
    sal_ipaddr_t            sysLogServerIp;
} sal_flash_syslog_t;

typedef struct sal_flash_syslog_buf_entry_e{
	uint8 sysLogIdBuf; //contains reference to syslogmsgcontent
	uint8 sysLogSevBuf; //can be removed now.. as new mem changes dont require this
	uint32 timestamp;
	uint32 addInfo; //can store IP or Port number or any other additional info
//	char sysLogMsgBuf[SAL_SYS_LOG_BUF_MAX_DESC_BUF_SIZE];
} sal_flash_syslog_buf_entry_t;

typedef struct sal_flash_syslog_buf_e{
	uint16 	headIdx;
	uint16 	tailIdx;
	sal_flash_syslog_buf_entry_t sysLogBuf[100];
} sal_flash_syslog_buf_t;

#endif

#if SAL_IGMP
typedef struct sal_flash_igmp_s
{
    sal_enable_t    enable;
    sal_lPortmask_t static_router;
} sal_flash_igmp_t;
#endif

typedef struct _ConfigCache
{
    sal_flash_beginMark_t       signature;
    sal_flash_sysInfo_t         sys;
    sal_flash_account_t         usr;
    sal_flash_fwd_t             fwd;
    sal_flash_port_t            port;
    sal_flash_l2_sttcEntry_t    mac[SAL_MAX_STATIC_MAC];
    sal_flash_trunk_t           trk;
    sal_flash_vlan_t            vlan;
    sal_flash_vlanDb_t          vlantmp;
    sal_flash_vlanEntry_t       vlanentry[SAL_MAX_STATIC_VLAN];
    sal_flash_qos_port_table_t  qosporttbl;
    sal_flash_dirtyMark_t       mark;
#if SAL_SERVICE_VLAN
    sal_flash_svlanDb_t         svlantmp;
    sal_flash_svlanEntry_t      svlanentry[SAL_MAX_STATIC_SVLAN];
    sal_vlan_t                  svlan_dft_vid;
    sal_vlan_t                  svlan_tpid;
    sal_pPortmask_t             svlan_uplink_msk;
#endif
#if SAL_LOOP
    sal_flash_loop_t    mode;    
#endif

#if SAL_RSTP
    sal_flash_stp_t     stp;
#endif

#if SAL_EEE
    sal_flash_eee_t     eee;
#endif

#if SAL_DSCP_REMARKING
    sal_flash_qos_dscp_remarking_t dscp;
#endif

#if SAL_GREENETH
    sal_flash_greeneth_t    greeneth;
#endif
#if SAL_IGMP
    sal_flash_igmp_t    igmp;
#endif
#if defined(INTERNAL_PRIORITY)
    sal_flash_qos_table_t qos;
#endif
#if SAL_SNMP
    sal_flash_snmp_t snmp;
#endif 
#if SAL_SYSLOG
	sal_flash_syslog_t syslog;
	sal_flash_syslog_buf_t syslogBuf;
#endif 
} ConfigCache;

extern ConfigCache configCache;


/* -------------------- General Functions ------------------------------ */
extern void SalFlshAccessCache(bool bCache);
extern void SalFlshInitConfigCache(void);
extern void SalFlshCopyFlshToCache(void);

extern int32 SalFlshInit(void);
extern int32 SalFlshInit_except_Mac();
extern void _SalFlshOnlyTrkLId2LogicId(int8 *pcLogicId, uint8 ucLId);
extern int32 SalFlshGetSize (uint32 *pulSize);

/* -------------------- Signature Functions ---------------------------- */
extern int32 SalFlshSetSignature(uint32 ulSignature);
extern int32 SalFlshGetSignature(uint32 *pulSignature);
 
/* -------------------- Marking Functions ------------------------------ */
extern int32 SalFlshSetInitOKMark(uint32 ulMark);
extern int32 SalFlshGetInitOKMark(uint32 *pulMark);

/* -------------------- System IP Functions ---------------------------- */

/* Function Name:
 *      sal_flash_sys_init
 * Description:
 *      Initialize the system configuration to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_sys_init(void);


/* Function Name:
 *      sal_flash_sysDhcp_set
 * Description:
 *      Save DHCP status to the flash.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_sysDhcp_set(sal_enable_t enable);


/* Function Name:
 *      sal_flash_sysDhcp_get
 * Description:
 *      Retrieve DHCP status from the flash.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of the enable status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_sysDhcp_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_flash_sysIp_set
 * Description:
 *      Save IP address to the flash.
 * Input:
 *      ip      - IP address
 *      netmask - netmask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_sysIp_set(sal_ipaddr_t ip, sal_ipaddr_t netmask);


/* Function Name:
 *      sal_flash_sysIp_get
 * Description:
 *      Retrieve IP address from the flash.
 * Input:
 *      None
 * Output:
 *      pIp      - pointer buffer of IP address
 *      pNetmask - pointer buffer of netmask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_sysIp_get(sal_ipaddr_t *pIp, sal_ipaddr_t *pNetmask);


/* Function Name:
 *      sal_flash_sysGateway_set
 * Description:
 *      Save IP address of the gateway to the flash.
 * Input:
 *      gateway - IP address
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_sysGateway_set(sal_ipaddr_t gateway);


/* Function Name:
 *      sal_flash_sysGateway_get
 * Description:
 *      Retrieve IP address of the gateway from the flash.
 * Input:
 *      None
 * Output:
 *      pGateway - pointer buffer of the IP address
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_flash_sysGateway_get(sal_ipaddr_t *pGateway);


/* Function Name:
 *      sal_flash_sysMac_set
 * Description:
 *      Save MAC address to the flash.
 * Input:
 *      pMac - pointer buffer of the MAC address
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_flash_sysMac_set(sal_mac_t *pMac);


/* Function Name:
 *      sal_flash_sysMac_set
 * Description:
 *      Retrieve MAC address from the flash.
 * Input:
 *      None
 * Output:
 *      pMac - pointer buffer of the MAC address
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_flash_sysMac_get(sal_mac_t *pMac);


#ifdef __BOOT_COMMAND__
int32 SalFlshSysSetBootCmd(uint8 *strCmd	);
int32 SalFlshSysGetBootCmd(uint8 *strCmd	);
#endif

/* Function Name:
 *      sal_flash_sysAccount_set
 * Description:
 *      Save management account to the flash.
 * Input:
 *      pUsername - pointer buffer of the username
 *      pPassword - pointer buffer of the password
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_flash_sysAccount_set(uint8 *pUsername, uint8 *pPassword);


/* Function Name:
 *      sal_flash_sysAccount_get
 * Description:
 *      Retrieve management account from the flash.
 * Input:
 *      None
 * Output:
 *      pUsername - pointer buffer of the username
 *      pPassword - pointer buffer of the password
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_flash_sysAccount_get(uint8 *pUsername, uint8 *pPassword);


/* -------------------- Rate Functions --------------------------- */
/* Function Name:
 *      sal_flash_rate_init
 * Description:
 *      Initialize rate configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_rate_init(void);

/* Function Name:
 *      sal_flash_StormCtrl_set
 * Description:
 *      Save storm control setting to flash.
 * Input:
 *      port    - port id
 *      type    - storm type
 *      rate    - storm rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_stormCtrl_set(
    sal_port_t port,
    sal_stormCtrl_type_t type,
    uint32 rate);

/* Function Name:
 *      sal_flash_StormCtrl_get
 * Description:
 *      Get storm control setting from flash.
 * Input:
 *      port    - port id
 *      type    - storm type
 * Output:
 *      pRate   - storm rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_stormCtrl_get(
    sal_port_t port,
    sal_stormCtrl_type_t type,
    uint32 *pRate);

/* Function Name:
 *      sal_flash_rate_cpuEgrCtrl_set
 * Description:
 *      Save egress bandwidth control on CPU port setting to flash
 * Input:
 *      enable  	- state of CPU port egress bandwidth control
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_cpuEgrCtrl_set(sal_enable_t enable);

/* Function Name:
 *      sal_flash_rate_cpuEgrCtrl_get
 * Description:
 *      Get egress bandwidth control on CPU port setting from flash
 * Input:
 *      None
 * Output:
 *      pEnable  	- state of CPU port egress bandwidth control
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_cpuEgrCtrl_get(sal_enable_t *pEnable);

/* Function Name:
 *      sal_flash_rate_igrIncludeIFG_set
 * Description:
 *      Save ingress bandwidth control to include IFG setting to flash
 * Input:
 *      include	- include or exclude
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_igrIncludeIFG_set(uint8 include);

/* Function Name:
 *      sal_flash_rate_igrIncludeIFG_get
 * Description:
 *      Get ingress bandwidth control IFG setting from flash
 * Input:
 *      None
 * Output:
 *      pInclude	- include or exclude
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_igrIncludeIFG_get(uint8 *pInclude);

/* Function Name:
 *      sal_flash_rate_egrIncludeIFG_set
 * Description:
 *      Save egress bandwidth control to include IFG setting to flash
 * Input:
 *      include	- include or exclude
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_egrIncludeIFG_set(uint8 include);

/* Function Name:
 *      sal_flash_rate_egrIncludeIFG_get
 * Description:
 *      Get egress bandwidth control IFG setting from flash
 * Input:
 *      None
 * Output:
 *      pInclude	- include or exclude
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_egrIncludeIFG_get(uint8 *pInclude);

/* Function Name:
 *      sal_flash_rate_igrBandwidthCtrl_set
 * Description:
 *      Save ingress bandwidth control setting to flash
 * Input:
 *      port    - port ID
 *      rate    - ingress rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_igrBandwidthCtrl_set(sal_port_t port, uint32 rate);

/* Function Name:
 *      sal_flash_rate_igrBandwidthCtrl_get
 * Description:
 *      Get ingress bandwidth control setting from flash
 * Input:
 *      port    - port ID
 * Output:
 *      pRate   - ingress rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_igrBandwidthCtrl_get(sal_port_t port, uint32 *pRate);

/* Function Name:
 *      sal_flash_rate_egrBandwidthCtrl_set
 * Description:
 *      Set egress bandwidth control setting to flash
 * Input:
 *      port    - port ID
 *      rate    - Egress rate
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_egrBandwidthCtrl_set(sal_port_t port, uint32 rate);

/* Function Name:
 *      sal_flash_rate_egrBandwidthCtrl_get
 * Description:
 *      Get egress bandwidth control setting from flash
 * Input:
 *      port    - port ID
 * Output:
 *      pRate   - Egress rate
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_rate_egrBandwidthCtrl_get(sal_port_t port, uint32 *pRate);

/* -------------------- Jumbo frame Functions --------------------------------- */

/* Function Name:
 *      sal_flash_rate_maxFrameSize_set
 * Description:
 *      Set the maximum frame size setting to flash
 * Input:
 *      size	- Maximum frame size
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_maxFrameSize_set(uint32 size);

/* Function Name:
 *      sal_flash_rate_maxFrameSize_get
 * Description:
 *      Get the maximum frame size setting from flash
 * Input:
 *      None
 * Output:
 *      pSize	- Maximum frame size
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_rate_maxFrameSize_get(uint32 *pSize);

/* -------------------- Mirror Functions --------------------------------- */

/* Function Name:
 *      sal_flash_mirror_init
 * Description:
 *      Initialize mirror configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_mirror_init(void);

/* Function Name:
 *      sal_flash_mirror_portMirror_set
 * Description:
 *      Save port mirror configuration to flash
 * Input:
 *      mirrId      - mirror set ID
 *      mode        - ingress or egress mirror
 *      port        - mirroring port
 *      pPortMask   - mirrored port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_mirror_portMirror_set(
    uint32 mirrId,
    uint32 mode,
    sal_port_t port,
    sal_lPortmask_t *pPortMask);

/* Function Name:
 *      sal_flash_mirror_portMirror_get
 * Description:
 *      Get port mirror configuration from flash
 * Input:
 *      mirrId		- mirror set ID
 * Output:
 *      pMode		- ingress or egress mirror
 *      pPort		- mirroring port
 *      pPortMask	- mirrored port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_mirror_portMirror_get(
    uint32 mirrId,
    uint32 *pMode,
    sal_port_t *pPort,
    sal_lPortmask_t *pPortMask);

/* -------------------- Port Functions --------------------------------- */

/* Function Name:
 *      sal_flash_port_init
 * Description:
 *      Initialize port configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_port_init(void);


/* Function Name:
 *      sal_flash_port_isolation_set
 * Description:
 *      Save port isolation settings of a port to flash
 * Input:
 *      port        - port id
 *      portMask    - port isolation mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_flash_port_isolation_set(sal_port_t port, sal_lPortmask_t portMask);


/* Function Name:
 *      sal_flash_port_isolation_get
 * Description:
 *      Retrieve the port isolation mask of the specific port from flash
 * Input:
 *      port        - port id
 * Output:
 *      pPortMask   - Pointer to the port isolation mask
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_flash_port_isolation_get(sal_port_t port, sal_lPortmask_t *pPortMask);


/* Function Name:
 *      sal_flash_port_adminEnable_set
 * Description:
 *      Save port enable status of the specific port to flash
 * Input:
 *      port        - port id
 *      enable      - enable status of port
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_adminEnable_set(sal_port_t port, sal_enable_t enable);


/* Function Name:
 *      sal_flash_port_adminEnable_get
 * Description:
 *      Retrieve port enable status of the specific port from flash
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - Pointer to the enable status of port
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_adminEnable_get(sal_port_t port, sal_enable_t *pEnable);


/* Function Name:
 *      sal_flash_port_phyAbility_set
 * Description:
 *      Save the port ability of the specific port to flash
 * Input:
 *      port    - port id
 *      pPhyAbility - Pointer to the port ability
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_phyAbility_set(sal_port_t port, sal_phyAbility_t *pPhyAbility);


/* Function Name:
 *      sal_flash_port_phyAbility_get
 * Description:
 *      Retrieve the port ability of the specific port from flash
 * Input:
 *      port        - port id
 * Output:
 *      pPhyAbility - Pointer to the port ability
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_phyAbility_get(sal_port_t port, sal_phyAbility_t *pPhyAbility);


/* Function Name:
 *      sal_flash_port_flowCtrl_set
 * Description:
 *      Save the flow control status of the specific port to flash
 * Input:
 *      port    - port id
 *      enable  - flow control status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_flowCtrl_set(sal_port_t port, sal_enable_t enable);



/* Function Name:
 *      sal_flash_port_flowCtrl_get
 * Description:
 *      Retrive the configured flow control status of the specific port from flash
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the configured flow control status
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_flowCtrl_get(sal_port_t port, sal_enable_t *pEnable);


/* Function Name:
 *      sal_flash_port_security_set
 * Description:
 *      Save the limited L2 learning entry count of the specific port for port security to flash
 * Input:
 *      port    - port id
 *      num     - number of limited L2 learning entry count.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_security_set(sal_port_t port, uint32 num);


/* Function Name:
 *      sal_flash_port_security_set
 * Description:
 *      Retrieve the limited L2 learning entry count of the specific port from flash
 * Input:
 *      port    - port id
 * Output:
 *      pNum    - pointer to the number of limited L2 learning entry count.
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_security_get(sal_port_t port, uint32 *pNum);

/* Function Name:
 *      sal_flash_port_securityAction_set
 * Description:
 *      Save the port security action of the specific port to flsh
 * Input:
 *      port    - port id
 *      act     - The action of port security
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_securityAction_set(sal_port_t port, uint32 act);


/* Function Name:
 *      sal_flash_port_securityAction_get
 * Description:
 *      Retrieve the port security action of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAct    - Pointer to the action of port security
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_port_securityAction_get(sal_port_t port, uint32 *pAct);

/* -------------------- L2 Table Functions ---------------------------- */
/* Function Name:
 *      sal_flash_l2_init
 * Description:
 *      Initialize L2 configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_l2_init(void);

/* Function Name:
 *      sal_flash_l2_sttcEntry_add
 * Description:
 *      Add a static L2 entry to flash.
 * Input:
 *      slot		- entry index
 *      pEntry		- entry data
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_ENTRY_INDEX
 * Note:
 *      None
 */
extern int32 sal_flash_l2_sttcEntry_add(
    uint32 slot,
    sal_flash_l2_sttcEntry_t *pEntry);

/* Function Name:
 *      sal_flash_l2_sttcEntry_del
 * Description:
 *      Delete a static L2 entry from flash.
 * Input:
 *      slot		- entry index
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_ENTRY_INDEX
 * Note:
 *      None
 */
extern int32 sal_flash_l2_sttcEntrySlot_del(uint32 slot);

/* Function Name:
 *      sal_flash_l2_sttcEntry_get
 * Description:
 *      Get a static L2 entry from flash.
 * Input:
 *      slot		- entry index
 *      pEntry		- entry data
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_ENTRY_INDEX
 * Note:
 *      None
 */
extern int32 sal_flash_l2_sttcEntry_get(
    uint32 slot,
    sal_flash_l2_sttcEntry_t *pEntry);

/* Function Name:
 *      sal_flash_l2_firstAvailableSlot_get
 * Description:
 *      Find the first available slot from flash.
 * Input:
 *      slot		- entry index
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_flash_l2_firstAvailableSlot_get(uint32	*pSlot);

/* Function Name:
 *      sal_flash_l2_sttcEntry_del
 * Description:
 *      Delete a static L2 entry by mac and vid from flash.
 * Input:
 *      mac		- mac address
 *      vid		- VLAN id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_l2_sttcEntry_del(sal_mac_t *mac, sal_vlan_t vid);

/* Function Name:
 *      sal_flash_l2_matchedSlot_get
 * Description:
 *      Get slot id by searching matched mac address and VID.
 * Input:
 *      pMac    - Pointer to the mac address
 *      vid     - VLAN id
 * Output:
 *      *pSlot  - Pointer to te matched slot
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_l2_matchedSlot_get(uint32 *slot, sal_mac_t *mac, sal_vlan_t vid);

/* Function Name:
 *      sal_flash_l2_mac_constraint_set
 * Description:
 *      Set MAC constraint configuration to flash
 * Input:
 *      port    - port id
 *      mac_cnt - allowed learn limit of this port.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_l2_mac_constraint_set(sal_port_t port, uint32 mac_cnt);

/* Function Name:
 *      sal_flash_l2_mac_constraint_get
 * Description:
 *      Get MAC constraint configuration from flash
 * Input:
 *      port     - port id
 *      pMac_cnt - allowed learn limit of this port.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_l2_mac_constraint_get(sal_port_t port, uint32* pMac_cnt);

/* Function Name:
 *      sal_flash_l2_mac_constraint_act_set
 * Description:
 *      Set MAC constraint learn over action to flash
 * Input:
 *      action    - learn over action
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_l2_mac_constraint_act_set(uint8 action);

/* Function Name:
 *      sal_flash_l2_mac_constraint_act_get
 * Description:
 *      Get MAC constraint learn over action from flash
 * Input:
 *      None
 * Output:
 *      action     - MAC constraint Learn over action
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_l2_mac_constraint_act_get(uint8 *action);


/* -------------------- Trunking Functions ----------------------------- */

/* Function Name:
 *      sal_flash_trunk_init
 * Description:
 *      Initialize the trunk configurations to factory default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_trunk_init(void);


/* Function Name:
 *      sal_flash_trunk_port_set
 * Description:
 *      Save the trunk member port mask to flash
 * Input:
 *      trkId       - Trunk ID
 *      is_Lacp      - lacp or static trunk type
 *      pPortMask   - pointer to the member port mask
 *      pActMask    - pointer to the active port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_trunk_port_set(uint8 trkId, uint8 is_Lacp, sal_pPortmask_t *pPortMask, sal_pPortmask_t *pActMask);


/* Function Name:
 *      sal_flash_trunk_port_del
 * Description:
 *      Delete all the trunk member ports of a trunk from flash
 * Input:
 *      trkId - trunk ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_trunk_port_del(uint8 trkId);


/* Function Name:
 *      sal_flash_trunk_port_get
 * Description:
 *      Retrieve the trunk member ports of a trunk from flash
 * Input:
 *      trkId - trunk ID
 * Output:
 *      pIs_Lacp       - pointer to Lacp/Static trunk type
 *      pPortMask   - pointer to the member port mask
 *      pActMask    - pointer to the active port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_trunk_port_get(uint8 trkId, uint8 *pIs_Lacp, sal_pPortmask_t *pPortMask, sal_pPortmask_t *pActMask);


/* -------------------- VLAN Functions --------------------------------- */

/* Function Name:
 *      sal_flash_vlan_init
 * Description:
 *      Initialize VLAN configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_vlan_init(void);


/* Function Name:
 *      sal_flash_vlan_enable_set
 * Description:
 *      Save the VLAN enable status to flash
 * Input:
 *      enable - VLAN enable status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_enable_set(sal_enable_t enable);


/* Function Name:
 *      sal_flash_vlan_enable_get
 * Description:
 *      Retrieve the VLAN enable status from flash
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the VLAN enalbe status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_enable_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_flash_vlan_tagMode_set
 * Description:
 *      Save the VLAN tag mode of a specific port to flash
 * Input:
 *      port - Port ID
 *      mode - VLAN mode
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_tagMode_set(sal_port_t port, sal_vlan_tagMode_t mode);


/* Function Name:
 *      sal_flash_vlan_tagMode_get
 * Description:
 *      Retrieve the VLAN tag mode of a specific port from flash
 * Input:
 *      port    - port ID
 * Output:
 *      pMode   - pointer to the VLAN mode
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_tagMode_get(sal_port_t port, sal_vlan_tagMode_t *pMode);


/*  Function Name:
 *      sal_flash_vlan_portPvid_set
 * Description:
 *      Save the PVID of a specific port to flash
 * Input:
 *      port    - port ID
 *      vlanId     - Port VLAN ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_portPvid_set(sal_port_t port, sal_vlan_t vlanId);


/* Function Name:
 *      sal_flash_vlan_portPvid_get
 * Description:
 *      Retrieve the PVID of a specific port from flash
 * Input:
 *      port    	- port ID
 * Output:
 *      pVlanId	- pointer to the port VLAN ID
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_portPvid_get(sal_port_t port, sal_vlan_t *pVlanId);


/* Function Name:
 *      sal_flash_vlan_portAcceptFrameType_set
 * Description:
 *      Save the VLAN accept frame type of a specific port to flash
 * Input:
 *      port    - port ID
 *      type    - VLAN accept frame type
 * Output:
 *      None
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_portAcceptFrameType_set(sal_port_t port, sal_vlan_acceptFrameType_t type);


/* Function Name:
 *      sal_flash_vlan_portAcceptFrameType_get
 * Description:
 *      Retrieve the VLAN accept frame type of a specific port from flash
 * Input:
 *      port    - port ID
 * Output:
 *      pType   - pointer to the VLAN accept frame type
 * Return:
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_portAcceptFrameType_get(sal_port_t port, sal_vlan_acceptFrameType_t *pType);


/* Function Name:
 *      sal_flash_vlan_db_set
 * Description:
 *      Save the VLAN entries local database to flash
 * Input:
 *      pDb - Pointer to the VLAN local database
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_db_set(sal_flash_vlanDb_t *pDb);


/* Function Name:
 *      sal_flash_vlan_db_set
 * Description:
 *      Retrieve the VLAN entries local database from flash
 * Input:
 *      None
 * Output:
 *      pDb - Pointer to the VLAN local database
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_db_get(sal_flash_vlanDb_t *pDb);


/* Function Name:
 *      sal_flash_vlan_sttcEntry_create
 * Description:
 *      Create a static VLAN in flash
 * Input:
 *      slot - The slot number of the local database that this VLAN entry stores
 *      pVlanEntry - Pointer to the VLAN entry
 * Output:
 *      None
 * Return:
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_sttcEntry_create(uint16 slot, sal_flash_vlanEntry_t *pVlanEntry);


/* Function Name:
 *      sal_flash_vlan_sttcEntry_set
 * Description:
 *      Save the member ports and tagged ports of a static VLAN to flash
 * Input:
 *      slot        - slot number of the local database that this VLAN entry stores
 *      pVlanEntry  - pointer to the VLAN entry
 * Output:
 *      None
 * Return:
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_sttcEntry_set(uint16 slot, sal_flash_vlanEntry_t *pVlanEntry);


/* Function Name:
 *      sal_flash_vlan_sttcEntry_remove
 * Description:
 *      Remove a static VLAN entry from flash
 * Input:
 *      slot - slot number of the local database that this VLAN entry stores
 * Output:
 *      None
 * Return:
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_sttcEntry_remove(uint16 slot);


/* Function Name:
 *      sal_flash_vlan_sttcEntry_get
 * Description:
 *      Retrieve the member port mask and tag port mask a static VLAN entry from flash
 * Input:
 *      slot        - slot number of the local database that this VLAN entry stores
 * Output:
 *      pVlanEntry  - pointer to the VLAN entry
 * Return:
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_sttcEntry_get(uint16 slot, sal_flash_vlanEntry_t *pVlanEntry);


#ifdef __PRIVATE_VLAN__
/* Function Name:
 *      sal_flash_vlan_private_init
 * Description:
 *      Initialize Private-VLAN configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_vlan_private_init(void);

/* Function Name:
 *      sal_flash_vlan_privateEnable_set
 * Description:
 *      Save the Private-VLAN enable status to flash
 * Input:
 *      enable - Private-VLAN enable status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_privateEnable_set(sal_enable_t enable);

/* Function Name:
 *      sal_flash_vlan_privateEnable_get
 * Description:
 *      Retrieve the Private-VLAN enable status from flash
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the Private-VLAN enalbe status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_privateEnable_get(sal_enable_t *pEnable);

/* Function Name:
 *      sal_flash_vlan_privateEntry_set
 * Description:
 *      Save the VLAN name and member ports of a Private-VLAN to flash
 * Input:
 *      slot          - slot number of the local database that this Private-VLAN entry stores
 *      pPrivateEntry - pointer to the Private-VLAN entry
 * Output:
 *      None
 * Return:
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_privateEntry_set(uint16 slot, sal_flash_vlanPrivateEntry_t *pPrivateEntry);

/* Function Name:
 *      sal_flash_vlan_privateEntry_get
 * Description:
 *      Retrieve the VLAN name and member ports of a Private-VLAN from flash
 * Input:
 *      slot          - slot number of the local database that this Private-VLAN entry stores
 * Output:
 *      pPrivateEntry - pointer to the VLAN entry
 * Return:
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_ENTRY_INDEX
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_vlan_privateEntry_get(uint16 slot, sal_flash_vlanPrivateEntry_t *pPrivateEntry);
#endif

/* -------------------------- QoS Functions --------------------------------- */

int32 _sal_flash_qos_portTable_get(sal_flash_qos_port_table_t *portTbl);

/* Function Name:
 *      sal_flash_qos_init
 * Description:
 *      Initialize the QoS configuration to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_qos_init(void);


/* Function Name:
 *      sal_flash_qos_portPri_set
 * Description:
 *      Save port-based priority to flash.
 * Input:
 *      port    - port id
 *      intPri  - priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *               the highest prioirty)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_portPri_set(sal_port_t port, sal_pri_t intPri);


/* Function Name:
 *      sal_flash_qos_portPri_get
 * Description:
 *      Retrieve port-based priority from flash.
 * Input:
 *      port       - port id
 * Output:
 *      pIntPri    - priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_portPri_get(sal_port_t port, sal_pri_t *pIntPri);

#if defined(INTERNAL_PRIORITY)
/* Function Name:
 *      sal_flash_qos_priSel_set
 * Description:
 *      Set the priority selection table.
 * Input:
 *      pPriSel    - priority assign for each priority-based selection
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 * Note:
 *      None
 */
extern int32 sal_flash_qos_priSel_set(sal_flash_qos_pri_selet_t *pPriSel);


/* Function Name:
 *      sal_flash_ qos_priSel_get
 * Description:
 *      Get the priority selection table.
 * Input:
 *      None
 * Output:
 *      pPriSel    - priority value of each priority-based selection
 * Return:
 *      SAL_ERR_SUCCESS 
 * Note:
 *      None
 */
extern int32 sal_flash_qos_priSel_get(sal_flash_qos_pri_selet_t *pPriSel); 

extern int32 sal_flash_qos_dscpPriRemap_set(sal_dscp_t dscp, sal_pri_t int_pri);

extern int32 sal_flash_qos_dscpPriRemap_get(sal_dscp_t dscp, sal_pri_t *int_pri);

extern int32 sal_flash_qos_priMap_set (sal_flash_qos_pri2queue_t *pri2queue);

extern int32 sal_flash_qos_priMap_get (sal_flash_qos_pri2queue_t *pri2queue);
#endif

/* Function Name:
 *      sal_flash_qos_queueNum_set
 * Description:
 *      Save queue number to flash.
 * Input:
 *      port        - port id
 *      queueNum    - the number of queue.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_queueNum_set(sal_port_t port, uint8 queueNum);


/* Function Name:
 *      sal_flash_qos_queueNum_get
 * Description:
 *      Retrieve queue number from flash.
 * Input:
 *      port        - port id
 * Output:
 *      pQueueNum  - the number of queue.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_queueNum_get(sal_port_t port, uint8 *pQueueNum);


/* Function Name:
 *      sal_flash_qos_schedulingAlgo_set
 * Description:
 *      Save scheduling algorithm to flash.
 * Input:
 *      port - port id
 *      scheduling_type - type of scheduling algorithm.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_schedulingAlgo_set(sal_port_t port, sal_qos_scheduling_type_t scheduling_type);


/* Function Name:
 *      sal_flash_qos_schedulingAlgo_get
 * Description:
 *      Retrieve scheduling algorithm from flash.
 * Input:
 *      port - port id
 * Output:
 *      pScheduling_type - type of scheduling algorithm.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_schedulingAlgo_get(sal_port_t port, sal_qos_scheduling_type_t *pScheduling_type);


/* Function Name:
 *      sal_flash_qos_schedulingQueue_set
 * Description:
 *      Save scheduling types and queue weight to flash.
 * Input:
 *      port - port id
 *      pQweights - the array of weights for WRR/WFQ queue
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_schedulingQueue_set(sal_port_t port, sal_qos_queue_weights_t *pQweights);


/* Function Name:
 *      sal_flash_qos_schedulingQueue_get
 * Description:
 *      Retrieve scheduling types and queue weight from flash.
 * Input:
 *      port - port id
 * Output:
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_flash_qos_schedulingQueue_get(sal_port_t port, sal_qos_queue_weights_t *pQweights);

#if SAL_DSCP_REMARKING
/* Function Name:
 *      sal_flash_qos_dscpRemarking_set
 * Description:
 *      Save DSCP remarking configuration to flash.
 * Input:
 *      qid    - queue ID
 *      dscp   - remarked DSCP value
 *      enable - Global DSCP Remarking function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_qos_dscpRemarking_set(uint8 qid, uint8 dscp, uint8 dscp_enable);

/* Function Name:
 *      sal_flash_qos_dscpRemarking_get
 * Description:
 *      Retrieve DSCP remarking configuration from flash.
 * Input:
 *      qid  - queue ID
 * Output:
 *      pDdscp  - poionter to remarked DSCP value
 *      pEnable - pointer to Global DSCP Remarking function 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_qos_dscpRemarking_get(uint8 qid, uint8 *pDscp, uint8 *pEnable);
#endif

/* -------------------- RRCP Functions --------------------------------- */
#ifdef __RRCPV1__
void   SalFlshRrcpInit(void);
extern int32 sal_flash_rrcp1_adminPortMask_set (sal_pPortmask_t *pAdmin_mask);
extern int32 sal_flash_rrcp1_adminPortMask_get (sal_pPortmask_t *pAdmin_mask);
extern int32 sal_flash_rrcp1_authPortMask_set (sal_pPortmask_t *pAuth_mask);
extern int32 sal_flash_rrcp1_authPortMask_get(sal_pPortmask_t *pAuth_mask);


/* Function Name:
 *      sal_flash_rrcp1_vlanLeaky_set
 * Description:
 *      Save the vlan egress leaky configure.
 * Input:
 *      enable	- vlan leaky enable status
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_flash_rrcp1_vlanLeaky_set(sal_enable_t enable);
extern int32 sal_flash_rrcp1_vlanLeaky_get(sal_enable_t *pEnable);
extern int32 sal_flash_rrcp1_portBasedVlan_set(sal_enable_t enable);
extern int32 sal_flash_rrcp1_portBasedVlan_get(sal_enable_t *pEnable);
extern int32 sal_flash_rrcp1_asicReplyMode_set(sal_rrcp_tx_tagMode_t mode);
extern int32 sal_flash_rrcp1_asicReplyMode_get(sal_rrcp_tx_tagMode_t *pMode);
extern int32 sal_flash_rrcp1_forwardMode_set(sal_rrcp_tx_tagMode_t mode);
extern int32 sal_flash_rrcp1_forwardMode_get(sal_rrcp_tx_tagMode_t *pMode);
extern int32 sal_flash_rrcp1_defaultTag_set(sal_vlan_t vid, sal_pri_t pri);
extern int32 sal_flash_rrcp1_defaultTag_get(sal_vlan_t *pVid, sal_pri_t *pPri);
#endif/*__RRCPV1__*/
#ifdef __RRCPV3__
void   SalFlshRRCPv3Init(void);
int32 SalFlshRRCPv3SetAbsTrustPort(uint32 bits);
int32 SalFlshRRCPv3GetAbsTrustPort (uint32 *pbits);
int32 SalFlshRRCPv3SetPrivateKey(uint32 ulKey);
int32 SalFlshRRCPv3GetPrivateKey (uint32 *pulKey);
#endif/*__RRCPV3__*/

#ifdef SAL_IGMP
/* -------------------- IGMP Functions --------------------------------- */

/* Function Name:
 *      sal_flash_igmp_init
 * Description:
 *      Initialize IGMP configurations to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_flash_igmp_init(void);

/* Function Name:
 *      sal_flash_igmp_configEnable_get
 * Description:
 *      Retrieve IGMP status (enable/disable) from flash.
 * Input:
 *      None
 * Output:
 *      pIgmpStatus - IGMP status (enable/disable)
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_igmp_configEnable_get(uint8 *pIgmpStatus);

/* Function Name:
 *      sal_flash_igmp_configEnable_set
 * Description:
 *      Save IGMP status (enable/disable) to flash.
 * Input:
 *      IgmpStatus - IGMP status (enable/disable)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_igmp_configEnable_set(uint8 igmpStatus);

/* Function Name:
 *      sal_flash_igmp_staticRouter_get
 * Description:
 *      Retrieve IGMP statistic router port mask from flash.
 * Input:
 *      None
 * Output:
 *      pPortMask - statistic router port mask
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_igmp_staticRouter_get(sal_lPortmask_t *pPortMask);

/* Function Name:
 *      sal_flash_igmp_staticRouter_set
 * Description:
 *      Save IGMP static router port mask to flash.
 * Input:
 *      portMask - static router port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_igmp_staticRouter_set(sal_lPortmask_t portMask);

#endif

/* -------------------- Service VLAN Definitions ------------------------ */
#if SAL_SERVICE_VLAN

/* Function Name:
 *      sal_flash_svlan_init
 * Description:
 *      Init SVLAN configuration
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
void  sal_flash_svlan_init(void);

/* Function Name:
 *      sal_flash_svlan_db_set
 * Description:
 *      Set SVLAN configuration to flash
 * Input:
 *      pstTmp - poionter to the SVLAN configuration.
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_db_set (sal_flash_svlanDb_t * pstTmp);

/* Function Name:
 *      sal_flash_svlan_db_get
 * Description:
 *      Get SVLAN configuration from flash
 * Input:
 *      None.
 * Output:
 *      pstTmp - poionter to the SVLAN configuration.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_db_get (sal_flash_svlanDb_t * pstTmp);

/* Function Name:
 *      sal_flash_svlan_sttcEntry_set
 * Description:
 *      Set SVLAN entry to flash
 * Input:
 *      usSlot - slot number.
 *      pstSVlanEntry - pointer to the SVLAN entry. 
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_sttcEntry_set (uint16 usSlot, sal_flash_svlanEntry_t *pstSVlanEntry);

/* Function Name:
 *      sal_flash_svlan_sttcEntry_remove
 * Description:
 *      Remove SVLAN entry from 
 * Input:
 *      usSlot - slot number.
 * Output:
 *      pstSVlanEntry - pointer to the SVLAN entry.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_sttcEntry_remove (uint16  usSlot);

/* Function Name:
 *      sal_flash_svlan_sttcEntry_get
 * Description:
 *      Get SVLAN entry from flash
 * Input:
 *      usSlot - slot number.
 * Output:
 *      pstSVlanEntry - pointer to the SVLAN entry. 
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_sttcEntry_get (uint16 usSlot, sal_flash_svlanEntry_t *pstSVlanEntry);

/* Function Name:
 *      sal_flash_svlan_svid_set
 * Description:
 *      Set defualt SVID from flash
 * Input:
 *      pstSvid - Pointer to the default svid.
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_svid_set (sal_vlan_t *pstSvid);

/* Function Name:
 *      sal_flash_svlan_svid_get
 * Description:
 *      Get defualt SVID from flash
 * Input:
 *      None.
 * Output:
 *      pstSvid - Pointer to the default svid.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_svid_get (sal_vlan_t *pstSvid);

/* Function Name:
 *      sal_flash_svlan_tpid_set
 * Description:
 *      Set uplink mask to flash
 * Input:
 *      tpid - Pointer to the TPID.
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_tpid_set (sal_vlan_t *tpid);

/* Function Name:
 *      sal_flash_svlan_tpid_get
 * Description:
 *      Get uplink mask from flash
 * Input:
 *      None
 * Output:
 *      pstTpid - Pointer to the TPID.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_tpid_get (sal_vlan_t *pstTpid);

/* Function Name:
 *      sal_flash_svlan_uplinkMask_set
 * Description:
 *      Set uplink mask to flash
 * Input:
 *      pPortMask - Pointer to the portmask.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_uplinkMask_set (sal_pPortmask_t *pPortMask);

/* Function Name:
 *      sal_flash_svlan_uplinkMask_get
 * Description:
 *      Get uplink mask from flash
 * Input:
 *      None
 * Output:
 *      pPortMask - Pointer to the portmask.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_svlan_uplinkMask_get (sal_pPortmask_t *pPortMask);

#endif

/* Function Name:
 *      sal_flash_cfg_save
 * Description:
 *      Save configuration to flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_cfg_save();

/* Function Name:
 *      sal_flash_cfg_reset
 * Description:
 *      Set configuration to factory default.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_cfg_reset();

/* Function Name:
 *      sal_flash_cfg_set
 * Description:
 *      Write configuration from buffer to cache
 * Input:
 *      off    - Offset of cache
 *      pData  - Pointer to buffer
 *      length - Buffer length
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER    - (length + addr) is out of range of config
 * Note:
 *      None
 */
extern int32 sal_flash_cfg_set(uint16 off, uint16 length, uint8* pData);

/* Function Name:
 *      sal_flash_cfg_get
 * Description:
 *      Get configuration from cache
 * Input:
 *      off    - Offset of cache
 *      length - Buffer length
 * Output:
 *      pData  - Pointer to buffer
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_cfg_get(uint16 off, uint16 length, uint8* pData);

/* Function Name:
 *      sal_flash_cfgSize_get
 * Description:
 *      Get size of configuration
 * Input:
 *      None
 * Output:
 *      pSize - pointer to configuration size
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_cfgSize_get(uint16 *pSize);

/* Function Name:
 *      sal_flash_cfgAddr_get
 * Description:
 *      Get address of configuration
 * Input:
 *      None
 * Output:
 *      ppConfig - pointer to address of configuration
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_cfgAddr_get(uint8 **ppConfig);

/* Function Name:
 *      sal_flash_cfgValidity_get
 * Description:
 *      Get whether the inputed pointer points to valid configuration
 * Input:
 *      pConfig - pointer to configuration
 * Output:
 *      pStatus - pointer to validity of configuration
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_cfgValidity_get(uint8 * pConfig, sal_flash_configValidity_t *pStatus);


#if SAL_LOOP

/* Function Name:
 *      sal_flash_loop_init
 * Description:
 *      Initialize Loop Detection/Prevention function from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern void sal_flash_loop_init (void);

/* Function Name:
 *      sal_flash_loop_get
 * Description:
 *      Get Loop Detection/Prevention mode from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_loop_get (sal_loop_mode_t *pMode);

/* Function Name:
 *      sal_flash_loop_set
 * Description:
 *      Set Loop Detection/Prevention mode to flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_flash_loop_set (sal_loop_mode_t *pMode);

/* Function Name:
 *      sal_flash_loop_interval_time_get
 * Description:
 *      Get Loop Detection/Prevention interval time from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_loop_interval_time_get(uint16 *pTime);

/* Function Name:
 *      sal_flash_loop_interval_time_set
 * Description:
 *      Set Loop Detection/Prevention interval time to flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_flash_loop_interval_time_set(uint16 time);

/* Function Name:
 *      sal_flash_loop_recover_time_get
 * Description:
 *      Get Loop Detection/Prevention recover time from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_loop_recover_time_get(uint32 *pTime);

/* Function Name:
 *      sal_flash_loop_recover_time_set
 * Description:
 *      Set Loop Detection/Prevention recover time to flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_flash_loop_recover_time_set(uint32 time);

/* Function Name:
 *      sal_flash_loop_port_enable_get
 * Description:
 *      Get Loop Detection/Prevention port from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_loop_port_enable_get(uint8 ucPortId, uint8 *pEnable);

/* Function Name:
 *      sal_flash_loop_port_enable_set
 * Description:
 *      Set Loop Detection/Prevention port to flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_flash_loop_port_enable_set(uint8 ucPortId, uint8 enable);

#endif

#if SAL_RSTP
/* Function Name:
 *      sal_flash_stp_init
 * Description:
 *      Initialize spanning tree configuration in the flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern void sal_flash_stp_init (void);

/* Function Name:
 *      sal_flash_stp_version_get
 * Description:
 *      Get stp/rstp version from flash.
 * Input:
 *      None
 * Output:
 *      version - pointer to version of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_stp_version_get (sal_stp_version_t *version);

/* Function Name:
 *      sal_flash_stp_version_set
 * Description:
 *      Set stp/rstp version into flash.
 * Input:
 *      version - version of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_stp_version_set (sal_stp_version_t version);

/* Function Name:
 *      sal_flash_stp_priority_get
 * Description:
 *      Get stp/rstp priority from flash.
 * Input:
 *      None
 * Output:
 *      pPiority - pointer to priority of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_priority_get (sal_stp_pri_t *pPiority);

/* Function Name:
 *      sal_flash_stp_priority_set
 * Description:
 *      Set stp/rstp priority into flash.
 * Input:
 *      priority - priority of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_priority_set (sal_stp_pri_t priority);

/* Function Name:
 *      sal_flash_stp_timePara_get
 * Description:
 *      Get maximum age, hello time, forward delay of stp/rstp from flash.
 * Input:
 *      None
 * Output:
 *      pMmaxAge   - pointer of maximum age of stp/rstp.
 *      pHelloTime - pointer of hello time of stp/rstp.
 *      pFwdDelay  - pointer of forward delay of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_timePara_get (sal_stp_time_t *pMaxAge, sal_stp_time_t *pHelloTime, sal_stp_time_t *pFwdDelay);

/* Function Name:
 *      sal_flash_stp_timePara_set
 * Description:
 *      Set maximum age, hello time, forward delay of stp/rstp into flash.
 * Input:
 *      maxAge    - maximum age of stp/rstp.
 *      helloTime - hello time of stp/rstp.
 *      fwdDelay  - forward delay of stp/rstp.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_timePara_set (sal_stp_time_t maxAge, sal_stp_time_t helloTime, sal_stp_time_t fwdDelay);

/* Function Name:
 *      sal_flash_stp_port_get
 * Description:
 *      Get port parameter of stp/rstp from flash.
 * Input:
 *      port        - logical port id
 * Output:
 *      pPort_config - Pointer to port parameter of stp/rstp.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_port_get (sal_port_t port, sal_stp_portPara_t *pPort_config);

/* Function Name:
 *      sal_flash_stp_port_set
 * Description:
 *      Set port parameter of stp/rstp into flash.
 * Input:
 *      pPortPara - Pointer to port parameter of stp/rstp.
 *      port      - logical port id 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_stp_port_set (sal_port_t port, sal_stp_portPara_t* pPortPara);
#endif

#if SAL_EEE
/* Function Name:
 *      sal_flash_eee_set
 * Description:
 *      Set configuration of EEE in flash
 * Input:
 *      eeeEnable - enable/disable eee function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_eee_set (sal_enable_t eeeEnable);

/* Function Name:
 *      sal_flash_eee_get
 * Description:
 *      Get configuration of EEE in flash
 * Input:
 *      None
 * Output:
 *      pEeeEnable - pointer to enable/disable eee function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_eee_get (sal_enable_t *pEeeEnable);

/* Function Name:
 *      sal_flash_eee_init
 * Description:
 *      Initialize EEE configuration in the flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern void sal_flash_eee_init (void);

#endif

#if SAL_GREENETH

/* Function Name:
 *      sal_flash_greenEth_port_enable_set
 * Description:
 *      Set configuration of Green Ethernet in flash
 * Input:
 *      enable - enable/disable Green Ethernet function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_greenEth_port_enable_set (sal_enable_t enable);

/* Function Name:
 *      sal_flash_greenEth_port_enable_get
 * Description:
 *      Get configuration of Green Ethernet in flash
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to enable/disable Green Ethernet function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_greenEth_port_enable_get (sal_enable_t *pEnable);

/* Function Name:
 *      sal_flash_greenEth_init
 * Description:
 *      Initialize Green Ethernet configuration in the flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern void sal_flash_greenEth_init (void);

#endif

#if SAL_LLDP
/* Function Name:
 *      sal_flash_lldp_set
 * Description:
 *      Set configuration of LLDP in flash
 * Input:
 *      enable - enable/disable LLDP function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_set (sal_enable_t enable);

/* Function Name:
 *      sal_flash_lldp_get
 * Description:
 *      Get configuration of LLDP from flash
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to enable/disable lldp function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_get (sal_enable_t *pEnable);

/* Function Name:
 *      sal_flash_lldp_time_set
 * Description:
 *      Set time configuration of LLDP in flash
 * Input:
 *      txIntvl       - Transmission Interval
 *      holdTimeMulti - Hold time multiplier
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_time_set (sal_lldp_timer_t txIntvl, sal_lldp_multi_t holdTimeMulti);

/* Function Name:
 *      sal_flash_lldp_time_get
 * Description:
 *      Get time configuration of LLDP from flash
 * Input:
 *      None
 * Output:
 *      pTxIntvl       - Pointer to transmission Interval
 *      pHoldTimeMulti - Pointer to hold time multiplier
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_time_get (sal_lldp_timer_t *pTxIntvl, sal_lldp_multi_t *pHoldTimeMulti);

/* Function Name:
 *      sal_flash_lldp_portState_set
 * Description:
 *      Set port state configuration of LLDP in flash
 * Input:
 *      port  - port id
 *      state - port state
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_portState_set(sal_port_t port, sal_lldp_port_state_t state);

/* Function Name:
 *      sal_flash_lldp_portState_get
 * Description:
 *      Get port state configuration of LLDP from flash
 * Input:
 *      port  - port id
 * Output:
 *      pState         - port state
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_portState_get(sal_port_t port, sal_lldp_port_state_t *pState);


/* Function Name:
 *      sal_flash_lldp_init
 * Description:
 *      Initialize LLDP configuration in the flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_lldp_init();

#endif

#if SAL_SNMP
/* Function Name:
 *      sal_flash_snmp_community_set
 * Description:
 *      Set community string to flash
 * Input:
 *      pReadComm   - Pointer to string of read community.
 *      pWriteComm  - Pointer to string of write community.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_community_set(char *pReadComm, char *pWriteComm);

/* Function Name:
 *      sal_flash_snmp_community_get
 * Description:
 *      Get community string from flash
 * Input:
 *      None 
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_community_get(char *pReadComm, char *pWriteComm);

/* Function Name:
 *      sal_flash_snmp_trapIp_set
 * Description:
 *      Set IP address of SNMP trap packet to flash
 * Input:
 *      trapIp - Trap IP Address 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_trapIp_set(sal_ipaddr_t trapIp);

/* Function Name:
 *      sal_flash_snmp_trapIp_get
 * Description:
 *      Get IP address of SNMP trap packet from flash
 * Input:
 *      None
 * Output:
 *      pTrapIp - Pointer to trap IP Address  
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_trapIp_get(sal_ipaddr_t *pTrapIp);

/* Function Name:
 *      sal_flash_snmp_init
 * Description:
 *      Initializtion of SNMP
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_init();

/* Function Name:
 *      sal_flash_snmp_state_set
 * Description:
 *      Set snmp enable state to flash
 * Input:
 *      enable - enable/disable snmp
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_state_set(sal_enable_t enable);

/* Function Name:
 *      sal_flash_snmp_state_get
 * Description:
 *      Get snmp enable/disable state from flash
 * Input:
 *      None
 * Output:
 *      enable - Pointer to snmp enable state  
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_snmp_state_get(sal_enable_t *enable);

/* Function Name:
 *      sal_flash_snmp_description_get
 * Description:
 *      Get description string from flash
 * Input:
 *      None 
 * Output:
 *      pDescription - Get the description from flash 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_description_get(char *pDescripion);

/* Function Name:
 *      sal_flash_snmp_location_get
 * Description:
 *      Get location string from flash
 * Input:
 *      None 
 * Output:
 *      plocation - Get the location from flash 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_location_get(char *pLocation);

/* Function Name:
 *      sal_flash_snmp_contact_get
 * Description:
 *      Get contact string from flash
 * Input:
 *      None 
 * Output:
 *      pContact - Get the contact from flash 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_contact_get(char *pContact);

/* Function Name:
 *      sal_flash_sys_description_set
 * Description:
 *      Set description string to flash
 * Input:
 *      pDescription - Set the description to flash 
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_description_set(char *pDescription);


/* Function Name:
 *      sal_flash_sys_contact_set
 * Description:
 *      Set Contact string to flash
 * Input:
 *      pContact - Set the Contact to flash 
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_contact_set(char *pContact);

/* Function Name:
 *      sal_flash_sys_location_set
 * Description:
 *      Set Location string to flash
 * Input:
 *      pLocation - Set the Location to flash 
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_flash_sys_location_set(char *pLocation);
#endif

#if SAL_SYSLOG


/* Function Name:
 *      sal_flash_syslog_serverIp_set
 * Description:
 *      Set IP address of Syslog server address to flash
 * Input:
 *      logServerIp - Syslog Server IP Address 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_serverIp_set(sal_ipaddr_t logServerIp);


/* Function Name:
 *      sal_flash_syslog_serverIp_get
 * Description:
 *      Get IP address of Syslog server IP from flash
 * Input:
 *      None
 * Output:
 *      plogServerIp - Pointer to IP Address  
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_serverIp_get(sal_ipaddr_t *plogServerIp);


/* Function Name:
 *      sal_flash_syslog_serverPort_set
 * Description:
 *      Set IP address of Syslog server port to flash
 * Input:
 *      logServerIp - Syslog Server port number 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_serverPort_set(uint16 logServerPort);


/* Function Name:
 *      sal_flash_syslog_serverPort_get
 * Description:
 *      Get IP address of Syslog server port from flash
 * Input:
 *      None
 * Output:
 *      plogServerIp - Pointer to port number  
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_serverPort_get(uint16 *plogServerPort);




/* Function Name:
 *      sal_flash_syslog_enable_set
 * Description:
 *      Set status of Syslog  to flash
 * Input:
 *      logServerIp - Syslog status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_state_set(sal_enable_t syslogStatus);


/* Function Name:
 *      sal_flash_syslog_enable_get
 * Description:
 *      Get status of Syslog from flash
 * Input:
 *      None
 * Output:
 *      pSyslogStatus - Pointer to syslog status  
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_state_get(sal_enable_t *pSyslogStatus);


extern int32 sal_flash_syslog_init();
extern int32 sal_flash_syslog_buf_init();

/* Function Name:
 *      sal_flash_syslog_buf_set
 * Description:
 *      Set Syslog buffer to flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_set(sal_flash_syslog_buf_entry_t *pSysLogBufEntry, uint16 index);

/* Function Name:
 *      sal_flash_syslog_buf_get
 * Description:
 *      Get Syslog buffer from flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_get(sal_flash_syslog_buf_entry_t *pSysLogBufEntry, uint16 index);


/* Function Name:
 *      sal_flash_syslog_buf_tail_index_set
 * Description:
 *      Set Syslog buffer tail Index to flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_tail_index_set(uint16 tailIndex);

/* Function Name:
 *      sal_flash_syslog_buf_tail_index_get
 * Description:
 *      Get Syslog buffer tail Index from flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_tail_index_get(uint16* pTailIndex);


/* Function Name:
 *      sal_flash_syslog_buf_head_index_set
 * Description:
 *      Set Syslog buffer free Index to flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_head_index_set(uint16 headIndex);

/* Function Name:
 *      sal_flash_syslog_buf_head_index_get
 * Description:
 *      Get Syslog buffer free Index from flash
 * Input:
 *      
 * Output:
 *      index
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_head_index_get(uint16* pHeadIndex);

#if 0
/* Function Name:
 *      sal_flash_syslog_buf_set_all
 * Description:
 *      Set Syslog buffer to flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_setAll(sal_flash_syslog_buf_t* pSysLogBuf);


/* Function Name:
 *      sal_flash_syslog_buf_getAll
 * Description:
 *      Set Syslog buffer to flash
 * Input:
 *      
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_syslog_buf_getAll(sal_flash_syslog_buf_t* pSysLogBuf);

#endif
extern int32 sal_flash_syslog_buf_init();


#endif


#if 0
/* Function Name:
 *      sal_flash_nvCfgMac_set
 * Description:
 *      Set MAC address to non-volatile configuration sector to flash
 * Input:
 *      pMac
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER  
 * Note:
 *      None
 */
extern int32 sal_flash_nvCfgMac_set(sal_mac_t *pMac);


/* Function Name:
 *      sal_flash_nvCfgMac_get
 * Description:
 *      Get MAC address from non-volatile configuration sector to flash
 * Input:
 *      None
 * Output:
 *      pMac
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_flash_nvCfgMac_get(sal_mac_t *pMac);


/* Function Name:
 *      sal_flash_userData_set
 * Description:
 *      Set user-defined data to non-volatile configuration sector to flash
 * Input:
 *      length - length of user-defined data. unit: byte.
 *      pData  - user-defined data.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NOT_SUPPORT  - length is too long
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_flash_userData_set(uint32 addr , uint16 length, uint8 *pData);


/* Function Name:
 *      sal_flash_userData_get
 * Description:
 *      Get user-defined data to non-volatile configuration sector from flash
 * Input:
 *      length - length of user-defined data. unit: byte.
 * Output:
 *      pData  - user-defined data.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NOT_SUPPORT  - length is too long
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      None
 */
extern int32 sal_flash_userData_get(uint32 addr , uint16 length, uint8 *pData);
extern int32 sal_flash_vlan_id_set(uint32 addr , uint16 length, uint8 *pData);
extern int32 sal_flash_vlan_id_get(uint32 addr , uint16 length, uint8 *pData);
#endif

/* Function Name:
 *      sal_flash_serial_number_set
 * Description:
 *      Set Serial to flash.
 * Input:
 *      serial - pointer to serial number
 *      length - serial number length
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_serial_number_set(char *serial, uint8 length);


/* Function Name:
 *      sal_flash_serial_number_get
 * Description:
 *      Get Serial from flash.
 * Input:
 *       None
 * Output:
 *      serial - pointer to serial number
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_serial_number_get(char *serial);

/* Function Name:
 *      sal_flash_device_type_set
 * Description:
 *      Set device type to flash.
 * Input:
 *      type - pointer to device type
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_device_type_set(char *type);

/* Function Name:
 *      sal_flash_device_type_get
 * Description:
 *      Get device type from flash.
 * Input:
 *       None
 * Output:
 *      type - pointer to device type
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_device_type_get(char *type);

/* Function Name:
 *      sal_flash_fw_version_set
 * Description:
 *      Set fw version to flash.
 * Input:
 *      type - pointer to fw version
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_fw_version_set(char *fwversion);


/* Function Name:
 *      sal_flash_fw_version_get
 * Description:
 *      Get fw version from flash.
 * Input:
 *       None
 * Output:
 *      type - pointer to fw version
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_fw_version_get(char *fwversion);

/* Function Name:
 *      sal_flash_hw_version_set
 * Description:
 *      Set hw version to flash.
 * Input:
 *      type - pointer to hw version
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_hw_version_set(char *hwversion);

/* Function Name:
 *      sal_flash_hw_version_get
 * Description:
 *      Get hw version from flash.
 * Input:
 *       None
 * Output:
 *      type - pointer to hw version
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_hw_version_get(char *hwversion);

/* Function Name:
 *      sal_flash_dummy_cfg_set
 * Description:
 *      Write configuration from buffer to dummy cache
 * Input:
 *      off    - Offset of dummy cache
 *      pData  - Pointer to buffer
 *      length - Buffer length
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER    - (length + addr) is out of range of config
 * Note:
 *      None
 */
extern int32 sal_flash_dummy_cfg_set(uint16 off, uint16 length, uint8* pData);
#endif/*__SAL_FLASH_H__*/

