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
 * Feature : Port Mirror
 *
 */
#ifndef __SAL_MIRROR_H__
#define __SAL_MIRROR_H__

#include <saltype.h>

typedef enum {
	PORT_MIRROR_OFF,
	PORT_MIRROR_RX,
	PORT_MIRROR_TX,
	PORT_MIRROR_BOTH
} LWS_PORT_MIRROR;


/* Function Name:
 *      sal_mirror_config_restore
 * Description:
 *      Restore mirror configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_mirror_config_restore(void);

/* Function Name:
 *      sal_mirror_portMirror_set
 * Description:
 *      Set port mirror configuration
 * Input:
 *      mirrId      - mirror set ID
 *      mode        - ingress or egress mirror
 *      port        - mirroring port
 *      pPortMask   - mirrored port mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_MIRROR_SET_FAILED
 * Note:
 *      None
 */
extern int32 sal_mirror_portMirror_set(
    uint32 mirrId,
    uint32 mode,
    sal_port_t port,
    sal_lPortmask_t *pPortMask);


/* Function Name:
 *      sal_mirror_portMirror_get
 * Description:
 *      Get port mirror configuration
 * Input:
 *      mirrId		- mirror set ID
 * Output:
 *      pMode		- ingress or egress mirror
 *      pPort		- mirroring port
 *      pPortMask	- mirrored port mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_MIRROR_GET_FAILED
 * Note:
 *      None
 */
extern int32 sal_mirror_portMirror_get(
    uint32 mirrId,
    uint32 *pMode,
    sal_port_t *pPort,
    sal_lPortmask_t *pPortMask);

#endif/* __SAL_MIRROR_H__ */

