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
 * Feature : Realtek Remote Control Protocol Type I
 *
 */
#ifndef __SAL_RRCP_H__
#define __SAL_RRCP_H__

#include <saltype.h>

typedef enum sal_rrcp_tx_tagMode_e
{
	SAL_RRCP_TX_TAGMODE_KEEP,
	SAL_RRCP_TX_TAGMODE_UNTAG,
	SAL_RRCP_TX_TAGMODE_TAG,
	SAL_RRCP_TX_TAGMODE_ALE
} sal_rrcp_tx_tagMode_t;

typedef uint16 sal_rrcpv1_chipId;
typedef uint32 sal_rrcpv1_venderId;

/* Function Name:
 *      sal_rrcp1Slv_adminPortMask_set
 * Description:
 *      Set the RRCP administration port mask.
 * Input:
 *      pAdmin_mask	- administration port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_adminPortMask_set(sal_pPortmask_t *pAdmin_mask);


/* Function Name:
 *      sal_rrcp1Slv_adminPortMask_get
 * Description:
 *      Get the RRCP administration port mask.
 * Input: 
 *      None
 * Output: 
 *      pAdmin_mask	- administration port mask
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_adminPortMask_get(sal_pPortmask_t *pAdmin_mask);


/* Function Name:
 *      sal_rrcp1Slv_authPortMask_set
 * Description:
 *      Set the RRCP authentication port mask.
 * Input:
 *      pAuth_mask	- authentication port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_authPortMask_set(sal_pPortmask_t *pAuth_mask);


/* Function Name:
 *      sal_rrcp1Slv_authPortMask_get
 * Description:
 *      Get the RRCP Auth port mask.
 * Input: 
 *      None
 * Output: 
 *      pAuth_mask	- authentication port mask
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_authPortMask_get(sal_pPortmask_t *pAuth_mask);


/* Function Name:
 *      sal_rrcp1Slv_vlanLeaky_set
 * Description:
 *      Set vlan egress leaky configure.
 * Input:
 *      enable	- vlan leaky status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_vlanLeaky_set(sal_enable_t enable);


/* Function Name:
 *      sal_rrcp1Slv_vlanLeaky_get
 * Description:
 *      Get vlan egress leaky configuration.
 * Input: 
 *      None
 * Output: 
 *      pEnable	- vlan leaky status
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_vlanLeaky_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_rrcp1Slv_portBasedVlan_set
 * Description:
 *      Set port-based vlan configuration.
 * Input:
 *      enable  - port-based vlan enable status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_portBasedVlan_set(sal_enable_t enable);


/* Function Name:
 *      sal_rrcp1Slv_portBasedVlan_get
 * Description:
 *      Get port-based vlan configuration.
 * Input: 
 *      None
 * Output: 
 *      pEnable - port-based vlan enable status
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_portBasedVlan_get(sal_enable_t *pEnable);


/* Function Name:
 *      sal_rrcp1Slv_asicReplyMode_set
 * Description:
 *      Set the tag-mode when RRCP slave replying RRCP packet.
 * Input:
 *      mode	- tag mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_asicReplyMode_set(sal_rrcp_tx_tagMode_t mode);


/* Function Name:
 *      sal_rrcp1Slv_asicReplyMode_get
 * Description:
 *      Get the tag-mode when RRCP slave replying RRCP packet.
 * Input: 
 *      None
 * Output: 
 *      *pMode	- tag mode
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_asicReplyMode_get(sal_rrcp_tx_tagMode_t *pMode);


/* Function Name:
 *      sal_rrcp1Slv_forwardMode_set
 * Description:
 *      Set the tag-mode when RRCP slave forwarding RRCP packet.
 * Input:
 *      mode	- tag mode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_forwardMode_set(sal_rrcp_tx_tagMode_t mode);


/* Function Name:
 *      sal_rrcp1Slv_forwardMode_get
 * Description:
 *      Get the tag-mode when RRCP slave forwarding RRCP packet.
 * Input: 
 *      None
 * Output: 
 *      *pMode	- tag mode
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_forwardMode_get(sal_rrcp_tx_tagMode_t *pMode);


/* Function Name:
 *      sal_rrcp1Slv_defaultTag_set
 * Description:
 *      Set the tag content that will be inserted into packet.
 * Input:
 *      vid     - vlan id
 *      pri 	- priority
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_defaultTag_set(sal_vlan_t vid, sal_pri_t pri);


/* Function Name:
 *      sal_rrcp1Slv_defaultTag_get
 * Description:
 *      Get the tag content that will be inserted into packet.
 * Input: 
 *      None
 * Output: 
 *      pVid    - vlan id
 *		pPri	- priority
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32 sal_rrcp1Slv_defaultTag_get(sal_vlan_t *pVid, sal_pri_t *pPri);


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
extern int32 sal_rrcp1Slv_chipId_get(sal_rrcpv1_chipId *chipId);

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
extern int32 sal_rrcp1Slv_venderId_get(sal_rrcpv1_venderId *venderId);


#endif /* __SAL_RRCP_H__ */

