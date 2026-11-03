#include <saltype.h>
#include <salflash.h>
#include <salgreeneth.h>
#include <salerror.h>

#include <common.h>
#include <rtl8367c_asicdrv_green.h>
#include <rtk_switch.h>

static uint8 green_enable;

/* Function Name:
 *      sal_greenEth_config_restore
 * Description:
 *      Restore Green Ethernet configuration
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_greenEth_config_restore()
{
    sal_enable_t    enable;
    
    sal_flash_greenEth_port_enable_get(&enable);
    sal_greenEth_enable_set(enable);
        
   return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_greenEth_enable_set
 * Description:
 *      Enable/Disable Green Ethernet function
 * Input:
 *      enable - enable Green Ethernet function
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
int32 sal_greenEth_enable_set(sal_enable_t enable)
{
//    uint16 i;

    if(enable >= SAL_ENABLE_END)    
        return SAL_ERR_PARAMETER;
        
	rtk_switch_greenEthernet_set(enable);
    green_enable = enable;

    return sal_flash_greenEth_port_enable_set(enable);
}

/* Function Name:
 *      sal_greenEth_enable_get
 * Description:
 *      Get Green Ethernet function state
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to Green Ethernet function
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
//int32 sal_greenEth_enable_get(sal_enable_t *pEnable)
//{
//    if(NULL == pEnable)
//        return SAL_ERR_PARAMETER;

//    *pEnable = green_enable;

//	return SAL_ERR_SUCCESS;
//}

