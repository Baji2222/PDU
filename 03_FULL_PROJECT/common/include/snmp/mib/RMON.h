/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, RFC 2819
* 
*$Id: rfc2819.h,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/

#ifndef __RMON_H__
#define __RMON_H__

#include <local/snmp_config.h>

#if LWPS_SNMP_RMON
#include "snmp/snmp.h"
#include "snmp/snmp_structs.h"
#include "snmp/snmp_asn1.h"
#include "snmp/snmp_msg.h"
#include <string.h>

#if LWPS_SNMP


#define MIB_RMON_ID          (16)
#define MIB_RMON_HANDLER     ((struct mib_node* const)&rmon)

extern struct mib_array_node rmon;

void rmon_stat_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_stat_get_value(struct obj_def *od, u16_t len, void *value) reentrant;

u8_t rmon_stat_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_stat_set_value(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_hisctrl_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_hisctrl_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
u8_t rmon_hisctrl_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_hisctrl_set_value(struct obj_def *od, u16_t len, void *value) reentrant;

void rmon_his_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_his_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_alarm_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_alarm_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
u8_t rmon_alarm_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_alarm_set_value(struct obj_def *od, u16_t len, void *value) reentrant;

void rmon_event_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_event_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
u8_t rmon_event_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_event_set_value(struct obj_def *od, u16_t len, void *value) reentrant;
void rmon_log_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
void rmon_log_get_value(struct obj_def *od, u16_t len, void *value) reentrant;





void mib2_rmon_init(void);


void snmp_insert_hisidx_tree(u32_t history_index, u32_t sample_index);
void snmp_rmon_get_sample(u32_t sample_index);
void snmp_delete_hisidx_tree(u32_t history_index, u32_t sample_index);

u32_t snmp_rmon_get_sample_value();
void snmp_rmon_event(u8_t event_index);
void snmp_rmon_time_event();

#endif /* end of LWPS_SNMP */
#endif /* end of LWPS_SNMP_RMON */


#endif /* end of __RMON_H__ */
