#ifndef __FLASH_CONFIG_H__
#define __FLASH_CONFIG_H__

/* -------------------- Flash Driver Support --------------------------  */
#define CONFIG_SPI_PIO_READ     (0)
#define FLASH_BLOCK_SIZE        (4*1024UL)
#define RUN_TIME_IROM_START     (4*1024UL)
#define RUN_TIME_BANK_START     ((16+96) * 1024UL)

/* Protect Area: (Addr_Start, Addr_End), (Addr_Start, Addr_End), ....*/
#define CONFIG_FLASH_PROTECT       \
	0x00000000UL, 0x00001000UL,    \
	0x00004000UL, 0x0001C000UL

/* following segment should be syncronized with flash_config.h in 6088_chip */
/* -------------------- Address Definitions ---------------------------- */

/* don't modify these address */
#define FLSH_CONFIG_ADDR_START  (2044UL * 1024UL)

/* VLAN data config */
#define FLSH_ADDR_SIGNATURE     FLSH_CONFIG_ADDR_START
#define FLSH_ADDR_SYS           FLSH_ADDR_SIGNATURE + sizeof(sal_flash_beginMark_t)

#define FLSH_ADDR_USR           FLSH_ADDR_SYS + sizeof(sal_flash_sysInfo_t)
#define FLSH_ADDR_FWD           FLSH_ADDR_USR + sizeof(sal_flash_account_t)
#define FLSH_ADDR_PORT          FLSH_ADDR_FWD + sizeof(sal_flash_fwd_t)
#define FLSH_ADDR_MAC_TBL_STTC  FLSH_ADDR_PORT + sizeof(sal_flash_port_t)
#define FLSH_ADDR_TRK           FLSH_ADDR_MAC_TBL_STTC + sizeof(sal_flash_l2_sttcEntry_t)*SAL_MAX_STATIC_MAC
#define FLSH_ADDR_VLAN          FLSH_ADDR_TRK + sizeof(sal_flash_trunk_t)
#define FLSH_ADDR_VLAN_TMP      FLSH_ADDR_VLAN + sizeof(sal_flash_vlan_t)
#define FLSH_ADDR_VLAN_TBL      FLSH_ADDR_VLAN_TMP + sizeof(sal_flash_vlanDb_t)
#define FLSH_ADDR_QOS_PORT_TBL  FLSH_ADDR_VLAN_TBL + sizeof(sal_flash_vlanEntry_t)*SAL_MAX_STATIC_VLAN
#define FLSH_ADDR_MARK          FLSH_ADDR_QOS_PORT_TBL + sizeof(sal_flash_qos_port_table_t)
#define FLSH_ADDR_END           FLSH_ADDR_MARK + sizeof(sal_flash_dirtyMark_t)

/* address configuration of non-volitile configuration */
#define FLSH_NVDATA_START            (2032UL * 1024UL)
#define FLSH_NVDATA_MAC              FLSH_NVDATA_START
#define FLSH_NVDATA_SERIAL_NUMBER    FLSH_NVDATA_MAC + sizeof(sal_mac_t)

/* address configuration of device type, hw version, sw version */
#define FLSH_INFO_START                 (2036UL * 1024UL)
#define FLSH_INFO_DEVICE_TYPE           FLSH_INFO_START
#define FLSH_INFO_FW_VERSION            FLSH_INFO_DEVICE_TYPE + (SAL_SYS_STRING_LEN)
#define FLSH_INFO_HW_VERSION           FLSH_INFO_FW_VERSION + (SAL_SYS_STRING_LEN)

#define FLSH_CONFIG_DUMMY_START  (2040UL * 1024UL)

/* default value setting */
#define DFLT_SYS_IP_IP                  {192, 168, 1, 1}
#define DFLT_SYS_IP_NETMASK             {255, 255, 255,0}
#define DFLT_SYS_IP_GATEWAY             {192, 168, 1, 254}
#define DFLT_SYS_MAC_ADDRESS            {0x00, 0x23, 0x79, 0x00, 0x23, 0x79}

#endif/*__FLASH_CFG_H__*/

