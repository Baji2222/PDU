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
 * Feature : Basic Type Defintion
 *
 */
#ifndef __SAL_TYPE_H__
#define __SAL_TYPE_H__

#include <rtk_types.h>
#include <local/sal_config.h>

/* 
 * Symbol Definition 
 */
#define SAL_ETHER_ADDR_LEN  6

#define SAL_POSITIVE    (1)
#define SAL_NEGATIVE    (-1)

/* 
 * Data Type Declaration 
 */
typedef uint16  sal_vlan_t;         /* vlan id type                     */
typedef uint16  sal_fid_t;          /* filter id type                   */
typedef uint8   sal_stg_t;          /* spanning tree instance id type   */
typedef uint8   sal_port_t;         /* port type                        */
typedef uint8   sal_pri_t;          /* priority vlaue                   */
typedef uint8   sal_qid_t;          /* queue id type                    */
typedef uint32  sal_ipaddr_t;       /* ip address                       */
typedef uint32  sal_stat_counter_t; /* 32-bit mib counter               */
typedef uint32  sal_dscp_t;         /* dscp vlaue                       */
typedef uint32  sal_queue_num_t;    /* queue number                     */

typedef enum sal_enable_e
{
    SAL_DISABLED = 0,
    SAL_ENABLED,
    SAL_ENABLE_END
} sal_enable_t;

typedef struct sal_mac_s
{
    uint8 octet[SAL_ETHER_ADDR_LEN];
} sal_mac_t;

typedef struct sal_physical_portmask_e
{    
    uint32  bits[1];
} sal_pPortmask_t;

typedef struct sal_logical_portmask_e
{    
    uint32  bits[1];
} sal_lPortmask_t;

typedef enum sal_entry_action_e
{    
    SAL_ENTRY_ACT_VALID = 0,         /* write to flash, and load into hardware */
    SAL_ENTRY_ACT_INVALID,           /* write to flash, but hardware */
    SAL_ENTRY_ACT_KEEP,              /* keep the valid status, if valid load entry into hardware*/
    SAL_ENTRY_ACT_END
} sal_entry_action_t;

#define FOREACH_PORTINMASK(port, mask)  for(port=0; port<SAL_PORT_ID_CPU; port++)  if(mask & ((uint32)1<<port))

#ifndef SAL_PRINT
#define SAL_PRINT   printf
#endif

#endif/* __SAL_TYPE_H__ */

