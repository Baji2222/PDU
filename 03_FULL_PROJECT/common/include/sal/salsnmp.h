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
 * $Revision: 7002 $
 * $Date: 2009-11-13 14:40:24 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : SNMP (Simple Network Management Protocol)
 *
 */
#ifndef __SAL_SNMP_H__
#define __SAL_SNMP_H__

#include <saltype.h>
#include <salsnmp.h>

#if SAL_SNMP

/* Function Name:
 *      sal_snmp_trapIp_set
 * Description:
 *      Set IP address of destination of trap packet.
 * Input:
 *      trapIp - Destination IP Address.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_snmp_trapIp_set(sal_ipaddr_t trapIp);


/* Function Name:
 *      sal_snmp_trapIp_get
 * Description:
 *      Get IP address of destination of trap packet.
 * Input:
 *      None
 * Output:
 *      pTrapIp - Pointer to Destination IP Address.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_snmp_trapIp_get(sal_ipaddr_t *pTrapIp);


/* Function Name:
 *      sal_snmp_community_set
 * Description:
 *      Set read community and read-write community string.
 * Input:
 *      pReadComm   - Pointer to read community string.
 *      pWriteComm  - Pointer to write community string.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER 
 * Note:
 *      None
 */
extern int32 sal_snmp_community_set(char *pReadComm, char *pWriteComm);


/* Function Name:
 *      sal_snmp_community_set
 * Description:
 *      Get read community and read-write community string.
 * Input:
 *      None 
 * Output:
 *      pReadComm   - Pointer to read community string.
 *      pWriteComm  - Pointer to write community string.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_snmp_community_get(char *pReadComm, char *pWriteComm);



/* Function Name:
 *      sal_snmp_config_restore
 * Description:
 *      Restore SNMP configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_snmp_config_restore(void);

#endif

#endif/*__SAL_SNMP_H__*/

