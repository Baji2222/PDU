
#ifndef __RRCPV3R_H__
#define __RRCPV3R_H__

#include <common_types.h>
#include <local/rtk_sal_rrcpv3r.h>

#define RRCPV3R_PORT_MIB_NUMBER  			SAL_RRCPV3R_PORT_MIB_NUMBER
#define RRCPV3R_PORT_ERRORNO				SAL_RRCPV3R_PORT_ERROR


typedef struct tlv_reg_s
{
    uint32 address;
    uint32 regData;
} tlv_reg_t;

typedef struct tlv_tbl_s
{
    uint8 tblType;
    uint16 index;
    uint8* tblData;
} tlv_tbl_t;

typedef struct rrcpv3_mib_counter_s
{
    uint8  subOID[5];
    uint8  mibData[8];
} rrcpv3_mib_counter_s;

typedef struct tlv_mib_portCounter_s
{
    uint8  portNumber;
	rrcpv3_mib_counter_s portMib[RRCPV3R_PORT_MIB_NUMBER];
} tlv_mib_portCounter_s;

typedef struct tlv_mib_s
{
    uint8  portNumber;
    uint8  subOID[5];
    uint8  mibData[8];
} tlv_mib_t;

typedef struct tlv_lut_s
{
    uint8  mac[6];
    uint16 fid;
    uint8  portNumber;
} tlv_lut_t;

typedef struct tlv_phy_s
{
    uint8  portNumber;
    uint8  page;
    uint8  address;
    uint16 phyData;
} tlv_phy_t;

typedef struct tlv_port_s
{
    uint8 portNumber;
    uint8 info;
} tlv_port_t;

typedef struct tlv_cfg_vlan_enable_s
{
    uint8 enable;
} tlv_cfg_vlan_enable_t;

typedef struct tlv_cfg_vlan_igrFilter_s
{
    uint8 enable;
    uint8 portNumber;
} tlv_cfg_vlan_igrFilter_t;

typedef struct tlv_cfg_vlan_port_s
{
    uint16 cfg;
    uint8  portNumber;
} tlv_cfg_vlan_port_t;

typedef struct tlv_cfg_vlan_name_s
{
    uint8  vlanName[16];
    uint16 vid;
} tlv_cfg_vlan_name_t;

typedef struct tlv_cfg_vlan_entry_s
{
    uint16 taggedPmsk[4];
    uint16 mbr[4];
    uint8  vlanName[16];
    uint16 vid;
    uint8  index;
} tlv_cfg_vlan_entry_t;

typedef union 
{
    tlv_cfg_vlan_enable_t    vlanEnable;
    tlv_cfg_vlan_igrFilter_t vlanIgrFilter;
    tlv_cfg_vlan_port_t      vlanPort;
    tlv_cfg_vlan_name_t      vlanName;
    tlv_cfg_vlan_entry_t     vlanEntry;
} tlv_cfg_vlan_t;

typedef struct tlv_cfg_qos_qnum_s
{
    uint8 queueNumber;
    uint8 portNumber;
} tlv_cfg_qos_qnum_t;

typedef union
{
    tlv_cfg_qos_qnum_t queueNumber;
} tlv_cfg_qos_t;

typedef struct tlv_cfg_trunk_s
{
    uint8  groupId;
    uint16 groupPmsk[4];
} tlv_cfg_trunk_t;

typedef union
{
    tlv_cfg_vlan_t  tlvCfgVlan;
    tlv_cfg_qos_t   tlvCfgQos;
    tlv_cfg_trunk_t tlvCfgTrunk;
} tlv_cfg_info_t;


typedef struct tlv_cfg_s
{
    uint8          functionType;
    tlv_cfg_info_t tlvCfgUnion;
} tlv_cfg_t;

typedef union
{
    tlv_reg_t  tlvReg;
    tlv_mib_t  tlvMib;
    tlv_lut_t  tlvLut;
    tlv_phy_t  tlvPhy;
    tlv_port_t tlvPort;
} tlv_type_union_t;

typedef struct rrcpv3_tlv_s
{
    uint8            type;
    uint8            length;
    uint8            subtype;
    tlv_type_union_t tlvUnion;
} rrcpv3_tlv_t;


typedef struct rrcpv3_header_s
{
	uint8			protocol;
	uint8   		typeCode;
	uint32   		frameLable;
    rrcpv3_tlv_t 	tlv;
} rrcpv3_header_t;



enum rrcp3_code_type_e
{
    RRCP3_CODE_TLV = 0,
    RRCP3_CODE_PATCH,
    RRCP3_CODE_END
};

enum rrcp3_patch_media_e
{
    RRCP3_MEDIA_EEPROM = 0,
    RRCP3_MEDIA_FLASH,
    RRCP3_MEDIA_8051_IROM,
    RRCP3_MEDIA_END
};

enum rrcp3_tlv_type_e
{
    RRCP3_TLV_END = 0,
    RRCP3_TLV_REG_ACCESS,
    RRCP3_TLV_TBL_ACCESS,
    RRCP3_TLV_MIB_ACCESS,
    RRCP3_TLV_LUT_ACCESS,
    RRCP3_TLV_PHY_ACCESS,
    RRCP3_TLV_PORT_PROPERTY_ACCESS,
    RRCP3_TLV_CONFIG_ACCESS,
    RRCP3_TLV_ERROR_CODE,
    RRCP3_TLV_MAX,
};

enum rrcp3_port_access_type_e
{
    RRCP3_PORT_STATE = 0,
    RRCP3_LINK_STATUS,
    RRCP3_NEGO_SETTING,
    RRCP3_SPEED_DUPLEX_STATUS,
    RRCP3_FLOW_CONTROL_SETTING,
    RRCP3_PHY_ABILITY,
    RRCP3_RLPP_STATUS,
    RRCP3_CABLE_TEST_STATUS,
    RRCP3_END
};

enum rrcp3_cfg_module_type_e
{
    RRCP3_CFG_VLAN = 0,
    RRCP3_CFG_QOS,
    RRCP3_CFG_MIRROR,
    RRCP3_CFG_TRUNK,
    RRCP3_CFG_JUMBO,
    RRCP3_CFG_BANDWIDTH_CTRL,
    RRCP3_CFG_STORM,
    RRCP3_CFG_SWITCH_INFO,
    RRCP3_CFG_END
};

enum rrcp3_func_vlan_setting_e
{
    RRCP3_FUNC_VLAN_ENABLE = 0,
    RRCP3_FUNC_VLAN_INGRESS_FILTER,
    RRCP3_FUNC_VLAN_PORT,
    RRCP3_FUNC_VLAN_NAME,
    RRCP3_FUNC_VLAN_STATIC_ENTRY,
    RRCP3_FUNC_VLAN_END
};

enum rrcp3_func_qos_setting_e
{
    RRCP3_FUNC_QOS_QUE_NUM = 0,
    RRCP3_FUNC_QOS_PORT_BASE_PRI,
    RRCP3_FUNC_QOS_1P_REMAPPING,
    RRCP3_FUNC_QOS_DSCP_REMAPPING,
    RRCP3_FUNC_QOS_PRI_SELECTION,
    RRCP3_FUNC_QOS_PRI_TO_QUE,
    RRCP3_FUNC_QOS_QUE_TO_WEIGHT,
    RRCP3_FUNC_QOS_PKT_SCHEDULE,
    RRCP3_FUNC_QOS_REMARKING_ENABLE,
    RRCP3_FUNC_QOS_REMARKING_PARAM,
    RRCP3_FUNC_QOS_REMARKING_END
};

enum rrcp3_func_qos_remapping_e
{
    RRCP3_FUNC_QOS_REMAPPING_8021P = 0,
    RRCP3_FUNC_QOS_REMAPPING_DSCP,
    RRCP3_FUNC_QOS_REMAPPING_END
};


enum rrcp3_func_bandwidth_ctrl_setting_e
{
    RRCP3_FUNC_BANDWIDTH_CTRL_SETTING_IFG = 0,
    RRCP3_FUNC_BANDWIDTH_CTRL_SETTING_BANDWIDTH,
    RRCP3_FUNC_BANDWIDTH_CTRL_SETTING_END
};

enum rrcp3_func_bandwidth_ctrl_e
{
    RRCP3_FUNC_BANDWIDTH_CTRL_INGRESS = 0,
    RRCP3_FUNC_BANDWIDTH_CTRL_EGRESS,
    RRCP3_FUNC_BANDWIDTH_CTRL_END
};

enum rrcp3_switch_info_e
{
    RRCP3_FUNC_PORT_NUM = 0,
    RRCP3_FUNC_RESET,
    RRCP3_FUNC_TRUST_PORTMASK,
    RRCP3_FUNC_SAVE_CFG,
    RRCP3_FUNC_RX_PORT,
    RRCP3_FUNC_END
};

enum rrcpv3r_lut_operation_e
{
    RRCPV3R_LUT_BLOCK_DA = 0,
    RRCPV3R_LUT_BLOCK_SA,    
    RRCPV3R_LUT_UNBLOCK_DA,
    RRCPV3R_LUT_UNBLOCK_SA,    
    RRCPV3R_LUT_RETRIVING,
    RRCPV3R_LUT_END
};

enum rrcp3_tbl_type_e
{
    RRCP3_TABLE_LUT = 0,
    RRCP3_TABLE_BCAM,
    RRCP3_TABLE_VLAN,
    RRCP3_TABLE_END
};

enum rrcp3_tbl_operation_e
{
    RRCP3_TBL_OP_READ = 0,
    RRCP3_TBL_OP_WRITE,    
    RRCP3_TBL_OP_DUMP,
    RRCP3_TBL_OP_CLEAR_STATIC,
    RRCP3_TBL_OP_CLEAR_DYNAMIC,
    RRCP3_TBL_OP_CLEAR_ALL,
    RRCP3_TBL_OP_CLEAR_END
};

enum rrcp3_mib_subtype_e
{
    RRCP3_MIB_SYBTYPE_READ = 0,
    RRCP3_MIB_SYBTYPE_RESET,    
    RRCP3_MIB_SYBTYPE_PORTMIB,    
    RRCP3_MIB_SYBTYPE_END    
};
enum rrcp3_reg_subtype_e
{
    RRCP3_REG_SYBTYPE_READ = 0,
    RRCP3_REG_SYBTYPE_WRITE,    
    RRCP3_REG_SYBTYPE_END    
};



#define RRCP3_TBL_LUT           0x0
#define RRCP3_TBL_BCAM          0x1
#define RRCP3_TBL_VLAN          0x10
#define RRCP3_TBL_VLAN_MC       0x10
#define RRCP3_TBL_ACL_RULE      0x20
#define RRCP3_TBL_ACL_ACT       0x21
#define RRCP3_TBL_ACL_TEMPLET   0x22
#define RRCP3_TBL_SVLAN_MC      0x30
#define RRCP3_TBL_C2S           0x31
#define RRCP3_TBL_MC2S          0x32
#define RRCP3_TBL_SP2C          0x33
#define RRCP3_TBL_HSA           0xF0
#define RRCP3_TBL_HSB           0xF1

extern int32 rrcpv3r_recv();
extern void  rrcpv3r_init();

#endif  /* __RRCPV3R_H__ */

