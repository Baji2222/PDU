/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter,  rfc2863
* 
*$Id: rfc2863.c,v 1.1 2008/06/20 08:53:35 caroline_liu Exp $
*/
#include "local/config.h"
#include "snmp/mib/OAMMib.h"
#include "snmp/mib/mib.h"
#include "snmp/snmp_debug.h"
#include "local/rtk_sal_mib.h"

#if LWPS_SNMP

#if LWPS_SNMP_OAM_MIB
#if 0
/* oamMIB.oamMIBObjects.oammibTable.oammibEntry .1.3.6.1.2.1.158.2.4.1 */
struct mib_list_rootnode xdata oammibTable_root = {
  &oammibTable_get_object_def,
  &oammibTable_get_value,
  &oammibTable_set_test,
  &oammibTable_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
#endif
/* oamMIB.oamMIBObjects.oammibTable.oammibEntry .1.3.6.1.2.1.158.2.4.1 */
struct mib_list_rootnode xdata oammibTable_root = {
  &oammibTable_get_object_def,
  &oammibTable_get_value,
  NULL,
  NULL,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};


far const s32_t oammibentry_ids[2] = {
  100,101
};
far struct mib_node *const oammibentry_nodes[2] = {
  (struct mib_node *) &oammibTable_root,
  (struct mib_node *) &oammibTable_root
};
far const struct mib_array_node oammibentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  2,
  oammibentry_ids,
  oammibentry_nodes
};

/* oamMIB.oamMIBObjects.oammibTable .1.3.6.1.2.1.158.2.4 */
far const s32_t oammibTable_ids[1] =  { 1 };
far struct mib_node *const oammibTable_nodes[1] = {
  (struct mib_node * const) &oammibentry
};
struct mib_ram_array_node xdata oammibTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  oammibTable_ids,
  oammibTable_nodes
};


/* oamMIB.oamMIBObjects .1.3.6.1.2.1.158.2 */
/* 6 subitems for RFC2863, only support 1 */
far const s32_t oamMIBObjects_ids[1] = { 4};
far struct mib_node *const oamMIBObjects_nodes[1] = {
  (struct mib_node *) &oammibTable,
};
far const struct mib_array_node oamMIBObjects = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1, /*4 for RFC2863 */
  oamMIBObjects_ids,
  oamMIBObjects_nodes
};

mib_scalar_node xdata oamMIBObjects_scalar;


/* oamMIB .1.3.6.1.2.1.158 */
far const s32_t oamMIB_ids[1] = { 2 };
far struct mib_node *const oamMIB_nodes[1] = {
  (struct mib_node *) &oamMIBObjects
};
far const struct mib_array_node oamMIB = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  oamMIB_ids,
  oamMIB_nodes
};


void snmp_inc_oammibTable(void)
{
    struct mib_list_node *oam_node = NULL;

	
    snmp_mib_node_insert(&oammibTable_root, oammibTable_root.count + 1, &oam_node);
    /* enable getnext traversal on filled table */
    oammibTable.maxlength = 1;

}

void oammibTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;
	u8_t xdata index;

	DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));

	index = od->id_inst_ptr[1];
	
    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];

        DBG_SNMP_PRINTF(("oammibTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 100:
        case 101:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
       

        default:
            DBG_SNMP_PRINTF(("oammibTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }

    }
    else
    {
        DBG_SNMP_PRINTF(("oammibTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}




void oammibTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t xdata index;
    u8_t xdata id;
    u32_t *xdata uint_ptr = value;


    /*kill keil warning */
    if (len)
    {
    }


    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];

    switch (id)
    {
    case 100:
        {
             /*Dot3OamFramesTx */
            GET_DOT3_OAM_FRAMES_TX(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;


    case 101:
        {

            /*Dot3OamFramesRx */
			GET_DOT3_OAM_FRAMES_RX(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;


    }


}
#if 0
u8_t oammibTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{


}


void oammibTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{


}
#endif
#endif /* end of LWPS_SNMP_INTERFACE_GROUP */

#endif

