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
 * Feature : System related
 *
 */
#ifndef __SAL_SYS_H__
#define __SAL_SYS_H__

#include <saltype.h>

typedef enum sal_sys_boot_mode_e
{
    SAL_SYS_BOOT_MODE_COLD = 0,
    SAL_SYS_BOOT_MODE_WARM,
    SAL_SYS_BOOT_MODE_END
} sal_sys_boot_mode_t;


/* Function Name:
 *      sal_flash_config_init
 * Description:
 *      Initialize the configurations from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_flash_config_init(void);


/* Function Name:
 *      sal_flash_config_restoreAll
 * Description:
 *      Restore the configurations of all modules from flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_flash_config_restoreAll(void);


/* Function Name:
 *      sal_sys_config_restore
 * Description:
 *      Restore the configurations of the system module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_sys_config_restore(void);


/* Function Name:
 *      sal_sys_mac_set
 * Description:
 *      Set MAC Address of the NIC.
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
extern int32 sal_sys_mac_set(sal_mac_t *pMac);

/* Function Name:
 *      sal_sys_mac_set_cache
 * Description:
 *      Set MAC Address of the NIC.
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
extern int32 sal_sys_mac_set_cache(sal_mac_t *pMac);


/* Function Name:
 *      sal_sys_mac_get
 * Description:
 *      Get MAC Address of the NIC.
 * Input:
 *      None
 * Output:
 *      pMac - pointer buffer of the MAC address
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_PARAMETER    - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_mac_get(sal_mac_t *pMac);


/* Function Name:
 *      sal_sys_dhcp_set
 * Description:
 *      Set the DHCP enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sys_dhcp_set(sal_enable_t enable);

/* Function Name:
 *      sal_sys_dhcp_get
 * Description:
 *      Get the DHCP enable status.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sys_dhcp_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_sys_ip_set
 * Description:
 *      Set IP address of the system.
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
extern int32 sal_sys_ip_set(sal_ipaddr_t ip, sal_ipaddr_t netmask);


/* Function Name:
 *      sal_sys_ip_get
 * Description:
 *      Get IP address of the system.
 * Input:
 *      None
 * Output:
 *      pIp      - pointer buffer of the IP address
 *      pNetmask - pointer buffer of the netmask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sys_ip_get(sal_ipaddr_t *pIp, sal_ipaddr_t *pNetmask);


/* Function Name:
 *      sal_sys_gateway_set
 * Description:
 *      Set IP address of the gateway of the system.
 * Input:
 *      gateway - IP address
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sys_gateway_set(sal_ipaddr_t gateway);


/* Function Name:
 *      sal_sys_gateway_get
 * Description:
 *      Get IP address of the gateway of the system.
 * Input:
 *      None
 * Output:
 *      pGateway - pointer buffer of the IP address of the gateway
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sys_gateway_get(sal_ipaddr_t *pGateway);


/* Function Name:
 *      sal_sys_chipVersion_get
 * Description:
 *      Get the version information of the chip.
 * Input:
 *      None
 * Output:
 *      pChipId  - pointer buffer of the chip id
 *      pVersion - pointer buffer of the chip version
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_PARAMETER    - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_chipVersion_get(uint8 *pChipId, uint8 *pVersion);


/* Function Name:
 *      sal_sys_firmVersion_get
 * Description:
 *      Get the version information of the firmware.
 * Input:
 *      None
 * Output:
 *      pVersion - pointer buffer of the firmware version
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sys_firmVersion_get(uint8 *pVersion);


/* Function Name:
 *      sal_sys_firmVersion_set
 * Description:
 *      Set the version information of the firmware.
 * Input:
 *      pVersion - pointer buffer of the firmware version
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pVersion is too long.
 * Note:
 *      None
 */
extern int32 sal_sys_firmVersion_set(uint8 *pVersion);


/* Function Name:
 *      sal_sys_firmVersion_set_cache
 * Description:
 *      Set the version information of the firmware.
 * Input:
 *      pVersion - pointer buffer of the firmware version
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pVersion is too long.
 * Note:
 *      None
 */
extern int32 sal_sys_firmVersion_set_cache(uint8 *pVersion);


/* Function Name:
 *      sal_sys_hardwareVersion_get
 * Description:
 *      Get the version information of the hardware.
 * Input:
 *      None
 * Output:
 *      pVersion - pointer buffer of the firmware version
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sys_hardwareVersion_get(uint8 *pVersion);


/* Function Name:
 *      sal_sys_hardwareVersion_set
 * Description:
 *      Set the version information of the hardware.
 * Input:
 *      pVersion - pointer buffer of the firmware version 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pVersion is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_hardwareVersion_set(uint8 *pVersion);


/* Function Name:
 *      sal_sys_hardwareVersion_set_cache
 * Description:
 *      Set the version information of the hardware.
 * Input:
 *      pVersion - pointer buffer of the firmware version 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pVersion is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_hardwareVersion_set_cache(uint8 *pVersion);


/* Function Name:
 *      sal_sys_firmDate_get
 * Description:
 *      Get the date information of the firmware.
 * Input:
 *      None
 * Output:
 *      pDate - pointer buffer of the firmware version
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sys_firmDate_get(uint8 *pDate);


/* Function Name:
 *      sal_sys_firmDate_set
 * Description:
 *      Set the date information of the firmware.
 * Input:
 *      pDate - pointer buffer of the firmware version 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDate is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_firmDate_set(uint8 *pDate);

/* Function Name:
 *      sal_sys_dev_type_get
 * Description:
 *      Get description of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_dev_type_get(uint8 *pDesc);

/* Function Name:
 *      sal_sys_dev_type_set
 * Description:
 *      Set description of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_dev_type_set(uint8 *pDesc);

/* Function Name:
 *      sal_sys_dev_type_set_cache
 * Description:
 *      Set description of this switch to cache
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_dev_type_set_cache(uint8 *pDesc);

/* Function Name:
 *      sal_sys_rdonly_community_get
 * Description:
 *      Get read only community of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_rdonly_community_get(uint8 *pDesc);


/* Function Name:
 *      sal_sys_rdonly_community_set
 * Description:
 *      Set read only community of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 *      setcache - Flag to set the cache for saving the data to flash
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_rdonly_community_set(uint8 *pDesc, bool setCache);


/* Function Name:
 *      sal_sys_rdonly_community_set_cache
 * Description:
 *      Set read only community of this switch to cache
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_rdonly_community_set_cache(uint8 *pDesc);



/* Function Name:
 *      sal_sys_rdwr_community_get
 * Description:
 *      Get read write community of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_rdwr_community_get(uint8 *pDesc);


/* Function Name:
 *      sal_sys_rdwr_community_set
 * Description:
 *      Set read write community of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 *      setcache - Flag to set the cache for saving the data to flash
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_rdwr_community_set(uint8 *pDesc, bool setCache);


/* Function Name:
 *      sal_sys_rdwr_community_set_cache
 * Description:
 *      Set read write community of this switch to cache
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_rdwr_community_set_cache(uint8 *pDesc);


/* Function Name:
 *      sal_sys_description_get
 * Description:
 *      Get description of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_description_get(uint8 *pDesc);

/* Function Name:
 *      sal_sys_description_set
 * Description:
 *      Set description of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 *      setcache - Flag to set the cache for saving the data to flash
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_description_set(uint8 *pDesc, bool setCache);

/* Function Name:
 *      sal_sys_description_set_cache
 * Description:
 *      Set description of this switch to cache
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the description
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_description_set_cache(uint8 *pDesc);

/* Function Name:
 *      sal_sys_contact_get
 * Description:
 *      Get contact of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the contact
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_contact_get(uint8 *pDesc);

/* Function Name:
 *      sal_sys_contact_set
 * Description:
 *      Set contact of this switch
 * Input:
 *      pDesc - Pointer to the contact
 *      setcache - Flag to set the cache for saving the data to flash
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_contact_set(uint8 *pDesc, bool setCache);

/* Function Name:
 *      sal_sys_contact_set_cache
 * Description:
 *      Set contact of this switch to cache
 * Input:
 *      pDesc - Pointer to the contact
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_contact_set_cache(uint8 *pDesc);

/* Function Name:
 *      sal_sys_location_get
 * Description:
 *      Get location of this switch
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the location
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_sys_location_get(uint8 *pDesc);

/* Function Name:
 *      sal_sys_location_set
 * Description:
 *      Set location of this switch
 * Input:
 *      pDesc - Pointer to the location
 *      setcache - Flag to set the cache for saving the data to flash
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_location_set(uint8 *pDesc, bool setCache);

/* Function Name:
 *      sal_sys_location_set_cache
 * Description:
 *      Set location of this switch to cache
 * Input:
 *      pDesc - Pointer to the location
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pDesc is too long. 
 * Note:
 *      None
 */
extern int32 sal_sys_location_set_cache(uint8 *pDesc);

extern int32 sal_sys_syslog_state_get(sal_enable_t *pState);

extern int32 sal_sys_syslog_state_set(sal_enable_t *pState, bool setCache);

extern int32 sal_sys_syslog_state_set_cache(sal_enable_t *pState);

extern int32 sal_sys_syslog_hostIp_get(sal_ipaddr_t *pIp);

extern int32 sal_sys_syslog_hostIp_set(sal_ipaddr_t *pIp, bool setCache);

extern int32 sal_sys_syslog_hostIp_set_cache(sal_ipaddr_t *pIp);

extern int32 sal_sys_syslog_hostUdpPort_get(uint16 *pPort);

extern int32 sal_sys_syslog_hostUdpPort_set(uint16 *pPort, bool setCache);

extern int32 sal_sys_syslog_hostUdpPort_set_cache(uint16 *pPort);

extern int32 sal_sys_syslog_headIdx_get(uint16 *pIdx);

extern int32 sal_sys_syslog_headIdx_set(uint16 *pIdx, bool setCache);

extern int32 sal_sys_syslog_headIdx_set_cache(uint16 *pIdx);

extern int32 sal_sys_syslog_tailIdx_get(uint16 *pIdx);

extern int32 sal_sys_syslog_tailIdx_set(uint16 *pIdx, bool setCache);

extern int32 sal_sys_syslog_tailIdx_set_cache(uint16 *pIdx);

/* Function Name:
 *      sal_sys_regBit_set
 * Description:
 *      Set register value by bit.
 * Input:
 *      addr  - register address
 *      bit   - bit number
 *      value - register value
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_regBit_set(uint32 addr, uint32 bit_idx, uint32 value);


/* Function Name:
 *      sal_sys_regBits_set
 * Description:
 *      Set register value by bitmask.
 * Input:
 *      addr  - register address
 *      bits  - bitmask
 *      value - register value
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_regBits_set(uint32 addr, uint32 bits, uint32 value);


/* Function Name:
 *      sal_sys_reg_set
 * Description:
 *      Set register of the chip.
 * Input:
 *      addr  - register address
 *      value - register value
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_reg_set(uint32 addr, uint32 value);


/* Function Name:
 *      sal_sys_reg_get
 * Description:
 *      Get register of the chip.
 * Input:
 *      None
 * Output:
 *      pAddr  - pointer buffer of the register address
 *      pValue - pointer buffer of the register value
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER - invalid parameter
 * Note:
 *      None
 */
extern int32 sal_sys_reg_get(uint32 addr, uint32 *pValue);


/* Function Name:
 *      sal_sys_userAccount_set
 * Description:
 *      Set username and password of system.
 * Input:
 *      pUsername, pPassword
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_userAccount_set(uint8 *pUsername, uint8 *pPassword);


/* Function Name:
 *      sal_sys_userAccount_get
 * Description:
 *      Get username and password of system.
 * Input:
 *      pUsername, pPassword
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_userAccount_get(uint8 *pUsername, uint8 *pPassword);


/* Function Name:
 *      sal_sys_system_reboot
 * Description:
 *      Reboot the system.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_system_reboot(void);


#if SAL_RUNTIME_LOADER
#if SAL_RUNTIME_MODE

/* Function Name:
 *      sal_sys_loaderMode_set
 * Description:
 *      Enter loader mode. Note that after this function is called,
 *      the system will switched from runtime mode to loader mode.
 *      This function is only available in runtime mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_loaderMode_set();
#endif

#if SAL_RUNTIME_MODE

/* Function Name:
 *      sal_sys_runtimeMode_set
 * Description:
 *      Enter runtime mode. Note that after this function is called,
 *      the system will switched from loader mode to runtime mode.
 *      This function is only available in loader mode.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_runtimeMode_set();

#endif
#endif

/* Function Name:
 *      sal_sys_fw_size_get
 * Description:
 *      Get firmware size.
 * Input:
 *      None
 * Output:
 *      size - pointer to firmware size
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sys_fw_size_get(uint32 *size);


#endif/* __SAL_SYS_H__ */

	
