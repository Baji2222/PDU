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
 * Feature : Firmware upgrade related function
 *
 */
#ifndef __SAL_FWUG_H__
#define __SAL_FWUG_H__

#include <saltype.h>
#include <tftpd/tftpc.h>

typedef enum sal_fwug_status_e
{
    SAL_FWUG_STATUS_NORMAL = 0,
    SAL_FWUG_STATUS_TRANSMITTING,
    SAL_FWUG_STATUS_COMPLETE,
    SAL_FWUG_STATUS_FILE_NOT_FOUND,
    SAL_FWUG_STATUS_IMAGE_ERR,    
    SAL_FWUG_STATUS_UNKNOWN_ERR,
    SAL_FWUG_STATUS_SEARCHING_SERVER,
    SAL_FWUG_STATUS_SOCKET_ALLOCATE_FAIL,
    SAL_FWUG_STATUS_SERVER_NOT_FOUND,
    SAL_FWUG_STATUS_END
} sal_fwug_status_t;

typedef enum sal_fwug_startState_e
{
    SAL_FWUG_STARTSTATE_WARM = 0,
    SAL_FWUG_STARTSTATE_COLD,
    SAL_FWUG_STARTSTATE_END
} sal_fwug_startState_t;

/* Function Name:
 *      sal_fwug_http_init
 * Description:
 *      Initialize connection for TFTP firmware upgrade
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED - socket allocation fail
 * Note:
 *      None
 */
extern int32 sal_fwug_http_init();

#if defined(UPGRADE_TFTP)
/* Function Name:
 *      sal_fwug_tftp_init
 * Description:
 *      Initialize connection for TFTP firmware upgrade
 * Input:
 *      servIp    - Server IP
 *      pFileName - Pointer to file name.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_FAILED - socket allocation fail
 * Note:
 *      None
 */
extern int32 sal_fwug_tftp_init(sal_ipaddr_t servIp, char *pFileName);

/* Function Name:
 *      sal_fwug_status_set
 * Description:
 *      Set status of firmware upgrade
 * Input:
 *      None
 * Output:
 *      status - firmware upgrade status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_fwug_status_set(sal_fwug_status_t status);

/* Function Name:
 *      sal_fwug_status_get
 * Description:
 *      Get status of firmware upgrade
 * Input:
 *      None
 * Output:
 *      status - firmware upgrade status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_fwug_status_get(sal_fwug_status_t *pStatus);
#endif
/* Function Name:
 *      sal_fwug_image_write
 * Description:
 *      Write image to chip
 * Input:
 *      pBuf   - pointer to palyload of tftp packet.
 *      length - length of tftp packet
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
extern int32 sal_fwug_image_write(uint8 *pBuf, uint16 length);


/* Function Name:
 *      sal_fwug_terminate
 * Description:
 *      Action for termination of firmware upgrade
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
extern int32 sal_fwug_termination_set();


/* Function Name:
 *      sal_fwug_startState_get
 * Description:
 *      Get start state of firmware upgrade
 * Input:
 *      None
 * Output:
 *      *pState - pointer to start state;
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
extern int32 sal_fwug_startState_get(sal_fwug_startState_t *pState);

#endif/*__SAL_FWUG_H__*/

