/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter,  rfc2863
* 
*$Id: rfc2863.h,v 1.1 2008/06/20 08:53:35 caroline_liu Exp $
*/


#ifndef __BRIDGE_MIB_H__
#define __BRIDGE_MIB_H__

#include <string.h>
#include "local/config.h"
#include "snmp/snmp.h"
#include "snmp/snmp_structs.h"
#include "snmp/snmp_debug.h"
#include "snmp/snmp_asn1.h"
#include "snmp/snmp_msg.h"

#if LWPS_SNMP

#if LWPS_SNMP_BRIDGE_MIB

#define MIB_BRIDGE_ID       (17)
#define MIB_BRIDGE_HANDLER  ((struct mib_node* const)&bridgeMIB)

extern struct mib_array_node bridgeMIB;

extern void snmp_inc_bridgeXTable(void);
extern void bridgeXTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
extern void bridgeXTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern void bridgeXTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern u8_t bridgeXTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant;





#endif /* end of LWPS_SNMP_BRIDGE_MIB */

#endif /* end of LWPS_SNMP */

#endif /* end of __BRIDGE_MIB_H__ */

