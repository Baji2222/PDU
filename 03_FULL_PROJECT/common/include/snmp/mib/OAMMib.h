
#ifndef __OAM_MIB_H__
#define __OAM_MIB_H__

#include <string.h>
#include "local/config.h"
#include "snmp/snmp.h"
#include "snmp/snmp_structs.h"
#include "snmp/snmp_debug.h"
#include "snmp/snmp_asn1.h"
#include "snmp/snmp_msg.h"

#if LWPS_SNMP

#if LWPS_SNMP_OAM_MIB

#define MIB_OAM_ID       (158)
#define MIB_OAM_HANDLER  ((struct mib_node* const)&oamMIB)

extern far const struct mib_array_node oamMIB;

extern void snmp_inc_oammibTable(void);
extern void oammibTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant;
extern void oammibTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern void oammibTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant;
extern u8_t oammibTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant;





#endif /* end of LWPS_SNMP_OAM_MIB */

#endif /* end of LWPS_SNMP */

#endif /* end of __OAM_MIB_H__ */

