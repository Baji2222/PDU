/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter,  rfc2863
* 
*$Id: rfc2863.c,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/
#include <local/config.h>
#include <salport.h>
#include <snmp/mib/InterfaceGroup.h>
#include <snmp/mib/mib.h>
#include <snmp/snmp_debug.h>
#include <snmp/snmp.h>
#if !defined(SAL_REPLACE)
#include <local/rtk_sal_mib.h>
#else
#include <snmp/mib_glue.h>
#endif


#if LWPS_SNMP

#if LWPS_SNMP_INTERFACE_GROUP

/* SNMP ifHighSpeed */
#define PORT_STATUS_SPEED_10M       (10UL)
#define PORT_STATUS_SPEED_100M      (100UL)
#define PORT_STATUS_SPEED_1000M     (1000UL)

/* SNMP ifConnectorPresent */
#define PORT_STATUS_LINK_UP         (1)
#define PORT_STATUS_LINK_DOWN       (2)

/* ifMIB.ifMIBObjects.ifXTable.ifXEntry .1.3.6.1.2.1.31.1.1 */
struct mib_list_rootnode xdata ifXTable_root = {
  &ifXTable_get_object_def,
  &ifXTable_get_value,
  &ifXTable_set_test,
  &ifXTable_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t ifXentry_ids[19] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 19
};
far struct mib_node *const ifXentry_nodes[19] = {
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root,
  (struct mib_node *) &ifXTable_root
};
far const struct mib_array_node ifXentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  19,
  ifXentry_ids,
  ifXentry_nodes
};

/* ifMIB.ifMIBObjects.ifXTable .1.3.6.1.2.1.31.1.1 */
far const s32_t ifXTable_ids[1] =  { 1 };
far struct mib_node *const ifXTable_nodes[1] = {
  (struct mib_node * const) &ifXentry
};
struct mib_ram_array_node xdata ifXTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  ifXTable_ids,
  ifXTable_nodes
};


/* ifMIB.ifMIBObjects .1.3.6.1.2.1.31.1 */
/* 6 subitems for RFC2863, only support 1 */
far const s32_t ifMIBObjects_ids[1] = { 1};
far struct mib_node *const ifMIBObjects_nodes[1] = {
  (struct mib_node *) &ifXTable,
};
far const struct mib_array_node ifMIBObjects = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1, /* 6 for RFC2863 */
  ifMIBObjects_ids,
  ifMIBObjects_nodes
};

mib_scalar_node xdata ifMIBObjects_scalar;


/* ifMIB .1.3.6.1.2.1.31 */
far const s32_t ifMIB_ids[1] = { 1 };
far struct mib_node *const ifMIB_nodes[1] = {
  (struct mib_node *) &ifMIBObjects
};
struct mib_array_node ifMIB = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  ifMIB_ids,
  ifMIB_nodes
};

u32_t xdata ifPromiscuousMode[COMBO_PORT_NUM];
u8_t xdata ifAlias[COMBO_PORT_NUM][CONFIG_SNMP_IF_ALIAS_LENGTH];
u8_t xdata ifAliasLen[COMBO_PORT_NUM];

/*
 * Function: snmp_inc_ifXTable
 * ---------------------------
 * Inserts a node into ifXTable for SNMP traversal.
 * Adds one entry to enable getnext operations.
 * Sets maxlength to 1 for table access.
 */
void snmp_inc_ifXTable(void)
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&ifXTable_root, ifXTable_root.count + 1, &if_node);
    /* enable getnext traversal on filled table */
    ifXTable.maxlength = 1;

}

/*
 * Function: ifXTable_get_object_def
 * ---------------------------------
 * Defines ifXTable entry objects for interface extension MIB.
 * Handles ifName, ifInMulticastPkts, ifInBroadcastPkts, ifHCInOctets, etc.
 * Sets ASN.1 types and access rights for each OID.
 */
void ifXTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
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

        DBG_SNMP_PRINTF(("ifXTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 1:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
			if( index>=10 )
			{
              od->v_len = 6;
			}
			else
			{
			  od->v_len = 5;
			}
            break;

        case 2:
        case 3:
        case 4:
        case 5:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        case 6:
        case 10:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64);
            od->v_len = sizeof(u32_t) + sizeof(u32_t);
            break;
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;

        case 14:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);
            break;

        case 15:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_GAUGE);
            od->v_len = sizeof(u32_t);
            break;

        case 16:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);
            break;
        case 17:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);
            break;

        case 18:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
            od->v_len = ifAliasLen[ident[1] - 1];
            break;
        case 19:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS);
            od->v_len = sizeof(u32_t);
            break;


        default:
            DBG_SNMP_PRINTF(("ifXTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }

    }
    else
    {
        DBG_SNMP_PRINTF(("ifXTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: ifXTable_get_value
 * ----------------------------
 * Retrieves ifXTable entry values for interface extension statistics.
 * Returns ifName, multicast/broadcast counters, HC counters, speed, link status.
 * Uses GET_IF_* macros to retrieve counter values.
 */
void ifXTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t xdata index;
    u8_t xdata id;
	u8_t xdata tmp[3];
	u8_t *u8_ptr = value;
    sal_port_macStatus_t status;


    /*kill keil warning */
    if (len)
    {
    }


    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];

	tmp[2] = '\0';
    switch (id)
    {
    case 1:
        {
			//ocstrncpy(value, "port", len);
			u8_ptr[0] = 'p';
			u8_ptr[1] = 'o';
			u8_ptr[2] = 'r';
			u8_ptr[3] = 't';
			if( index >= 10)
			{
				tmp[0] = (index/10)+'0';
				tmp[1] = (index%10)+'0';
			}
			else
			{
				tmp[0] = (index)+'0';
				tmp[1] = '\0';
			}
			if( (len-4) < ((u16_t)0xFFF0U) )
			{
				ocstrncpy(value+4, tmp, len-4);
			}
        }
        break;
    case 2:
        {
            u32_t *xdata uint_ptr = value;

            /*ifInMulticastPkts */
	    GET_IF_IN_MULTICAST_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 3:
        {
            u32_t *xdata uint_ptr = value;

            /*ifInBroadcastPkts */
            GET_IF_IN_BROADCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;


    case 4:
        {
            u32_t *xdata uint_ptr = value;

            /*ifOutMulticastPkts */
	    GET_IF_OUT_MULTICAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 5:
        {
            u32_t *xdata uint_ptr = value;

            /*ifOutBroadcastPkts */
	    GET_IF_OUT_BROADCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 6:                    /*ifHCInOctets */
        {
            u32_t *xdata uint_ptr = value;
            u32_t xdata value64[2];

            /* ifHCInOctets */
	    GET_IF_HC_IN_OCTETS(((u8_t)index-1), ((u8_t)(sizeof(u32_t)*2)), ((u8_t*)value64));
           uncpy((u32_t *) value, (u32_t *) value64, 2);
        }
        break;

    case 7:
        {
            u32_t  *xdata uint_ptr = value;
            /*ifHCInUcastPkts */
            GET_IF_HC_IN_UCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;


    case 8:
        {
            u32_t  *xdata uint_ptr = value;

            /*ifHCInMulticastPkts */
            GET_IF_HC_IN_MULTICAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 9:
        {
           u32_t  *xdata uint_ptr = value;
            /*ifHCInBroadcastPkts */
            GET_IF_HC_IN_BROADCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 10:                   /*ifHCOutOctets */
        {
            u32_t xdata value64[2];
            /*ifHCOutOctets */
            GET_IF_HC_OUT_OCTETS(((u8_t)index-1), ((u8_t)(sizeof(u32_t)*2)), ((u8_t*)value64));
            uncpy((u32_t *) value, (u32_t *) value64, 2);

        }
        break;

    case 11:
        {
            u32_t  *xdata uint_ptr = value;

            /*ifHCOutUcastPkts */
            GET_IF_HC_OUT_UCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;


    case 12:
        {
            u32_t  *xdata uint_ptr = value;
            /*ifHCOutMulticastPkts */
            GET_IF_HC_OUT_MULTICAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 13:
        {
            u32_t  *xdata uint_ptr = value;
            /*ifHCOutBroadcastPkts */
            GET_IF_HC_OUT_BROADCAST_PKTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;

    case 14:
        {
            u32_t *xdata uint_ptr = value;
#if LWPS_SNMP_TRAP_ENABLE
            u8_t  enable;
            snmp_linkUpdownTrap_get_enable(((u8_t)index-1), &enable);
            if(SAL_ENABLED == enable)
               *uint_ptr = 1;
            else
               *uint_ptr = 2; 
#else
            *uint_ptr = 2;
#endif
        }
        break;


    case 15:
        {
            u32_t *xdata uint_ptr = value;
            sal_port_macStatus_get(index-1, &status);

            //*uint_ptr = (u32_t)GET_PORT_SPEED( ((u8_t)index-1) );

            if (status.speed == SAL_PORT_SPEED_10) 
            {
				*uint_ptr = PORT_STATUS_SPEED_10M;
			}
			else if (status.speed == SAL_PORT_SPEED_100) 
            {
				*uint_ptr = PORT_STATUS_SPEED_100M;
			}
			else if (status.speed == SAL_PORT_SPEED_1000) 
            {
				*uint_ptr = PORT_STATUS_SPEED_1000M;
			}
            else
            {
                *uint_ptr = 0;
            }

        }
        break;

    case 16:
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = ifPromiscuousMode[index - 1];
        }
        break;

    case 17:
        {
            u32_t *xdata uint_ptr = value;
            sal_port_macStatus_get(index-1, &status);
            if( SAL_DISABLED==status.linkup )
            {
                *uint_ptr = (u32_t)( PORT_STATUS_LINK_DOWN );
            }
            else
            {
                *uint_ptr = (u32_t)( PORT_STATUS_LINK_UP );
            }

        }
        break;

    case 18:
        {
            ocstrncpy(value, &ifAlias[index - 1][0], len);
        }
        break;

    case 19:
        {

            u32_t *xdata uint_ptr = value;
            *uint_ptr = linkChangeTime[index-1];
        }
        break;

    }


}

/*
 * Function: ifXTable_set_test
 * ---------------------------
 * Tests write permission for ifXTable objects.
 * Validates ifLinkUpDownTrapEnable (1/2), ifPromiscuousMode (1/2), ifAlias (<64 chars).
 * Returns 1 if write allowed, 0 if denied.
 */
u8_t ifXTable_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t id, set_ok;
    xdata u32_t *xdata int_ptr = value;


    if (len)
    {
    }
    if (value)
    {
    }
    set_ok = 0;
    id = od->id_inst_ptr[0];
    switch (id)
    {
#if LWPS_SNMP_TRAP_ENABLE
     case 14:
        if ((*int_ptr == 1) || (*int_ptr == 2))
            set_ok = 1;    
        break;
#endif
    
    case 16:
        if ((*int_ptr == 1) || (*int_ptr == 2))
            set_ok = 1;
        break;

    case 18:
        if (len < 64)
            set_ok = 1;
        break;
    }
    return set_ok;


}

/*
 * Function: ifXTable_set_value
 * ----------------------------
 * Writes values to ifXTable entry objects.
 * Sets ifLinkUpDownTrapEnable, ifPromiscuousMode, ifAlias.
 * Updates global arrays for alias and promiscuous mode.
 */
void ifXTable_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t id, index;
    u32_t *uint_ptr = value;
#if LWPS_SNMP_TRAP_ENABLE
    u8_t enable;
#endif
      
    if (len)
    {
    }
    id = od->id_inst_ptr[0];
    index = od->id_inst_ptr[1];
    switch (id)
    {
#if LWPS_SNMP_TRAP_ENABLE
     case 14:  
        if(*uint_ptr == 2)
            enable = SAL_DISABLED;
        else
            enable = SAL_ENABLED;
        snmp_linkUpdownTrap_set_enable(((u8_t)index-1),enable);
        break;
#endif

    case 16:
        ifPromiscuousMode[index - 1] = *uint_ptr;
        break;

    case 18:

        ifAliasLen[index - 1] = len;
        ocstrncpy(&ifAlias[index - 1][0], value, len);
        break;
    }

}

#endif /* end of LWPS_SNMP_INTERFACE_GROUP */

#endif