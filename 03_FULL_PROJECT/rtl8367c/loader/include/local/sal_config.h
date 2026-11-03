#ifndef __SAL_CONFIG_H_
#define __SAL_CONFIG_H_

extern uint8 salMaxPhyPort;
extern uint8 salMaxExtPort;
extern uint8 salMaxStaticTrunk;
extern uint8 salMaxTrkPort;
extern uint8 salPortIdCpu;
//extern uint8 salPortSkipEnd;
//extern uint8 salPortSkipStart;

/* Feature */
#define SAL_ENABLE                      (1)
#define SAL_RTL8316D                    (0)
#define SAL_IGMP                        (0)
#define SAL_LOOP                        (0)
#define SAL_RLPP                        (0)
#define SAL_RRCPv1                      (0)
#define SAL_RRCPv3                      (0)
#define SAL_SERVICE_VLAN                (0)
#define SAL_SNTP                        (0)
#define SAL_RSTP                        (0)
#define SAL_EEE                         (1)
#define SAL_RTCT                        (0)
#define SAL_DSCP_REMARKING              (0)
#define SAL_GREENETH                    (0)
#define SAL_LLDP                        (0)
#define SAL_RUNTIME_LOADER              (1)
#define SAL_RUNTIME_MODE                (0)
#define SAL_LOADER_MODE                 (1)
#define SAL_SNMP                        (0)
#define SAL_SYSLOG                      (0)


/* L2 */
#define SAL_MAX_STATIC_MAC              32
#define SAL_MAX_MAC_SRAM_ENTRY          (8*1024)
#define SAL_MAX_MAC_CAM_ENTRY           64
#define SAL_MAX_MAC_HASH_INDEX          4096
#define SAL_MAX_MAC_HASH_WAY            4
#define SAL_WHOLE_SYSTEM                0xff

/* Mirror */
#define SAL_MAX_MIRROR_GROUP            1

#define SAL_MAX_PHY_PORT                (salMaxPhyPort)
#define SAL_MAX_EXT_PORT                (salMaxExtPort)
//#define SAL_PORT_SKIP_START             (salPortSkipStart)
//#define SAL_PORT_SKIP_END               (salPortSkipEnd)
#define SAL_MAX_STATIC_TRUNK            (salMaxStaticTrunk)
#define SAL_MAX_TRK_PORT                (salMaxTrkPort)
#define SAL_PORT_ID_CPU                 (salPortIdCpu)
#define SAL_MAX_CPU_PORT                1
#define SAL_MAX_PHY_N_CPU_PORT          (SAL_MAX_PHY_PORT + SAL_MAX_CPU_PORT)
#if defined(LWS_8G_2F)
#define SAL_MAX_PHY_N_CPU_COMBO_PORT          (18) //logic cpu port
#define SAL_MAX_LOGIC_PORT              (18)//(SAL_MAX_PHY_N_CPU_PORT + SAL_MAX_TRK_PORT)
#define SAL_MAX_LOGIC_COMBO_PORT              (18) //logic cpu port
#define SAL_MAX_STATIC_COMBO_TRUNK       3
#elif defined(LWS_5G_1F)  
#define SAL_MAX_PHY_N_CPU_COMBO_PORT          (18) //logic cpu port
#define SAL_MAX_LOGIC_PORT              (18)//(SAL_MAX_PHY_N_CPU_PORT + SAL_MAX_TRK_PORT)
#define SAL_MAX_LOGIC_COMBO_PORT              (18) //logic cpu port
#define SAL_MAX_STATIC_COMBO_TRUNK       3
#else
#define SAL_MAX_PHY_N_CPU_COMBO_PORT          (8 + 1)
#define SAL_MAX_LOGIC_PORT              (SAL_MAX_PHY_N_CPU_PORT + SAL_MAX_TRK_PORT)
#define SAL_MAX_LOGIC_COMBO_PORT              (11)
#define SAL_MAX_STATIC_COMBO_TRUNK       2
#endif

/* QoS */
#define SAL_MAX_NUM_OF_PRIORITY         8
#define SAL_MAX_NUM_OF_QUEUE            4
#define SAL_MAX_VALUE_OF_DSCP           64
#define SAL_MAX_WEIGHT_OF_PRIO_SELET    4

/* RRCPv3 */
#define SAL_RRCP_MAX_PKT_LENGTH         1518
#define SAL_RRCP_L2_HASH_WAY            2

/* Trunk */
#define SAL_MAX_PORTS_PER_TRK_DUMB      4
#define SAL_MAX_LOGIC_ID_LEN            8

/* VLAN */
#define SAL_MAX_NUM_VLAN                4094
#define SAL_VLAN_NAME_LEN_MAX           (20+1)
#define SAL_MAX_STATIC_VLAN             32
#define SAL_MAX_VLAN_NAME_LEN           15

/* SVLAN */
#define SAL_MAX_STATIC_SVLAN            8
#define SAL_MAX_SVLAN_NAME_LEN          15

/* Rate */
#define SAL_RATE_MIN                    8
#define SAL_RATE_MAX                    (0x1FFFE * 8)
#define SAL_RATE_DISABLE                (0x1FFFF * 8)
#define SAL_STRM_CTRL_BLOCK             0
#define SAL_STRM_CTRL_OFF               0xFFFFF
#define SAL_STRM_CTRL_MIN_RATE          STRM_CTRL_BLOCK
#define SAL_STRM_CTRL_MAX_RATE          STRM_CTRL_OFF
#define SAL_RATE_GRANULARITY_IN_KB      (8UL)
#define SAL_BWCTRL_DISABLE_THRES        (0x1FFFF * 8)

/* Spanning Tree */
#define SAL_MSTI_MAX                    16

/* STP */
#define SAL_MSTP_MSTI_MAX           16

/* NVCFG */
#define SAL_FLASH_NVDATA_LEN_MAX    256
#define SAL_FLASH_SERIAL_NUMBER_LEN_MAX    32


#endif /* end of __SAL_CONFIG_H__ */

