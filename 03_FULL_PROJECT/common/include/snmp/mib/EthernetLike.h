/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, rfc3635
* 
*$Id: rfc3635.h,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/

#ifndef __ETHERNET_LIKE_H__
#define __ETHERNET_LIKE_H__
#include <string.h>
#include "snmp/snmp.h"
#include "snmp/snmp_structs.h"
#include "snmp/snmp_debug.h"
#include "snmp/snmp_asn1.h"
#include "snmp/snmp_msg.h"


#if LWPS_SNMP && LWPS_SNMP_ETHERNET_LIKE


#define MIB_TRANSMISSION_ID         (10)
#define MIB_TRANSMISSION_HANDLER    ((struct mib_node* const)&transmission)

extern struct mib_array_node transmission;
void dot3Statsentry_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void dot3Statsentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
void dot3CollTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void dot3CollTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
void dot3ControlTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void dot3ControlTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
void dot3PauseTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void dot3PauseTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
void dot3HCStatsTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void dot3HCStatsTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;

void construct_mib2_transmission();
#endif /* end of LWPS_SNMP&& LWPS_SNMP_ETHERNET_LIKE */


#endif /* end of __ETHERNET_LIKE_H__ */
