#pragma STRING(far)
#include <salerror.h>
#include <salsys.h>
#include <salflash.h>

#include <local/flash_config.h>
#include <SPIFlashDrv.h>
#include <common.h>
#include <runtime_loader_msg.h>

#include <stdio.h>
#include <string.h>

typedef void (*rt_entry)(void);
#define FLASH_READ(start_addr, buf, len)    spi_flash_read(start_addr, buf, len)

ConfigCache configCache;
extern uint8     xdata this_mac[];
extern uint8     xdata this_ip[];   
extern uint8     xdata netmask[];
extern ip_addr_t xdata default_gateway;

static uint8     runtime_crc_error = 0;

int32 sal_sys_runtime_crc_get(uint8 *error)large
{

    *error = runtime_crc_error;
    return SAL_ERR_SUCCESS;
}

int32 sal_sys_runtime_crc_set(uint8 error)large
{

    runtime_crc_error = error;
    return SAL_ERR_SUCCESS;
}

void sal_sys_config_restore ()
{
    sal_ipaddr_t ip;
    sal_ipaddr_t netmask;
    sal_ipaddr_t gateway;
    sal_mac_t sysMac;

    sal_flash_sysMac_get(&sysMac);
    if(((sysMac.octet[0] == 0xff) && (sysMac.octet[1] == 0xff) && (sysMac.octet[2] == 0xff) && 
    (sysMac.octet[3] == 0xff) && (sysMac.octet[4] == 0xff) && (sysMac.octet[5] == 0xff)) ||
    ((sysMac.octet[0] == 0) && (sysMac.octet[1] == 0) && (sysMac.octet[2] == 0) && 
    (sysMac.octet[3] == 0) && (sysMac.octet[4] == 0) && (sysMac.octet[5] == 0))
    )
    {
        sysMac.octet[5] = 0;
        sysMac.octet[4] = 0x23;
        sysMac.octet[3] = 0x79;
        sysMac.octet[2] = 0;
        sysMac.octet[1] = 0x23;
        sysMac.octet[0] = 0x79;
        
    }
    sal_sys_mac_set(&sysMac);
    sal_flash_sysIp_get(&ip, &netmask);
    if((ip == 0) || (ip == 0xffffffff))
    {
        ip = 0xC0A80101; /* 192.168.1.1 */
        netmask = 0xFFFFFF00; /*255.255.255.0*/
    }
    sal_sys_ip_set(ip, netmask);
    sal_flash_sysGateway_get(&gateway);
    sal_sys_gateway_set(gateway);
}


int32 sal_flash_config_init(void)
{
    uint32 ulMark;
    uint8 error;
    uint8 magic[4];

    ulMark = FLSH_MARK_INIT_NO;

    SalFlshGetInitOKMark(&ulMark);

    if (ulMark != FLSH_MARK_INIT_OK) 
    {
        sal_sys_runtime_crc_get(&error);
        
        if(error)
        {
            FLASH_READ((uint32)(FLSH_CONFIG_ADDR_START), (int8 *)&magic, 4);
            
            if((magic[0]== 0x23) && (magic[1]== 0x79) && (magic[2]== 0x23) &&(magic[3]== 0x79))
            {
                //if()
                SalFlshCopyFlshToCache();
                sal_sys_config_restore();
            }
            
        }
        else
        {
        printf("Use default IP and MAC\n");    
        /* configuration is invalid, set MAC address and ip setting as default */
        /* default value is defined in global_variable.c */
        /* no change to ip */
    }
    }
    else
    {
        printf("Get information IP and MAC form flash\n");            
        SalFlshCopyFlshToCache();
        sal_sys_config_restore();
    }   

    return SAL_ERR_SUCCESS;
}

int32 sal_sys_mac_set (
    sal_mac_t *pSysMac
    )
{
    if(pSysMac == NULL)
        return SAL_ERR_PARAMETER;

    memcpy(this_mac, pSysMac->octet, SAL_ETHER_ADDR_LEN);

    return SAL_ERR_SUCCESS;
}


int32
sal_sys_gateway_set (
    sal_ipaddr_t gateway
    )
{

    default_gateway.addr[0] = (gateway & 0xFF000000) >> 24;
    default_gateway.addr[1] = (gateway & 0x00FF0000) >> 16;
    default_gateway.addr[2] = (gateway & 0x0000FF00) >> 8;
    default_gateway.addr[3] = (gateway & 0x000000FF) >> 0;

    return SAL_ERR_SUCCESS;
}


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

    return SAL_ERR_SUCCESS;
}


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
int32 sal_sys_system_reboot(void)
{
    setReg(0x1322, 1);

    return SAL_ERR_SUCCESS;
}

#if 0
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
int32 sal_sys_runtimeMode_set()
{
    rt_entry fp;

    runtime_start_mode_set(START_MODE_WARM);
    loader_start_mode_set(START_MODE_COLD);
    loader_isr_set(FALSE);    
    setReg(0x1336, 0x71);
    fp = (rt_entry)(0);
    fp();

    return SAL_ERR_SUCCESS;
}
#endif

#if 0
/* Function Name:
 *      sal_sys_loaderBootMode_get
 * Description:
 *      Get Boot mode of runtime code. 
 * Input:
 *      None
 * Output:
 *      *pBootMode - pointer to boot mode
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_sys_loaderBootMode_get(sal_sys_boot_mode_t *pBootMode)
{
    uint8 loaderMode;

    loader_start_mode_get(&loaderMode);

    if(START_MODE_WARM == loaderMode)
        *pBootMode = SAL_SYS_BOOT_MODE_WARM;
    else
        *pBootMode = SAL_SYS_BOOT_MODE_COLD;

    return SAL_ERR_SUCCESS;
}

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

int32 sal_sys_gateway_get(sal_ipaddr_t *pGateway)
{
    if(NULL == pGateway)
        return SAL_ERR_NULL_POINTER;

    *pGateway = ((uint32)default_gateway.addr[0] << 24) | ((uint32)default_gateway.addr[1] << 16) | \
        ((uint32)default_gateway.addr[2] << 8) | (uint32)default_gateway.addr[3];

    return SAL_ERR_SUCCESS;
}
#endif

