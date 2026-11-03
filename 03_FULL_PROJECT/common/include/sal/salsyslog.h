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
#ifndef __SAL_SYSLOG_H__
#define __SAL_SYSLOG_H__

#include <saltype.h>
//#include <salsyslog.h>

#if SAL_SYSLOG

/* Function Name:
 *      sal_syslog_serverIp_set
 * Description:
 *      Set IP address of destination of syslog server.
 * Input:
 *      serverIp - Destination IP Address.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_serverIp_set(sal_ipaddr_t serverIp);


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
extern int32 sal_syslog_serverIp_get(sal_ipaddr_t *pServerIp);

/* Function Name:
 *      sal_syslog_serverPort_set
 * Description:
 *      Set port of destination of syslog server.
 * Input:
 *      serverPort - Destination port.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_serverPort_set(uint16 serverPort);

/* Function Name:
 *      sal_syslog_serverPort_get
 * Description:
 *      Get port of destination of syslog server.
 * Input:
 *      None
 * Output:
 *      serverPort - Destination port.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_serverPort_get(uint16 *pServerPort);

extern sal_enable_t syslog_enable;

/* Function Name:
 *      sal_syslog_state_set
 * Description:
 *      Set syslog Enable state.
 * Input:
 *      enable - syslog enable state.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_state_set(sal_enable_t enable);


/* Function Name:
 *      sal_syslog_state_get
 * Description:
 *      Get state of syslog.
 * Input:
 *      None
 * Output:
 *      enable - status of syslog.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_syslog_state_get(sal_enable_t *enable);


/* Function Name:
 *      sal_syslog_buf_head_idx_set
 * Description:
 *      Set head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_buf_head_idx_set(uint16 headIdx);


/* Function Name:
 *      sal_syslog_buf_head_idx_get
 * Description:
 *      Get head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_buf_head_idx_get(uint16* pHeadIdx);


/* Function Name:
 *      sal_syslog_buf_head_idx_set
 * Description:
 *      Set head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */

extern int32 sal_syslog_buf_tail_idx_set(uint16 tailIdx);


/* Function Name:
 *      sal_syslog_buf_tail_idx_get
 * Description:
 *      Get tail index of syslog bufer.
 * Input:
 *      tail index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_syslog_buf_tail_idx_get(uint16* pTailIdx);



/* Function Name:
 *      sal_syslog_config_restore
 * Description:
 *      Restore Syslog configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */

extern int32 sal_syslog_config_restore(void);

#endif

#endif/*__SAL_SYSLOG_H__*/

