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
 * $Revision: 6226 $
 * $Date: 2009-10-01 18:07:30 +0800 (Thu, 01 Oct 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : System related
 *
 */
#ifndef __SWITCH_CONFIG_H__
#define __SWITCH_CONFIG_H__





#ifndef MAX_MIRROR_GROUP
#define MAX_MIRROR_GROUP  (1)
#endif

#ifndef MAX_CPU_PORT
#define MAX_CPU_PORT     (1)
#endif





#ifndef MAX_STATIC_TRUNK
#define MAX_STATIC_TRUNK       (2)
#endif

#ifndef CHIP_MAX_NUM_VLAN
#define CHIP_MAX_NUM_VLAN      (4094)
#endif

#ifndef CHIP_MAX_STATIC_VLAN
#define CHIP_MAX_STATIC_VLAN   (32)
#endif

#ifndef CHIP_MAX_STATIC_SVLAN
#define CHIP_MAX_STATIC_SVLAN  (64)
#endif

#endif /* end of __SWITCH_CONFIG_H__ */

