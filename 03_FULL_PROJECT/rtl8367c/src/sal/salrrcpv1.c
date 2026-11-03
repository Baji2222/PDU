#include <salrrcp1.h>
#include <salerror.h>

#define RRCP1SLV_CHIP_ID 0x7001

/* Function Name:
 *      sal_rrcp1Slv_adminPortMask_get
 * Description:
 *      Get the RRCP administration port mask.
 * Input: 
 *      None
 * Output: 
 *      pAdmin_mask - administration port mask
 * Return:
 *      None
 * Note:
 *      None
 */
int32 sal_rrcp1Slv_adminPortMask_get(sal_pPortmask_t *pAdmin_mask)
{
    pAdmin_mask->bits[0] = 0xFF;
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_rrcp1Slv_chipId_get
 * Description:
 *      Get rrcpv1 chip id
 * Input: 
 *      None
 * Output: 
 *      pChipId - Pointer to chip ID
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_rrcp1Slv_chipId_get(sal_rrcpv1_chipId *chipId)
{
    *chipId = RRCP1SLV_CHIP_ID;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_rrcp1Slv_venderId_get
 * Description:
 *      Get rrcpv1 vender id
 * Input: 
 *      None
 * Output: 
 *      pChipId - Pointer to vender ID
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_rrcp1Slv_venderId_get(sal_rrcpv1_venderId *venderId)
{
    *venderId = 0;

    return SAL_ERR_SUCCESS;
}

