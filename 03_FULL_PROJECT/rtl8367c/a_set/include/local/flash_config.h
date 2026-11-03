#ifndef __FLASH_CONFIG_H__
#define __FLASH_CONFIG_H__

#include <local/config.h>
#include <common_types.h>
#include <local/sal_config.h>
#include <vlan.h>

#define FLASH_BASE 0x00000

/* ------------------ Light Web Smart Flash Layout --------------------- */
#define CONFIG_FLASH_SPI_04KB

#if defined(CONFIG_FLASH_SPI_64KB)
    #define BLOCK_SIZE (64UL *1024UL)
#elif defined(CONFIG_FLASH_SPI_04KB)
    #define BLOCK_SIZE (4UL * 1024UL)
#elif defined(CONFIG_FLASH_NOR_64KB)
    #define BLOCK_SIZE (64UL * 1024UL)
#endif

#define FLSH_CODE_SIZE          (FLASH_SIZE)

#define FLSH_BOOT_TOP           (1)
#define FLSH_BOOT_BOTTOM        (0)
#define FLSH_BOOT_NONE          (0)

/* address configuration of non-volitile configuration */
#define FLSH_NVDATA_START            (2032UL * 1024UL)
#define FLSH_NVDATA_MAC              FLSH_NVDATA_START
#define FLSH_NVDATA_SERIAL_NUMBER    FLSH_NVDATA_MAC + sizeof(sal_mac_t)
//#define FLSH_NVDATA_USER            FLSH_NVDATA_SERIAL_NUMBER + (SAL_FLASH_SERIAL_NUMBER_LEN_MAX)

/* address configuration of device type, hw version, sw version */
#define FLSH_INFO_START                 (2036UL * 1024UL)
#define FLSH_INFO_DEVICE_TYPE           FLSH_INFO_START
#define FLSH_INFO_FW_VERSION            FLSH_INFO_DEVICE_TYPE + (SAL_SYS_STRING_LEN)
#define FLSH_INFO_HW_VERSION           FLSH_INFO_FW_VERSION + (SAL_SYS_STRING_LEN)

#define FLSH_CONFIG_DUMMY_START  (2040UL * 1024UL)

#define FLSH_CONFIG_ADDR_START   (2044UL * 1024UL)

/* VLAN data config */
#define FLSH_ADDR_SIGNATURE     FLSH_CONFIG_ADDR_START
#define FLSH_ADDR_SYS               FLSH_ADDR_SIGNATURE + sizeof(sal_flash_beginMark_t)

#define FLSH_ADDR_USR           FLSH_ADDR_SYS + sizeof(sal_flash_sysInfo_t)
#define FLSH_ADDR_FWD		    FLSH_ADDR_USR + sizeof(sal_flash_account_t)
#define FLSH_ADDR_PORT		    FLSH_ADDR_FWD + sizeof(sal_flash_fwd_t)
#define FLSH_ADDR_MAC_TBL_STTC	FLSH_ADDR_PORT + sizeof(sal_flash_port_t)
#define FLSH_ADDR_TRK		    FLSH_ADDR_MAC_TBL_STTC + sizeof(sal_flash_l2_sttcEntry_t)*SAL_MAX_STATIC_MAC
#define FLSH_ADDR_VLAN		    FLSH_ADDR_TRK + sizeof(sal_flash_trunk_t)
#define FLSH_ADDR_VLAN_TMP	    FLSH_ADDR_VLAN + sizeof(sal_flash_vlan_t)
#define FLSH_ADDR_VLAN_TBL		FLSH_ADDR_VLAN_TMP + sizeof(sal_flash_vlanDb_t)
#define FLSH_ADDR_QOS_PORT_TBL  FLSH_ADDR_VLAN_TBL + sizeof(sal_flash_vlanEntry_t)*SAL_MAX_STATIC_VLAN
#define FLSH_ADDR_MARK		    FLSH_ADDR_QOS_PORT_TBL + sizeof(sal_flash_qos_port_table_t)

#if SAL_SERVICE_VLAN
  #define FLSH_ADDR_SVLAN_BEGIN     FLSH_ADDR_MARK + sizeof(sal_flash_dirtyMark_t)
  #define FLSH_ADDR_SVLAN_TMP       FLSH_ADDR_SVLAN_BEGIN
  #define FLSH_ADDR_SVLAN_TBL       FLSH_ADDR_SVLAN_TMP + sizeof(sal_flash_svlanDb_t)
  #define FLSH_ADDR_SVLAN_DFT       FLSH_ADDR_SVLAN_TBL + sizeof(sal_flash_svlanEntry_t)*SAL_MAX_STATIC_SVLAN
  #define FLSH_ADDR_SVLAN_TPID      FLSH_ADDR_SVLAN_DFT + sizeof(sal_vlan_t)
  #define FLSH_ADDR_SVLAN_UPLINK    FLSH_ADDR_SVLAN_TPID + sizeof(sal_vlan_t)
  #define FLSH_ADDR_SVLAN_END       FLSH_ADDR_SVLAN_UPLINK + sizeof(sal_pPortmask_t)
#else
  #define FLSH_ADDR_SVLAN_BEGIN     FLSH_ADDR_MARK + sizeof(sal_flash_dirtyMark_t)  
  #define FLSH_ADDR_SVLAN_END       FLSH_ADDR_SVLAN_BEGIN
#endif

#if SAL_LOOP
  #define FLSH_ADDR_LOOP_BEGIN      FLSH_ADDR_SVLAN_END
  #define FLSH_ADDR_LOOP_END        FLSH_ADDR_LOOP_BEGIN + sizeof(sal_flash_loop_t)    
#else
  #define FLSH_ADDR_LOOP_BEGIN      FLSH_ADDR_SVLAN_END
  #define FLSH_ADDR_LOOP_END        FLSH_ADDR_LOOP_BEGIN
#endif

#if SAL_RSTP
  #define FLSH_ADDR_RSTP_BEGIN      FLSH_ADDR_LOOP_END
  #define FLSH_ADDR_RSTP_END        FLSH_ADDR_RSTP_BEGIN  + sizeof(sal_flash_stp_t)       
#else
  #define FLSH_ADDR_RSTP_BEGIN      FLSH_ADDR_LOOP_END
  #define FLSH_ADDR_RSTP_END        FLSH_ADDR_RSTP_BEGIN
#endif

#if SAL_EEE
  #define FLSH_ADDR_EEE_BEGIN      FLSH_ADDR_RSTP_END
  #define FLSH_ADDR_EEE_END        FLSH_ADDR_EEE_BEGIN  + sizeof(sal_flash_eee_t)       
#else
  #define FLSH_ADDR_EEE_BEGIN      FLSH_ADDR_RSTP_END
  #define FLSH_ADDR_EEE_END        FLSH_ADDR_EEE_BEGIN
#endif

#if SAL_DSCP_REMARKING
#define FLSH_ADDR_DSCP_BEGIN       FLSH_ADDR_EEE_END
#define FLSH_ADDR_DSCP_END         FLSH_ADDR_DSCP_BEGIN + sizeof(sal_flash_qos_dscp_remarking_t)
#else
#define FLSH_ADDR_DSCP_BEGIN       FLSH_ADDR_EEE_END
#define FLSH_ADDR_DSCP_END         FLSH_ADDR_DSCP_BEGIN
#endif

#if SAL_GREENETH
#define FLSH_ADDR_GREENETH_BEGIN       FLSH_ADDR_DSCP_END
#define FLSH_ADDR_GREENETH_END         FLSH_ADDR_GREENETH_BEGIN + sizeof(sal_flash_greeneth_t)
#else
#define FLSH_ADDR_GREENETH_BEGIN       FLSH_ADDR_DSCP_END
#define FLSH_ADDR_GREENETH_END         FLSH_ADDR_GREENETH_BEGIN
#endif

#if SAL_LLDP
#define FLSH_ADDR_LLDP_BEGIN       FLSH_ADDR_GREENETH_END
#define FLSH_ADDR_LLDP_END         FLSH_ADDR_LLDP_BEGIN + sizeof(sal_flash_lldp_t)
#else
#define FLSH_ADDR_LLDP_BEGIN       FLSH_ADDR_GREENETH_END
#define FLSH_ADDR_LLDP_END         FLSH_ADDR_LLDP_BEGIN
#endif

#if SAL_RSTP
  #define FLSH_ADDR_RSTP_STATE_BEGIN    FLSH_ADDR_LLDP_END
  #define FLSH_ADDR_RSTP_STATE_END      FLSH_ADDR_RSTP_STATE_BEGIN  + sizeof(sal_enable_t)         
#else
  #define FLSH_ADDR_RSTP_STATE_BEGIN    FLSH_ADDR_LLDP_END
  #define FLSH_ADDR_RSTP_STATE_END      FLSH_ADDR_RSTP_STATE_BEGIN
#endif

#if SAL_IGMP
#define FLSH_ADDR_IGMP_BEGIN       FLSH_ADDR_RSTP_STATE_END
#define FLSH_ADDR_IGMP_END         FLSH_ADDR_IGMP_BEGIN + sizeof(sal_flash_igmp_t)
#else
#define FLSH_ADDR_IGMP_BEGIN       FLSH_ADDR_RSTP_STATE_END
#define FLSH_ADDR_IGMP_END         FLSH_ADDR_IGMP_BEGIN
#endif

#if defined(INTERNAL_PRIORITY)
#define FLSH_ADDR_QOS_TBL_BEGIN         FLSH_ADDR_IGMP_END
#define FLSH_ADDR_QOS_TBL_END		    FLSH_ADDR_QOS_TBL_BEGIN + sizeof(sal_flash_qos_table_t)
//#define FLSH_ADDR_END                   FLSH_ADDR_QOS_TBL_END  
#else
#define FLSH_ADDR_QOS_TBL_END		    FLSH_ADDR_IGMP_END
//#define FLSH_ADDR_END                   FLSH_ADDR_IGMP_END  
#endif

#if defined(SAL_SNMP)
#define FLSH_ADDR_SNMP_BEGIN            FLSH_ADDR_QOS_TBL_END
#define FLSH_ADDR_SNMP_END              FLSH_ADDR_SNMP_BEGIN + sizeof(sal_flash_snmp_t)    
#else
#define FLSH_ADDR_SNMP_END                   FLSH_ADDR_QOS_TBL_END
#endif

#if defined(SAL_SYSLOG)
#define FLSH_ADDR_SYSLOG_BEGIN            	FLSH_ADDR_SNMP_END
#define FLSH_ADDR_SYSLOG_BUF_HEAD_IDX_BEGIN    FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t)
#define FLSH_ADDR_SYSLOG_BUF_TAIL_IDX_BEGIN		FLSH_ADDR_SYSLOG_BUF_HEAD_IDX_BEGIN + sizeof(uint16)	
#define FLSH_ADDR_SYSLOG_BUF_BEGIN		FLSH_ADDR_SYSLOG_BUF_TAIL_IDX_BEGIN + sizeof(uint16)
#define FLSH_ADDR_SYSLOG_END              FLSH_ADDR_SYSLOG_BUF_HEAD_IDX_BEGIN + sizeof(sal_flash_syslog_buf_t)   
#define FLSH_ADDR_END						FLSH_ADDR_SYSLOG_END
#else
#define FLSH_ADDR_SYSLOG_END              FLSH_ADDR_SNMP_END    
#define FLSH_ADDR_END                   FLSH_ADDR_SNMP_END
#endif

/* address configuration of non-volitile configuration */
#define FLSH_NVDATA_START            (2032UL * 1024UL)
#define FLSH_NVDATA_MAC              FLSH_NVDATA_START
#define FLSH_NVDATA_SERIAL_NUMBER    FLSH_NVDATA_MAC + sizeof(sal_mac_t)

/* address configuration of device type, hw version, sw version */
#define FLSH_INFO_START                 (2036UL * 1024UL)
#define FLSH_INFO_DEVICE_TYPE           FLSH_INFO_START
#define FLSH_INFO_FW_VERSION            FLSH_INFO_DEVICE_TYPE + (SAL_SYS_STRING_LEN)
#define FLSH_INFO_HW_VERSION           FLSH_INFO_FW_VERSION + (SAL_SYS_STRING_LEN)

/* -------------------- System MAC Default Values ----------------------- */

/* -------------------- System IP Default Values ----------------------- */
#define DFLT_SYS_IP_DHCP              DISABLE
#define DFLT_SYS_IP_IP			          0xC0A80101 /*192.168.1.1*///
#define DFLT_SYS_IP_NETMASK		        0xFFFFFF00 /*255.255.255.0*/
#define DFLT_SYS_IP_GATEWAY		        0xC0A801FE /*192.168.1.254*/
#define DFLT_SYS_MAC_ADDRESS          {0x00, 0x23, 0x79, 0x00, 0x23, 0x79}

/* -------------------- Forwarding Default Values ---------------------- */
#define DFLT_FWD_INGRS_PREABMLE         0
#define DFLT_FWD_EGRS_PREAMBLE          0
#define DFLT_FWD_STRM_FLTR_RATE         0
#define DFLT_FWD_MIRROR_MODE            PORT_MIRROR_OFF
#define DFLT_FWD_MIRROR_PORT            0
#define DFLT_FWD_MAX_FRAME_SIZE         SAL_JUMBO_FRAME_16k
#define DFLT_FWD_CPU_EGRS_CTRL_EBL      ENABLE
#define DFLT_FWD_CPU_EGRS_RATE          64

/* -------------------- Trunking Default Values ------------------------ */
#define DFLT_TRK_DISTRIBUTION_ALGO      7
#define DFLT_TRK_HASH_MAPPING_TABLE_VAL 0

/* -------------------- VLAN Default Values ---------------------------- */

#define DFLT_VLAN_EBL                   ENABLE
#define DFLT_VLAN_PVID                  1
#define DFLT_VLAN_MODE                  VLAN_TAG_MODE_ORIGINAL
#define DFLT_VLAN_ACCEPT_TYPE           ACCEPT_FRAME_TYPE_ALL

/* -------------------- SVLAN default Values ---------------------------- */
#define DFLT_SVLAN_TPID                 0x88A8
#define DFLT_SVLAN_UPLINK_MSK           0
#define DFLT_SVLAN_PVID                 1

/* -------------------- QoS Default Values ---------------------------- */
/* Port to internal priority */
#define DFLT_QOS_PORT_TO_INTERNAL_PRI       8

/* Queue Number (Number of Queue) */
#define DFLT_QOS_QUEUE_NUMBER		    	MAX_LWS_QUEUE_NUM

/* -------------------- SNMP Default Values ---------------------------- */
#define DFLT_SNMP_ENABLE                    SAL_ENABLED
#define DFLT_SNMP_READ_COMMUNITY            "public"
#define DFLT_SNMP_WRITE_COMMUNITY           "private"
#define DFLT_SNMP_TRAP_IP                   (0xc0a8010a)
#define DFLT_SNMP_DESCRIPTION               "OPL SMART PDU RACK "
#define DFLT_SNMP_CONTACT                   "Network Administrator"
#define DFLT_SNMP_LOCATION                  "Office"
/* -------------------- Syslog Default Values ---------------------------- */
#define DFLT_SYSLOG_ENABLE                    SAL_ENABLED
#define DFLT_SYSLOG_SERVER_IP                 (0xc0a8010a)
#define DFLT_SYSLOG_SERVER_PORT                514
#define DFLT_SYSLOG_BUF_HEAD_IDX               0
#define DFLT_SYSLOG_BUF_TAIL_IDX               0

/* Scheduling */
#define DFLT_QOS_SCHED_ALGORITHM            SAL_WFQ
#define DFLT_QOS_SCHED_WEIGHT               0 // strict priority
#define DFLT_QOS_SCHED_QUEUE0_WEIGHT		1 

/* weight 0: Strict Priority */
#define DFLT_QOS_SCHED_QUEUE1_WEIGHT		2
#define DFLT_QOS_SCHED_QUEUE2_WEIGHT		3
#define DFLT_QOS_SCHED_QUEUE3_WEIGHT		4
#define DFLT_QOS_SCHED_QUEUE4_WEIGHT		5
#define DFLT_QOS_SCHED_QUEUE5_WEIGHT		6
#define DFLT_QOS_SCHED_QUEUE6_WEIGHT		7
#define DFLT_QOS_SCHED_QUEUE7_WEIGHT		8

/* -------------------- CFI info ---------------------------------------- */
#define FLSH_CFI_REGION_INFO_START      (0x58) 
#define FLSH_CFI_REGION_DATA_SIZE       8
#define FLSH_CFI_REGION_INFO_LEN        (FLSH_CFI_REGION_DATA_SIZE * 4 + 2)

/* -------------------- Port Default Values ---------------------------- */
#define DFLT_PORT_EBL                   ENABLE
#define DFLT_PORT_NEGO                  SAL_PORT_NEGO_AUTO
#define DFLT_PORT_10HALF                1
#define DFLT_PORT_10FULL                1
#define DFLT_PORT_100HALF               1
#define DFLT_PORT_100FULL               1
#define DFLT_PORT_1000FULL              1
#define DFLT_MEGA_PORT_SPEED            SAL_PORT_SPEED_100
#define DFLT_GIGA_PORT_SPEED            SAL_PORT_SPEED_1000
#define DFLT_PORT_DUPLEX                SAL_PORT_DUPLEX_FULL
#define DFLT_PORT_FLOW_CTRL             SAL_PORT_FLOW_CTRL_ENABLE
#define DFLT_PORT_RATE_INGRS            BWCTRL_DISABLE_THRES
#define DFLT_PORT_RATE_EGRS             BWCTRL_DISABLE_THRES
#define DFLT_PORT_ISOLATION_MASK        0x83FF

/* -------------------- User Name and Pass  --------------------------- */
#define DFLT_SYS_USERNAME               "admin"
#define DFLT_SYS_PASSWORD               "admin"

/* -------------------- FIRMWARE UPGRADE info --------------------------  */
#define UPGRADE_FLSH_CODE_SPACE_ADDR     0UL
#define UPGRADE_FLSH_CODE_SPACE_SIZE     FLSH_CODE_SIZE
#define UPGRADE_FLSH_COVER_SPACE_ADDR    0UL
#define UPGRADE_FLSH_COVER_SPACE_SIZE    0UL
#define UPGRADE_FLSH_BACKUP_SPACE_ADDR   (512UL * 1024UL)
#define UPGRADE_FLSH_BACKUP_SPACE_SIZE   FLSH_CODE_SIZE

#define UPGRADE_BIN_SIZE                 (UPGRADE_BIN_BACKUP_SPACE_SIZE + UPGRADE_BIN_COVER_SPACE_SIZE)
#define UPGRADE_BIN_BACKUP_SPACE_ADDR    UPGRADE_FLSH_CODE_SPACE_ADDR
#define UPGRADE_BIN_BACKUP_SPACE_SIZE    UPGRADE_FLSH_CODE_SPACE_SIZE
#define UPGRADE_BIN_COVER_SPACE_ADDR     (UPGRADE_BIN_BACKUP_SPACE_ADDR + UPGRADE_BIN_BACKUP_SPACE_SIZE)
#define UPGRADE_BIN_COVER_SPACE_SIZE     0UL

/* ----------------------- Spanning Tree ------------------------------  */
#if SAL_RSTP

#define DFLT_STP_VERSION                SAL_STP_VERSION_RSTP
#define DFLT_STP_PRI                    (32768)
#define DFLT_STP_AGE                    (20)
#define DFLT_STP_HELLO                  (2)
#define DFLT_STP_FWD                    (15)
#define DFLT_STP_PORT_EDGE              FALSE
#define DFLT_STP_PORT_P2P               SAL_STP_PORT_P2P_AUTO
#define DFLT_STP_PORT_PATHCOST          (0)
#define DFLT_STP_PORT_PRI               (0x80)

#endif 

/* ----------------------- RSTP ------------------------------  */
#if SAL_RSTP

#define DFLT_LLDP_ENABLE                SAL_DISABLED
#define DFLT_LLDP_TX_INTVL              8
#define DFLT_LLDP_HOLD_MULTI            4
#define DFLT_LLDP_PORT_STATE            SAL_LLDP_PORT_BOTH

#endif 


/* -------------------- Flash Driver Support --------------------------  */

#define CONFIG_SPI_PIO_READ    (1)
#define CONFIG_FLASH_PROTECT       \
	0x00000000UL, 0x00001000UL,    \
	0x00004000UL, 0x0001C000UL

#endif/*__LWS_DEFAULT_H__*/

