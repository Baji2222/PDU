#pragma STRING (far)
/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, rfc3635
* 
*$Id: rfc3635.c,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/

#include <local/config.h>
#include <salport.h>
#include <snmp/mib/EthernetLike.h>
#include <snmp/mib/mib.h>
#include <snmp/snmp_debug.h>
#if !defined(SAL_REPLACE)
#include <local/rtk_sal_mib.h>
#else
#include <snmp/mib_glue.h>
#endif

#if LWPS_SNMP


#if LWPS_SNMP_ETHERNET_LIKE

/* SNMP dot3StatsDuplexStatus value */
#define PORT_STATUS_DUPLEX_UNKNOWN  (1)
#define PORT_STATUS_DUPLEX_HALF     (2)
#define PORT_STATUS_DUPLEX_FULL     (3)

/* SNMP dot3PauseAdminMode/dot3PauseOperMode */
#define PORT_STATUS_PAUSE_MODE_DISABLE         (1)
#define PORT_STATUS_PAUSE_MODE_XMIT            (2)
#define PORT_STATUS_PAUSE_MODE_RCV             (3)
#define PORT_STATUS_PAUSE_MODE_XMITANDRCV      (4)

struct mib_list_rootnode xdata dot3HCStatsTable_root = {
  &dot3HCStatsTable_get_object_def,
  &dot3HCStatsTable_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};

/* .1.3.6.1.2.1.10.7.11.1 */
far const s32_t dot3HCStatsentry_ids[6] = {1, 2, 3, 4, 5, 6};
far struct mib_node *const dot3HCStatsentry_nodes[6] = {
  (struct mib_node *) &dot3HCStatsTable_root,
  (struct mib_node *) &dot3HCStatsTable_root,
  (struct mib_node *) &dot3HCStatsTable_root,
  (struct mib_node *) &dot3HCStatsTable_root,
  (struct mib_node *) &dot3HCStatsTable_root,
  (struct mib_node *) &dot3HCStatsTable_root
};
far const struct mib_array_node dot3HCStatsentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  6,
  dot3HCStatsentry_ids,
  dot3HCStatsentry_nodes
};


/* .1.3.6.1.2.1.10.7.11 */
far const s32_t dot3HCStatsTable_id[1] =  { 1 };
far struct mib_node *const dot3HCStatsTable_node[1] = {
  (struct mib_node * const) &dot3HCStatsentry
};
struct mib_ram_array_node xdata dot3HCStatsTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  dot3HCStatsTable_id,
  dot3HCStatsTable_node
};

/*  .1.3.6.1.2.1.10.7.10.1 */
struct mib_list_rootnode xdata dot3PauseTable_root = {
  &dot3PauseTable_get_object_def,
  &dot3PauseTable_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t dot3Pauseentry_ids[6] = {1, 2, 3, 4, 5, 6};
far struct mib_node * const dot3Pauseentry_nodes[6] = {
  (struct mib_node *) &dot3PauseTable_root,
  (struct mib_node *) &dot3PauseTable_root,
  (struct mib_node *) &dot3PauseTable_root,
  (struct mib_node *) &dot3PauseTable_root,
  (struct mib_node *) &dot3PauseTable_root,
  (struct mib_node *) &dot3PauseTable_root
};
far const struct mib_array_node dot3Pauseentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  6,
  dot3Pauseentry_ids,
  dot3Pauseentry_nodes
};

/*  .1.3.6.1.2.1.10.7.10 */
far const s32_t dot3PauseTable_ids[1] = {1};
far struct mib_node *const dot3PauseTable_nodes[1] = {
  (struct mib_node * const) &dot3Pauseentry
};
struct mib_ram_array_node xdata dot3PauseTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  dot3PauseTable_ids,
  dot3PauseTable_nodes
};

/*  .1.3.6.1.2.1.10.7.9.1 */
struct mib_list_rootnode xdata dot3ControlTable_root = {
  &dot3ControlTable_get_object_def,
  &dot3ControlTable_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t dot3Controlentry_ids[3] = {1, 2, 3};
far struct mib_node *const dot3Controlentry_nodes[3] = {
  (struct mib_node *) &dot3ControlTable_root,
  (struct mib_node *) &dot3ControlTable_root,
  (struct mib_node *) &dot3ControlTable_root
};
far const struct mib_array_node dot3Controlentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  3,
  dot3Controlentry_ids,
  dot3Controlentry_nodes
};

/*  .1.3.6.1.2.1.10.7.9 */
far const s32_t dot3ControlTable_ids[1] = { 1 };
far struct mib_node *const dot3ControlTable_nodes[1] = {
  (struct mib_node * const) &dot3Controlentry
};
struct mib_ram_array_node xdata dot3ControlTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  dot3ControlTable_ids,
  dot3ControlTable_nodes
};

/* .1.3.6.1.2.1.10.7.5.1 */
struct mib_list_rootnode xdata dot3CollTable_root = {
  &dot3CollTable_get_object_def,
  &dot3CollTable_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t dot3Collentry_ids[1] = { 3 };
far struct mib_node *const dot3Collentry_nodes[1] = {
	(struct mib_node *) &dot3CollTable_root
};
far const struct mib_array_node dot3Collentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR, 
  1,
  dot3Collentry_ids,
  dot3Collentry_nodes
};

/* .1.3.6.1.2.1.10.7.5 */
far const s32_t dot3CollTable_ids[1] = { 1 };
far struct mib_node *const dot3CollTable_nodes[1] = {
  (struct mib_node * const) &dot3Collentry
};
struct mib_ram_array_node xdata dot3CollTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  dot3CollTable_ids,
  dot3CollTable_nodes
};

/* .1.3.6.1.2.1.10.7.2.1 */
struct mib_list_rootnode xdata dot3StatsTable_root = {
  &dot3Statsentry_get_object_def,
  &dot3Statsentry_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t dot3Statsentry_ids[18] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 16, 17, 18, 19, 20, 21
};
far struct mib_node *const dot3Statsentry_nodes[18] = {
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
  (struct mib_node *) &dot3StatsTable_root,
};
far const struct mib_array_node dot3Statsentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  18,
  dot3Statsentry_ids,
  dot3Statsentry_nodes
};

/* .1.3.6.1.2.1.10.7.2 */
far const s32_t dot3StatsTable_ids[1] = { 1 };
far struct mib_node * const dot3StatsTable_nodes[1] = {
  (struct mib_node * const) &dot3Statsentry
};
struct mib_ram_array_node xdata dot3StatsTable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  dot3StatsTable_ids,
  dot3StatsTable_nodes
};

/* .1.3.6.1.2.1.10.7 */
//far const s32_t dot3_ids[5] = {2, 5, 9, 10, 11};
far const s32_t dot3_ids[3] = {2, 9, 10 };
//far struct mib_node * const dot3_nodes[5] = {
far struct mib_node * const dot3_nodes[3] = {
  (struct mib_node *) &dot3StatsTable,
  //(struct mib_node *) &dot3CollTable,
  (struct mib_node *) &dot3ControlTable,
  (struct mib_node *) &dot3PauseTable
//  (struct mib_node *) &dot3HCStatsTable
};
far const struct mib_array_node dot3 = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
//  5,
  3,
  dot3_ids,
  dot3_nodes
};

/* .1.3.6.1.2.1.10 */
far const s32_t transmission_ids[1] = { 7 };
far struct mib_node* const transmission_nodes[1] = { (struct mib_node *) &dot3 };
struct mib_array_node transmission = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  transmission_ids,
  transmission_nodes
};

/*interface*/
far const struct snmp_obj_id chipspecific = { 2, {0, 0} };


/*
 * Function: snmp_inc_dot3StatsTable
 * --------------------------------
 * Inserts a node into dot3StatsTable for SNMP traversal.
 * Adds one entry to enable getnext operations.
 * Sets maxlength to 1 for table access.
 */
void snmp_inc_dot3StatsTable(void) large
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&dot3StatsTable_root, dot3StatsTable_root.count + 1, &if_node);
    /* enable getnext traversal on filled table */
    dot3StatsTable.maxlength = 1;
}

/*
 * Function: snmp_insert_dot3CollTable_tree
 * ----------------------------------------
 * Inserts a two-level tree entry into dot3CollTable.
 * Creates nested nodes for ifIndex and collision count.
 * Used for SNMP getnext traversal of collision statistics.
 */
void snmp_insert_dot3CollTable_tree(u32_t if_index, u32_t dot3CollCount) large
{

    struct mib_list_rootnode *iprte_rn;
    struct mib_list_node *iprte_node;
    s32_t iprteidx[2];
    u8_t xdata level;

    iprteidx[0] = if_index;
    iprteidx[1] = dot3CollCount;
    level = 0;
    iprte_rn = &dot3CollTable_root;
    while (level < 2)
    {
        iprte_node = NULL;
        snmp_mib_node_insert(iprte_rn, iprteidx[level], &iprte_node);
        if ((level != 1) && (iprte_node != NULL))
        {
            if (iprte_node->nptr == NULL)
            {
                iprte_rn = snmp_mib_lrn_alloc();
                iprte_node->nptr = (struct mib_node *) iprte_rn;
                if (iprte_rn != NULL)
                {
                    if (level == 0)
                    {
                    	iprte_rn->get_object_def = dot3CollTable_get_object_def;
						iprte_rn->get_value = dot3CollTable_get_value;
                    }
                }
                else
                {
                    /* iprte_rn == NULL, malloc failure */
                    DBG_SNMP_PRINTF(("snmp_insert_hisidx_tree() insert failed, mem full"));
                    break;
                }
            }
            else
            {
                iprte_rn = (struct mib_list_rootnode *) iprte_node->nptr;
            }
        }
        level++;
    }
    /* enable getnext traversal on filled table */
}

/*
 * Function: snmp_inc_dot3ControlTable
 * -----------------------------------
 * Inserts a node into dot3ControlTable for SNMP traversal.
 * Adds one entry to enable getnext operations.
 * Sets maxlength to 1 for table access.
 */
void snmp_inc_dot3ControlTable(void) large
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&dot3ControlTable_root, dot3ControlTable_root.count + 1, &if_node);
    /* enable getnext traversal on filled table */
    dot3ControlTable.maxlength = 1;
}

/*
 * Function: snmp_inc_dot3PauseTable
 * ---------------------------------
 * Inserts a node into dot3PauseTable for SNMP traversal.
 * Adds one entry to enable getnext operations.
 * Sets maxlength to 1 for table access.
 */
void snmp_inc_dot3PauseTable(void) large
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&dot3PauseTable_root, dot3PauseTable_root.count + 1, &if_node);
    /* enable getnext traversal on filled table */
    dot3PauseTable.maxlength = 1;
}

/*
 * Function: snmp_inc_dot3HCStatsTable
 * -----------------------------------
 * Inserts a node into dot3HCStatsTable for SNMP traversal.
 * Adds one entry to enable getnext operations.
 * High-capacity statistics table for 64-bit counters.
 */
void snmp_inc_dot3HCStatsTable(void) large
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&dot3HCStatsTable_root, dot3HCStatsTable_root.count + 1, &if_node);
    /* enable getnext traversal on filled table */
}

/*
 * Function: dot3Statsentry_get_object_def
 * ---------------------------------------
 * Defines dot3Stats entry objects for Ethernet-like MIB.
 * Handles alignment errors, FCS errors, collision frames, etc.
 * Sets ASN.1 types as counters with read-only access.
 */
void dot3Statsentry_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;
    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("dot3Statsentry_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
        case 16:
        case 18:
        case 19:
        case 20:
        case 21:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;

        case 17:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID);
            od->v_len = chipspecific.len * sizeof(s32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("dot3Statsentry_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }
    }
    else
    {
        DBG_SNMP_PRINTF(("dot3Statsentry_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: dot3Statsentry_get_value
 * ----------------------------------
 * Retrieves dot3Stats entry values for Ethernet statistics.
 * Returns alignment errors, FCS errors, collisions, duplex status, etc.
 * Uses GET_DOT3_STATS macros to retrieve counter values.
 */
void dot3Statsentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant
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
    case 1:                    /* dot3StatsIndex */
        {
            s32_t *xdata sint_ptr = value;
            *sint_ptr = od->id_inst_ptr[1];
        }
        break;
    case 2:                    /* dot3StatsAlignmentErrors */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_ALIGNMENT_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 3:                    /* dot3StatsFCSErrors */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_FCS_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;
    case 4:                    /* dot3StatsSingleCollisionFrames */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_SINGLE_COLLISION_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 5:                    /* dot3StatsMultipleCollisionFrames */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_MULTIPLE_COLLISION_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 6:                    /* dot3StatsSQETestErrors */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = 0;
		//GET_DOT3_STATS_SQETEST_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 7:                    /* dot3StatsDeferredTransmissions */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_DEFERED_TRANSMISSIONS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;

    case 8:                    /* dot3StatsLateCollisions */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_LATE_COLLISIONS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 9:                    /* dot3StatsExcessiveCollisions */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_EXCESSIVE_COLLISIONS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 10:                   /* dot3StatsInternalMacTransmitErrors */
        {
            u32_t *uint_ptr = value;
            *uint_ptr = 0;
           // GET_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 11:                   /* dot3StatsCarrierSenseErrors */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = 0;
           //GET_DOT3_STATS_CARRIER_SENSE_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 13:                   /* dot3StatsFrameTooLongs */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_FRAME_TOO_LONGS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 16:                   /* dot3StatsInternalMacReceiveErrors */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = 0;
	     //GET_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 17:                  /* dot3StatsEtherChipSet */
        {
            objectidncpy((s32_t *) value, (s32_t *) chipspecific.id, len / sizeof(s32_t));
			//GET_DOT3_STATS_ETHER_CHIPSET( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 18:                   /* dot3StatsSymbolErrors */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_SYMBOL_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 19:                   /* dot3StatsDuplexStatus */
        {
            u32_t *xdata uint_ptr = value;
            sal_port_macStatus_t status;
            
            sal_port_macStatus_get(index-1, &status);
            if( SAL_PORT_DUPLEX_FULL==status.duplex )
            {
                *uint_ptr = (u32_t) PORT_STATUS_DUPLEX_FULL;
            }
            else if( SAL_PORT_DUPLEX_HALF==status.duplex )
            {
                *uint_ptr = (u32_t) PORT_STATUS_DUPLEX_HALF;
            }
            else
            {
                *uint_ptr = (u32_t) PORT_STATUS_DUPLEX_UNKNOWN;
            }
        }
        break;
    case 20:                   /* dot3StatsRateControlAbility */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = 1;
	     //	GET_DOT3_STATS_RATE_CONTROL_ABILITY( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 21:                   /* dot3StatsRateControlStatus */
        {
            u32_t *xdata uint_ptr = value;
            GET_DOT3_STATS_RATE_CONTROL_STATUS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;
    };
}

/*
 * Function: dot3CollTable_get_object_def
 * --------------------------------------
 * Defines dot3CollTable entry objects for collision statistics.
 * Handles dot3CollFrequencies object.
 * Sets ASN.1 type as counter with read-only access.
 */
void dot3CollTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;

    /* return to object name, adding index depth (2) */
    ident_len += 2;
    ident -= 2;
    if (ident_len == 3)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("rmon_dot3CollTable_get_object_def  .%" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 3:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("rmon_dot3CollTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;
        };
    }
    else
    {
        DBG_SNMP_PRINTF(("rmon_dot3CollTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }

}

/*
 * Function: dot3CollTable_get_value
 * ---------------------------------
 * Retrieves dot3CollTable collision frequency values.
 * Returns collision count for specified port.
 * Uses GET_DOT3_COLL_FREQUENCIES macro.
 */
void dot3CollTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t xdata id;
    u32_t xdata index;
    u32_t *xdata uint_ptr = value;

    /*kill keil warning */
    if (len)
    {
    }
    id = od->id_inst_ptr[0];
    index = od->id_inst_ptr[1];


    switch (id)
    {
    case 3:
        GET_DOT3_COLL_FREQUENCIES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        break;
    }

}

/*
 * Function: dot3ControlTable_get_object_def
 * -----------------------------------------
 * Defines dot3ControlTable entry objects for Ethernet control.
 * Handles dot3ControlFunctionsSupported, InUnknownOpcodes, HCInUnknownOpcodes.
 * Sets ASN.1 types for OCTET STRING, COUNTER, and COUNTER64.
 */
void dot3ControlTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;

    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("dot3ControlTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 1:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
            od->v_len = 1;

            break;
        case 2:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        case 3:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64);
            od->v_len = sizeof(u32_t) + sizeof(u32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("dot3ControlTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }



    }
    else
    {
        DBG_SNMP_PRINTF(("dot3ControlTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: dot3ControlTable_get_value
 * ------------------------------------
 * Retrieves dot3ControlTable control statistics.
 * Returns supported functions (fixed value), unknown opcodes (32-bit and 64-bit).
 * Uses GET_CONTROL_UNKNOWN_OPCODES macro for 32-bit counter.
 */
void dot3ControlTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t xdata index;
    u8_t xdata id;
    u8_t temp[1];

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
            temp[0] = 0xFF;
            ocstrncpy(value, temp, len);
        }
        break;
    case 2:
        {
            u32_t *xdata uint_ptr = value;

            /*dot3ControlInUnknownOpcodes */
            GET_CONTROL_UNKNOWN_OPCODES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));

        }
        break;
    case 3:
        {
            u32_t xdata value64[2];

            /*dot3HCControlInUnknownOpcodes */
            GET_HC_CONTROL_UNKNOWN_OPCODES( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);
        }
        break;

    }


}

/*
 * Function: dot3PauseTable_get_object_def
 * ---------------------------------------
 * Defines dot3PauseTable entry objects for PAUSE frame statistics.
 * Handles admin/oper mode, pause frames sent/received (32-bit and 64-bit).
 * Sets ASN.1 types for INTEGER, COUNTER, and COUNTER64.
 */
void dot3PauseTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;

    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("dot3PauseTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 1:
        case 2:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);

            break;
        case 3:
        case 4:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        case 5:
        case 6:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64);
            od->v_len = sizeof(u32_t) + sizeof(u32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("dot3PauseTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }
    }
    else
    {
        DBG_SNMP_PRINTF(("dot3PauseTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: dot3PauseTable_get_value
 * ----------------------------------
 * Retrieves dot3PauseTable PAUSE frame statistics.
 * Returns admin/oper pause mode, and pause frame counts (32/64-bit).
 * Uses GET_IN_PAUSE_FRAMES, GET_OUT_PAUSE_FRAMES, and HC versions.
 */
void dot3PauseTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u32_t *uint_ptr = value;
    u8_t   index;
    u8_t   id;
    sal_phyAbility_t ability;


    /*kill keil warning */
    if (len)
    {
    }


    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];

    switch (id)
    {
    case 1:
        sal_port_phyAbility_get((u8_t)index-1, &ability);
        if (ability.FC == SAL_PORT_FLOW_CTRL_DISABLE) {
			*uint_ptr = PORT_STATUS_PAUSE_MODE_DISABLE;
		}
		else if (ability.FC == SAL_PORT_FLOW_CTRL_ENABLE) {
			*uint_ptr = PORT_STATUS_PAUSE_MODE_XMITANDRCV;
		}

        break;
    case 2:
        {
            sal_port_macStatus_t    status;

            sal_port_macStatus_get((u8_t)index-1, &status);
            if ( SAL_PORT_FLOW_CTRL_DISABLE==status.fc || SAL_DISABLED==status.linkup ) {
    			*uint_ptr = PORT_STATUS_PAUSE_MODE_DISABLE;
    		}
    		else if ( SAL_PORT_FLOW_CTRL_ENABLE==status.fc ) {
    			*uint_ptr = PORT_STATUS_PAUSE_MODE_XMITANDRCV;
    		}
        }
        break;
    case 3:
        /*dot3InPauseFrames */
        GET_IN_PAUSE_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
    case 4:
        /*dot3OutPauseFrames */
        GET_OUT_PAUSE_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;

    case 5:
        {
            u32_t xdata value64[2];

            /*dot3HCInPauseFrames */
            GET_HC_IN_PAUSE_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);
        }
        break;

    case 6:
        {
            u32_t xdata value64[2];

            /*dot3HCOutPauseFrames */
            GET_HC_OUT_PAUSE_FRAMES( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);
        }
        break;
    }
}

/*
 * Function: dot3HCStatsTable_get_object_def
 * -----------------------------------------
 * Defines dot3HCStatsTable entry objects for high-capacity Ethernet statistics.
 * Handles 64-bit counters for alignment errors, FCS errors, etc.
 * Sets ASN.1 type as COUNTER64 for all objects.
 */
void dot3HCStatsTable_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
{
    u8_t xdata id;

    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("dot3HCStatsTable_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64);
            od->v_len = sizeof(u32_t) + sizeof(u32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("dot3HCStatsTable_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;


        }
    }
    else
    {
        DBG_SNMP_PRINTF(("dot3HCStatsTable_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: dot3HCStatsTable_get_value
 * ------------------------------------
 * Retrieves dot3HCStatsTable high-capacity Ethernet statistics.
 * Returns 64-bit counters for alignment errors, FCS errors, frame too longs, etc.
 * Uses GET_DOT3_HC_STATS macros and uncpy for 64-bit values.
 */
void dot3HCStatsTable_get_value(struct obj_def *od, u16_t len, void *value) reentrant
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
            u32_t xdata value64[2];

            /* dot3HCStatsAligmentError */
            GET_DOT3_HC_STATS_ALIGNMENT_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);

        }
        break;

    case 2:
        {
            u32_t xdata value64[2];

            /*dot3HCStatsFCSErrors */
            GET_DOT3_HC_STATS_FCS_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);

        }

        break;

    case 3:
        {
            u32_t xdata value64[2];

			/* dot3HCStatsInternalMacTransmitErrors */
            value64[0] = 0;
            value64[1] = 0;
			//GET_DOT3_HC_STATS_INTERNAL_MAC_TRANSMIT_ERRORS( ((u8_t)index-1)*2, ((u8_t)sizeof(u32_t)), ((u8_t*)value64));
            uncpy((u32_t *) value, (u32_t *) value64, 2);

        }
        break;

    case 4:
        {
            u32_t xdata value64[2];

			/* dot3HCStatsFrameTooLongs */
            GET_DOT3_HC_STATS_FRAME_TOO_LONGS( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
			uncpy((u32_t *) value, (u32_t *) value64, 2);
        }
        break;

    case 5:
        {
            u32_t xdata value64[2];

			/* dot3HCStatsInternalMacReceiveErrors */
            value64[0] = 0;
            value64[1] = 0;

			//GET_DOT3_HC_STATS_INTERNAL_MAC_RECEIVE_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
            uncpy((u32_t *) value, (u32_t *) value64, 2);

        }
        break;
    case 6:
        {
            u32_t xdata value64[2];

            /*dot3HCStatsSymbolErrors */
            GET_DOT3_HC_STATS_SYMBOL_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t))*2, ((u8_t*)value64));
            uncpy((u32_t *) value, (u32_t *) value64, 2);

        }
        break;

    }


}

#endif /* end of LWPS_SNMP_ETHERNET_LIKE */

#endif /* end of LWPS_SNMP */