/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter,  rfc2863
* 
*$Id: rfc2863.h,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/


#ifndef __INTERFACE_GROUP_H__
#define __INTERFACE_GROUP_H__

#include <string.h>
#include "local/config.h"
#include "snmp/snmp.h"
#include "snmp/snmp_structs.h"
#include "snmp/snmp_debug.h"
#include "snmp/snmp_asn1.h"
#include "snmp/snmp_msg.h"

#if LWPS_SNMP

#if LWPS_SNMP_INTERFACE_GROUP

#define MIB_INTERFACE_GROUP_ID       (31)
#define MIB_INTERFACE_GROUP_HANDLER  ((struct mib_node* const)&ifMIB)

extern struct mib_array_node ifMIB;
extern u32_t xdata ifPromiscuousMode[COMBO_PORT_NUM];
extern u8_t xdata ifAliasLen[COMBO_PORT_NUM];

extern int if_link_change_trap_enable(u8_t index);
extern void snmp_inc_ifXTable(void);
extern void ifXTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
extern void ifXTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern void ifXTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern u8_t ifXTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant;



extern int if_link_change_trap_enable(u8_t index);

#endif /* end of LWPS_SNMP_INTERFACE_GROUP */

#endif /* end of LWPS_SNMP */

#endif /* end of __INTERFACE_GROUP_H__ */
