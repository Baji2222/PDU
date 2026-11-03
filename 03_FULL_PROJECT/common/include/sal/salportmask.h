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
 * Feature : Utility functions for logical and physical ports 
 *
 */
#ifndef __SAL_PORT_MASK_H__
#define __SAL_PORT_MASK_H__

#include <saltype.h>
#include <salerror.h>


/* Function Name:
 *      sal_pPortMask_addAll
 * Description:
 *      Add all ports for a physicl port mask
 * Input:
 *      pPPortMask - pointer to the target physical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_addAll(sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_pPortMask_delAll
 * Description:
 *      Delete all ports for a port mask
 * Input:
 *      pPPortMask - pointer to the target physical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_delAll(sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_pPortMask_addPort
 * Description:
 *      Add a port for a physical port mask
 * Input:
 *      pPPortMask - Pointer to the target physical port mask
 *      port - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_addPort(sal_pPortmask_t *pPPortMask, sal_port_t port);


/* Function Name:
 *      sal_pPortMask_delPort
 * Description:
 *      Delete a port for a physical port mask
 * Input:
 *      pPPortMask  - pointer to the target physical port mask
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_delPort(sal_pPortmask_t *pPPortMask, sal_port_t port);


/* Function Name:
 *      sal_pPortMask_addPorts
 * Description:
 *      Add ports for a physical port mask
 * Input:
 *      pPPortMask      - pointer to the target physical port mask
 *      pPPortMaskPorts - pointer to the physcial port mask with ports to be added
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_addPorts(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskPorts);


/* Function Name:
 *      sal_pPortMask_delPorts
 * Description:
 *      Delete ports for a physical port mask
 * Input:
 *      pPPortMask      - pointer to the target physical port mask
 *      pPPortMaskPorts - pointer to the physcial port mask with ports to be deleted
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_delPorts(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskPorts);


/* Function Name:
 *      sal_pPortMask_chkPort
 * Description:
 *      Check if a port is set for a physical port mask
 * Input:
 *      pPPortMask  - pointer to the physical port mask
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - The specific port is set for the port mask
 *      SAL_ERR_FAILED - The specific port is not set for the port mask
 * Note:
 *      None
 */
extern int32 sal_pPortMask_chkPort(sal_pPortmask_t *pPPortMask, sal_port_t port);


/* Function Name:
 *      sal_pPortMask_copy
 * Description:
 *      Copy the source physical port mask to the target physical port mask
 * Input:
 *      pPPortMas       - pointer to the target physical port mask
 *      pPPortMaskSrc   - pointer to the source physical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_copy(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskSrc);


/* Function Name:
 *      sal_pPortMask_copyLMask
 * Description:
 *      Copy the physical ports part of the source logical port mask to the target physical port mask 
 * Input:
 *      pPPortMask      - pointer to the target physical port mask
 *      pPPortMaskSrc   - pointer to the source logical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_copyLMask(sal_pPortmask_t *pPPortMask, sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_pPortMask_copyUL
 * Description:
 *      Copy the unsigned long integer to the first 32 bits of the target physical port mask
 * Input:
 *      pPPortMask  - pointer to the target physical port mask
 *      pMask       - pointer to the source unsigned long integer
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_copyUL(sal_pPortmask_t *pPPortMask, uint32 *pMask);


/* Function Name:
 *      sal_pPortMask_toUL
 * Description:
 *      Copy the first 32 bits of the target physical port mask to a unsigned long integer
 * Input:
 *      pMask       - pointer to the target unsigned long integer
 *      pPPortMask  - pointer to the source physical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_toUL(uint32 *pMask, sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_pPortMask_and
 * Description:
 *      AND two physical port mask bitwisely
 * Input:
 *      pPPortMaskA - pointer to one of the source physical port mask
 *      pPPortMaskB - pointer to another source physical port mask
 * Output:
 *      pPPortMask  - pointer to the physical port mask of AND result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_and(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskA, sal_pPortmask_t *pPPortMaskB);


/* Function Name:
 *      sal_pPortMask_or
 * Description:
 *      OR two physical port mask bitwisely
 * Input:
 *      pPPortMaskA - Pointer to one of the source physical port mask
 *      pPPortMaskB - Pointer to another source physical port mask
 * Output:
 *      pPPortMask  - pointer to the physical port mask of OR result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_or(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskA, sal_pPortmask_t *pPPortMaskB);


/* Function Name:
 *      sal_pPortMask_xor
 * Description:
 *      XOR two physical port mask bitwisely
 * Input:
 *      pPPortMaskA - pointer to one of the source physical port mask
 *      pPPortMaskB - pointer to another source physical port mask
 * Output:
 *      pPPortMask  - pointer to the physical port mask of XOR result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_pPortMask_xor(sal_pPortmask_t *pPPortMask, sal_pPortmask_t *pPPortMaskA, sal_pPortmask_t *pPPortMaskB);



/* Function Name:
 *      sal_pPortMask_cmp
 * Description:
 *      Compare if two physical port masks are the same
 * Input:
 *      pPPortMaskA - pointer to one of the source physical port mask
 *      pPPortMaskB - pointer to another source physical port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - Compare result shows that the two port masks are the same
 *      SAL_ERR_FAILED - Compare result shows that the two port masks are different
 * Note:
 *      None
 */
extern int32 sal_pPortMask_cmp(sal_pPortmask_t *pPPortMaskA, sal_pPortmask_t *pPPortMaskB);


/* Function Name:
 *      sal_pPortMask_isZero
 * Description:
 *      Check if a physical port mask is a zero mask
 * Input:
 *      pPPortMask - pointer to the source physical port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - This port mask is a zero mask
 *      SAL_ERR_FAILED - This port mask is not a zero mask
 * Note:
 *      None
 */
extern int32 sal_pPortMask_isZero(sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_lPortMask_addAll
 * Description:
 *      Add all ports for the logical port mask
 * Input:
 *      pPPortMask - pointer to the target logical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_addAll(sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_lPortMask_delAll
 * Description:
 *      Delete all ports for the logical port mask
 * Input:
 *      pLPortMask - pointer to the target logical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_delAll(sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_lPortMask_addPort
 * Description:
 *      Add a port for the logical port mask
 * Input:
 *      pLPortMask  - pointer to the target physical port mask
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_addPort(sal_lPortmask_t *pLPortMask, sal_port_t port);


/* Function Name:
 *      sal_lPortMask_delPort
 * Description:
 *      Delete a port for the logical port mask
 * Input:
 *      pPPortMask  - pointer to the target logical port mask
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_delPort(sal_lPortmask_t *pLPortMask, sal_port_t port);


/* Function Name:
 *      sal_lPortMask_addPorts
 * Description:
 *      Add ports for the logical port mask
 * Input:
 *      pPPortMask      - pointer to the target logical port mask
 *      pLPortMaskPorts - pointer to the logical port mask with ports to be added
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_addPorts(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskPorts);


/* Function Name:
 *      sal_lPortMask_delPorts
 * Description:
 *      Add ports for the logical port mask
 * Input:
 *      pPPortMask      - pointer to the target logical port mask
 *      pLPortMaskPorts - pointer to the logical port mask with ports to be deleted
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_delPorts(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskPorts);


/* Function Name:
 *      sal_lPortMask_chkPort
 * Description:
 *      Check if a port is added for the logical port mask
 * Input:
 *      pPPortMask  - pointer to the logical port mask
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - Check result shows the port is on the port mask
 *      SAL_ERR_FAILED - Check result shows the port is not on the port mask
 * Note:
 *      None
 */
extern int32 sal_lPortMask_chkPort(sal_lPortmask_t *pLPortMask, sal_port_t port);


/* Function Name:
 *      sal_lPortMask_copy
 * Description:
 *      Copy the logical port mask to another
 * Input:
 *      pPPortMask      - pointer to the target logical port mask
 *      pLPortMaskSrc   - pointer to the source logical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_copy(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskSrc);


/* Function Name:
 *      sal_lPortMask_copyPMask
 * Description:
 *      Copy the physical ports part of physical port mask to the logical port mask
 * Input:
 *      pLPortMask      - pointer to the target logical port mask
 *      pPPortMaskSrc   - pointer to the source physical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_copyPMask(sal_lPortmask_t *pLPortMask, sal_pPortmask_t *pPPortMask);


/* Function Name:
 *      sal_lPortMask_copyUL
 * Description:
 *      Copy the unsigned long integer to the first 32 bits of logical port mask
 * Input:
 *      pLPortMask  - pointer to the target logical port mask
 *      pMask       - pointer to the unsigned long integer
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_copyUL(sal_lPortmask_t *pLPortMask, uint32 *pMask);


/* Function Name:
 *      sal_lPortMask_toUL
 * Description:
 *      Copy the first 32 bits of logical port mask to the unsigned long integer
 * Input:
 *      pMask       - pointer to the unsigned long integer
 *      pLPortMask  - pointer to the logical port mask
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_toUL(uint32 *pMask, sal_lPortmask_t *pLPortMask);


/* Function Name:
 *      sal_lPortMask_and
 * Description:
 *      AND two logical port masks bitwisely
 * Input:
 *      pPPortMaskA - pointer to one of the source logical port mask
 *      pPPortMaskB - pointer to another source logical port mask
 * Output:
 *      pPPortMask  - pointer to the logical port mask of AND result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_and(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskA, sal_lPortmask_t *pLPortMaskB);


/* Function Name:
 *      sal_lPortMask_or
 * Description:
 *      OR two logical port masks bitwisely
 * Input:
 *      pPPortMaskA - pointer to one of the source logical port mask
 *      pPPortMaskB - pointer to another source logical port mask
 * Output:
 *      pPPortMask  - pointer to the logical port mask of OR result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_or(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskA, sal_lPortmask_t *pLPortMaskB);


/* Function Name:
 *      sal_lPortMask_xor
 * Description:
 *      XOR two logical port masks bitwisely
 * Input:
 *      pPPortMaskA - pointer to one of the source logical port mask
 *      pPPortMaskB - pointer to another source logical port mask
 * Output:
 *      pPPortMask  - pointer to the logical port mask of XOR result
 * Return:
 *      None
 * Note:
 *      None
 */
extern void sal_lPortMask_xor(sal_lPortmask_t *pLPortMask, sal_lPortmask_t *pLPortMaskA, sal_lPortmask_t *pLPortMaskB);


//void sal_lPortMask_delPorts2 (sal_lPortmask_t *pstLPortMask, sal_lPortmask_t *pstLPortMaskA, sal_lPortmask_t *pstLPortMaskB);

/* Function Name:
 *      sal_lPortMask_cmp
 * Description:
 *      Check if two logical port masks are the same
 * Input:
 *      pPPortMaskA - pointer to one of the source logical port mask
 *      pPPortMaskB - pointer to another source logical port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - Compare result shows that the two port masks are the same
 *      SAL_ERR_FAILED - Compare result shows that the two port masks are different
 * Note:
 *      None
 */
extern int32 sal_lPortMask_cmp(sal_lPortmask_t *pLPortMaskA, sal_lPortmask_t *pLPortMaskB);


/* Function Name:
 *      sal_lPortMask_isZero
 * Description:
 *      Check if a logical port mask is a zero mask
 * Input:
 *      pPPortMask - pointer to the logical port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS - This port mask is a zero mask
 *      SAL_ERR_FAILED - This port mask is not a zero mask
 * Note:
 *      None
 */
extern int32 sal_lPortMask_isZero(sal_lPortmask_t *pLPortMask);

#endif/*__SAL_PORT_MASK_H__*/

