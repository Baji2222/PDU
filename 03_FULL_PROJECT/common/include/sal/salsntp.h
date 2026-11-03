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
 * Feature : SNTP related
 *
 */
#ifndef __SAL_SNTP_H__
#define __SAL_SNTP_H__

#include <saltype.h>

typedef enum sal_sntp_status_e
{
    SAL_SNTP_SYNC = 0,
    SAL_SNTP_NON_SYNC
} sal_sntp_status_t;

/* Function Name:
 *      sal_sntp_state_set
 * Description:
 *      Set the SNTP enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sntp_state_set(sal_enable_t enable);

/* Function Name:
 *      sal_sntp_state_get
 * Description:
 *      Get the SNTP enable status.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sntp_state_get(sal_enable_t *pEnable);

/* Function Name:
 *      sal_sntp_status_get
 * Description:
 *      Get the SNTP status.
 * Input:
 *      None
 * Output:
 *      pStatus - pointer to the synchronization status
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
extern int32 sal_sntp_status_get(sal_sntp_status_t *pStatus);

/* Function Name:
 *      sal_sntp_ip_set
 * Description:
 *      Set IP address of the SNTP server.
 * Input:
 *      ip      - IP address
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_sntp_ip_set(sal_ipaddr_t ip);

/* Function Name:
 *      sal_sntp_ip_get
 * Description:
 *      Get IP address of the SNTP server.
 * Input:
 *      None
 * Output:
 *      pIp      - pointer buffer of the IP address.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32 sal_sntp_ip_get(sal_ipaddr_t *pIp);

#endif /* end of __SAL_SNTP_H__ */

