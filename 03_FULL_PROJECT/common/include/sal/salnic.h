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
 * Feature : NIC related
 *
 */
#ifndef __SAL_NIC_H__
#define __SAL_NIC_H__

#include <saltype.h>

#define SAL_NIC_NULL_VID    0xFFFF
#define SAL_NIC_NULL_SPA    0xFF

/* Function Name:
 *      sal_nic_rxPort_get
 * Description:
 *      Get source port number of last received packet
 * Input:
 *      pktStrPtr - pointer to the packet structure.
 * Output:
 *      *pSrcPort - pointer to the received port number.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_nic_rxPort_get(void *pktStrPtr, sal_port_t *spa);


/* Function Name:
 *      sal_nic_rxPort_set
 * Description:
 *      Set source port number of last received packet. This function is only called by eth.c.
 * Input:
 *      pBuf - poionter to packet structure
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED - Cannot get source port number from last packet.
 * Note:
 *      None
 */
extern int32 sal_nic_rxPort_set(void *pBuf);


/* Function Name:
 *      sal_nic_tx_init
 * Description:
 *      Initialize tx related structure. TX buffer are also allocated in this function.
 * Input:
 *      pktStrPtr - pointer to the packet structure.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_nic_tx_init(void *pktStrPtr);

/* Function Name:
 *      sal_nic_tx
 * Description:
 *      Send packet
 * Input:
 *      pktStrPtr - pointer to the packet structure.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_nic_tx(void *pktStrPtr);


/* Function Name:
 *      sal_nic_rxVid_get
 * Description:
 *      Get VID of currently received packet
 * Input:
 *      pVid - Pointer to VID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_nic_rxVid_get(uint16 *pVid);


/* Function Name:
 *      sal_nic_rxVid_set
 * Description:
 *      Set VID of currently received packet to memory.
 * Input:
 *      pBuf - pointer to packet structure
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_nic_rxVid_set(void *pBuf);


#endif

