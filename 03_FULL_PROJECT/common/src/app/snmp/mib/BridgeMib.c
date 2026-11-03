/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter,  rfc2863
* 
*$Id: rfc2863.c,v 1.1 2008/06/20 08:53:35 caroline_liu Exp $
*/
#pragma STRING(far)
#include "local/config.h"
#include "snmp/mib/BridgeMib.h"
#include "snmp/mib/mib.h"
#include "snmp/snmp_debug.h"

#if !defined(SAL_REPLACE)
#include <local/rtk_sal_mib.h>
#else
#include <snmp/mib_glue.h>
#endif

#if LWPS_SNMP

#if LWPS_SNMP_BRIDGE_MIB
#if 0
/* ifMIB.bridgeMIBObjects.bridgeXTable.bridgeXEntry .1.3.6.1.2.1.17.4.4.1 */
struct mib_list_rootnode xdata bridgeXTable_root = {
  &bridgeXTable_get_object_def,
  &bridgeXTable_get_value,
  &bridgeXTable_set_test,
  &bridgeXTable_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
#endif
/* ifMIB.bridgeMIBObjects.bridgeXTable.bridgeXEntry .1.3.6.1.2.1.17.4.4.1 */
struct mib_list_rootnode  xdata bridgeXTable_root = {
  &bridgeXTable_get_object_def,
  &bridgeXTable_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};


far const s32_t bridgeXentry_ids[5] = {
  1,2, 3, 4, 5
};
far struct mib_node *const bridgeXentry_nodes[5] = {
  (struct mib_node *) &bridgeXTable_root,
  (struct mib_node *) &bridgeXTable_root,
  (struct mib_node *) &bridgeXTable_root,
  (struct mib_node *) &bridgeXTable_root,
  (struct mib_node *) &bridgeXTable_root
};
far const struct mib_array_node bridgeXentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  5,
  bridgeXentry_ids,
  bridgeXentry_nodes
};

/* bridgeMIB.bridgeMIBObjects.bridgeXTable .1.3.6.1.2.1.17.4.4 */
far const s32_t bridgeXTable_ids[1] =  {1};
far struct mib_node *const bridgeXTable_nodes[1] = {
  (struct mib_node * const) &bridgeXentry
};
struct mib_ram_array_node xdata bridgeXTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  bridgeXTable_ids,
  bridgeXTable_nodes
};


/* bridgeMIB.bridgeMIBObjects .1.3.6.1.2.1.17.4 */
/* 6 subitems for RFC2863, only support 1 */
far const s32_t bridgeMIBObjects_ids[1] = { 4};
far struct mib_node *const bridgeMIBObjects_nodes[1] = {
  (struct mib_node *) &bridgeXTable,
};


far const struct mib_array_node bridgeMIBObjects = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1, /*4 for RFC2863 */
  bridgeMIBObjects_ids,
  bridgeMIBObjects_nodes
};


/* bridgeMIB .1.3.6.1.2.1.17 */
far const s32_t bridgeMIB_ids[1] = { 4 };
far struct mib_node *const bridgeMIB_nodes[1] = {
  (struct mib_node *) &bridgeMIBObjects
};
struct mib_array_node bridgeMIB = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  bridgeMIB_ids,
  bridgeMIB_nodes
};

/*
 * Function: snmp_inc_bridgeXTable
 * -------------------------------
 * Inserts a node into the bridgeXTable list for SNMP traversal.
 * Adds one entry with count+1 as object ID.
 * Enables getnext traversal on the bridgeXTable.
 */
void snmp_inc_bridgeXTable(void)
{
    struct mib_list_node *bridge_node = NULL;
	
   snmp_mib_node_insert(&bridgeXTable_root, bridgeXTable_root.count + 1, &bridge_node) ;
    /* enable getnext traversal on filled table */
    bridgeXTable.maxlength = 1;

}

/*
 * Function: bridgeXTable_get_object_def
 * -------------------------------------
 * Defines bridgeXTable entry objects for SNMP MIB.
 * Handles bridgeXPort, bridgeXPortMtu, and bridgeXPort counters.
 * Sets ASN.1 types and access rights for each OID.
 */
void bridgeXTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
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

        DBG_SNMP_PRINTF(("bridgeXTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {
        case 1:
        case 2:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);
            break;
        case 3:
        case 4:
        case 5:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
       

        default:
            DBG_SNMP_PRINTF(("bridgeXTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }

    }
    else
    {
        DBG_SNMP_PRINTF(("bridgeXTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: bridgeXTable_get_value
 * --------------------------------
 * Retrieves bridgeXTable entry values for each interface.
 * Returns bridgeXPort (index), bridgeXPortMtu, and bridgeXPort counters.
 * Uses GET_DOT1D macros to retrieve bridge statistics.
 */
void bridgeXTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t xdata index;
    u8_t xdata id;

    /*kill keil warning */
    if (len)
    {
    }

    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];

    switch (id)
    {
    case 1:
        {
           s32_t *xdata int_ptr = value;
	    *int_ptr = index;
        }
        break;
    case 2:
       {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = P_BUFFER_SIZE;
            break;
        }
    case 3:
        {
            /*Dot1dTpHCPortInFrames */
            u32_t *xdata uint_ptr = value;
            GET_DOT1D_TPHCPORT_IN_FRAMES(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;


    case 4:
        {
           u32_t *xdata uint_ptr = value;
            /*Dot1dTpHCPortOutFrames */
	     GET_DOT1D_TPHCPORT_OUT_FRAMES(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 5:
        {
            u32_t *xdata uint_ptr = value;
            /*Dot1dTpHCPortInDiscards */
	    GET_DOT1D_TPHCPORT_IN_DISCARDS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    }


}
#if 0
u8_t bridgeXTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{

		return 1;
}


void bridgeXTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{
	return;
}
#endif
#endif /* end of LWPS_SNMP_BRIDGE_MIB */

#endif