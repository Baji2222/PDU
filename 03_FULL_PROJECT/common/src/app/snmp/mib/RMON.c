/*
* Copyright (c)                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, RFC 2819
* 
*$Id: rfc2819.c,v 1.1 2008/06/20 08:53:35 yajin_zhou Exp $
*/

#include <local/config.h>
#include <snmp/mib/RMON.h>
#include <snmp/mib/mib.h>
#include <snmp/snmp_debug.h>

#if !defined(SAL_REPLACE)
#include <local/rtk_sal_mib.h>
#else
#include <snmp/mib_glue.h>
#endif


#if LWPS_SNMP

#if LWPS_SNMP_RMON

extern volatile uint32 data current_time;
extern far const struct snmp_obj_id ifspecific;


/* RMON.statistics.etherstatstable.etherStatsEntry  .1.3.6.1.2.1.16.1.1.1 */
struct mib_list_rootnode statistics_root = {
  &rmon_stat_get_object_def,
  &rmon_stat_get_value,
  &rmon_stat_set_test,
  &rmon_stat_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t statisticsentry_ids[21] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21
};
far struct mib_node *const statisticsentry_nodes[21] = {
  (struct mib_node *) &statistics_root,
  (struct mib_node *) &statistics_root,
  (struct mib_node *) &statistics_root,
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root, 
  (struct mib_node *) &statistics_root
};
far const struct mib_array_node statisticsentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  21,
  statisticsentry_ids,
  statisticsentry_nodes
};

/* RMON.statistics.etherstatstable  .1.3.6.1.2.1.16.1.1 */
far const s32_t statisticstable_ids[1] = { 1 };
far struct mib_node *const statisticstable_nodes[1] = {
  (struct mib_node *) &statisticsentry
};
struct mib_ram_array_node xdata statisticstable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  1,
  statisticstable_ids,
  statisticstable_nodes
};

/* RMON.statistics  .1.3.6.1.2.1.16.1 */
far const s32_t statistics_ids[1] = { 1 };
far struct mib_node *const statistics_nodes[1] = {
  (struct mib_node *) &statisticstable
};
far const struct mib_array_node statistics = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  statistics_ids,
  statistics_nodes
};

/* RMON .1.3.6.1.2.1.16 */
far const s32_t rmon_ids[1] = { 1};
far struct mib_node *const rmon_nodes[1] = {
  (struct mib_node *) &statistics
};
struct mib_array_node rmon = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  rmon_ids,
  rmon_nodes
};

/* mib-2.rmon counter(s) */
struct snmp_obj_id xdata rmon_stat_datasource[COMBO_PORT_NUM];
u8_t xdata rmon_stat_owner[COMBO_PORT_NUM][16];
u8_t xdata rmon_stat_owner_len[COMBO_PORT_NUM];
u32_t xdata rmon_stat_status[COMBO_PORT_NUM];


/*
 * Function: snmp_insert_statsidx_tree
 * -----------------------------------
 * Inserts a node into the RMON statistics table for SNMP traversal.
 * Adds one entry with the specified index.
 * Enables getnext traversal on the statisticstable.
 */
void snmp_insert_statsidx_tree(s32_t index)
{
    struct mib_list_node *if_node = NULL;

    snmp_mib_node_insert(&statistics_root, index, &if_node);
    /* enable getnext traversal on filled table */
    statisticstable.maxlength = 1;
}

/*
 * Function: rmon_stat_get_object_def
 * ----------------------------------
 * Defines RMON statistics entry objects for RFC2819.
 * Handles etherStatsIndex, etherStatsDataSource, etherStatsDropEvents, etc.
 * Sets ASN.1 types and access rights for each OID.
 */
void rmon_stat_get_object_def(u8_t ident_len, s32_t * ident, struct obj_def *od) reentrant
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
        DBG_SNMP_PRINTF(("rmon_stat_get_object_def  .%" U16_F "\n", (u16_t) id));
        switch (id)
        {
        case 1:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(s32_t);
            break;

        case 2:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID);
            //od->v_len = ifspecific.len * sizeof(s32_t);
            od->v_len = rmon_stat_datasource[0].len * sizeof(s32_t);
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        case 20:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
            od->v_len = (u16_t)rmon_stat_owner_len[ident[1] - 1];
            break;
        case 21:
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(u32_t);
            break;
        default:
            DBG_SNMP_PRINTF(("rmon_stat_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;
        };
    }
    else
    {
        DBG_SNMP_PRINTF(("rmon_stat_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }

}

/*
 * Function: rmon_stat_get_value
 * -----------------------------
 * Retrieves RMON statistics entry values for each interface.
 * Returns etherStatsIndex, DataSource, DropEvents, Octets, Pkts, BroadcastPkts, etc.
 * Uses GET_ETHER_STATS macros to retrieve counter values.
 */
void rmon_stat_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{

    u8_t xdata index;
    u8_t xdata id;
    xdata u32_t *xdata uint_ptr = value;
	xdata s32_t *xdata int_ptr = value;

    /*kill keil warning */
    if (len)
    {
    }
    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];
    //DBG_SNMP_PRINTF(("id %bd index %bd \n", id, index));
	
    /*id=1: port no */
    switch (id)
    {
    case 1:
		*int_ptr = index;
		break;
		
	case 2: /* etherStatsDataSource */
		//objectidncpy((s32_t *) value, (s32_t *) ifspecific.id, len / sizeof(s32_t));
		objectidncpy((s32_t *) value, (s32_t *) rmon_stat_datasource[index-1].id, len / sizeof(s32_t));
		//GET_ETHER_STATS_DATA_SOURCE( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
		break;
		
    case 3: /* etherStatsDropEvent */
		GET_ETHER_STATS_DROP_EVENTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
	case 4: /* etherStatsOctets */		
            {
              u32_t xdata value64[2];
		GET_ETHER_STATS_OCTETS( ((u8_t)index-1), ((u8_t)(sizeof(u32_t) * 2)), ((u8_t*)value64));
              *uint_ptr = value64[1];
            }
		break;

    case 5: /* etherStatsPkts */
		{ //GET_ETHER_STATS_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
		u32_t xdata tmp = 0;
		GET_ETHER_STATS_PKTS_64_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr = tmp;
              GET_ETHER_STATS_PKTS_65TO127_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr += tmp;
              GET_ETHER_STATS_PKTS_128TO255_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr += tmp;
              GET_ETHER_STATS_PKTS_256TO511_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr += tmp;
              GET_ETHER_STATS_PKTS_512TO1023_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr += tmp;
              GET_ETHER_STATS_PKTS_1024TO1518_OCTECTS(((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)&tmp));
              *uint_ptr += tmp; 
		 }
		break;

    case 6: /* etherStatsBroadcastPkts */
        GET_ETHER_STATS_BROADCAST_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 7: /* etherStatsMulticastPkts */
        GET_ETHER_STATS_MULTICAST_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 8: /* etherStatsCRCAlignErrors */
        //GET_ETHER_STATS_CRC_ALIGN_ERRORS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
         *uint_ptr = 0;
        break;
		
    case 9: /* etherStatsUndersizePkts */
        GET_ETHER_STATS_UNDERSIZE_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 10: /* etherStatsOversizePkts */
        GET_ETHER_STATS_OVERSIZE_PKTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 11: /* etherStatsFragments */
        GET_ETHER_STATS_FRAGMENTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 12: /* etherStatsJabbers */
        GET_ETHER_STATS_JABBERS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 13: /* etherStatsCollisions */
        GET_ETHER_STATS_COLLISIONS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 14: /* etherStatsPkts64Octets */
        GET_ETHER_STATS_PKTS_64_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 15: /* etherStatsPkts65To127Octets */
        GET_ETHER_STATS_PKTS_65TO127_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 16:/* etherStatsPkts128To255Octets */
        GET_ETHER_STATS_PKTS_128TO255_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 17:/* etherStatsPkts256To511Octets */
        GET_ETHER_STATS_PKTS_256TO511_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 18:/* etherStatsPkts512To1023Octets */
        GET_ETHER_STATS_PKTS_512TO1023_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
    case 19:/* etherStatsPkts1024To1518Octets */
        GET_ETHER_STATS_PKTS_1024TO1518_OCTECTS( ((u8_t)index-1), ((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        break;
		
	case 20:/* etherStatsOwner */
		ocstrncpy(value, &rmon_stat_owner[index - 1][0], len);
		break;
		
	case 21:/* etherStatsStatus */
		*int_ptr = rmon_stat_status[index - 1];
		break;
		
	default:
		*int_ptr = 0;
    }

}

/*
 * Function: rmon_stat_set_test
 * ----------------------------
 * Tests write permission for RMON statistics objects.
 * Allows writing to etherStatsStatus (id=21) and etherStatsOwner (id=20).
 * Validates owner string length (<64 characters).
 */
u8_t rmon_stat_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t id, set_ok;

    if (len)
    {
    }
    if (value)
    {
    }
    set_ok = 0;
    id = od->id_inst_ptr[0];
    if ((id == 21))
        set_ok = 1;
    else if (id == 20)
    {
        if (len < 64)
            set_ok = 1;
    }

    return set_ok;
}

/*
 * Function: rmon_stat_set_value
 * -----------------------------
 * Writes values to RMON statistics entry objects.
 * Sets etherStatsOwner and etherStatsStatus.
 * Updates global arrays for owner string and status.
 */
void rmon_stat_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{
    u8_t id;
    u8_t index;
    s32_t *sint_ptr = value;
    if (len)
    {
    }
    id = od->id_inst_ptr[0];
    index = od->id_inst_ptr[1];

    DBG_SNMP_PRINTF(("index %bd id %bd *sint_ptr ld \n", index, id, *sint_ptr));

    //if (id==2)
    // {
    //     rmon_stat_datasource[index-1]= *sint_ptr;
    // }
    // else 
    if (id == 21)
    {
        rmon_stat_status[index - 1] = *sint_ptr;
    }
    else if (id == 20)
    {
        rmon_stat_owner_len[index - 1] = len;
        ocstrncpy(&rmon_stat_owner[index - 1][0], value, len);
    }
}

/*
 * Function: mib2_rmon_init
 * ------------------------
 * Initializes RMON MIB (RFC2819) statistics tables.
 * Sets owner name to PROJECT_NAME, status to 0.
 * Builds datasource OID for each port (ifIndex).
 * Inserts statistics entry for each interface.
 */
void mib2_rmon_init(void)
{
    u8_t xdata i;
    DBG_SNMP_PRINTF(("\nmib2_rmon_init\n"));
    
	for(i=0;i<INTERFACE_NUMBER;i++)
	{
		strcpy(rmon_stat_owner[i], PROJECT_NAME);
		rmon_stat_owner_len[i] = (u8_t)strlen(rmon_stat_owner[i]);
		rmon_stat_status[i] = 0;
		rmon_stat_datasource[i].len = 11;
		/* 1.3.6.1.2.1.2.2.1.1.x */
		rmon_stat_datasource[i].id[0] = 1UL;
		rmon_stat_datasource[i].id[1] = 3UL;
		rmon_stat_datasource[i].id[2] = 6UL;
		rmon_stat_datasource[i].id[3] = 1UL;
		rmon_stat_datasource[i].id[4] = 2UL;
		rmon_stat_datasource[i].id[5] = 1UL;
		rmon_stat_datasource[i].id[6] = 2UL;
		rmon_stat_datasource[i].id[7] = 2UL;
		rmon_stat_datasource[i].id[8] = 1UL;
		rmon_stat_datasource[i].id[9] = 1UL;
		rmon_stat_datasource[i].id[10] = (uint32)(i+1);
		
		snmp_insert_statsidx_tree(i+1);
	}

}

#endif /* end of LWPS_SNMP_RMON */

#endif /* end of LWPS_SNMP */