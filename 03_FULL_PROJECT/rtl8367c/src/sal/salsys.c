#pragma STRING (far)

#include <local/config.h>
#include <salsys.h>
#include <salflash.h>
#include <salmirror.h>
#include <salrate.h>
#include <sall2.h>
#include <salqos.h>
#include <salvlan.h>
#include <salsvlan.h>
#include <saltrunk.h>
#include <salloop.h>
#include <saleee.h>
#include <salgreeneth.h>
#include <salsnmp.h>
#include <salsyslog.h>
#if SAL_RSTP
#include <salstp.h>
#endif
#include <stdio.h>
#include <string.h>
#include <salerror.h>
#include <dhcpc/dhcpc.h>
#include <rtl8367c_asicdrv.h>
#include <image.h>
#include <SPIFlashDrv.h>
#include <runtime_loader_msg.h>
#include <salflash.h>

#include <snmp/mib/mib.h>

#if SAL_IGMP
#include <saligmp.h>
#endif
#include <flash_api.h>
#if SAL_ENABLE

extern uint8     xdata this_mac[];
extern uint8     xdata this_ip[];   
extern uint8     xdata netmask[];
extern ip_addr_t xdata default_gateway;
extern uint8     xdata dhcp_enable;
extern uint8     xdata firmVer[];
extern uint8     xdata firmDate[];
extern uint8     xdata chipVer[];
extern uint8     xdata chipId[];

extern uint16 getReg(uint16 addr);
extern void setReg(uint16 addr, uint16 value);

static uint8 updateFlag;
#define SNMP_COMMUNITY_STR_LEN 16 //temp charan TBD

static char rdonlycomm[SNMP_COMMUNITY_STR_LEN];
static char rdwrcomm[SNMP_COMMUNITY_STR_LEN];
static char sysDescr[SAL_SYS_STRING_LEN];
static char sysDescription[SAL_SNMP_SYS_INFO_STR_LEN];
static char sysLocation[SAL_SNMP_SYS_INFO_STR_LEN];
static char sysContact[SAL_SNMP_SYS_INFO_STR_LEN];
static char fwVer[SAL_SYS_STRING_LEN];
static char hwVer[SAL_SYS_STRING_LEN];
static char fwDate[SAL_SYS_STRING_LEN];
static uint16 syslogHostUdpPort;
static sal_ipaddr_t syslogHostIp;
static sal_enable_t syslogState;
static uint16 syslogHeadIdx;
static uint16 syslogTailIdx;

#define DBG_PRINT printf
//#define FLASH_WRITE(start_addr, buf, len)   spi_flash_write(start_addr, buf, len)
#define FLASH_MAGICNUMBER_ADDR              ((RUNTIME_HEADER_BANK - 1) * BANK_SIZE + IROM_SIZE + (RUNTIME_HEADER_ADDRESS & 0xFFFF) - IROM_SIZE)
#define LOADR_START_ADDR                    0

typedef void (*loader_entry_t)(void); 

extern uint16 r_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR);
extern void w_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR,uint16 Odata) ;

// Sal Flash Config Init: Initializes flash configuration by checking initialization marker
// Restores all configuration from flash if valid, or initializes default config if not
int32 sal_flash_config_init(void)
{
    int32  ret;
    uint32 ulMark;

    ulMark = FLSH_MARK_INIT_NO;

    SalFlshAccessCache(FALSE); /*access flash*/
    
    SalFlshGetInitOKMark(&ulMark);

    SalFlshAccessCache(TRUE); /*change back to access cache*/
      
    if (ulMark != FLSH_MARK_INIT_OK) 
    {
        /* No config data(first bootup) or incorrect config data, 
           initial flash with a default configuration*/
        DBG_PRINT("SalFlshInit()\n");
        if ((ret = SalFlshInit()) != SAL_ERR_SUCCESS)
        {
            DBG_PRINT("SalFlshInit() failed!!!");
            return ret;
        }
    }
    else
    {
        /*copy flash's config to cache for later restore*/
        DBG_PRINT("SalFlshCopyFlshToCache()\n");        
        SalFlshCopyFlshToCache();
    }   
    
    sal_flash_config_restoreAll();

    return SAL_ERR_SUCCESS;
}

// Sal Flash Config Restore All: Restores all system configurations from flash
// Calls individual restore functions for each subsystem (green, sys, trunk, mirror, etc.)
void
sal_flash_config_restoreAll(void)
{
//    sal_enable_t state;
#if 0
    {
    uint32 rate;
    sal_rate_stormControl_get(0, SAL_STORM_TYPE_BCAST, &rate);
    printf("ker  -  %lu\n", rate);
    }
#endif
#if SAL_GREENETH
        DBG_PRINT("sal_green_restore()");
        sal_greenEth_config_restore();
        DBG_PRINT("...OK\n");
#endif

    DBG_PRINT("sal_sys_config_restore()");
    sal_sys_config_restore();
    DBG_PRINT("...OK\n");

    DBG_PRINT("sal_trunk_config_restore()");
    sal_trunk_config_restore();
    DBG_PRINT("...OK\n");       

    DBG_PRINT("sal_mirror_config_restore()");
    sal_mirror_config_restore();
    DBG_PRINT("...OK\n");
 
    DBG_PRINT("sal_qos_config_restore()");
    sal_qos_config_restore();
    DBG_PRINT("...OK\n");   

    DBG_PRINT("sal_vlan_config_restore()");
    sal_vlan_config_restore();
    DBG_PRINT("...OK\n");

#if SAL_SERVICE_VLAN
    DBG_PRINT("sal_svlan_config_restore()");
    sal_svlan_config_restore();
    DBG_PRINT("...OK\n");
#endif

#if 0
{
uint32 rate;
sal_rate_stormControl_get(0, SAL_STORM_TYPE_BCAST, &rate);
printf("ker  -  %lu\n", rate);
}
#endif
 
    DBG_PRINT("sal_rate_config_restore()");
    sal_rate_config_restore();
    DBG_PRINT("...OK\n");
 
    DBG_PRINT("sal_port_config_restore()");
    sal_port_config_restore();
    DBG_PRINT("...OK\n"); 

    DBG_PRINT("sal_trunk_config_restore()");
    sal_trunk_config_restore();
    DBG_PRINT("...OK\n");    

    DBG_PRINT("sal_l2_config_restore()");
    sal_l2_config_restore();
    DBG_PRINT("...OK\n");
  
#if SAL_LOOP
    DBG_PRINT("sal_loop_config_restore()");
    sal_loop_config_restore();
    DBG_PRINT("...OK\n");
#endif

#if SAL_EEE
    DBG_PRINT("sal_eee_restore()");
    sal_eee_config_restore();
    DBG_PRINT("...OK\n");
#endif


#if SAL_RSTP
    DBG_PRINT("sal_stp_config_restore()");
    sal_stp_config_restore();
    DBG_PRINT("...OK\n");
#endif

#if SAL_SNMP
    DBG_PRINT("sal_snmp_restore()");
    sal_snmp_config_restore();
    DBG_PRINT("...OK\n");
#endif

#if SAL_SYSLOG
	DBG_PRINT("sal_syslog_restore()");
	sal_syslog_config_restore();
	DBG_PRINT("...OK\n");
#endif
#if SAL_IGMP
    DBG_PRINT("sal_igmp_restore()");
    sal_igmp_config_restore();
    DBG_PRINT("...OK\n");
#endif

}

extern int run_command (const char *cmd, int flag);

// Sal Sys Config Restore: Restores system configuration from flash
// Sets MAC, IP, gateway, DHCP, SNMP, syslog, and version information
void
sal_sys_config_restore ()
{
    sal_ipaddr_t ip;
    sal_ipaddr_t netmask;
    sal_ipaddr_t gateway;
    sal_enable_t bEbl;
    sal_mac_t sysMac;
#ifdef __BOOT_COMMAND__
    int8 bootCmd[FLSH_SYS_BOOTCMD_LEN]="";
#endif
    uint8 firmware_version[SAL_SYS_STRING_LEN];
    uint8 hardware_version[SAL_SYS_STRING_LEN];
    uint8 device_type[SAL_SYS_STRING_LEN];
    int32 ret;

    /* [TODO] - Change the firmware version and date for new releases  */ 
	uint8 firmVer[] = "V2.5";
	uint8 hdrVer[] = "20180924";
	uint8 devDesc[32] = "OPL SMART PDU RACK";
	uint8 devLocation[32] = "Default Location";
	uint8 devContact[32] = "Default Contact";
	uint8 snmprdcomm[16] = "public";	
	uint8 snmprdwrcomm[16] = "private";
	uint16 syslogHostPort;
	sal_ipaddr_t syslogHostIpAddr;
	uint16 syslogHdIdx;
	uint16 syslogTlIdx;
	sal_enable_t syslogstate;
	
    sal_flash_sysMac_get(&sysMac);
    #if 1
		if(((sysMac.octet[0] == 0xff) && (sysMac.octet[1] == 0xff) && (sysMac.octet[2] == 0xff) && 
    (sysMac.octet[3] == 0xff) && (sysMac.octet[4] == 0xff) && (sysMac.octet[5] == 0xff)) ||
    ((sysMac.octet[0] == 0) && (sysMac.octet[1] == 0) && (sysMac.octet[2] == 0) && 
    (sysMac.octet[3] == 0) && (sysMac.octet[4] == 0) && (sysMac.octet[5] == 0)) ||
		((sysMac.octet[0] == 0) && (sysMac.octet[1] == 0x23) && (sysMac.octet[2] == 0x79) && 
    (sysMac.octet[3] == 0) && (sysMac.octet[4] == 0x23) && (sysMac.octet[5] == 0x79))
    )
#endif 
    {
	
			#if 0
        sysMac.octet[5] = 0xA4;
        sysMac.octet[4] = 0xDA;
        sysMac.octet[3] = 0x22;
        sysMac.octet[2] = 0x50;
        sysMac.octet[1] = 0x20;
        sysMac.octet[0] = 0x01;
			#endif 

 	  		sysMac.octet[0] = 0xA4;
        sysMac.octet[1] = 0xDA;
        sysMac.octet[2] = 0x22;
        sysMac.octet[3] = 0x50;
        sysMac.octet[4] = 0x22;
			  sysMac.octet[5] = 0xE3;

    }    
    sal_sys_mac_set_cache(&sysMac);
		sal_flash_sysMac_set(&sysMac);
    sal_flash_sysIp_get(&ip, &netmask);
    if((ip == 0) || (ip == 0xffffffff))
    {
        ip = 0xC0A80101; /* 192.168.1.1 */
        netmask = 0xFFFFFF00; /*255.255.255.0*/
    }    
    sal_sys_ip_set(ip, netmask);
    sal_flash_sysGateway_get(&gateway);
    sal_sys_gateway_set(gateway);
    sal_flash_sysDhcp_get(&bEbl);
    ret = sal_sys_dhcp_set(bEbl);
    if(ret != SAL_ERR_SUCCESS)
    {
        sal_sys_dhcp_set(SAL_DISABLED);
    }
#ifdef __BOOT_COMMAND__
    SalFlshSysGetBootCmd(bootCmd);
    DBG_PRINT("Run bootcmd '%s'\n", bootCmd);
    setenv("bootcmd", bootCmd);
    run_command (bootCmd, 0);
#endif

    sal_flash_device_type_get(device_type);
    printf("\nDevice type: %s\n",device_type);
    sal_sys_dev_type_set_cache(device_type);
    
    sal_sys_firmDate_set(firmDate);
    
	sal_sys_firmVersion_set(firmVer);
    //sal_sys_firmDate_set(firmDate);
    sal_sys_hardwareVersion_set(hdrVer);


	/* set snmp params */
    sal_flash_sys_description_get(devDesc);

    /** Dont set the cache during restore  */ 
    sal_sys_description_set(devDesc, 0);
	    
    sal_flash_sys_contact_get(devContact);
    /** Dont set the cache during restore  */ 
    sal_sys_contact_set(devContact, 0);

    sal_flash_sys_location_get(devLocation);
    /** Dont set the cache during restore  */ 
    sal_sys_location_set(devLocation, 0);
    
    sal_flash_snmp_community_get(snmprdcomm, snmprdwrcomm);
    /** Dont set the cache during restore  */ 
    sal_sys_rdonly_community_set(snmprdcomm, 0);
    sal_sys_rdwr_community_set(snmprdwrcomm, 0);

/* set syslog params */
#if 1
	sal_flash_syslog_buf_head_index_get(&syslogHdIdx);
	sal_sys_syslog_headIdx_set(&syslogHdIdx, 0);
	
	sal_flash_syslog_buf_tail_index_get(&syslogTlIdx);
	sal_sys_syslog_tailIdx_set(&syslogTlIdx, 0);

	sal_flash_syslog_state_get(&syslogstate);
	sal_sys_syslog_state_set(&syslogstate, 0);
	
	sal_flash_syslog_serverIp_get(&syslogHostIpAddr);
	sal_sys_syslog_hostIp_set(&syslogHostIpAddr, 0);
	
	sal_flash_syslog_serverPort_get(&syslogHostPort);
	sal_sys_syslog_hostUdpPort_set(&syslogHostPort, 0);
#endif

    sal_flash_fw_version_get(firmware_version);
    sal_sys_firmVersion_set_cache(firmware_version);

    sal_flash_hw_version_get(hardware_version);
    sal_sys_hardwareVersion_set_cache(hardware_version);
}

/* Function Name:
 *      sal_sys_mac_set
 * Description:
 *      Set MAC Address of the NIC.
 * Input:
 *      pMac - pointer buffer of the MAC address
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys MAC Set: Sets MAC address and writes to flash
// Updates system MAC and persists to flash configuration
int32 sal_sys_mac_set (
    sal_mac_t *pSysMac
    )
{
    if(pSysMac == NULL)
        return SAL_ERR_PARAMETER;

    memcpy(this_mac, pSysMac->octet, SAL_ETHER_ADDR_LEN);

    sal_flash_sysMac_set(pSysMac);
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_mac_set_cache
 * Description:
 *      Set MAC Address of the NIC(not write to flash).
 * Input:
 *      pMac - pointer buffer of the MAC address
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys MAC Set Cache: Sets MAC address in cache without writing to flash
// Updates system MAC in memory only (for temporary changes)
int32 sal_sys_mac_set_cache (
    sal_mac_t *pSysMac
    )
{
    if(pSysMac == NULL)
        return SAL_ERR_PARAMETER;

    memcpy(this_mac, pSysMac->octet, SAL_ETHER_ADDR_LEN);

    return SAL_ERR_SUCCESS;
}

// Sal Sys MAC Get: Retrieves current MAC address
// Copies system MAC address to output buffer
int32
sal_sys_mac_get (
    sal_mac_t *pSysMac
    )
{
    if(pSysMac == NULL)
        return SAL_ERR_NULL_POINTER;

    memcpy(pSysMac->octet, this_mac, SAL_ETHER_ADDR_LEN);
    
    return SAL_ERR_SUCCESS;
}

// Sal Sys IP Set: Sets IP address and netmask, writes to flash
// Converts IP to byte array and persists to flash configuration
int32
sal_sys_ip_set (
    sal_ipaddr_t ip,
    sal_ipaddr_t net_mask
    )
{
    this_ip[0] = (ip & 0xFF000000) >> 24;
    this_ip[1] = (ip & 0x00FF0000) >> 16;
    this_ip[2] = (ip & 0x0000FF00) >> 8;
    this_ip[3] = (ip & 0x000000FF) >> 0;
    netmask[0] = (net_mask & 0xFF000000) >> 24;
    netmask[1] = (net_mask & 0x00FF0000) >> 16;
    netmask[2] = (net_mask & 0x0000FF00) >> 8;
    netmask[3] = (net_mask & 0x000000FF) >> 0;

    sal_flash_sysIp_set(ip, net_mask);
    return SAL_ERR_SUCCESS;
}

// Sal Sys IP Get: Retrieves current IP address and netmask
// Converts byte arrays to 32-bit IP and netmask values
int32 sal_sys_ip_get(sal_ipaddr_t *pIp, sal_ipaddr_t *pNetmask)
{
    if(NULL == pIp || NULL == pNetmask)
        return SAL_ERR_NULL_POINTER;

    *pIp = ((uint32)this_ip[0] << 24) | ((uint32)this_ip[1] << 16) | ((uint32)this_ip[2] << 8) \
        | (uint32)this_ip[3];
    *pNetmask = ((uint32)netmask[0] << 24) | ((uint32)netmask[1] << 16) | ((uint32)netmask[2] << 8) \
        | (uint32)netmask[3];

    return SAL_ERR_SUCCESS;    
}

// Sal Sys Gateway Set: Sets default gateway address and writes to flash
// Converts gateway IP to byte array and persists to flash
int32
sal_sys_gateway_set (
    sal_ipaddr_t gateway
    )
{

    default_gateway.addr[0] = (gateway & 0xFF000000) >> 24;
    default_gateway.addr[1] = (gateway & 0x00FF0000) >> 16;
    default_gateway.addr[2] = (gateway & 0x0000FF00) >> 8;
    default_gateway.addr[3] = (gateway & 0x000000FF) >> 0;

    sal_flash_sysGateway_set(gateway);
    return SAL_ERR_SUCCESS;
}

// Sal Sys Gateway Get: Retrieves current default gateway address
// Converts byte array to 32-bit gateway IP
int32 sal_sys_gateway_get(sal_ipaddr_t *pGateway)
{
    if(NULL == pGateway)
        return SAL_ERR_NULL_POINTER;

    *pGateway = ((uint32)default_gateway.addr[0] << 24) | ((uint32)default_gateway.addr[1] << 16) | \
        ((uint32)default_gateway.addr[2] << 8) | (uint32)default_gateway.addr[3];

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_dhcp_set
 * Description:
 *      Set the DHCP enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys DHCP Set: Enables or disables DHCP client
// Updates DHCP status and writes to flash configuration
int32 sal_sys_dhcp_set(sal_enable_t enable)
{
    dhcp_enable = enable;

    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_FAILED;

#if LWPS_DHCPC
    if(enable == SAL_ENABLED)
        dhcpc_setEnable(TRUE);
    else
        dhcpc_setEnable(FALSE);
#endif

    sal_flash_sysDhcp_set(enable);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_dhcp_get
 * Description:
 *      Get the DHCP enable status.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys DHCP Get: Retrieves DHCP enable status
// Returns current DHCP client state
int32 sal_sys_dhcp_get(sal_enable_t *pEnable)
{
    if(NULL == pEnable)
        return SAL_ERR_NULL_POINTER;

    *pEnable = dhcp_enable;

    return SAL_ERR_SUCCESS;    
}

/* Function Name:
 *      sal_sysdev_type_get
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
// Sal Sys Dev Type Get: Retrieves device type/description string
// Copies device description from system descriptor
int32 sal_sys_dev_type_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, sysDescr, SAL_SYS_STRING_LEN);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sysdev_type_set
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
// Sal Sys Dev Type Set: Sets device type/description and writes to flash
// Updates device description and persists to flash configuration
int32 sal_sys_dev_type_set(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;

    memset(sysDescr,0,SAL_SYS_STRING_LEN);
    memcpy(sysDescr, pDesc, len);
    sal_flash_device_type_set(pDesc);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sysdev_type_set_cache
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
// Sal Sys Dev Type Set Cache: Sets device type in cache without writing to flash
// Updates device description in memory only
int32 sal_sys_dev_type_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;

    memset(sysDescr,0,SAL_SYS_STRING_LEN);
    memcpy(sysDescr, pDesc, len);

    return SAL_ERR_SUCCESS;
}


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
// Sal Sys RDONLY Community Get: Retrieves SNMP read-only community string
// Copies read-only community from system variable
int32 sal_sys_rdonly_community_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, rdonlycomm, SNMP_COMMUNITY_STR_LEN);

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys RDONLY Community Set: Sets SNMP read-only community string
// Updates community and optionally writes to flash cache
int32 sal_sys_rdonly_community_set(uint8 *pDesc, bool setCache)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_FAILED;

    memset(rdonlycomm,0,SNMP_COMMUNITY_STR_LEN);
    memcpy(rdonlycomm, pDesc, len);
    
    if(setCache)
    {
        sal_sys_rdonly_community_set_cache(pDesc);
    }

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys RDONLY Community Set Cache: Sets SNMP read-only community in cache
// Updates community in config cache without writing to flash
int32 sal_sys_rdonly_community_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_FAILED;
        
    memset(configCache.snmp.read_community,0,SNMP_COMMUNITY_STR_LEN);
    memcpy(configCache.snmp.read_community, pDesc, len);
//    snmp_inc_system_info(); //:: TBD CHARAN??

    return SAL_ERR_SUCCESS;
}


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
// Sal Sys RDWR Community Get: Retrieves SNMP read-write community string
// Copies read-write community from system variable
int32 sal_sys_rdwr_community_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, rdwrcomm, SNMP_COMMUNITY_STR_LEN);

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys RDWR Community Set: Sets SNMP read-write community string
// Updates community and optionally writes to flash cache
int32 sal_sys_rdwr_community_set(uint8 *pDesc, bool setCache)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_FAILED;

    memset(rdwrcomm,0,SNMP_COMMUNITY_STR_LEN);
    memcpy(rdwrcomm, pDesc, len);
    
    if(setCache)
    {
        sal_sys_rdwr_community_set_cache(pDesc);
    }

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys RDWR Community Set Cache: Sets SNMP read-write community in cache
// Updates community in config cache without writing to flash
int32 sal_sys_rdwr_community_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_FAILED;
        
    memset(configCache.snmp.write_community,0,SNMP_COMMUNITY_STR_LEN);
    memcpy(configCache.snmp.write_community, pDesc, len);
//    snmp_inc_system_info(); // TBD

    return SAL_ERR_SUCCESS;
}



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
// Sal Sys Description Get: Retrieves system description string (SNMP sysDescr)
// Copies system description from variable
int32 sal_sys_description_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, sysDescription, SAL_SNMP_SYS_INFO_STR_LEN);

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Description Set: Sets system description and optionally writes to cache
// Updates SNMP sysDescr field
int32 sal_sys_description_set(uint8 *pDesc, bool setCache)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;

    memset(sysDescription,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(sysDescription, pDesc, len);
    
    if(setCache)
    {
        sal_sys_description_set_cache(pDesc);
    }

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Description Set Cache: Sets system description in config cache
// Updates SNMP sysDescr in cache without writing to flash
int32 sal_sys_description_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;
        
    memset(configCache.snmp.extSysinfo.description,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(configCache.snmp.extSysinfo.description, pDesc, len);
    snmp_inc_system_info();

    return SAL_ERR_SUCCESS;
}


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
// Sal Sys Contact Get: Retrieves system contact string (SNMP sysContact)
// Copies system contact from variable
int32 sal_sys_contact_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, sysContact, SAL_SNMP_SYS_INFO_STR_LEN);
    
    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Contact Set: Sets system contact and optionally writes to cache
// Updates SNMP sysContact field
int32 sal_sys_contact_set(uint8 *pDesc, bool setCache)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;

    memset(sysContact,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(sysContact, pDesc, len);
   
    if(setCache)
    {
        sal_sys_contact_set_cache(pDesc);
    }

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Contact Set Cache: Sets system contact in config cache
// Updates SNMP sysContact in cache without writing to flash
int32 sal_sys_contact_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;
        
    memset(configCache.snmp.extSysinfo.contact,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(configCache.snmp.extSysinfo.contact, pDesc, len);
    snmp_inc_system_info();

    return SAL_ERR_SUCCESS;
}


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
// Sal Sys Location Get: Retrieves system location string (SNMP sysLocation)
// Copies system location from variable
int32 sal_sys_location_get(uint8 *pDesc)
{
    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDesc, sysLocation, SAL_SNMP_SYS_INFO_STR_LEN);

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Location Set: Sets system location and optionally writes to cache
// Updates SNMP sysLocation field
int32 sal_sys_location_set(uint8 *pDesc, bool setCache)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;

    memset(sysLocation,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(sysLocation, pDesc, len);
    
    if(setCache)
    {
        sal_sys_location_set_cache(pDesc);
    }

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Location Set Cache: Sets system location in config cache
// Updates SNMP sysLocation in cache without writing to flash
int32 sal_sys_location_set_cache(uint8 *pDesc)
{
    uint16 len;

    if(NULL == pDesc)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pDesc);
    if(len > SAL_SNMP_SYS_INFO_STR_LEN)
        return SAL_ERR_FAILED;
        
    memset(configCache.snmp.extSysinfo.location,0,SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(configCache.snmp.extSysinfo.location, pDesc, len);
    snmp_inc_system_info();

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog State Get: Retrieves syslog enable/disable state
// Copies syslog state from variable
int32 sal_sys_syslog_state_get(sal_enable_t *pState)
{
    if(NULL == pState)
        return SAL_ERR_NULL_POINTER;

    memcpy(pState, &syslogState, sizeof(sal_enable_t));

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog State Set: Sets syslog state and optionally writes to cache
// Updates syslog enable/disable status
int32 sal_sys_syslog_state_set(sal_enable_t *pState, bool setCache)
{
    if(NULL == pState)
        return SAL_ERR_NULL_POINTER;

    memset(&syslogState,0,sizeof(sal_enable_t));
    memcpy(&syslogState, pState, sizeof(sal_enable_t));
    
    if(setCache)
    {
        sal_sys_syslog_state_set_cache(pState);
    }

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog State Set Cache: Sets syslog state in config cache
// Updates syslog enable/disable in cache without writing to flash
int32 sal_sys_syslog_state_set_cache(sal_enable_t *pState)
{
    if(NULL == pState)
        return SAL_ERR_NULL_POINTER;
        
    memset(&(configCache.syslog.enable),0,sizeof(sal_enable_t));
    memcpy(&(configCache.syslog.enable), pState, sizeof(sal_enable_t));

    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_sys_syslog_hostIp_get
 * Description:
 *      Get host IP of syslog server
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the location
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys Syslog Host IP Get: Retrieves syslog server IP address
// Copies syslog server IP from variable
int32 sal_sys_syslog_hostIp_get(sal_ipaddr_t *pIp)
{
    if(NULL == pIp)
        return SAL_ERR_NULL_POINTER;

    memcpy(pIp, &syslogHostIp, sizeof(syslogHostIp));

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_syslog_hostIp_set
 * Description:
 *      Set syslog Host IP of this switch
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
// Sal Sys Syslog Host IP Set: Sets syslog server IP and optionally writes to cache
// Updates syslog server IP address
int32 sal_sys_syslog_hostIp_set(sal_ipaddr_t *pIp, bool setCache)
{
    if(NULL == pIp)
        return SAL_ERR_NULL_POINTER;

    memset(&syslogHostIp,0,sizeof(syslogHostIp));
    memcpy(&syslogHostIp, pIp, sizeof(syslogHostIp));
    
    if(setCache)
    {
        sal_sys_syslog_hostIp_set_cache(pIp);
    }

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_syslog_hostIp_set_cache
 * Description:
 *      Set ip add of host server to cache
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
// Sal Sys Syslog Host IP Set Cache: Sets syslog server IP in config cache
// Updates syslog server IP in cache without writing to flash
int32 sal_sys_syslog_hostIp_set_cache(sal_ipaddr_t *pIp)
{
    if(NULL == pIp)
        return SAL_ERR_NULL_POINTER;
        
    memset(&(configCache.syslog.sysLogServerIp),0,sizeof(sal_ipaddr_t));
    memcpy(&(configCache.syslog.sysLogServerIp), pIp, sizeof(sal_ipaddr_t));

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_syslog_hostIp_get
 * Description:
 *      Get host IP of syslog server
 * Input:
 *      None
 * Output:
 *      pDesc - Pointer to the location
 * Return:
 *      None
 * Note:
 *      None
 */
// Sal Sys Syslog Host UDP Port Get: Retrieves syslog server UDP port
// Copies syslog server port from variable
int32 sal_sys_syslog_hostUdpPort_get(uint16 *pPort)
{
    if(NULL == pPort)
        return SAL_ERR_NULL_POINTER;

    memcpy(pPort, &syslogHostUdpPort, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_syslog_hostIp_set
 * Description:
 *      Set syslog Host IP of this switch
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
// Sal Sys Syslog Host UDP Port Set: Sets syslog server UDP port and optionally writes to cache
// Updates syslog server port number
int32 sal_sys_syslog_hostUdpPort_set(uint16 *pPort, bool setCache)
{
    if(NULL == pPort)
        return SAL_ERR_NULL_POINTER;

    memset(&syslogHostUdpPort,0,sizeof(uint16));
    memcpy(&syslogHostUdpPort, pPort, sizeof(uint16));
    
    if(setCache)
    {
        sal_sys_syslog_hostUdpPort_set_cache(pPort);
    }

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_syslog_hostIp_set_cache
 * Description:
 *      Set ip add of host server to cache
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
// Sal Sys Syslog Host UDP Port Set Cache: Sets syslog server port in config cache
// Updates syslog server port in cache without writing to flash
int32 sal_sys_syslog_hostUdpPort_set_cache(uint16 *pPort)
{
    if(NULL == pPort)
        return SAL_ERR_NULL_POINTER;
        
    memset(&(configCache.syslog.sysLogServerPort),0,sizeof(uint16));
    memcpy(&(configCache.syslog.sysLogServerPort), pPort, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Head Index Get: Retrieves syslog buffer head index
// Copies syslog buffer head position from variable
int32 sal_sys_syslog_headIdx_get(uint16 *pIdx)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;

    memcpy(pIdx, &syslogHeadIdx, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Head Index Set: Sets syslog buffer head index and optionally writes to cache
// Updates syslog buffer head position
int32 sal_sys_syslog_headIdx_set(uint16 *pIdx, bool setCache)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;

    memset(&syslogHeadIdx,0,sizeof(uint16));
    memcpy(&syslogHeadIdx, pIdx, sizeof(uint16));
    
    if(setCache)
    {
        sal_sys_syslog_headIdx_set_cache(pIdx);
    }

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Head Index Set Cache: Sets syslog buffer head index in config cache
// Updates syslog buffer head position in cache without writing to flash
int32 sal_sys_syslog_headIdx_set_cache(uint16 *pIdx)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;
        
    memset(&(configCache.syslogBuf.headIdx),0,sizeof(uint16));
    memcpy(&(configCache.syslogBuf.headIdx), pIdx, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Tail Index Get: Retrieves syslog buffer tail index
// Copies syslog buffer tail position from variable
int32 sal_sys_syslog_tailIdx_get(uint16 *pIdx)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;

    memcpy(pIdx, &syslogTailIdx, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Tail Index Set: Sets syslog buffer tail index and optionally writes to cache
// Updates syslog buffer tail position
int32 sal_sys_syslog_tailIdx_set(uint16 *pIdx, bool setCache)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;

    memset(&syslogTailIdx,0,sizeof(uint16));
    memcpy(&syslogTailIdx, pIdx, sizeof(uint16));
    
    if(setCache)
    {
        sal_sys_syslog_tailIdx_set_cache(pIdx);
    }

    return SAL_ERR_SUCCESS;
}

// Sal Sys Syslog Tail Index Set Cache: Sets syslog buffer tail index in config cache
// Updates syslog buffer tail position in cache without writing to flash
int32 sal_sys_syslog_tailIdx_set_cache(uint16 *pIdx)
{
    if(NULL == pIdx)
        return SAL_ERR_NULL_POINTER;
        
    memset(&(configCache.syslogBuf.tailIdx),0,sizeof(uint16));
    memcpy(&(configCache.syslogBuf.tailIdx), pIdx, sizeof(uint16));

    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Firm Version Get: Retrieves firmware version string
// Copies firmware version from system variable
int32 sal_sys_firmVersion_get(uint8 *pVersion)
{
    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;
    
    memcpy(pVersion, fwVer, SAL_SYS_STRING_LEN);
    
    return SAL_ERR_SUCCESS;
}

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
// Sal Sys Firm Version Set: Sets firmware version and writes to flash
// Updates firmware version string and persists to flash
int32 sal_sys_firmVersion_set(uint8 *pVersion)
{
    uint16 len;

    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pVersion);

    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;

    memset(fwVer,0,SAL_SYS_STRING_LEN);
    memcpy(fwVer, pVersion, len);

    sal_flash_fw_version_set(pVersion);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_firmVersion_set_cache
 * Description:
 *      Set the version information of the firmware to cache.
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
// Sal Sys Firm Version Set Cache: Sets firmware version in cache without writing to flash
// Updates firmware version in memory only
int32 sal_sys_firmVersion_set_cache(uint8 *pVersion)
{
    uint16 len;

    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;

    len = strlen(pVersion);

    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;
        
    memset(fwVer,0,SAL_SYS_STRING_LEN);
    memcpy(fwVer, pVersion, len);

    return SAL_ERR_SUCCESS;
}


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
// Sal Sys Hardware Version Get: Retrieves hardware version string
// Copies hardware version from system variable
int32 sal_sys_hardwareVersion_get(uint8 *pVersion)
{
    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;

    memcpy(pVersion, hwVer, SAL_SYS_STRING_LEN);
    
    return SAL_ERR_SUCCESS;
}


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
// Sal Sys Hardware Version Set: Sets hardware version and writes to flash
// Updates hardware version string and persists to flash
int32 sal_sys_hardwareVersion_set(uint8 *pVersion)
{
    uint16 len;
    
    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;
    
    len = strlen(pVersion);
    
    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;

    memset(hwVer,0,SAL_SYS_STRING_LEN);
    memcpy(hwVer, pVersion, len);

    sal_flash_hw_version_set(pVersion);
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_hardwareVersion_set_cache
 * Description:
 *      Set the version information of the hardware to cache.
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
// Sal Sys Hardware Version Set Cache: Sets hardware version in cache without writing to flash
// Updates hardware version in memory only
int32 sal_sys_hardwareVersion_set_cache(uint8 *pVersion)
{
    uint16 len;
    
    if(NULL == pVersion)
        return SAL_ERR_NULL_POINTER;
    
    len = strlen(pVersion);
    
    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;
        
    memset(hwVer,0,SAL_SYS_STRING_LEN);
    memcpy(hwVer, pVersion, len);

    return SAL_ERR_SUCCESS;
}



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
// Sal Sys Firm Date Get: Retrieves firmware date string
// Copies firmware build date from system variable
int32 sal_sys_firmDate_get(uint8 *pDate)
{
    if(NULL == pDate)
        return SAL_ERR_NULL_POINTER;

    memcpy(pDate, fwDate, SAL_SYS_STRING_LEN);
    
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_sys_firmDate_set
 * Description:
 *      Get the date information of the firmware.
 * Input:
 *      pDate - pointer buffer of the firmware version 
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 *      SAL_ERR_FAILED       - Length of pVersion is too long.   
 * Note:
 *      None
 */
// Sal Sys Firm Date Set: Sets firmware date string
// Updates firmware build date in memory only
int32 sal_sys_firmDate_set(uint8 *pDate)
{
    uint16 len;
    
    if(NULL == pDate)
        return SAL_ERR_NULL_POINTER;
    
    len = strlen(pDate);
    
    if(len > SAL_SYS_STRING_LEN)
        return SAL_ERR_FAILED;

    memset(fwDate,0,SAL_SYS_STRING_LEN);
    memcpy(fwDate, pDate, len);
    
    return SAL_ERR_SUCCESS;
}


/* Function Name: 
 *      sal_sys_chipVersion_get 
 * Description: 
 *      Get the version information of the chip. 
 * Input: 
 *      None * Output: 
 *      pChipId  - pointer buffer of the chip id 
 *      pVersion - pointer buffer of the chip version 
 * Return: 
 *      None 
 * Note: 
 *      None 
 */
// Sal Sys Chip Version Get: Retrieves chip ID and version information
// Copies chip ID and version from system variables
int32 sal_sys_chipVersion_get(uint8 *pChipId, uint8 *pVersion)
{    
    if ((NULL == pChipId) || (NULL == pVersion))
        return SAL_ERR_NULL_POINTER;

    memcpy(pChipId, chipId, strlen(chipId) + 1);
    memcpy(pVersion, chipVer, strlen(chipVer) + 1);

    return SAL_ERR_SUCCESS;
}
#if 0
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
 *      SAL_ERR_PARAMETER
 * Note: 
 *      None 
 */
int32 sal_sys_regBits_set(uint32 addr, uint32 bits, uint32 value)
{
    rtk_api_ret_t ret;

    if(addr >= 0x10000 || bits >= 0x10000 || value >= 0x10000)
        return SAL_ERR_PARAMETER;
    
    if((ret = rtl8367c_setAsicRegBits(addr, bits, value)) != RT_ERR_OK)
        return ret;

    return SAL_ERR_SUCCESS;
}

/* Function Name: 
 *      sal_sys_regBit_set 
 * Description: 
 *      Set register value by bit. 
 * Input: 
 *      addr    - register address 
 *      bit_idx - bit number 
 *      value   - register value 
 * Output: 
 *      None 
 * Return: 
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note: 
 *      None 
 */
int32 sal_sys_regBit_set(uint32 addr, uint32 bit_idx, uint32 value)
{
    rtk_api_ret_t ret;    

    if(addr >= 0x10000 || bit_idx >= 16 || value >= 2)
        return SAL_ERR_PARAMETER;
    
    if((ret = rtl8367c_setAsicRegBit(addr, bit_idx, value)) != RT_ERR_OK)
        return ret;    
    return SAL_ERR_SUCCESS;
}

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
 *      SAL_ERR_PARAMETER
 * Note: 
 *      None 
 */
int32 sal_sys_reg_set(uint32 addr, uint32 value)
{
    if(addr >= 0x10000 || value >= 0x10000)
        return SAL_ERR_FAILED;

    setReg(addr, value);
    return SAL_ERR_SUCCESS;
}

/* Function Name: 
 *      sal_sys_reg_get 
 * Description: 
 *      Get register of the chip. 
 * Input: 
 *      addr   - register address
 * Output: 
 *      pValue - pointer buffer of the register value 
 * Return: 
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_NULL_POINTER
 * Note: 
 *      None 
 */

int32 sal_sys_reg_get(uint32 addr, uint32 *pValue)
{
    if(addr >= 0x10000)
        return SAL_ERR_PARAMETER;

    if(NULL == pValue)
        return SAL_ERR_NULL_POINTER;

    *pValue = getReg(addr);

    return SAL_ERR_SUCCESS;

}
#endif

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
// Sal Sys System Reboot: Reboots the entire system
// Sets reboot register to trigger system reset
int32 sal_sys_system_reboot(void)
{
    setReg(0x1322, 1);

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_sys_loaderMode_set
 * Description:
 *      Enter loader mode. Note that after this function is called,
 *      the system will switched from runtime mode to loader mode and
 *      cannot be switched to runtime mode again.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
// Sal Sys Loader Mode Set: Jumps to loader (bootloader) mode
// Saves config, sets warm start flag, and jumps to loader entry point
int32 sal_sys_loaderMode_set()
{
    loader_entry_t loader_fp;
    uint16 i;
	
    /* store configuration */
    sal_flash_cfg_save();

    /* set loader start mode as warm start */
    loader_start_mode_set(START_MODE_WARM);

    /* set loader ip */
    runtime_ip_set(this_ip, netmask, default_gateway.addr);
		
    setReg(0x1336, 0x41);
    for(i=0;i<65535;i++); //delay after set clock

    /* jump to initial code of irom */
    loader_fp = (loader_entry_t)LOADR_START_ADDR;
    loader_fp();

    return SAL_ERR_SUCCESS;
}
#endif