#include <salerror.h>
#include <salflash.h>
#include <salsys.h>

#include <local/flash_api.h>
#include <local/sal_config.h>
#include <string.h>
#include <stdio.h>

#define _SalFlshRead(var, addr, vartype)  FLASH_READ((uint32)addr, (int8 *)var, sizeof(vartype))

static uint32 config_offset = 0;

int32
SalFlshGetInitOKMark (
    uint32 *    pulMark
    )
{
    sal_flash_beginMark_t   stFlshMark;

    _SalFlshRead(&stFlshMark, FLSH_ADDR_SIGNATURE, sal_flash_beginMark_t);
    *pulMark = stFlshMark.signature;

    return SAL_ERR_SUCCESS;
}

void SalFlshCopyFlshToCache(void)
{
    memset(&configCache, 0x00, sizeof(ConfigCache));
    FLASH_READ((uint32)(config_offset + FLSH_CONFIG_ADDR_START), (int8 *)&configCache, FLSH_ADDR_END - FLSH_CONFIG_ADDR_START);
}

int32
sal_flash_sysIp_get (
    sal_ipaddr_t *pIp,
    sal_ipaddr_t *pNetmask
    )
{
    sal_flash_sysInfo_t stFlshSys;

    _SalFlshRead(&stFlshSys, config_offset + FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    *pIp = stFlshSys.ip;
    *pNetmask = stFlshSys.netmask;
    return SAL_ERR_SUCCESS;
}

int32
sal_flash_sysGateway_get (
    sal_ipaddr_t *pGateway
    )
{
    sal_flash_sysInfo_t stFlshSys;

    if (pGateway == NULL)
        return SAL_ERR_NULL_POINTER;

    _SalFlshRead(&stFlshSys, config_offset + FLSH_ADDR_SYS, sal_flash_sysInfo_t);
    *pGateway = stFlshSys.gateway;
    return SAL_ERR_SUCCESS;
}
#if 0
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
#endif
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

