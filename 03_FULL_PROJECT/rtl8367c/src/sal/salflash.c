#pragma STRING (far)

#include <local/flash_config.h>
#include <local/flash_api.h>
#include <salflash.h>
#include <salqos.h>
#include <salmirror.h>
#include <salrate.h>
#include <saltrunk.h>
#include <salsys.h>
#if SAL_RSTP
  #include <salstp.h>
#endif

#include <string.h>
#include <stdio.h>
#include <salerror.h>
#include <salutil.h>
#include <sallldp.h>
#include <sal_config.h>
#include <rlpp_config.h>
#include <l2.h>

#if SAL_ENABLE

/* -------------------- Struct Definitions ------------------------------ */

extern uint8     xdata this_mac[];
extern uint8     xdata this_ip[];   
extern uint8     xdata target_ip[];
extern uint8     xdata netmask[];
extern ip_addr_t xdata default_gateway;

Tbool bAccessCache = TRUE;
ConfigCache configCache;

/* -------------------- General Functions ------------------------------ */

/* -------------------- Macro Definitions ------------------------------ */

#define _SalFlshRead(var, addr, vartype) {\
    if(bAccessCache)\
        _SalFlshReadConfigCache((int8 *)var, addr, sizeof(vartype));\
    else\
        FLASH_READ((uint32)addr, (int8 *)var, sizeof(vartype));} 
#define _SalFlshWrite(addr, var, vartype) _SalFlshWriteConfigCache((int8 *)var, addr, sizeof(vartype))
//flash_read_sec((int8 *)var, (uint32)addr, sizeof(vartype));} 

/* -------------------- General Functions ------------------------------ */

// Sal Flash Read Config Cache: Reads configuration data from cache memory
// Copies data from config cache to variable based on address offset
void
_SalFlshReadConfigCache(int8 *var, uint32 addr, uint32 size)
{
    int8 *p = (int8 *)&configCache;
    //printf("Flash read: Addr %lx\n", (uint32)(addr - FLSH_CONFIG_ADDR_START));
    memcpy(var, p+(addr-FLSH_CONFIG_ADDR_START), size);
}

// Sal Flash Write Config Cache: Writes configuration data to cache memory
// Copies data from variable to config cache at specified address offset
void
_SalFlshWriteConfigCache(int8 *var, uint32 addr, uint32 size)
{
    int8 *p = (int8 *)&configCache;
    //printf("Flash write: Addr %lx\n", (uint32)(addr - FLSH_CONFIG_ADDR_START));
    memcpy(p+(addr-FLSH_CONFIG_ADDR_START), var, size);
}

// Sal Flash Access Cache: Enables or disables cache access mode
// Sets bAccessCache flag to control direct flash vs cache reads/writes
void
SalFlshAccessCache(bool bCache)
{
    bAccessCache = bCache;
}

// Sal Flash Init Config Cache: Initializes configuration cache to zeros
// Clears entire config cache structure with memset
void
SalFlshInitConfigCache(void)
{
    memset(&configCache, 0x00, sizeof(ConfigCache));
}

// Sal Flash Copy Flash To Cache: Copies configuration from flash to cache
// Reads entire config sector from flash into memory cache
void
SalFlshCopyFlshToCache(void)
{
    memset(&configCache, 0x00, sizeof(ConfigCache));
    FLASH_READ((uint32)(FLSH_CONFIG_ADDR_START), (int8 *)&configCache, FLSH_ADDR_END - FLSH_CONFIG_ADDR_START);
    //flash_read_sec((int8 *)&configCache, (uint32)(FLSH_CONFIG_ADDR_START), FLSH_ADDR_END - FLSH_CONFIG_ADDR_START);
}

// Sal Flash Init: Initializes flash configuration with default values
// Clears cache, sets signature, initializes all subsystems, and saves to flash
int32
SalFlshInit ()
{
    /*clear cache first*/
    SalFlshInitConfigCache();

    SalFlshSetSignature(FLSH_SIGNATURE_WORD);
    sal_flash_trunk_init();
    sal_flash_sys_init();
    sal_flash_rate_init();
    sal_flash_mirror_init();
    sal_flash_l2_init();
    sal_flash_port_init();
    sal_flash_qos_init();
    sal_flash_vlan_init();
#if SAL_SERVICE_VLAN
    sal_flash_svlan_init();
#endif

#if SAL_LOOP
    sal_flash_loop_init();
#endif

#if SAL_RSTP
    sal_flash_stp_init ();
#endif

#if SAL_EEE
    sal_flash_eee_init();
#endif

#if SAL_GREENETH
    sal_flash_greenEth_init();
#endif

#if SAL_IGMP
    sal_flash_igmp_init();
#endif

#if SAL_SNMP
    sal_flash_snmp_init();
#endif
#if SAL_SYSLOG
    sal_flash_syslog_init();
	sal_flash_syslog_buf_init();
#endif
    SalFlshSetInitOKMark(FLSH_MARK_INIT_OK);

    /*a default cache is created, copy cache to flash*/
    sal_flash_cfg_save();
    
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys Init Except MAC: Initializes system config except MAC address
// Sets default IP, netmask, gateway, DHCP, and account settings
void
sal_flash_sys_init_except_Mac ()
{
    uint32 default_ip;
    uint32 default_netmask;
    uint32 default_gateway_u32;
    uint8  tmp[6] = DFLT_SYS_MAC_ADDRESS;

    sal_flash_sysDhcp_set(DFLT_SYS_IP_DHCP);

    default_ip = DFLT_SYS_IP_IP;
    default_netmask = DFLT_SYS_IP_NETMASK;
    sal_flash_sysIp_set(default_ip, default_netmask);

    default_gateway_u32 = DFLT_SYS_IP_GATEWAY;

    sal_flash_sysGateway_set(default_gateway_u32);

    sal_flash_sysAccount_set(DFLT_SYS_USERNAME, DFLT_SYS_PASSWORD);
}

// Sal Flash Init Except MAC: Initializes all config except MAC address
// Similar to SalFlshInit but preserves existing MAC address
int32
SalFlshInit_except_Mac ()
{
    /*clear cache first*/
    SalFlshInitConfigCache();

    SalFlshSetSignature(FLSH_SIGNATURE_WORD);
    sal_flash_trunk_init();
    sal_flash_sys_init_except_Mac();
    sal_flash_rate_init();
    sal_flash_mirror_init();
    sal_flash_l2_init();
    sal_flash_port_init();
    sal_flash_qos_init();
    sal_flash_vlan_init();
#if SAL_SERVICE_VLAN
    sal_flash_svlan_init();
#endif

#if SAL_LOOP
    sal_flash_loop_init();
#endif

#if SAL_RSTP
    sal_flash_stp_init ();
#endif

#if SAL_EEE
    sal_flash_eee_init();
#endif

#if SAL_GREENETH
    sal_flash_greenEth_init();
#endif

#if SAL_SNMP
    sal_flash_snmp_init();
#endif
#if SAL_SYSLOG
	sal_flash_syslog_init();
	sal_flash_syslog_buf_init();
#endif

    SalFlshSetInitOKMark(FLSH_MARK_INIT_OK);
    
    return SAL_ERR_SUCCESS;
}

#if 0
void
_SalFlshOnlyTrkLId2LogicId (
    Tchar *pcLogicId,
    uint8   ucLId
)
{
    if ((SAL_MAX_PHY_N_CPU_PORT <= ucLId) && (ucLId < SAL_MAX_LOGIC_PORT)) {
        sal_trunk_logicId_toStr(pcLogicId, ucLId);
    }
    else {
        sprintf(pcLogicId, "%d", ucLId+1);
    }
}

int32
SalFlshGetSize (
    uint32 *pulSize
)
{
    *pulSize = FLSH_ADDR_END - FLSH_ADDR_START;
    return SAL_ERR_SUCCESS;
}
#endif

/* -------------------- Signature Functions ---------------------------- */

// Sal Flash Set Signature: Writes signature word to flash
// Stores signature at beginning of config area for validation
int32
SalFlshSetSignature(
    uint32     ulSignature
    )
{
    sal_flash_beginMark_t stFlshSignature;

    _SalFlshRead(&stFlshSignature, FLSH_ADDR_SIGNATURE, sal_flash_beginMark_t);
    stFlshSignature.signature = ulSignature;
    _SalFlshWrite(FLSH_ADDR_SIGNATURE, &stFlshSignature, sal_flash_beginMark_t);
    return SAL_ERR_SUCCESS;
}
#if 0
int32
SalFlshGetSignature(
    uint32 *   pulSignature
    )
{
    sal_flash_beginMark_t stFlshSignature;

    _SalFlshRead(&stFlshSignature, FLSH_ADDR_SIGNATURE, sal_flash_beginMark_t);
    *pulSignature = stFlshSignature.ulSignatureWord;
    return SAL_ERR_SUCCESS;
}
#endif
/* -------------------- Marking Functions ------------------------------ */

// Sal Flash Set Init OK Mark: Sets initialization complete marker in flash
// Marks config area as valid and initialized
int32
SalFlshSetInitOKMark (
    uint32  ulMark
    )
{
    sal_flash_dirtyMark_t   stFlshMark;

    _SalFlshRead(&stFlshMark, FLSH_ADDR_MARK, sal_flash_dirtyMark_t);
    stFlshMark.initialized = ulMark;
    _SalFlshWrite(FLSH_ADDR_MARK, &stFlshMark, sal_flash_dirtyMark_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Get Init OK Mark: Retrieves initialization marker from flash
// Returns whether config area has been properly initialized
int32
SalFlshGetInitOKMark (
    uint32 *    pulMark
    )
{
    sal_flash_dirtyMark_t   stFlshMark;

    _SalFlshRead(&stFlshMark, FLSH_ADDR_MARK, sal_flash_dirtyMark_t);
    *pulMark = stFlshMark.initialized;
    return SAL_ERR_SUCCESS;
}

#if 1

/* -------------------- System IP Functions ---------------------------- */

// Sal Flash Sys Init: Initializes system configuration including MAC
// Sets default IP, netmask, gateway, DHCP, MAC, and account settings
void
sal_flash_sys_init ()
{
    //Tuint8 sysMac[ETHER_ADDR_LEN];
    uint32 default_ip;
    uint32 default_netmask;
    uint32 default_gateway_u32;
    uint8  tmp[6] = DFLT_SYS_MAC_ADDRESS;
    sal_mac_t sysMac;
    
    sal_flash_sysDhcp_set(DFLT_SYS_IP_DHCP);

    default_ip = DFLT_SYS_IP_IP;
    default_netmask = DFLT_SYS_IP_NETMASK;
    sal_flash_sysIp_set(default_ip, default_netmask);

    default_gateway_u32 = DFLT_SYS_IP_GATEWAY;

    sal_flash_sysGateway_set(default_gateway_u32);
    memcpy(sysMac.octet, tmp, SAL_ETHER_ADDR_LEN);
    sal_flash_sysMac_set(&sysMac);

    sal_flash_sysAccount_set(DFLT_SYS_USERNAME, DFLT_SYS_PASSWORD);
}

// Sal Flash Sys DHCP Set: Saves DHCP enable/disable status to flash
// Writes DHCP configuration to system info structure
int32
sal_flash_sysDhcp_set (
    sal_enable_t enable
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    stFlshSys.dhcp = (enable == SAL_ENABLED)? ENABLE : DISABLE;
    _SalFlshWrite(FLSH_ADDR_SYS, &stFlshSys, sal_flash_sysInfo_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys DHCP Get: Retrieves DHCP enable/disable status from flash
// Reads DHCP configuration from system info structure
int32
sal_flash_sysDhcp_get (
    sal_enable_t *pEnable
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    *pEnable = (stFlshSys.dhcp == ENABLE)? SAL_ENABLED : SAL_DISABLED;
    return SAL_ERR_SUCCESS;
}


// Sal Flash Sys IP Set: Saves IP address and netmask to flash
// Writes IP configuration to system info structure
int32
sal_flash_sysIp_set (
    sal_ipaddr_t ip,
    sal_ipaddr_t netmask
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    stFlshSys.ip = ip;
    stFlshSys.netmask = netmask;
    _SalFlshWrite(FLSH_ADDR_SYS, &stFlshSys, sal_flash_sysInfo_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys IP Get: Retrieves IP address and netmask from flash
// Reads IP configuration from system info structure
int32
sal_flash_sysIp_get (
    sal_ipaddr_t *pIp,
    sal_ipaddr_t *pNetmask
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    *pIp = stFlshSys.ip;
    *pNetmask = stFlshSys.netmask;
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys Gateway Set: Saves gateway address to flash
// Writes gateway configuration to system info structure
int32
sal_flash_sysGateway_set (
    sal_ipaddr_t gateway
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    stFlshSys.gateway = gateway;
    _SalFlshWrite(FLSH_ADDR_SYS, &stFlshSys, sal_flash_sysInfo_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys Gateway Get: Retrieves gateway address from flash
// Reads gateway configuration from system info structure
int32
sal_flash_sysGateway_get (
    sal_ipaddr_t *pGateway
    )
{
    sal_flash_sysInfo_t stFlshSys;

    if (pGateway == NULL)
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    *pGateway = stFlshSys.gateway;
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys MAC Set: Saves MAC address to flash in non-volatile area
// Writes MAC address while preserving serial number in same sector
int32
sal_flash_sysMac_set (
    sal_mac_t *pMac
    )
{
    uint8 xdata buf[100];

    if (NULL == pMac)
        return SAL_ERR_NULL_POINTER;

    /* read MAC and serial number */    
    FLASH_READ((uint32)(FLSH_NVDATA_START), buf, sizeof(buf));
    /* keep serial number and replace MAC part */
    memcpy(&buf[0],pMac->octet,ETHER_ADDR_LEN);
    
    FLASH_ERASE_SECTOR(FLSH_NVDATA_START);
    FLASH_WRITE((uint32)FLSH_NVDATA_START, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys MAC Get: Retrieves MAC address from flash
// Reads MAC address from non-volatile storage area
int32
sal_flash_sysMac_get (
    sal_mac_t *pMac
    )
{

    uint8 xdata buf[6];
    memset(buf,0,sizeof(buf));
    FLASH_READ((uint32)(FLSH_NVDATA_MAC), buf, sizeof(buf));
    memcpy(pMac->octet, &buf[0], sizeof(buf));

    return SAL_ERR_SUCCESS;
}


#ifdef __BOOT_COMMAND__
// Sal Flash Sys Set Boot Cmd: Saves boot command string to flash
// Stores boot command in system info structure
int32
SalFlshSysSetBootCmd (
    uint8   *strCmd
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    strcpy(stFlshSys.bootCmd, strCmd);
    _SalFlshWrite(FLSH_ADDR_SYS, &stFlshSys, sal_flash_sysInfo_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys Get Boot Cmd: Retrieves boot command string from flash
// Reads boot command from system info structure
int32
SalFlshSysGetBootCmd (
    uint8   *strCmd
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    strcpy(strCmd, stFlshSys.bootCmd);
    return SAL_ERR_SUCCESS;
}
#endif

#endif

/* -------------------- User Account Functions ---------------------------- */

// Sal Flash Sys Account Set: Saves username and password to flash
// Writes user account credentials to flash configuration
int32
sal_flash_sysAccount_set (
    uint8 *pUsername,
    uint8 *pPassword
    )
{
    sal_flash_account_t stFlshUsr;

    if ((pUsername == NULL) || (pPassword == NULL))
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&stFlshUsr, FLSH_ADDR_USR, sal_flash_account_t);
    
    memcpy(stFlshUsr.username, pUsername, FLSH_USR_USERNAME_LEN + 1);
    stFlshUsr.username[SAL_FLSH_USR_USERNAME_LEN] = 0;
   
    memcpy(stFlshUsr.password, pPassword, FLSH_USR_PASSWORD_LEN + 1);
    stFlshUsr.password[SAL_FLSH_USR_PASSWORD_LEN] = 0;    
    
    _SalFlshWrite(FLSH_ADDR_USR, &stFlshUsr, sal_flash_account_t);

    return SAL_ERR_SUCCESS;
}

// Sal Flash Sys Account Get: Retrieves username and password from flash
// Reads user account credentials from flash configuration
int32
sal_flash_sysAccount_get (
    uint8 *pUsername,
    uint8 *pPassword
    )
{
    sal_flash_account_t stFlshUsr;
    
    if ((pUsername == NULL) || (pPassword == NULL))
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&stFlshUsr, FLSH_ADDR_USR, sal_flash_account_t);
    memcpy(pUsername, stFlshUsr.username, FLSH_USR_USERNAME_LEN + 1);
    memcpy(pPassword, stFlshUsr.password, FLSH_USR_PASSWORD_LEN + 1);

    return SAL_ERR_SUCCESS;
}

/* -------------------- Forwarding Functions --------------------------- */

// Sal Flash Rate Init: Initializes default rate control settings
// Sets storm control, bandwidth control, and frame size defaults
void
sal_flash_rate_init ()
{
    sal_lPortmask_t portMask;
    sal_port_t      port;

    sal_flash_rate_igrIncludeIFG_set(DFLT_FWD_INGRS_PREABMLE);
    sal_flash_rate_egrIncludeIFG_set(DFLT_FWD_EGRS_PREAMBLE);

    sal_trunk_logicPort_setAll(&portMask);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portMask, port))
        {
            continue;
        }

        sal_flash_rate_stormCtrl_set(port, SAL_STORM_TYPE_UNKNOWN_UCAST, DFLT_FWD_STRM_FLTR_RATE);
        sal_flash_rate_stormCtrl_set(port, SAL_STORM_TYPE_UNKNOWN_MCAST, DFLT_FWD_STRM_FLTR_RATE);
        sal_flash_rate_stormCtrl_set(port, SAL_STORM_TYPE_MCAST, DFLT_FWD_STRM_FLTR_RATE);
        sal_flash_rate_stormCtrl_set(port, SAL_STORM_TYPE_BCAST, DFLT_FWD_STRM_FLTR_RATE);

        sal_flash_rate_igrBandwidthCtrl_set(port, DFLT_PORT_RATE_INGRS);
        sal_flash_rate_egrBandwidthCtrl_set(port, DFLT_PORT_RATE_EGRS);
    }

    /*Defence Engine*/
    sal_flash_rate_cpuEgrCtrl_set(DFLT_FWD_CPU_EGRS_CTRL_EBL);
    sal_flash_rate_egrBandwidthCtrl_set(SAL_PORT_ID_CPU, DFLT_FWD_CPU_EGRS_RATE);

    sal_lPortMask_delAll(&portMask);
    sal_flash_rate_maxFrameSize_set(DFLT_FWD_MAX_FRAME_SIZE);
}
 
// Sal Flash Rate Storm Ctrl Set: Saves storm control rate for a port
// Stores rate limit for specific storm type in flash configuration
int32
sal_flash_rate_stormCtrl_set(sal_port_t portId, sal_stormCtrl_type_t type, uint32 rate)
{
    sal_flash_fwd_t flshFwd;
    sal_pPortmask_t portMask;
    sal_port_t      port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

#if 0
    if(DFLT_FWD_STRM_FLTR_RATE == rate)
        rate = 0;
#endif    

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    sal_trunk_logicId_toPhysicMask(&portMask, portId);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port)) {
            if (type == SAL_STORM_TYPE_UNKNOWN_UCAST)
                flshFwd.stormCtrl[port].uKnwnUcastRate = rate;
            else if (type == SAL_STORM_TYPE_UNKNOWN_MCAST)
                flshFwd.stormCtrl[port].uKnwnMcastRate = rate;
            else if (type == SAL_STORM_TYPE_BCAST)
                flshFwd.stormCtrl[port].bCastRate = rate;
            else if (type == SAL_STORM_TYPE_MCAST)
                flshFwd.stormCtrl[port].mCastRate = rate;
        }
    }
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
} 

// Sal Flash Rate Storm Ctrl Get: Retrieves storm control rate for a port
// Reads rate limit for specific storm type from flash configuration
int32
sal_flash_rate_stormCtrl_get (
    sal_port_t  portId,
    sal_stormCtrl_type_t    type,
    uint32 *pRate
    )
{
    sal_flash_fwd_t flshFwd;
    sal_port_t      port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    sal_trunk_logicId_toPhysicId(&port, portId);

    if (type == SAL_STORM_TYPE_UNKNOWN_UCAST)
        *pRate = flshFwd.stormCtrl[port].uKnwnUcastRate;
    else if (type == SAL_STORM_TYPE_UNKNOWN_MCAST)
        *pRate = flshFwd.stormCtrl[port].uKnwnMcastRate;
    else if (type == SAL_STORM_TYPE_BCAST)
        *pRate = flshFwd.stormCtrl[port].bCastRate;
    else if (type == SAL_STORM_TYPE_MCAST)
        *pRate = flshFwd.stormCtrl[port].mCastRate;

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate CPU Egr Ctrl Set: Saves CPU egress control setting
// Stores CPU egress rate limit configuration in flash
int32
sal_flash_rate_cpuEgrCtrl_set (sal_enable_t enable)
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    flshFwd.cpuEgrCtrl = enable;
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
}

#if 0
int32
sal_flash_rate_cpuEgrCtrl_get (sal_enable_t *pEnable)
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    *pEnable = flshFwd.cpuEgrCtrl;

    return SAL_ERR_SUCCESS;
}
#endif

// Sal Flash Rate Igr Include IFG Set: Saves ingress IFG inclusion setting
// Stores whether Inter-Frame Gap is included in ingress rate calculation
int32
sal_flash_rate_igrIncludeIFG_set (
    uint8   include
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    flshFwd.igrIncludeIFG = include;
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
}
#if 0
int32
sal_flash_rate_igrIncludeIFG_get (
    uint8 *pInclude
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    *pInclude = flshFwd.igrIncludeIFG;

    return SAL_ERR_SUCCESS;
}
#endif

// Sal Flash Rate Egr Include IFG Set: Saves egress IFG inclusion setting
// Stores whether Inter-Frame Gap is included in egress rate calculation
int32
sal_flash_rate_egrIncludeIFG_set (
    uint8 include
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    flshFwd.egrIncludeIFG = include;
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
}
#if 0
int32
sal_flash_rate_egrIncludeIFG_get (
    uint8 *pInclude
)
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    *pInclude = flshFwd.egrIncludeIFG;

    return SAL_ERR_SUCCESS;
}
#endif

// Sal Flash Rate Max Frame Size Set: Saves maximum frame size setting
// Stores jumbo frame size configuration in flash
int32
sal_flash_rate_maxFrameSize_set (
    uint32 size
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    flshFwd.frameSize = size;
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate Max Frame Size Get: Retrieves maximum frame size setting
// Reads jumbo frame size configuration from flash
int32
sal_flash_rate_maxFrameSize_get (
    uint32 *pSize
    )
{
    sal_flash_fwd_t flshFwd;
    
    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    *pSize = flshFwd.frameSize;

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate Igr Bandwidth Ctrl Set: Saves ingress bandwidth limit
// Stores ingress rate limit for specified port in flash
int32
sal_flash_rate_igrBandwidthCtrl_set (
    sal_port_t  portId,
    uint32      rate
    )
{
    sal_flash_port_t flshPort;
    sal_pPortmask_t  portMask;
    sal_port_t       port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flshPort, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&portMask, portId);
    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port)) {
            flshPort.igrRate[port] = rate;
        }
    }
    _SalFlshWrite(FLSH_ADDR_PORT, &flshPort, sal_flash_port_t);

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate Igr Bandwidth Ctrl Get: Retrieves ingress bandwidth limit
// Reads ingress rate limit for specified port from flash
int32
sal_flash_rate_igrBandwidthCtrl_get (
    sal_port_t  portId,
    uint32     *pRate
    )
{
    sal_flash_port_t flshPort;
    sal_port_t       port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flshPort, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&port, portId);
    *pRate = flshPort.igrRate[port];

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate Egr Bandwidth Ctrl Set: Saves egress bandwidth limit
// Stores egress rate limit for specified port in flash
int32
sal_flash_rate_egrBandwidthCtrl_set (
    sal_port_t  portId,
    uint32      rate
    )
{
    sal_flash_port_t flshPort;
    sal_pPortmask_t portMask;
    sal_port_t      port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flshPort, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&portMask, portId);

    /*Modify this for Defence Engine*/
    for (port = 0; port <= SAL_PORT_ID_CPU; ++port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port)) {
            flshPort.egrRate[port] = rate;
        }
    }

    _SalFlshWrite(FLSH_ADDR_PORT, &flshPort, sal_flash_port_t);

    return SAL_ERR_SUCCESS;
}

// Sal Flash Rate Egr Bandwidth Ctrl Get: Retrieves egress bandwidth limit
// Reads egress rate limit for specified port from flash
int32
sal_flash_rate_egrBandwidthCtrl_get (
    sal_port_t  portId,
    uint32      *pRate
    )
{
    sal_flash_port_t flshPort;
    sal_port_t      port;

    if (portId > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flshPort, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&port, portId);
    *pRate = flshPort.egrRate[port];

    return SAL_ERR_SUCCESS;
}

/* -------------------- Mirror Functions --------------------------------- */

// Sal Flash Mirror Init: Initializes default mirroring configuration
// Sets all mirror groups to disabled with empty port masks
void
sal_flash_mirror_init(void)
{
    uint8           index;
    sal_lPortmask_t portMask;

    sal_lPortMask_delAll(&portMask);

    for (index = 0; index < SAL_MAX_MIRROR_GROUP; index++)
    {
        sal_flash_mirror_portMirror_set(index, DFLT_FWD_MIRROR_MODE,
            DFLT_FWD_MIRROR_PORT, &portMask);
    }
}

// Sal Flash Mirror Port Mirror Set: Saves mirror configuration to flash
// Stores mirror mode, port, and mirrored port mask for a mirror group
int32
sal_flash_mirror_portMirror_set (
    uint32      mirrId,
    uint32      mode,
    sal_port_t  portId,
    sal_lPortmask_t *pPortMask
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    flshFwd.mirror[mirrId].mode = mode;
    flshFwd.mirror[mirrId].mirrPort = portId;

    sal_trunk_logicMask_toPhysicMask(&flshFwd.mirror[mirrId].mirredPortMask, pPortMask);
    _SalFlshWrite(FLSH_ADDR_FWD, &flshFwd, sal_flash_fwd_t);

    return SAL_ERR_SUCCESS;
}

// Sal Flash Mirror Port Mirror Get: Retrieves mirror configuration from flash
// Reads mirror mode, port, and mirrored port mask for a mirror group
int32
sal_flash_mirror_portMirror_get (
    uint32  mirrId,
    uint32 *pMode,
    sal_port_t *pPortId,
    sal_lPortmask_t *pPortMask
    )
{
    sal_flash_fwd_t flshFwd;

    _SalFlshRead(&flshFwd, FLSH_ADDR_FWD, sal_flash_fwd_t);
    *pMode = flshFwd.mirror[mirrId].mode;
    *pPortId = flshFwd.mirror[mirrId].mirrPort;
    sal_trunk_physicMask_toLogicMask(pPortMask, &flshFwd.mirror[mirrId].mirredPortMask);

    return SAL_ERR_SUCCESS;
}

/* -------------------- Port Functions --------------------------------- */

// Sal Flash Port Init: Initializes default port configuration
// Sets admin enable, PHY ability, flow control, and isolation for all ports
void
sal_flash_port_init ()
{
    sal_port_t  port;
    sal_lPortmask_t l_portMask;
    sal_phyAbility_t    ability;
    sal_lPortmask_t isolationMask;

    ability.AutoNegotiation = DFLT_PORT_NEGO;
    ability.Half_10         = DFLT_PORT_10HALF;
    ability.Full_10         = DFLT_PORT_10FULL;
    ability.Half_100        = DFLT_PORT_100HALF;
    ability.Full_100        = DFLT_PORT_100FULL;
    ability.Full_1000       = DFLT_PORT_1000FULL;

    memset(&isolationMask, 0, sizeof(sal_lPortmask_t));
    isolationMask.bits[0] = DFLT_PORT_ISOLATION_MASK;

    sal_trunk_logicPort_setAll(&l_portMask);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_SUCCESS != sal_lPortMask_chkPort(&l_portMask, port))
        {
            continue;
        }

        sal_flash_port_adminEnable_set(port, DFLT_PORT_EBL);
        sal_flash_port_phyAbility_set(port, &ability);
        sal_flash_port_flowCtrl_set(port, DFLT_PORT_FLOW_CTRL);
        sal_flash_port_isolation_set(port, isolationMask);
    }
}

// Sal Flash Port Admin Enable Set: Saves port admin enable status to flash
// Stores whether a port is administratively enabled or disabled
int32
sal_flash_port_adminEnable_set (
    sal_port_t port,
    sal_enable_t enable
    )
{
    sal_flash_port_t    flash_port;
    sal_pPortmask_t p_portMask;
    sal_port_t  p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);

    for (p_port = 0; p_port < SAL_PORT_ID_CPU; ++p_port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port)) {
            flash_port.enable[p_port] = enable;
        }
    }
    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Port Admin Enable Get: Retrieves port admin enable status from flash
// Reads whether a port is administratively enabled or disabled
int32
sal_flash_port_adminEnable_get (
    sal_port_t port,
    sal_enable_t *pEnable
    )
{
    sal_flash_port_t    flash_port;
    sal_port_t  p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);
    *pEnable = flash_port.enable[p_port];
    return SAL_ERR_SUCCESS;
}

// Sal Flash Port PHY Ability Set: Saves PHY ability settings to flash
// Stores auto-negotiation, speed, duplex, and flow control capabilities
int32
sal_flash_port_phyAbility_set (
    sal_port_t port,
    sal_phyAbility_t *pPhyAbility
    )
{
    sal_port_t  p_port;
    sal_flash_port_t    flash_port;
    sal_pPortmask_t p_portMask;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);

    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (p_port = 0; p_port < SAL_PORT_ID_CPU; ++p_port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port)) {
            flash_port.AutoNegotiation[p_port] = pPhyAbility->AutoNegotiation;
            flash_port.Half_10[p_port]  = pPhyAbility->Half_10;
            flash_port.Full_10[p_port]  = pPhyAbility->Full_10;
            flash_port.Half_100[p_port] = pPhyAbility->Half_100;
            flash_port.Full_100[p_port] = pPhyAbility->Full_100;
            flash_port.Full_1000[p_port]= pPhyAbility->Full_1000;
            flash_port.FC[p_port]       = pPhyAbility->FC;
            flash_port.AsyFC[p_port]    = pPhyAbility->AsyFC;
        }
    }

    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

// Sal Flash Port PHY Ability Get: Retrieves PHY ability settings from flash
// Reads auto-negotiation, speed, duplex, and flow control capabilities
int32
sal_flash_port_phyAbility_get (
    sal_port_t port,
    sal_phyAbility_t *pPhyAbility
    )
{
    sal_flash_port_t flash_port;
    sal_port_t       p_port;

//    if (port > SAL_PORT_ID_CPU - 1) {
//        return SAL_ERR_PORT_RANGE;
//    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);

    sal_trunk_logicId_toPhysicId(&p_port, port);

    pPhyAbility->AutoNegotiation = flash_port.AutoNegotiation[p_port];
    pPhyAbility->Half_10   = flash_port.Half_10[p_port];
    pPhyAbility->Full_10   = flash_port.Full_10[p_port];
    pPhyAbility->Half_100  = flash_port.Half_100[p_port];
    pPhyAbility->Full_100  = flash_port.Full_100[p_port] ;
    pPhyAbility->Full_1000 = flash_port.Full_1000[p_port] ;
    pPhyAbility->FC        = flash_port.FC[p_port] ;
    pPhyAbility->AsyFC = flash_port.AsyFC[p_port] ;

    return SAL_ERR_SUCCESS;
}
int32
sal_flash_port_flowCtrl_set (
    sal_port_t port,
    sal_enable_t enable
    )
{
    sal_flash_port_t flash_port;
    sal_pPortmask_t  p_portMask;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (p_port = 0; p_port < SAL_PORT_ID_CPU; ++p_port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port)) {
            flash_port.FC[p_port] = enable;
        }
    }
    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_port_flowCtrl_get (
    sal_port_t port,
    sal_enable_t *pEnable
    )
{
    sal_flash_port_t flash_port;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);

    *pEnable = flash_port.FC[p_port];
    return SAL_ERR_SUCCESS;
}

#if 0
int32
sal_flash_port_security_set (
    sal_port_t port,
    uint32 num
    )
{
    sal_flash_port_t    flash_port;
    sal_pPortmask_t     p_portMask;
    sal_port_t  p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (p_port = 0; p_port < SAL_MAX_PHY_PORT; ++p_port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port)) {
            flash_port.securityNum[p_port] = num;
        }
    }
    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_port_security_get (
    sal_port_t port,
    uint32 *pNum
    )
{
    sal_flash_port_t flash_port;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);
    *pNum = flash_port.securityNum[p_port];
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_port_securityAction_set (
    sal_port_t port,
    uint32 act
    )
{
    sal_flash_port_t flash_port;
    sal_pPortmask_t  p_portMask;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (p_port = 0; p_port < SAL_MAX_PHY_PORT; ++p_port) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port)) {
            flash_port.securityAct[p_port] = act;
        }
    }

    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_port_securityAction_get (
    sal_port_t port,
    uint32 *pAct
    )
{
    sal_flash_port_t flash_port;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);
    *pAct = flash_port.securityAct[p_port];

    return SAL_ERR_SUCCESS;
}
#endif

int32
sal_flash_port_isolation_set(
    sal_port_t port,
    sal_lPortmask_t portMask
    )
{
    sal_flash_port_t flash_port;
    sal_pPortmask_t p_portMask, mask;
    sal_port_t      p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);

    for (p_port = 0; p_port < SAL_PORT_ID_CPU; ++p_port)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_port))
        {
            sal_trunk_logicMask_toPhysicMask(&mask, &portMask);
            sal_pPortMask_copy(&flash_port.PortIsolationMask[p_port], &mask);
        }
    }

    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_port_isolation_get(
    sal_port_t port,
    sal_lPortmask_t *pPortMask
    )
{
    sal_port_t  p_port;    
    sal_flash_port_t flash_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    sal_trunk_logicId_toPhysicId(&p_port, port);
    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_physicMask_toLogicMask(pPortMask, &flash_port.PortIsolationMask[p_port]);

    return SAL_ERR_SUCCESS;
}

/* -------------------- MAC Table Functions ---------------------------- */

void
sal_flash_l2_init ()
{
    uint32 slot;
    sal_port_t  port;
    sal_lPortmask_t portmask;

    /* clear static MAC table entries in flash */
    for (slot = 0; slot < SAL_MAX_STATIC_MAC; ++slot) {
        sal_flash_l2_sttcEntrySlot_del(slot);
    }

    sal_trunk_logicPort_setAll(&portmask);
    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_SUCCESS != sal_lPortMask_chkPort(&portmask, port))
        {
            continue;
        }

        sal_flash_l2_mac_constraint_set(port, rtk_switch_maxLutAddrNumber_get());
    }
}

int32
sal_flash_l2_sttcEntry_add (
    uint32  slot,
    sal_flash_l2_sttcEntry_t *pEntry
    )
{
    if (slot > SAL_MAX_STATIC_MAC-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    /* save physical port mask from logical port mask */
    //sal_trunk_logicMask_toPhysicMask(&pEntry->pPortMask, &pEntry->lPortMask);
    _SalFlshWrite(FLSH_ADDR_MAC_TBL_STTC+slot*sizeof(sal_flash_l2_sttcEntry_t), pEntry, sal_flash_l2_sttcEntry_t);

    return SAL_ERR_SUCCESS;
}

int32
sal_flash_l2_sttcEntrySlot_del (
    uint32 slot
    )
{
    sal_flash_l2_sttcEntry_t entry;

    if (slot > SAL_MAX_STATIC_MAC-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    memset(&entry, 0, sizeof(sal_flash_l2_sttcEntry_t));
    _SalFlshWrite(FLSH_ADDR_MAC_TBL_STTC+slot*sizeof(sal_flash_l2_sttcEntry_t), &entry, sal_flash_l2_sttcEntry_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_l2_sttcEntry_get (
    uint32  slot,
    sal_flash_l2_sttcEntry_t *pEntry
    )
{
    if (slot > SAL_MAX_STATIC_MAC-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    _SalFlshRead(pEntry, FLSH_ADDR_MAC_TBL_STTC+slot*sizeof(sal_flash_l2_sttcEntry_t), sal_flash_l2_sttcEntry_t);
    /* restore logical port mask from saved physical port mask */
    //sal_trunk_physicMask_toLogicMask(&pEntry->lPortMask, &pEntry->pPortMask);

    return SAL_ERR_SUCCESS;
}

int32
sal_flash_l2_firstAvailableSlot_get(uint32 *pSlot)
{
    uint8                       empty[MAC_ADDR_LEN];
    uint16                      index = 0;
    sal_flash_l2_sttcEntry_t    entry;
    int32                       ret = SAL_ERR_FAILED;

    *pSlot = SAL_MAX_STATIC_MAC + 1;

    memset(empty, 0, MAC_ADDR_LEN);
    for (index = 0; index < SAL_MAX_STATIC_MAC; ++index)
    {
        if (SAL_ERR_SUCCESS == sal_flash_l2_sttcEntry_get(index, &entry))
        {
            if (memcmp(entry.mac.octet, empty, MAC_ADDR_LEN) == 0)
            {
                *pSlot = index; /* found empty entry */
                ret = SAL_ERR_SUCCESS;
                break;
            }
        }
    }

    return ret;
}

int32
sal_flash_l2_matchedSlot_get(
    uint32    *usSlot, 
    sal_mac_t *mac, 
    uint16    fid)
{
    uint16                      index = 0;
    sal_flash_l2_sttcEntry_t    Entry;
    int32                       ret = SAL_ERR_FAILED;

    *usSlot = SAL_MAX_STATIC_MAC + 1;

    for (index = 0; index < SAL_MAX_STATIC_MAC; ++index)
    {
        if (SAL_ERR_SUCCESS == sal_flash_l2_sttcEntry_get(index, &Entry))
        {
            if ((memcmp(Entry.mac.octet, mac, MAC_ADDR_LEN) == 0) \
                && (Entry.fid == fid))
            {
                *usSlot = index; /* found matched entry */
                return SAL_ERR_SUCCESS;
            }
        }
    }

    return SAL_ERR_ENTRY_NOTFOUND;
}

int32
sal_flash_l2_sttcEntry_del(
    sal_mac_t  *mac,
    sal_vlan_t vid)
{
    sal_flash_l2_sttcEntry_t    entry;
    uint16                      slot = 0;

    if (NULL == mac) return SAL_ERR_NULL_POINTER;

    for (slot = 0; slot < SAL_MAX_STATIC_MAC; ++slot)
    {
        _SalFlshRead(&entry, FLSH_ADDR_MAC_TBL_STTC + slot * sizeof(sal_flash_l2_sttcEntry_t),
            sal_flash_l2_sttcEntry_t);

        if ((memcmp(entry.mac.octet, mac, MAC_ADDR_LEN) == 0) &&
            (vid == entry.fid))
        {
            memset(&entry, 0, sizeof(sal_flash_l2_sttcEntry_t));
            _SalFlshWrite(FLSH_ADDR_MAC_TBL_STTC + slot * sizeof(sal_flash_l2_sttcEntry_t),
                &entry, sal_flash_l2_sttcEntry_t);
            return SAL_ERR_SUCCESS;
        }
    }

    return SAL_ERR_FAILED;
}

int32 sal_flash_l2_mac_constraint_set(sal_port_t port, uint32 mac_cnt)
{
    sal_flash_port_t flash_port;
    sal_pPortmask_t  portmask;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    sal_trunk_logicId_toPhysicMask(&portmask, port);

    for (p_port = 0; p_port < SAL_PORT_ID_CPU; ++p_port)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portmask, p_port))
        {
            flash_port.entry_limits[p_port] = mac_cnt;
        }
    }

    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32 sal_flash_l2_mac_constraint_get(sal_port_t port, uint32* pMac_cnt)
{
    sal_port_t  p_port;
    sal_flash_port_t    flash_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    sal_trunk_logicId_toPhysicId(&p_port, port);
    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    *pMac_cnt = flash_port.entry_limits[p_port];
    
    return SAL_ERR_SUCCESS;
}

int32 sal_flash_l2_mac_constraint_act_set(uint8 action)
{
    sal_flash_port_t flash_port;

    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);

    flash_port.entry_action = action;

    _SalFlshWrite(FLSH_ADDR_PORT, &flash_port, sal_flash_port_t);
    return SAL_ERR_SUCCESS;
}

int32 sal_flash_l2_mac_constraint_act_get(uint8 *action)
{
    sal_flash_port_t    flash_port;


    _SalFlshRead(&flash_port, FLSH_ADDR_PORT, sal_flash_port_t);
    *action = flash_port.entry_action;
    
    return SAL_ERR_SUCCESS;
}


/* -------------------- Trunking Functions ----------------------------- */

void
sal_flash_trunk_init (void)
{
    sal_port_t  trkId;
    sal_pPortmask_t p_portMask;

    sal_pPortMask_delAll(&p_portMask);
    for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {
        sal_flash_trunk_port_set(trkId, FALSE, &p_portMask, &p_portMask);
    }
}
#if 0 //NOT used
int32
sal_flash_trunk_distributionAlgorithm_set(
    uint8 trkId,
    uint8 paraMsk
    )
{
    sal_flash_trunk_t flash_trunk;

    _SalFlshRead(&flash_trunk, FLSH_ADDR_TRK, sal_flash_trunk_t);
    flash_trunk.flashTrunkGroup[trkId].paraMsk= paraMsk;
    _SalFlshWrite(FLSH_ADDR_TRK, &flash_trunk, sal_flash_trunk_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_trunk_distributionAlgorithm_get(
    uint8 trkId,
    uint8 *pParaMsk
    )
{
    sal_flash_trunk_t flash_trunk;

    _SalFlshRead(&flash_trunk, FLSH_ADDR_TRK, sal_flash_trunk_t);
    *pParaMsk = flash_trunk.flashTrunkGroup[trkId].paraMsk;

    return SAL_ERR_SUCCESS;
}
#endif
int32
sal_flash_trunk_port_set (
    uint8 trkId,
    uint8 is_Lacp,
    sal_pPortmask_t *pPortMask,
    sal_pPortmask_t *pActMask
    )
{
    sal_flash_trunk_t flash_trunk;
    is_Lacp = 0; // kill compiler warning

    _SalFlshRead(&flash_trunk, FLSH_ADDR_TRK, sal_flash_trunk_t);
    sal_pPortMask_copy(&flash_trunk.flashTrunkGroup[trkId].portMask, pPortMask);
    sal_pPortMask_copy(&flash_trunk.flashTrunkGroup[trkId].actMask, pActMask);
    _SalFlshWrite(FLSH_ADDR_TRK, &flash_trunk, sal_flash_trunk_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_trunk_port_del (
    uint8 trkId
    )
{
    sal_flash_trunk_t flash_trunk;

    _SalFlshRead(&flash_trunk, FLSH_ADDR_TRK, sal_flash_trunk_t);
    sal_pPortMask_delAll(&flash_trunk.flashTrunkGroup[trkId].portMask);
    sal_pPortMask_delAll(&flash_trunk.flashTrunkGroup[trkId].actMask);
    _SalFlshWrite(FLSH_ADDR_TRK, &flash_trunk, sal_flash_trunk_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_trunk_port_get (
    uint8 trkId,
    uint8 *pIs_Lacp,
    sal_pPortmask_t *pPortMask,
    sal_pPortmask_t *pActMask
    )
{
    sal_flash_trunk_t flash_trunk;
    pIs_Lacp = NULL; // kill compiler warning

    _SalFlshRead(&flash_trunk, FLSH_ADDR_TRK, sal_flash_trunk_t);
    sal_pPortMask_copy(pPortMask, &flash_trunk.flashTrunkGroup[trkId].portMask);
    sal_pPortMask_copy(pActMask, &flash_trunk.flashTrunkGroup[trkId].actMask);
    
    return SAL_ERR_SUCCESS;
}

/* -------------------- VLAN Functions --------------------------------- */

void
sal_flash_vlan_init (void)
{
    sal_port_t  port;
    uint16      slot;
    sal_flash_vlanDb_t flashVlanDb;
    sal_flash_vlanEntry_t dftVlanEntry;

    sal_flash_vlan_enable_set(DFLT_VLAN_EBL);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        sal_flash_vlan_portPvid_set(port, DFLT_VLAN_PVID);
        sal_flash_vlan_tagMode_set(port, DFLT_VLAN_MODE);
        sal_flash_vlan_portAcceptFrameType_set(port, DFLT_VLAN_ACCEPT_TYPE);
    }

    /* clear static VLAN table entries in flash */
    for (slot = 0; slot < SAL_MAX_STATIC_VLAN; ++slot) {
        flashVlanDb.sortedSlot[slot] = -1;
        flashVlanDb.slotVid[slot] = -1;
        sal_flash_vlan_db_set(&flashVlanDb);
        sal_flash_vlan_sttcEntry_remove(slot);
    }

    /* insert default VLAN */
    sal_lPortMask_addAll(&dftVlanEntry.p_portMask);
    sal_pPortMask_delAll(&dftVlanEntry.p_tagMask);
    //strcpy(dftVlanEntry.name, "Default_VLAN");
    strcpy(dftVlanEntry.name, "");
    dftVlanEntry.vlan_id = DFLT_VLAN_PVID;
    dftVlanEntry.valid   = 1;
    sal_flash_vlan_sttcEntry_set(0, &dftVlanEntry);
    
    flashVlanDb.sortedSlot[0] = 0;
    flashVlanDb.slotVid[0] = DFLT_VLAN_PVID;
    sal_flash_vlan_db_set(&flashVlanDb);
}

int32
sal_flash_vlan_enable_set (
    sal_enable_t enable
    )
{
    sal_flash_vlan_t flashVlan;

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    flashVlan.enable = enable;
    _SalFlshWrite(FLSH_ADDR_VLAN, &flashVlan, sal_flash_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_enable_get (
    sal_enable_t *pEnable
    )
{
    sal_flash_vlan_t flashVlan;
    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    *pEnable = flashVlan.enable;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_tagMode_set(
    sal_port_t port,
    sal_vlan_tagMode_t mode
    )
{
    sal_flash_vlan_t flashVlan;
    sal_pPortmask_t  p_portMask;
    sal_port_t  p_portId;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (p_portId = 0; p_portId < SAL_PORT_ID_CPU; ++p_portId) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, p_portId)) {
            flashVlan.mode[p_portId] = mode;
        }
    }
    _SalFlshWrite(FLSH_ADDR_VLAN, &flashVlan, sal_flash_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_tagMode_get(
    sal_port_t port,
    sal_vlan_tagMode_t *pMode
    )
{
    sal_flash_vlan_t flashVlan;
    sal_port_t       p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);
    *pMode = flashVlan.mode[p_port];
    
    return SAL_ERR_SUCCESS;
}


int32
sal_flash_vlan_portPvid_set (
    sal_port_t port,
    sal_vlan_t vlan_id
    )
{
    sal_flash_vlan_t flashVlan;
    sal_pPortmask_t  physicPortMask;
    sal_port_t       physicPortId;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicMask(&physicPortMask, port);
    for (physicPortId = 0; physicPortId < SAL_PORT_ID_CPU; ++physicPortId) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&physicPortMask, physicPortId)) {
            flashVlan.pvid[physicPortId] = vlan_id;
        }
    }
    _SalFlshWrite(FLSH_ADDR_VLAN, &flashVlan, sal_flash_vlan_t);
    return SAL_ERR_SUCCESS;
}


int32
sal_flash_vlan_portPVId_get (
    sal_port_t port,
    sal_vlan_t *pVlanId
    )
{
    sal_flash_vlan_t flashVlan;
    sal_port_t  physicPortId;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicId(&physicPortId, port);
    *pVlanId = flashVlan.pvid[physicPortId];
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_portAcceptFrameType_set (
    sal_port_t port,
    sal_vlan_acceptFrameType_t type
    )
{
    sal_flash_vlan_t flashVlan;
    sal_pPortmask_t  p_portMask;
    sal_port_t  physicPortId;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (physicPortId = 0; physicPortId < SAL_PORT_ID_CPU; ++physicPortId) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&p_portMask, physicPortId)) {
            flashVlan.accpetType[physicPortId] = type;
        }
    }
    _SalFlshWrite(FLSH_ADDR_VLAN, &flashVlan, sal_flash_vlan_t);
    return SAL_ERR_SUCCESS;

}


int32
sal_flash_vlan_portAcceptFrameType_get (
    sal_port_t port,
    sal_vlan_acceptFrameType_t *pType
    )
{
    sal_flash_vlan_t flashVlan;
    uint8   p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&flashVlan, FLSH_ADDR_VLAN, sal_flash_vlan_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);
    *pType = flashVlan.accpetType[p_port];

    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_db_set (
    sal_flash_vlanDb_t *pDb
    )
{
    _SalFlshWrite(FLSH_ADDR_VLAN_TMP, pDb, sal_flash_vlanDb_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_db_get (
    sal_flash_vlanDb_t *pDb
    )
{
    _SalFlshRead(pDb, FLSH_ADDR_VLAN_TMP, sal_flash_vlanDb_t);
    return SAL_ERR_SUCCESS;
}
#if 0 //NOT used
int32
sal_flash_vlan_sttcEntry_create (
    uint16 slot,
    sal_flash_vlanEntry_t *pVlanEntry
    )
{
    if (slot > SAL_MAX_STATIC_VLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    _SalFlshWrite(FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), pVlanEntry, sal_flash_vlanEntry_t);
    return SAL_ERR_SUCCESS;
}
#endif
int32
sal_flash_vlan_sttcEntry_set (
    uint16 slot,
    sal_flash_vlanEntry_t *pVlanEntry
    )
{
#if 0
    sal_flash_vlanEntry_t oldVlanEntry;
    
    if (slot > SAL_MAX_STATIC_VLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }
    
    _SalFlshRead(&oldVlanEntry, FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), sal_flash_vlanEntry_t);
    
    strcpy(pVlanEntry->name, oldVlanEntry.name);
    
    _SalFlshWrite(FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), pVlanEntry, sal_flash_vlanEntry_t);
    return SAL_ERR_SUCCESS;
#endif    

    if (slot > SAL_MAX_STATIC_VLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    _SalFlshWrite(FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), pVlanEntry, sal_flash_vlanEntry_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_sttcEntry_remove (
    uint16  slot
    )
{
    sal_flash_vlanEntry_t   stEntry;

    if (slot > SAL_MAX_STATIC_VLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    memset(&stEntry, 0, sizeof(sal_flash_vlanEntry_t));
    _SalFlshWrite(FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), &stEntry, sal_flash_vlanEntry_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_vlan_sttcEntry_get (
    uint16 slot,
    sal_flash_vlanEntry_t *pVlanEntry
    )
{
    if (slot > SAL_MAX_STATIC_VLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    _SalFlshRead(pVlanEntry, FLSH_ADDR_VLAN_TBL+slot*sizeof(sal_flash_vlanEntry_t), sal_flash_vlanEntry_t);
    
    return SAL_ERR_SUCCESS;
}

#if SAL_SERVICE_VLAN
void
sal_flash_svlan_init ()
{
    uint16  usSlot;
    sal_flash_svlanDb_t     stFlshVlanTmp;
    sal_vlan_t              tpid;
    sal_pPortmask_t         portMask;
    sal_flash_svlanEntry_t  svlan_entry;

    /* clear static SVLAN table entries in flash */
    for (usSlot = 0; usSlot < SAL_MAX_STATIC_SVLAN; ++usSlot) {
        stFlshVlanTmp.sortedSlot[usSlot] = -1;
        stFlshVlanTmp.slotVid[usSlot] = -1;
        sal_flash_svlan_db_set(&stFlshVlanTmp);
        sal_flash_svlan_sttcEntry_remove(usSlot);
    }
    
    tpid = DFLT_SVLAN_TPID;
    sal_flash_svlan_tpid_set(&tpid);
    
    memset(&portMask, 0, sizeof(sal_pPortmask_t));
    portMask.bits[0] = DFLT_SVLAN_UPLINK_MSK;
    sal_flash_svlan_uplinkMask_set(&portMask);

    /* insert default SVLAN */
    sal_pPortMask_addAll(&svlan_entry.p_portMask);
    strcpy(svlan_entry.name, "Default_SVLAN");
    svlan_entry.vlan_id = DFLT_SVLAN_PVID;
    svlan_entry.valid   = TRUE;
    sal_flash_svlan_sttcEntry_set(0, &svlan_entry);

    stFlshVlanTmp.slotVid[0] = DFLT_SVLAN_PVID;
    stFlshVlanTmp.sortedSlot[0] = 0;

    sal_flash_svlan_db_set(&stFlshVlanTmp);
}

int32
sal_flash_svlan_db_set (
    sal_flash_svlanDb_t *pstTmp
    )
{
    _SalFlshWrite(FLSH_ADDR_SVLAN_TMP, pstTmp, sal_flash_svlanDb_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_db_get (
    sal_flash_svlanDb_t *pstTmp
    )
{
    _SalFlshRead(pstTmp, FLSH_ADDR_SVLAN_TMP, sal_flash_svlanDb_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_svid_set (
    sal_vlan_t *pstSvid
    )
{
    _SalFlshWrite(FLSH_ADDR_SVLAN_DFT, pstSvid, sal_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_svid_get (
    sal_vlan_t *pstSvid
    )
{
    _SalFlshRead(pstSvid, FLSH_ADDR_SVLAN_DFT, sal_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_tpid_set (
    sal_vlan_t *tpid
    )
{
    _SalFlshWrite(FLSH_ADDR_SVLAN_TPID, tpid, sal_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_tpid_get (
    sal_vlan_t *pstTpid
    )
{
    _SalFlshRead(pstTpid, FLSH_ADDR_SVLAN_TPID, sal_vlan_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_uplinkMask_set (
    sal_pPortmask_t *pPortMask
    )
{
    _SalFlshWrite(FLSH_ADDR_SVLAN_UPLINK, pPortMask, sal_pPortmask_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_uplinkMask_get (
    sal_pPortmask_t *pPortMask
    )
{
    _SalFlshRead(pPortMask, FLSH_ADDR_SVLAN_UPLINK, sal_pPortmask_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_sttcEntry_set (
    uint16  usSlot,
    sal_flash_svlanEntry_t *pSVlanEntry
    )
{
    if (usSlot > SAL_MAX_STATIC_SVLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }
    
    _SalFlshWrite(FLSH_ADDR_SVLAN_TBL+usSlot*sizeof(sal_flash_svlanEntry_t), pSVlanEntry, sal_flash_svlanEntry_t);
    return SAL_ERR_SUCCESS;
}


int32
sal_flash_svlan_sttcEntry_remove (
    uint16  usSlot
    )
{
    sal_flash_svlanEntry_t  stEntry;

    if (usSlot > SAL_MAX_STATIC_SVLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    memset(&stEntry, 0, sizeof(sal_flash_svlanEntry_t));
    _SalFlshWrite(FLSH_ADDR_SVLAN_TBL+usSlot*sizeof(sal_flash_svlanEntry_t), &stEntry, sal_flash_svlanEntry_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_svlan_sttcEntry_get (
    uint16  slot,
    sal_flash_svlanEntry_t *pSVlanEntry
    )
{
    if (slot > SAL_MAX_STATIC_SVLAN-1) {
        return SAL_ERR_ENTRY_INDEX;
    }

    _SalFlshRead(pSVlanEntry, FLSH_ADDR_SVLAN_TBL+slot*sizeof(sal_flash_svlanEntry_t), sal_flash_svlanEntry_t);

    return SAL_ERR_SUCCESS;
}

#endif
void
sal_flash_qos_init ()
{
    sal_lPortmask_t    stLPortMask;
    sal_qos_queue_weights_t qweights;
    uint8          ucPortId;
    uint8          i;
#if defined(INTERNAL_PRIORITY)
    sal_flash_qos_pri_selet_t priSel;
    sal_dscp_t j;
    sal_flash_qos_pri2queue_t pri2queue;
#endif

    sal_trunk_logicPort_setAll(&stLPortMask); 

    for (i = 0; i < SAL_MAX_NUM_OF_QUEUE; i++)
    {
        qweights.weights[i] = DFLT_QOS_SCHED_WEIGHT;     
    }

    for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ucPortId++)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&stLPortMask, ucPortId))
            continue;

        sal_flash_qos_portPri_set(ucPortId, DFLT_QOS_PORT_TO_INTERNAL_PRI);
        sal_flash_qos_queueNum_set(ucPortId, DFLT_QOS_QUEUE_NUMBER);
        sal_flash_qos_schedulingAlgo_set(ucPortId, DFLT_QOS_SCHED_ALGORITHM);
        sal_flash_qos_schedulingQueue_set(ucPortId, &qweights);   
    }

#if SAL_DSCP_REMARKING
    for(i = 0; i < SAL_MAX_NUM_OF_QUEUE;i++)
    {
        sal_flash_qos_dscpRemarking_set(i, 0, SAL_DISABLED);
    }
#endif    
#if defined(INTERNAL_PRIORITY)
    priSel.acl_pri = 7UL;
    priSel.cvlan_pri = 0;
    priSel.dmac_pri = 0UL;
    priSel.dot1q_pri = 0UL;
    priSel.dscp_pri = 0UL;
    priSel.port_pri = 6UL;
    priSel.smac_pri = 0UL;
    priSel.svlan_pri = 0UL;
    sal_flash_qos_priSel_set(&priSel);

    for(j = 0; j < SAL_MAX_VALUE_OF_DSCP; j++)
    {
        sal_flash_qos_dscpPriRemap_set(j, 0);
    }

    pri2queue.pri2queue[0] = 0UL;
    pri2queue.pri2queue[1] = 0UL;
    pri2queue.pri2queue[2] = 1UL;
    pri2queue.pri2queue[3] = 1UL;
    pri2queue.pri2queue[4] = 2UL;
    pri2queue.pri2queue[5] = 2UL;
    pri2queue.pri2queue[6] = 3UL;
    pri2queue.pri2queue[7] = 3UL;

    sal_flash_qos_priMap_set(&pri2queue);
#endif
}

int32
_sal_flash_qos_portTable_get(sal_flash_qos_port_table_t *pstQosPortTbl)
{
    _SalFlshRead(pstQosPortTbl, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_portPri_set (
    sal_port_t port,
    sal_pri_t intPri
    )
{
    sal_flash_qos_port_table_t  portPri;
    sal_pPortmask_t pPortMask;
    uint8   pPortId;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&portPri, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicMask(&pPortMask, port);

    for (pPortId = 0; pPortId < SAL_PORT_ID_CPU; ++pPortId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, pPortId))
        {
            portPri.qosPort[pPortId].pri = intPri;
        }
    }
    _SalFlshWrite(FLSH_ADDR_QOS_PORT_TBL, &portPri, sal_flash_qos_port_table_t);

    return SAL_ERR_SUCCESS;
}

#if 0
int32
sal_flash_qos_portPri_get (
    sal_port_t port,
    sal_pri_t *pIntPri
    )
{
    sal_flash_qos_port_table_t  portPri;
    uint8   pPortId;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&portPri, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicId(&pPortId, port);
    *pIntPri = portPri.qosPort[pPortId].pri;
    return SAL_ERR_SUCCESS;
}
#endif

int32
sal_flash_qos_queueNum_set (
    sal_port_t port,
    uint8 queueNum
    )
{
    sal_pPortmask_t pPortMask;
    uint8   pPortId;
    sal_flash_qos_port_table_t portQNum;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&portQNum, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicMask(&pPortMask, port);

    for (pPortId = 0; pPortId < SAL_PORT_ID_CPU; ++pPortId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, pPortId))
        {
            portQNum.qosPort[pPortId].numOfQ = queueNum;
        }
    }
    _SalFlshWrite(FLSH_ADDR_QOS_PORT_TBL, &portQNum, sal_flash_qos_port_table_t);
    return SAL_ERR_SUCCESS;
}


int32
sal_flash_qos_queueNum_get (
    sal_port_t port,
    uint8 *pQueueNum
    )
{
    uint8   pPortId;
    sal_flash_qos_port_table_t portQNum;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&portQNum, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicId(&pPortId, port);
    *pQueueNum = portQNum.qosPort[pPortId].numOfQ;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_schedulingAlgo_set (
    sal_port_t port,
    sal_qos_scheduling_type_t scheduling_type
    )
{
    sal_pPortmask_t pPortMask;
    uint8   pPortId;
    sal_flash_qos_port_table_t portSche;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&portSche, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicMask(&pPortMask, port);

    for (pPortId = 0; pPortId < SAL_PORT_ID_CPU; ++pPortId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, pPortId))
        {
            portSche.qosPort[pPortId].scheAlg = scheduling_type;
        }
    }
    _SalFlshWrite(FLSH_ADDR_QOS_PORT_TBL, &portSche, sal_flash_qos_port_table_t);

    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_schedulingQueue_set (
    sal_port_t port,
    sal_qos_queue_weights_t *pQweights
    )
{
    sal_pPortmask_t pPortMask;
    uint8   pPortId, index;
    sal_flash_qos_port_table_t scheQWeight;

    if (port > SAL_PORT_ID_CPU - 1)
        return SAL_ERR_PORT_RANGE;

    _SalFlshRead(&scheQWeight, FLSH_ADDR_QOS_PORT_TBL, sal_flash_qos_port_table_t);
    sal_trunk_logicId_toPhysicMask(&pPortMask, port);

    for (pPortId = 0; pPortId < SAL_PORT_ID_CPU; ++pPortId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, pPortId))
        {
            for (index = 0; index < SAL_MAX_NUM_OF_QUEUE; index++)
            {
                scheQWeight.qosPort[pPortId].weight[index] = pQweights->weights[index];
            }
        }
    }
    _SalFlshWrite(FLSH_ADDR_QOS_PORT_TBL, &scheQWeight, sal_flash_qos_port_table_t);

    return SAL_ERR_SUCCESS;
}

#if defined(INTERNAL_PRIORITY)
int32
sal_flash_qos_priSel_set (
    sal_flash_qos_pri_selet_t *pPriSel
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    memcpy(&qosTable.priSelection, pPriSel, sizeof(sal_flash_qos_pri_selet_t));
    _SalFlshWrite(FLSH_ADDR_QOS_TBL_BEGIN, &qosTable, sal_flash_qos_table_t);
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_priSel_get (
    sal_flash_qos_pri_selet_t *priSel
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    memcpy(priSel, &qosTable.priSelection, sizeof(sal_flash_qos_pri_selet_t));
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_dscpPriRemap_set (
    sal_dscp_t dscp, sal_pri_t int_pri
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    qosTable.dscpRemaping.intPri[dscp] = int_pri;
    _SalFlshWrite(FLSH_ADDR_QOS_TBL_BEGIN, &qosTable, sal_flash_qos_table_t);
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_dscpPriRemap_get (
    sal_dscp_t dscp, sal_pri_t *int_pri
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    *int_pri = qosTable.dscpRemaping.intPri[dscp];
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_priMap_set (
    sal_flash_qos_pri2queue_t *pri2queue
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    memcpy(&qosTable.pri2queue, pri2queue, sizeof(sal_flash_qos_pri2queue_t));
    _SalFlshWrite(FLSH_ADDR_QOS_TBL_BEGIN, &qosTable, sal_flash_qos_table_t);
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_qos_priMap_get (
    sal_flash_qos_pri2queue_t *pri2queue
    )
{
    sal_flash_qos_table_t qosTable;

    _SalFlshRead(&qosTable, FLSH_ADDR_QOS_TBL_BEGIN, sal_flash_qos_table_t);
    memcpy(pri2queue, &qosTable.pri2queue, sizeof(sal_flash_qos_pri2queue_t));
    
    return SAL_ERR_SUCCESS;
}
#endif

#if SAL_DSCP_REMARKING

/* Function Name:
 *      sal_flash_qos_dscpRemarking_set
 * Description:
 *      Save DSCP remarking configuration to flash.
 * Input:
 *      qid  - queue ID
 *      dscp - remarked DSCP value
 *      enable - Global DSCP Remarking function  
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
int32 sal_flash_qos_dscpRemarking_set(uint8 qid, uint8 dscp, uint8 enable)
{
    sal_flash_qos_dscp_remarking_t dscp_cfg;

    if(qid >= SAL_MAX_NUM_OF_QUEUE || dscp >= SAL_MAX_VALUE_OF_DSCP || enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;

    _SalFlshRead(&dscp_cfg, FLSH_ADDR_DSCP_BEGIN, sal_flash_qos_dscp_remarking_t);
    dscp_cfg.dscp[qid] = dscp;
    dscp_cfg.dscp_enable = enable;

    _SalFlshWrite(FLSH_ADDR_DSCP_BEGIN, &dscp_cfg, sal_flash_qos_dscp_remarking_t);

    _SalFlshRead(&dscp_cfg, FLSH_ADDR_DSCP_BEGIN, sal_flash_qos_dscp_remarking_t);    
}

/* Function Name:
 *      sal_flash_qos_dscpRemarking_get
 * Description:
 *      Retrieve DSCP remarking configuration from flash.
 * Input:
 *      qid    - queue ID
 * Output:
 *      pDscp   - pointer to remarked DSCP value
 *      pEnable - pointer to global DSCP Remarking function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
int32 sal_flash_qos_dscpRemarking_get(uint8 qid, uint8 *pDscp, uint8 *pEnable)
{
    sal_flash_qos_dscp_remarking_t dscp_cfg;
    
    if(qid >= SAL_MAX_NUM_OF_QUEUE)
        return SAL_ERR_PARAMETER;
    
    _SalFlshRead(&dscp_cfg, FLSH_ADDR_DSCP_BEGIN, sal_flash_qos_dscp_remarking_t);

    *pDscp = dscp_cfg.dscp[qid];
    *pEnable = dscp_cfg.dscp_enable;
}
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
int32 sal_flash_cfg_save()
{
    /*erase Configuration sector*/
    //flash_configSpace_erase();
    FLASH_ERASE_SECTOR(FLSH_CONFIG_ADDR_START);
    
    //flash_sect_erase((Tuint32)CONFIG_START, CONFIG_START+CONFIG_SIZE-1);
    /*erase cache to Configuration sector*/
    //flash_write_sec((int8 *)&configCache, (uint32)FLSH_CONFIG_ADDR_START, sizeof(ConfigCache));
    FLASH_WRITE((uint32)FLSH_CONFIG_ADDR_START, (int8 *)&configCache, sizeof(ConfigCache));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_cfg_reset()
{
    //SalFlshInit();
    uint8 xdata buf[4];
    char devDesc[32] = "OPL SMART PDU RACK";
    printf("\nResetting device type to default\n");
    sal_flash_device_type_set(devDesc);
    memset(&configCache, 0x00, sizeof(ConfigCache));
    FLASH_WRITE((uint32)FLSH_CONFIG_ADDR_START, (int8 *)&configCache, sizeof(ConfigCache));    
	
	//=====================================================================
    FLASH_READ((uint32)(FLSH_CONFIG_DUMMY_START), buf, sizeof(buf));
    
    memset(&buf[0], 0, sizeof(buf));
    memcpy(&buf[0], "NEW ", 4);
    
    FLASH_ERASE_SECTOR(FLSH_CONFIG_DUMMY_START);
    FLASH_WRITE((uint32)FLSH_CONFIG_DUMMY_START, buf, 4);
	
	//===================================================================
    SalFlshInit_except_Mac();
    /*a default cache is created, copy cache to flash*/
    sal_flash_cfg_save();

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_cfg_set(uint16 off, uint16 length, uint8* pData)
{
    if(NULL == pData)
        return SAL_ERR_NULL_POINTER;
    if(off + length > sizeof(configCache))
        return SAL_ERR_PARAMETER;

    memcpy((uint8*)&configCache + off, pData, length);

    return SAL_ERR_SUCCESS;
}
#if 0 //NOT used
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
int32 sal_flash_cfg_get(uint16 off, uint16 length, uint8* pData)
{
    if(NULL == pData)
        return SAL_ERR_NULL_POINTER;
    if(off + length > sizeof(configCache))
        return SAL_ERR_PARAMETER;

    memcpy(pData, &configCache + off, length);

    return SAL_ERR_SUCCESS;
}
#endif

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
int32 sal_flash_cfgSize_get(uint16 *pSize)
{
    *pSize = sizeof(configCache);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_cfgAddr_get(uint8 **ppConfig)
{
    if(NULL == ppConfig)
        return SAL_ERR_NULL_POINTER;

    *ppConfig = (uint8*)&configCache;

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_cfgValidity_get(uint8 * pConfig, sal_flash_configValidity_t *pStatus)
{
    if(NULL == pConfig || NULL == pStatus)
        return SAL_ERR_NULL_POINTER;

    if(0 == strncmp(pConfig, FLSH_SIGNATURE_STR, 4))
    {
        *pStatus = SAL_FLASH_CONFIG_VALID;
    }
    else
    {
        *pStatus = SAL_FLASH_CONFIG_INVALID;    
    }

    return SAL_ERR_SUCCESS;
}


#if SAL_LOOP

void
sal_flash_loop_init (void)
{
    sal_loop_mode_t mode;

    mode = SAL_LOOP_MODE_DISABLE;
    sal_flash_loop_set(&mode);
}


int32
sal_flash_loop_get (
    sal_loop_mode_t *pMode
    )
{
    sal_flash_loop_t rlpp;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    *pMode = rlpp.loopMode;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_set (
    sal_loop_mode_t *pMode
    )
{
    sal_flash_loop_t rlpp;

    if(*pMode >= SAL_LOOP_MODE_END)
        return SAL_ERR_FAILED;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    rlpp.loopMode = *pMode;
    _SalFlshWrite(FLSH_ADDR_LOOP_BEGIN, &rlpp, sal_flash_loop_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_interval_time_get(uint16 *pTime)
{
    sal_flash_loop_t rlpp;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    *pTime = rlpp.interval_time;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_interval_time_set(uint16 time)
{
    sal_flash_loop_t rlpp;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    rlpp.interval_time = time;
    _SalFlshWrite(FLSH_ADDR_LOOP_BEGIN, &rlpp, sal_flash_loop_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_recover_time_get(uint32 *pTime)
{
    sal_flash_loop_t rlpp;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    *pTime = rlpp.recover_time;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_recover_time_set(uint32 time)
{
    sal_flash_loop_t rlpp;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    rlpp.recover_time= time;
    _SalFlshWrite(FLSH_ADDR_LOOP_BEGIN, &rlpp, sal_flash_loop_t);
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_port_enable_get(uint8 ucPortId, uint8 *pEnable)
{
    sal_flash_loop_t rlpp;
    sal_port_t phyPort;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);

    sal_trunk_logicId_toPhysicId(&phyPort, ucPortId);
    if(0 != (rlpp.rlpp_en_pmsk & (1UL << phyPort)))
        *pEnable = 1;
    else
        *pEnable = 0;
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_loop_port_enable_set(uint8 ucPortId, uint8 enable)
{
    sal_flash_loop_t rlpp;
    sal_pPortmask_t phyPm;
    sal_port_t phyPort;

    _SalFlshRead(&rlpp, FLSH_ADDR_LOOP_BEGIN, sal_flash_loop_t);
    sal_trunk_logicId_toPhysicMask(&phyPm, ucPortId);
    for (phyPort = 0; phyPort < SAL_PORT_ID_CPU; ++phyPort) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&phyPm, phyPort)) 
        {
            rlpp.rlpp_en_pmsk &= ~(1UL << phyPort);
            if(enable)
                rlpp.rlpp_en_pmsk |= (1UL << phyPort);
        }
    }
    _SalFlshWrite(FLSH_ADDR_LOOP_BEGIN, &rlpp, sal_flash_loop_t);
    return SAL_ERR_SUCCESS;
}
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
void
sal_flash_stp_init (void)
{
    sal_flash_stp_t     init_info;
    sal_lPortmask_t     portmask;
    sal_port_t          port;

    memset(&init_info, 0, sizeof(sal_flash_stp_t));

    init_info.stp_ver = DFLT_STP_VERSION;
    init_info.stp_pri = DFLT_STP_PRI;
    init_info.stp_age = DFLT_STP_AGE;
    init_info.stp_hello = DFLT_STP_HELLO;
    init_info.stp_fwd = DFLT_STP_FWD;

    sal_trunk_logicPort_setAll(&portmask);
    
    for (port = 0; port < SAL_PORT_ID_CPU; port++)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portmask, port))
            continue;

        init_info.stp_port[port].edge = DFLT_STP_PORT_EDGE;
        init_info.stp_port[port].p2p  = DFLT_STP_PORT_P2P;
        init_info.stp_port[port].pathCost = DFLT_STP_PORT_PATHCOST;
        init_info.stp_port[port].priority = DFLT_STP_PORT_PRI;  
    }

    _SalFlshWrite(FLSH_ADDR_RSTP_BEGIN, &init_info, sal_flash_stp_t);
}  

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
int32
sal_flash_stp_version_get (
    sal_stp_version_t *version
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    *version = cache.stp_ver;
    return SAL_ERR_SUCCESS;
}

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
int32
sal_flash_stp_version_set (
    sal_stp_version_t version
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    cache.stp_ver = version;

    _SalFlshWrite(FLSH_ADDR_RSTP_BEGIN, &cache, sal_flash_stp_t);
    return SAL_ERR_SUCCESS;
}

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
int32
sal_flash_stp_priority_get (
    sal_stp_pri_t *pPiority
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    *pPiority = cache.stp_pri;
    return SAL_ERR_SUCCESS;
}

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
int32
sal_flash_stp_priority_set (
    sal_stp_pri_t priority
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    cache.stp_pri = priority;

    _SalFlshWrite(FLSH_ADDR_RSTP_BEGIN, &cache, sal_flash_stp_t);
    return SAL_ERR_SUCCESS;
}

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
int32
sal_flash_stp_timePara_get (
    sal_stp_time_t *pMaxAge, 
    sal_stp_time_t *pHelloTime, 
    sal_stp_time_t *pFwdDelay
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    *pMaxAge    = cache.stp_age;
    *pHelloTime = cache.stp_hello;
    *pFwdDelay  = cache.stp_fwd;
    return SAL_ERR_SUCCESS;
}

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
int32
sal_flash_stp_timePara_set (
    sal_stp_time_t maxAge, 
    sal_stp_time_t helloTime, 
    sal_stp_time_t fwdDelay
    )
{
    sal_flash_stp_t  cache;
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    cache.stp_age   = maxAge;
    cache.stp_hello = helloTime;
    cache.stp_fwd   = fwdDelay;
    
    _SalFlshWrite(FLSH_ADDR_RSTP_BEGIN, &cache, sal_flash_stp_t);
    return SAL_ERR_SUCCESS;
}
    
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
int32
sal_flash_stp_port_get (
    sal_port_t port,
    sal_stp_portPara_t *pPort_config
    )
{
    sal_flash_stp_t  cache;
    uint8   p_port;

    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }
    
    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);
    sal_trunk_logicId_toPhysicId(&p_port, port);

    *pPort_config = cache.stp_port[p_port];

    return SAL_ERR_SUCCESS;
}   

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
int32
sal_flash_stp_port_set (
    sal_port_t port,
    sal_stp_portPara_t* pPortPara
    )
{
    sal_flash_stp_t  cache;
    sal_pPortmask_t  portmask;
    sal_port_t  portid;

    if (pPortPara == NULL)
        return SAL_ERR_NULL_POINTER;
        
    if (port > SAL_PORT_ID_CPU - 1) {
        return SAL_ERR_PORT_RANGE;
    }

    _SalFlshRead(&cache, FLSH_ADDR_RSTP_BEGIN, sal_flash_stp_t);

    sal_trunk_logicId_toPhysicMask(&portmask, port);
    for (portid = 0; portid < SAL_PORT_ID_CPU; ++portid) {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portmask, portid)) {
            cache.stp_port[portid] = *pPortPara;
        }
    }
    _SalFlshWrite(FLSH_ADDR_RSTP_BEGIN, &cache, sal_flash_stp_t);
    return SAL_ERR_SUCCESS;

}

#endif

#if SAL_EEE
/* Function Name:
 *      sal_flash_eee_set
 * Description:
 *      Set EEE configuration in the flash.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_flash_eee_set(sal_enable_t eeeEnable)
{
    sal_flash_eee_t eee;

    if(eeeEnable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;

    eee.enable = eeeEnable;
    
    _SalFlshWrite(FLSH_ADDR_EEE_BEGIN, &eee, sal_flash_eee_t);
    
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_eee_get (
    sal_enable_t *pEeeEnable
    )
{
    sal_flash_eee_t eee;

    if(NULL == pEeeEnable)
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&eee, FLSH_ADDR_EEE_BEGIN, sal_flash_eee_t);

    *pEeeEnable = eee.enable;
    
    return SAL_ERR_SUCCESS;
}


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
void sal_flash_eee_init (void)
{
    sal_flash_eee_set(SAL_DISABLED);
}  

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
int32 sal_flash_greenEth_port_enable_set (sal_enable_t enable)
{
    sal_flash_greeneth_t greeneth;
    
    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;
    
    greeneth.enable = enable;
    
    _SalFlshWrite(FLSH_ADDR_GREENETH_BEGIN, &greeneth, sal_flash_greeneth_t);
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_greenEth_port_enable_get (sal_enable_t *pEnable)
{
    sal_flash_greeneth_t greeneth;

    if(NULL == pEnable)
        return SAL_ERR_NULL_POINTER;
    
    _SalFlshRead(&greeneth, FLSH_ADDR_GREENETH_BEGIN, sal_flash_greeneth_t);
    
    *pEnable = greeneth.enable;
    
    return SAL_ERR_SUCCESS;

}

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
void sal_flash_greenEth_init (void)
{
    sal_flash_greenEth_port_enable_set(SAL_DISABLED);
}

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
int32 sal_flash_lldp_set (sal_enable_t enable)
{
    sal_flash_lldp_t lldp;
    
    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;
    
    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);

    lldp.enable = enable;
    
    _SalFlshWrite(FLSH_ADDR_GREENETH_BEGIN, &lldp, sal_flash_lldp_t);
    
    return SAL_ERR_SUCCESS;

}

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
int32 sal_flash_lldp_get (sal_enable_t *pEnable)
{
    sal_flash_lldp_t lldp;

    if(NULL == pEnable)
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);
    
    *pEnable = lldp.enable;

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_lldp_time_set (sal_lldp_timer_t txIntvl, sal_lldp_multi_t holdTimeMulti)
{
    sal_flash_lldp_t lldp;
    
    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);
    
    lldp.txIntvl     = txIntvl;
    lldp.holdMulti = holdTimeMulti;
    
    _SalFlshWrite(FLSH_ADDR_GREENETH_BEGIN, &lldp, sal_flash_lldp_t);
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_lldp_time_get(sal_lldp_timer_t *pTxIntvl, sal_lldp_multi_t *pHoldTimeMulti)
{
    sal_flash_lldp_t lldp;
    
    if(NULL == pTxIntvl || NULL == pHoldTimeMulti)
        return SAL_ERR_NULL_POINTER;
    
    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);
    
    *pTxIntvl = lldp.txIntvl;
    *pHoldTimeMulti = lldp.holdMulti;
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_lldp_portState_set(sal_port_t port, sal_lldp_port_state_t state)
{
    sal_flash_lldp_t lldp;
    
    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);

    lldp.state[port] = state;
    
    _SalFlshWrite(FLSH_ADDR_GREENETH_BEGIN, &lldp, sal_flash_lldp_t);
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_lldp_portState_get(sal_port_t port, sal_lldp_port_state_t *pState)
{
    sal_flash_lldp_t lldp;
    
    if(NULL == pState)
        return SAL_ERR_NULL_POINTER;
    
    _SalFlshRead(&lldp, FLSH_ADDR_LLDP_BEGIN, sal_flash_lldp_t);
    
    *pState = lldp.state[port];
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_lldp_init()
{
    sal_port_t port;

    sal_flash_lldp_set(SAL_DISABLED);
    sal_flash_lldp_time_set(DFLT_LLDP_TX_INTVL, DFLT_LLDP_HOLD_MULTI);

    for(port = 0; port < SAL_MAX_PHY_PORT;port++)
        sal_flash_lldp_portState_set(port, DFLT_LLDP_PORT_STATE);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_community_set(char *pReadComm, char *pWriteComm)
{
    sal_flash_snmp_t stFlshSnmp;
    
    if(pReadComm == NULL || pWriteComm == NULL)
        return SAL_ERR_NULL_POINTER;

    if(strlen(pReadComm) > SAL_SNMP_COMMUNITY_STR_LEN || strlen(pWriteComm) > SAL_SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_PARAMETER;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memcpy(stFlshSnmp.read_community, pReadComm, SAL_SNMP_COMMUNITY_STR_LEN);
    memcpy(stFlshSnmp.write_community, pWriteComm, SAL_SNMP_COMMUNITY_STR_LEN);
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_community_get(char *pReadComm, char *pWriteComm)
{
    sal_flash_snmp_t stFlshSnmp;
    
    if(pReadComm == NULL || pWriteComm == NULL)
        return SAL_ERR_NULL_POINTER;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memcpy(pReadComm, stFlshSnmp.read_community, SAL_SNMP_COMMUNITY_STR_LEN);
    memcpy(pWriteComm, stFlshSnmp.write_community, SAL_SNMP_COMMUNITY_STR_LEN);
   
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_trapIp_set(sal_ipaddr_t trapIp)
{
    sal_flash_snmp_t stFlshSnmp;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    stFlshSnmp.trapIp = trapIp;
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_trapIp_get(sal_ipaddr_t *pTrapIp)
{
    sal_flash_snmp_t stFlshSnmp;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    *pTrapIp = stFlshSnmp.trapIp;
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_state_set(sal_enable_t enable)
{
    sal_flash_snmp_t stFlshSnmp;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    stFlshSnmp.enable = enable;
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_state_get(sal_enable_t *enable)
{
    sal_flash_snmp_t stFlshSnmp;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    *enable = stFlshSnmp.enable;
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_snmp_init()
{
    sal_flash_snmp_state_set(DFLT_SNMP_ENABLE);
    sal_flash_snmp_community_set(DFLT_SNMP_READ_COMMUNITY, DFLT_SNMP_WRITE_COMMUNITY);
    sal_flash_sys_description_set(DFLT_SNMP_DESCRIPTION);
    sal_flash_sys_contact_set(DFLT_SNMP_CONTACT);
    sal_flash_sys_location_set(DFLT_SNMP_LOCATION);
    sal_flash_snmp_trapIp_set(DFLT_SNMP_TRAP_IP);

    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_flash_sys_description_get
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
 *      String length should be 32
 */
int32 sal_flash_sys_description_get(char *pDescripion)
{
    sal_flash_snmp_t stFlshSnmp;
    
    if(pDescripion == NULL)
        return SAL_ERR_NULL_POINTER;
    
    memset(pDescripion, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memcpy(pDescripion, stFlshSnmp.extSysinfo.description, (SAL_SNMP_SYS_INFO_STR_LEN - 1));
   
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_flash_sys_location_get
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
 *      String length should be 32
 */
int32 sal_flash_sys_location_get(char *pLocation)
{
    sal_flash_snmp_t stFlshSnmp;
    
    if(pLocation == NULL)
        return SAL_ERR_NULL_POINTER;
    
    memset(pLocation, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memcpy(pLocation, stFlshSnmp.extSysinfo.location, (SAL_SNMP_SYS_INFO_STR_LEN - 1));
   
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_flash_sys_contact_get
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
 *      String length should be 32
 */
int32 sal_flash_sys_contact_get(char *pContact)
{
    sal_flash_snmp_t stFlshSnmp;
    
    if(pContact == NULL)
        return SAL_ERR_NULL_POINTER;
    
    memset(pContact, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memcpy(pContact, stFlshSnmp.extSysinfo.contact, (SAL_SNMP_SYS_INFO_STR_LEN - 1));
   
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_sys_description_set(char *pDescription)
{
    sal_flash_snmp_t stFlshSnmp;
    uint8 strSize = 0;
    
    if(pDescription == NULL)
        return SAL_ERR_NULL_POINTER;

    strSize = strlen(pDescription);
    if(strSize == 0)
        return SAL_ERR_NULL_POINTER;

    if(strSize >= SAL_SNMP_SYS_INFO_STR_LEN)
        strSize = SAL_SNMP_SYS_INFO_STR_LEN - 1;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memset(stFlshSnmp.extSysinfo.description, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(stFlshSnmp.extSysinfo.description, pDescription, strSize);
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);
   
    return SAL_ERR_SUCCESS;
}


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
int32 sal_flash_sys_contact_set(char *pContact)
{
    sal_flash_snmp_t stFlshSnmp;
    uint8 strSize = 0;
    
    if(pContact == NULL)
        return SAL_ERR_NULL_POINTER;
    
    strSize = strlen(pContact);
    if(strSize == 0)
        return SAL_ERR_NULL_POINTER;

    if(strSize >= SAL_SNMP_SYS_INFO_STR_LEN)
        strSize = SAL_SNMP_SYS_INFO_STR_LEN - 1;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memset(stFlshSnmp.extSysinfo.contact, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(stFlshSnmp.extSysinfo.contact, pContact, strSize);
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);
   
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_sys_location_set(char *pLocation)
{
    sal_flash_snmp_t stFlshSnmp;
    uint8 strSize = 0;
    
    if(pLocation == NULL)
        return SAL_ERR_NULL_POINTER;
    
    strSize = strlen(pLocation);
    
    if(strSize == 0)
        return SAL_ERR_NULL_POINTER;

    if(strSize >= SAL_SNMP_SYS_INFO_STR_LEN)
        strSize = SAL_SNMP_SYS_INFO_STR_LEN - 1;
    
    _SalFlshRead(&stFlshSnmp, FLSH_ADDR_SNMP_BEGIN, sal_flash_snmp_t);
    memset(stFlshSnmp.extSysinfo.location, 0, SAL_SNMP_SYS_INFO_STR_LEN);
    memcpy(stFlshSnmp.extSysinfo.location, pLocation, strlen(pLocation));
    _SalFlshWrite(FLSH_ADDR_SNMP_BEGIN, &stFlshSnmp, sal_flash_snmp_t);
   
    return SAL_ERR_SUCCESS;
}
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
int32 sal_flash_serial_number_set(char *serial, uint8 length)
{
    uint8 xdata buf[100];
    if(length > 16)
        return SAL_ERR_FAILED;
    /* read MAC and serial number */    
    FLASH_READ((uint32)(FLSH_NVDATA_START), buf, sizeof(buf));
    /* keep MAC part and replace serial number*/
    memcpy(&buf[sizeof(sal_mac_t)],serial,length);
    buf[sizeof(sal_mac_t) + length] = 0;

    FLASH_ERASE_SECTOR(FLSH_NVDATA_START);
    FLASH_WRITE((uint32)FLSH_NVDATA_START, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_serial_number_get(char *serial)
{
    uint8 xdata buf[16];
    memset(buf,0,sizeof(buf));
    FLASH_READ((uint32)(FLSH_NVDATA_SERIAL_NUMBER), buf, sizeof(buf));
    memcpy(serial, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_serverIp_set(sal_ipaddr_t logServerIp)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    stFlshSyslog.sysLogServerIp = logServerIp;
    _SalFlshWrite(FLSH_ADDR_SYSLOG_BEGIN, &stFlshSyslog, sal_flash_syslog_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_serverIp_get(sal_ipaddr_t *plogServerIp)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    *plogServerIp = stFlshSyslog.sysLogServerIp;
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_serverPort_set(uint16 logServerPort)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    stFlshSyslog.sysLogServerPort = logServerPort;
    _SalFlshWrite(FLSH_ADDR_SYSLOG_BEGIN, &stFlshSyslog, sal_flash_syslog_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_serverPort_get(uint16 *plogServerPort)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    *plogServerPort = stFlshSyslog.sysLogServerPort;
    
    return SAL_ERR_SUCCESS;
}



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
int32 sal_flash_syslog_state_set(sal_enable_t syslogStatus)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    stFlshSyslog.enable = syslogStatus;
    _SalFlshWrite(FLSH_ADDR_SYSLOG_BEGIN, &stFlshSyslog, sal_flash_syslog_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_state_get(sal_enable_t *pSyslogStatus)
{
    sal_flash_syslog_t stFlshSyslog;
    
    _SalFlshRead(&stFlshSyslog, FLSH_ADDR_SYSLOG_BEGIN, sal_flash_syslog_t);
    *pSyslogStatus = stFlshSyslog.enable;
    
    return SAL_ERR_SUCCESS;
}

int32 sal_flash_syslog_init()
{
    sal_flash_syslog_state_set(DFLT_SYSLOG_ENABLE);
    sal_flash_syslog_serverIp_set(DFLT_SYSLOG_SERVER_IP);
    sal_flash_syslog_serverPort_set(DFLT_SYSLOG_SERVER_PORT);
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_buf_set(sal_flash_syslog_buf_entry_t *pSysLogBufEntry, uint16 index)
{
    sal_flash_syslog_buf_entry_t stFlshSyslogBuf;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_BEGIN + sizeof(sal_flash_syslog_buf_entry_t)*index ;//FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t) + sizeof(uint16) + sizeof(uint16) + sizeof(sal_flash_syslog_buf_entry_t)*index;
    _SalFlshRead(&stFlshSyslogBuf, flashMemoryBegin , sal_flash_syslog_buf_entry_t);
    memcpy(&stFlshSyslogBuf, pSysLogBufEntry, sizeof(sal_flash_syslog_buf_entry_t)); 
    _SalFlshWrite(flashMemoryBegin, &stFlshSyslogBuf, sal_flash_syslog_buf_entry_t);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_buf_get(sal_flash_syslog_buf_entry_t *pSysLogBufEntry, uint16 index)
{
    sal_flash_syslog_buf_entry_t stFlshSyslogBuf;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_BEGIN + sizeof(sal_flash_syslog_buf_entry_t)*index ;//FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t) + sizeof(uint16) + sizeof(uint16) + sizeof(sal_flash_syslog_buf_entry_t)*index;
    _SalFlshRead(&stFlshSyslogBuf, flashMemoryBegin , sal_flash_syslog_buf_entry_t);
    memcpy(pSysLogBufEntry, &stFlshSyslogBuf, sizeof(sal_flash_syslog_buf_entry_t)); 

    return SAL_ERR_SUCCESS;
}

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

int32 sal_flash_syslog_buf_tail_index_set(uint16 tailIndex)
{
    uint16 stFlshSyslogTailIdx;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_TAIL_IDX_BEGIN; 
//    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t) + sizeof(uint16); //go to add past headIdx
    _SalFlshRead(&stFlshSyslogTailIdx, flashMemoryBegin , uint16);
    stFlshSyslogTailIdx = tailIndex; 
    _SalFlshWrite(flashMemoryBegin, &stFlshSyslogTailIdx, uint16);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_buf_tail_index_get(uint16* pTailIndex)
{
    uint16 stFlshSyslogTailIdx;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_TAIL_IDX_BEGIN; 
    _SalFlshRead(&stFlshSyslogTailIdx, flashMemoryBegin , uint16);
    *pTailIndex = stFlshSyslogTailIdx;
    return SAL_ERR_SUCCESS;
}


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
int32 sal_flash_syslog_buf_head_index_set(uint16 headIndex)
{
    uint16 stFlshSyslogHeadIdx;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_HEAD_IDX_BEGIN; 
    _SalFlshRead(&stFlshSyslogHeadIdx, flashMemoryBegin , uint16);
    stFlshSyslogHeadIdx = headIndex; 
    _SalFlshWrite(flashMemoryBegin, &stFlshSyslogHeadIdx, uint16);

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_buf_head_index_get(uint16* pHeadIndex)
{
    uint16 stFlshSyslogHeadIdx;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BUF_HEAD_IDX_BEGIN; 
    _SalFlshRead(&stFlshSyslogHeadIdx, flashMemoryBegin , uint16);
    *pHeadIndex = stFlshSyslogHeadIdx; 

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_syslog_buf_setAll(sal_flash_syslog_buf_t* pSysLogBuf)
{
    sal_flash_syslog_buf_t stFlshSyslogBuf;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t);
    _SalFlshRead(&stFlshSyslogBuf, flashMemoryBegin , sal_flash_syslog_buf_t);
    memcpy(&stFlshSyslogBuf, pSysLogBuf, sizeof(sal_flash_syslog_buf_t)); 
    _SalFlshWrite(flashMemoryBegin, &stFlshSyslogBuf, sal_flash_syslog_buf_t);
	
    return SAL_ERR_SUCCESS;
}


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
int32 sal_flash_syslog_buf_getAll(sal_flash_syslog_buf_t* pSysLogBuf)
{
    sal_flash_syslog_buf_t stFlshSyslogBuf;
    uint32 flashMemoryBegin = FLSH_ADDR_SYSLOG_BEGIN + sizeof(sal_flash_syslog_t);
    _SalFlshRead(&stFlshSyslogBuf, flashMemoryBegin , sal_flash_syslog_buf_t);
    memcpy(pSysLogBuf, &stFlshSyslogBuf, sizeof(sal_flash_syslog_buf_t)); 
    return SAL_ERR_SUCCESS;
}

#endif

int32 sal_flash_syslog_buf_init()
{
	printf("\n IN sal_flash_syslog_buf_init:::::: \n");
	sal_flash_syslog_buf_head_index_set(DFLT_SYSLOG_BUF_HEAD_IDX);
    sal_flash_syslog_buf_tail_index_set(DFLT_SYSLOG_BUF_TAIL_IDX);
    return SAL_ERR_SUCCESS;
}


#endif

#if SAL_IGMP
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
int32 sal_flash_igmp_configEnable_get(uint8 *pIgmpStatus)
{
    sal_flash_igmp_t igmp;

    _SalFlshRead(&igmp, FLSH_ADDR_IGMP_BEGIN, sal_flash_igmp_t);
    *pIgmpStatus = igmp.enable;
    
    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_igmp_configEnable_set(uint8 igmpStatus)
{
    sal_flash_igmp_t igmp;
    
    _SalFlshRead(&igmp, FLSH_ADDR_IGMP_BEGIN, sal_flash_igmp_t);
    igmp.enable = igmpStatus;
    _SalFlshWrite(FLSH_ADDR_IGMP_BEGIN, &igmp, sal_flash_igmp_t);

    return SAL_ERR_SUCCESS;

}

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
int32 sal_flash_igmp_staticRouter_get(sal_lPortmask_t *pPortMask)
{
    sal_flash_igmp_t igmp;
    
    _SalFlshRead(&igmp, FLSH_ADDR_IGMP_BEGIN, sal_flash_igmp_t);
    *pPortMask = igmp.static_router;
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_flash_igmp_respInterval_set
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
int32 sal_flash_igmp_staticRouter_set(sal_lPortmask_t portMask)
{
    sal_flash_igmp_t igmp;
    
    _SalFlshRead(&igmp, FLSH_ADDR_IGMP_BEGIN, sal_flash_igmp_t);
    igmp.static_router = portMask;
    _SalFlshWrite(FLSH_ADDR_IGMP_BEGIN, &igmp, sal_flash_igmp_t);

    return SAL_ERR_SUCCESS;
}
#endif

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
int32 sal_flash_device_type_set(char *type)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN*3];

    /* read device type, fw version and hw version */    
    FLASH_READ((uint32)(FLSH_INFO_START), buf, sizeof(buf));
    
    /* keep fw version, hw version and replace device type*/
    memset(&buf[0], 0, SAL_SYS_STRING_LEN);
    memcpy(&buf[0], type, strlen(type));
    
    FLASH_ERASE_SECTOR(FLSH_INFO_START);
    FLASH_WRITE((uint32)FLSH_INFO_START, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_device_type_get(char *type)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN];
    memset(buf,0,sizeof(buf));
    //FLASH_READ((uint32)(FLSH_INFO_DEVICE_TYPE), buf, sizeof(buf));
    FLASH_READ((uint32)(FLSH_INFO_START), buf, sizeof(buf));
    memcpy(type, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
//int32 sal_flash_userData_set(uint32 addr , uint16 length, uint8 *pData)
//{
//		uint8 xdata buf[16];

//    /* read device type, fw version and hw version */    
//    FLASH_READ((uint32)(addr), buf, sizeof(buf));
//    
//    /* keep device type, hw version and replace fw version*/
//    memset(&buf[0], 0, sizeof(buf));
//    memcpy(&buf[0], pData, length);
//    
//    FLASH_ERASE_SECTOR(addr);
//    FLASH_WRITE((uint32)addr, buf, length);

//    return SAL_ERR_SUCCESS;
//}


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
//int32 sal_flash_userData_get(uint32 addr , uint16 length, uint8 *pData)
//{
//	uint8 xdata buf[16];
//	memset(buf,0,sizeof(buf));
//	FLASH_READ((uint32)(addr), buf, sizeof(buf));
//	memcpy(pData, buf, sizeof(buf));

//	return SAL_ERR_SUCCESS;
//}


extern int32 sal_flash_vlan_id_set(uint32 addr , uint16 length, uint8 *pData)
{
			uint8 xdata buf[4];

    /* read device type, fw version and hw version */    
    FLASH_READ((uint32)(addr), buf, sizeof(buf));
    
    /* keep device type, hw version and replace fw version*/
    memset(&buf[0], 0, sizeof(buf));
    memcpy(&buf[0], pData, length);
    
    FLASH_ERASE_SECTOR(addr);
    FLASH_WRITE((uint32)addr, buf, length);

    return SAL_ERR_SUCCESS;
}
extern int32 sal_flash_vlan_id_get(uint32 addr , uint16 length, uint8 *pData)
{
	uint8 xdata buf[4];
	memset(buf,0,sizeof(buf));
	FLASH_READ((uint32)(addr), buf, sizeof(buf));
	memcpy(pData, buf, sizeof(buf));

	return SAL_ERR_SUCCESS;
}


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
int32 sal_flash_fw_version_set(char *fwversion)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN*3];

    /* read device type, fw version and hw version */    
    FLASH_READ((uint32)(FLSH_INFO_START), buf, sizeof(buf));
    
    /* keep device type, hw version and replace fw version*/
    memset(&buf[SAL_SYS_STRING_LEN], 0, SAL_SYS_STRING_LEN);
    memcpy(&buf[SAL_SYS_STRING_LEN], fwversion, strlen(fwversion));
    
    FLASH_ERASE_SECTOR(FLSH_INFO_START);
    FLASH_WRITE((uint32)FLSH_INFO_START, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_fw_version_get(char *fwversion)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN];
    memset(buf,0,sizeof(buf));
    FLASH_READ((uint32)(FLSH_INFO_FW_VERSION), buf, sizeof(buf));
    memcpy(fwversion, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_hw_version_set(char *hwversion)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN*3];

    /* read device type, fw version and hw version */    
    FLASH_READ((uint32)(FLSH_INFO_START), buf, sizeof(buf));
    
    /* keep device type, fw version and replace hw version*/
    memset(&buf[SAL_SYS_STRING_LEN*2], 0, SAL_SYS_STRING_LEN);
    memcpy(&buf[SAL_SYS_STRING_LEN*2], hwversion, strlen(hwversion));
    
    FLASH_ERASE_SECTOR(FLSH_INFO_START);
    FLASH_WRITE((uint32)FLSH_INFO_START, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

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
int32 sal_flash_hw_version_get(char *hwversion)
{
    uint8 xdata buf[SAL_SYS_STRING_LEN];
    memset(buf,0,sizeof(buf));
    FLASH_READ((uint32)(FLSH_INFO_HW_VERSION), buf, sizeof(buf));
    memcpy(hwversion, buf, sizeof(buf));

    return SAL_ERR_SUCCESS;
}

#endif

