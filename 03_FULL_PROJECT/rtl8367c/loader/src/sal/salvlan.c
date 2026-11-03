//#pragma STRING (far)

#include <string.h>
#include <stdio.h>

#include <salvlan.h>			  
#include <salflash.h>
#include <salerror.h>


#if SAL_ENABLE

int32 sal_vlan_enable_get(sal_enable_t *pEnable)
{
    sal_flash_vlan_enable_get(pEnable);
    return SAL_ERR_SUCCESS;
}

#endif

