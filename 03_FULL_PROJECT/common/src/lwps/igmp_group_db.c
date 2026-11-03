/*
 * Copyright Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * Program  : IGMP snooping Database
 * Abstract : 
 * Author   : 
 * $Id: $
 */

#pragma STRING (far)

/*
 * Include Files
 */
#include <local/lwps_config.h>

#if LWPS_IGMP

#include <common.h>
#include <common_types.h>
#include <lwps/lwps.h>
#include <lwps/igmp_snooping.h>
#include <lwps/igmp_group_db.h>
#include <saligmp.h>
#include <saltrunk.h>
#include <salutil.h>


/*
 * Symbol Definition
 */
typedef struct igmpdb_group_s{
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
}ipaddr_group_t;

/*
 * Data Declaration
 */
igmp_snoopingRouters_t xdata igmpRouter;
ipaddr_group_head_t    xdata igmpGroupHead;
ipaddr_group_head_t    xdata igmpGroupFreeHead;
ipaddr_group_list_t    xdata igmpGroupPool[MAX_GROUP_SIZE];
portmask_type          xdata igmpFastLeavePort;

/*
 * Macro Definition
 */
#if   CONFIG_IGMP_MLD_ENABLE

#define IGMP_CHECK_GROUP(addr1, addr2, cuur_mode, target_mode) \
    (IS_IPADDR_EQUAL((addr1),(addr2)) && ((target_mode)==(cuur_mode)))

#define IGMP_MLD_CALLING( inst, callv4, callv6 )               \
do{                                                            \
    if( IGMP_GRP_DB_IPV4==inst->ipvn )                         \
    {                                                          \
        (callv4); /* No SIP */                                   \
    }                                                          \
    else if( IGMP_GRP_DB_IPV6==inst->ipvn )                    \
    {                                                          \
        (callv6); /* No SIP */                                   \
    }                                                          \
}while(0)

#else /* else of CONFIG_IGMP_MLD_ENABLE */

#define IGMP_CHECK_GROUP(addr1, addr2, cuur_mode, target_mode) \
    ( IS_IPADDR_EQUAL((addr1),(addr2)) )

#define IGMP_MLD_CALLING( inst, callv4, callv6 )               \
do{                                                            \
    (callv4);                                                    \
}while(0)
    
#endif /* end of else of CONFIG_IGMP_MLD_ENABLE */


/*
 * Function Declaration
 */
#if DBG_LWPS_IGMP_ENABLE
// Print IGMP All: Debug function to display all IGMP group members and router ports
// Prints group member information and router port mask for debugging purposes
void print_igmp_all( igmp_print_type_t type)
{
    uint16 group_number;
    uint16 i;
    ipaddr_group_list_t *iter_group;

    group_number = igmpGroupHead.listNumber;
    iter_group = igmpGroupHead.item;

    OSAL_PRINTF("--print_igmp_all--------------------------------\n");

    if ( type&IGMP_PRINT_TYPE_GROUP_MEMBER )
    {
        OSAL_PRINTF("   [Group_Member]:\n");
        for (i=0; i<group_number; i++, iter_group=iter_group->next)
        {
            OSAL_PRINTF("                  [%d:%d:%d:%d]:0x%04X", \
                    (uint16)iter_group->groupAddr.addr[0], \ 
                    (uint16)iter_group->groupAddr.addr[1], \
                    (uint16)iter_group->groupAddr.addr[2], \
                    (uint16)iter_group->groupAddr.addr[3], \
                    (uint16)iter_group->fwdPortMask[0]);
            OSAL_PRINTF("(%d)", (uint16)iter_group->ipvn);
            OSAL_PRINTF("\n");
        }
    }

    if ( type&IGMP_PRINT_TYPE_ROUTER )
    {
        OSAL_PRINTF("  [Router_Member]:\n");
        OSAL_PRINTF("                  0x%08lX\n", igmpRouter.router_portMask);
    }

    OSAL_PRINTF("------------------------------------------------\n");
}
#endif /* end of DBG_LWPS_IGMP_ENABLE */

#if LWPS_IGMP_TRUNK_SUPPORT
/* FIXME, Move those functions to SAL */
// Get Trunk Portmask: Retrieves port mask for a specific trunk group
// Returns the port mask of all ports in the specified trunk group
static portmask_type get_trunk_portmask(uint8 tid){
    sal_pPortmask_t trunk_portmask;

    if ( SUCCESS==sal_trunk_port_get(tid, NULL, &trunk_portmask, NULL) )
    {
        return (trunk_portmask.bits[0]);
    }
    else
    {
        return 0;
    }
}

// Check Trunk Portmask: Checks if a port mask contains any trunk group ports
// Returns the trunk portmask if found, otherwise 0
portmask_type check_trunk_portmask(portmask_type portmask){
    portmask_type trunk_portmask;
    uint8 i;

    for(i=0; i<MAX_TRK_PORT; i++)
    {
        trunk_portmask = get_trunk_portmask(i);
        if ( 0!=(portmask&trunk_portmask) )
        {
            return (trunk_portmask);
        }
    }

    return 0;
}

#define IGMP_SET_DEFAULT_PORT(x, y)  ((x) = (y))

// IGMP Add Group Member by Trunk: Adds trunk group ports to multicast group
// Handles trunk port membership and updates hardware forwarding entries
static uint8 igmp_add_groupMember_byTrunk(ipaddr_group_list_t *inst, portmask_type portmask)
{
    portmask_type   trk_pm;
    uint8           hit = 0;
    uint8           pn, pn2;
	sal_ipaddr_t    groupAddr;
	sal_lPortmask_t grp_pm;
    portmask_type   router_pm;
    sal_enable_t    linkup;

    FOREACH_PORTINMASK(pn, portmask)
    {
        hit = 1;
        /* trk_pm will be zero if pn is not in any trunking group */
        trk_pm = check_trunk_portmask(1UL<<pn);
        FOREACH_PORTINMASK(pn2, trk_pm)
        {
            /* Remove un-linkup ports in trunking group */
            sal_port_link_get(pn2, &linkup);
            if(SAL_ENABLED != linkup)
            {
                trk_pm &= ~(1UL<<pn2);
            }
        }
        inst->fwdPortMask[0] |= (trk_pm | portmask);

        /* Force update HW record */
        groupAddr = ((uint32)inst->groupAddr.addr[0] << 24) |
                   ((uint32)inst->groupAddr.addr[1] << 16) |
                   ((uint32)inst->groupAddr.addr[2] <<  8) |               
                   ((uint32)inst->groupAddr.addr[3]);
        grp_pm.bits[0] = inst->fwdPortMask[0];

        /* Get and Add router ports to hardware port mask */
        igmp_group_get_queryPort(&router_pm);
        grp_pm.bits[0] |= router_pm;
        sal_igmp_ipAddrEntry_add(groupAddr, 0, inst->vid, &grp_pm);


        /* Update all trunk ports' timer */
        FOREACH_PORTINMASK(pn2, (trk_pm | portmask))
        {
            inst->portTimer[pn2] = IGMP_GROUP_MEM_INTERVAL;
        }
    }

    return hit;
}

// IGMP Delete Group Member by Trunk: Removes trunk group ports from multicast group
// Handles fast leave and timer expiration for trunk port memberships
static uint8 igmp_delete_groupMember_byTrunk(ipaddr_group_list_t *inst, portmask_type portmask)
{
    portmask_type   trk_pm;
    uint8           pn, pn2;
    uint8           hit = 0;
	sal_ipaddr_t    groupAddr;
	sal_lPortmask_t grp_pm;
    portmask_type router_pm;

    FOREACH_PORTINMASK(pn, portmask)
    {
        hit = 1;
        trk_pm = check_trunk_portmask(1UL<<pn);
        if ( trk_pm&igmpFastLeavePort ) /* if one port is fast leave, all trunk ports are fast leave */
        {
            inst->fwdPortMask[0] &= ~(trk_pm);    /* Max Supported Port Number == 32 */
            FOREACH_PORTINMASK(pn2, portmask)
            {
                inst->portTimer[pn2] = 0;
            }
        }
        else
        {
            if( (1UL<<pn)&&igmpFastLeavePort )
            {
                inst->fwdPortMask[0] &= ~(1UL<<pn);
                inst->portTimer[pn] = 0;
            }
            else
            {
                inst->portTimer[pn] = IGMP_LEAVE_TIME;
            }
        }

        /* only left router port in group->delete this group */
        if ( inst->fwdPortMask[0] == 0UL )
        {
            IGMP_MLD_CALLING( inst, \
                (igmp_group_del_group( &(inst->groupAddr), inst->vid, NULL )), \
                (mld_group_del_group( &(inst->groupAddr), inst->vid, NULL ))   \
            );
        }
        else
        {
		    groupAddr = ((uint32)inst->groupAddr.addr[0] << 24) | \
                        ((uint32)inst->groupAddr.addr[1] << 16) | \
                        ((uint32)inst->groupAddr.addr[2] << 8) | \
                        ((uint32)inst->groupAddr.addr[3]);
            grp_pm.bits[0] = inst->fwdPortMask[0];
            /* Get and Add router ports to hardward port mask */
            igmp_group_get_queryPort(&router_pm);
            grp_pm.bits[0] |= router_pm;

            IGMP_MLD_CALLING( inst, \
				(sal_igmp_ipAddrEntry_add(groupAddr, 0, inst->vid, &grp_pm)),    \
                (sal_igmp_ipv6AddrEntry_add(groupAddr, 0, inst->vid, &grp_pm))    \
            );
        }
    }
    return hit;
}

#else /* else of LWPS_IGMP_TRUNK_SUPPORT */
  #define IGMP_SET_DEFAULT_PORT(x, y)
  #define check_trunk_portmask(x)               (0)
  #define igmp_delete_groupMember_byTrunk(x, y) (0)
#endif /* end of else of LWPS_IGMP_TRUNK_SUPPORT */

#if   SAL_ENABLE
#else /* else of SAL_ENABLE */
// U32 Mask to Num: Converts a 32-bit port mask to a single port number
// Returns port number if mask has exactly one bit set, otherwise -1
int8 u32_mask_to_num(uint32 pm)
{
    int8 i;
    int8  pn;
    uint8 hit = FALSE;

    if( 0==pm )
    {
        return (-1);
    }
    
    for (i=0; i<SAL_PORT_ID_CPU; i++)
    {
        if( FALSE==hit )
        {
            if( pm&0x01 )
            {
                hit = TRUE;
                pn = i;
            }
        }
        else if( (TRUE==hit) && (pm&0x01) )
        {
            return (-1);
        }
        pm = pm>>1;
    }

    return pn;
}
#endif  /* end of else of SAL_ENABLE */

// IGMP Group DB Init: Initializes the IGMP group database and free pool
// Sets up router ports, fast leave ports, and allocates group entries from pool
int32 igmp_group_db_init(void)
{
    ipaddr_group_list_t *iter_group;
    uint16 i;
    
    /* IGMP Router Port Init */
    igmpRouter.router_portMask = 0;

    /* Fast Leave */
    igmpFastLeavePort = 0xFFFFFFFFUL;

    for(i=0; i<SAL_PORT_ID_CPU; i++)
    {
        igmpRouter.portTimer[i] = 0;
    }

    /* IGMP Group/Member Init */
    igmpGroupHead.listNumber = 0;
    igmpGroupHead.item = NULL;

    igmpGroupFreeHead.listNumber = MAX_GROUP_SIZE;
    
    iter_group = igmpGroupFreeHead.item = &igmpGroupPool[0];
    for (i=1; i<MAX_GROUP_SIZE; iter_group=iter_group->next, i++)
    {
        iter_group->next = &igmpGroupPool[i];
    }
    iter_group->next = NULL;
    
    return LWPS_OK;
}

// IGMP Static Router Set: Sets static router ports for IGMP snooping
// Updates the static router port mask for multicast forwarding
int32 igmp_staticRouter_set(sal_lPortmask_t mask)
{
    if(igmpRouter.routerStatic_portMask != mask.bits[0])
    {
        igmpRouter.routerStatic_portMask = mask.bits[0];
    }
    return LWPS_OK;
}

// IGMP Dynamic Router Get: Retrieves the dynamic router port mask
// Returns the current dynamic router ports discovered via IGMP queries
int32 igmp_dynamicRouter_get(sal_lPortmask_t *mask)
{
    mask->bits[0] = igmpRouter.router_portMask;
    return LWPS_OK;
}

// IGMP DB Get Group: Searches for a group entry in the database
// Finds group by IP address and VLAN ID, returns iterator and previous pointer
static void igmpdb_get_group(ipaddr_group_t *grp, ip_addr_t *grupAddr, uint16 vid, uint8 mode)
{
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;

    if(mode){}

    iter_last = igmpGroupHead.item;
    for (iter=igmpGroupHead.item; iter;iter_last=iter, iter=iter->next)
    {
#if   CONFIG_IGMP_MLD_ENABLE    
        if ( IGMP_CHECK_GROUP(grupAddr->addr, iter->groupAddr.addr, iter->ipvn, mode) && (iter->vid == vid))
        {
            break;
        }
#else
        if ( IGMP_CHECK_GROUP(grupAddr->addr, iter->groupAddr.addr, IGMP_GRP_DB_IPV4, mode) && (iter->vid == vid))
        {
            break;
        }
#endif        
    }

    grp->iter = iter;
    grp->iter_last = iter_last;
}

// IGMP DB Get Empty Entry: Allocates a free group entry from the pool
// Returns pointer to new entry or NULL if pool is exhausted
static ipaddr_group_list_t * igmpdb_get_empty_entry(ipaddr_group_list_t *iter_last)
{
    ipaddr_group_list_t *iter = (ipaddr_group_list_t *)NULL;

    if ( igmpGroupFreeHead.listNumber>0 )
    {
        /* add available group number*/
        igmpGroupHead.listNumber++;
        igmpGroupFreeHead.listNumber--;
        DBG_IGMPDB_PRINTF(("[%s][%d][NewGroup, free=%d]\n", __FILE__, \
                          (uint16)__LINE__ , 
                          (uint16)igmpGroupFreeHead.listNumber));

        /* get a free group slot */
        iter = igmpGroupFreeHead.item;
        igmpGroupFreeHead.item = igmpGroupFreeHead.item->next;

        if( !iter_last )
        {
            igmpGroupHead.item = iter;        
        }
        else
        {
            iter_last->next = iter;
        }
        
        iter->next = (ipaddr_group_list_t *)NULL;

        iter->fwdPortMask[0] = 0UL;
    }
    else
    {
        return (ipaddr_group_list_t *)NULL;
    }
    return iter;
}

// IGMP DB Timer: Periodic timer handler for aging out expired group members
// Checks router and member port timers, removes expired entries, updates hardware
void igmpdb_timer(u8_t pass_seconds)
{
    static uint8 pass_time = 0;
    uint16 i, j;
    uint8  time;
    uint16 group_size = igmpGroupHead.listNumber;
    ipaddr_group_list_t *iter_group = igmpGroupHead.item;
    ipaddr_group_list_t *iter_group_rm;
    portmask_type  pm_org;
    sal_ipaddr_t    groupAddr;
    sal_lPortmask_t grp_pm;
    sal_enable_t enable;
    portmask_type router_pm;


    sal_igmp_state_get(&enable);
    if(enable == DISABLE)
    {
        pass_time = 0;
        return;
    }

    pass_time++;
    if( pass_time < IGMPDB_CHECK_PERIOD )
    {
        return;
    }
    pass_time = 0;

    /* Check router timer */
    for (i=0; i<SAL_PORT_ID_CPU; i++)
    {
        if( 0!=(time=igmpRouter.portTimer[i]) )
        {
            if ( time>pass_seconds )
            {
                igmpRouter.portTimer[i] -= pass_seconds;
            }
            else
            {
                igmp_group_del_queryPort(i);
                DBG_IGMPDB_PRINTF(("[%s][%d][Router Expired:%d]\n", __FILE__, \
                    (uint16)__LINE__, i));
            }
        }
    }

    /* Check memberPort timer */
    for(i=0; i<group_size; i++)
    {
        pm_org = iter_group->fwdPortMask[0];
        for (j=0; j<SAL_PORT_ID_CPU; j++)
        {
            if ( 0!=(time=iter_group->portTimer[j]) )
            {
                if ( time>((u16_t)pass_seconds) )
                {
                    iter_group->portTimer[j] -= pass_seconds;
                }
                else
                {
                    iter_group->portTimer[j] = 0; 
                    iter_group->fwdPortMask[0] &= ~(1UL<<j);
                }
            }
        }

        iter_group_rm = iter_group;
        iter_group = iter_group->next;
        
        /* only leave when there is no any members */
        if( iter_group_rm->fwdPortMask[0] == 0UL )
        {
            IGMP_MLD_CALLING( iter_group_rm, \
                (igmp_group_del_group( &(iter_group_rm->groupAddr), iter_group_rm->vid, NULL)), \
                (mld_group_del_group( &(iter_group_rm->groupAddr), iter_group_rm->vid, NULL))   \
            );
        }
        else if ( iter_group_rm->fwdPortMask[0]!=pm_org )
        {
		    groupAddr = ((uint32)iter_group_rm->groupAddr.addr[0] << 24) | \
                        ((uint32)iter_group_rm->groupAddr.addr[1] << 16) | \
                        ((uint32)iter_group_rm->groupAddr.addr[2] << 8) | \
                        ((uint32)iter_group_rm->groupAddr.addr[3]);
            grp_pm.bits[0] = iter_group_rm->fwdPortMask[0];
            /* Get and Add router ports to hardward port mask */
            igmp_group_get_queryPort(&router_pm);
            grp_pm.bits[0] |= router_pm;

            IGMP_MLD_CALLING( iter_group_rm, \
				(sal_igmp_ipAddrEntry_add(groupAddr, 0, iter_group_rm->vid, &grp_pm)),    \
                (sal_igmp_ipv6AddrEntry_add(groupAddr, 0, iter_group_rm->vid, &grp_pm))    \
            );
        }
    }
}

/* - add ports into routerPort 
   - source address 0.0.0.0 is not the routerPort
   - IGMP core program doesn't call this function when
     source address is 0.0.0.0
 */
// IGMP Group Add Query Port: Adds a port to the router port list
// Sets router timer and updates router port mask for IGMP snooping
int32 igmp_group_add_queryPort(portmask_type portmask)
{
    int8 pn = u32_mask_to_num(portmask);

    if( pn<0 )
    {
        return LWPS_ERR;
    }

    igmpRouter.portTimer[pn]    = V1_ROUTER_PRESENT_TIME;
    igmpRouter.router_portMask |= portmask;
    
    return LWPS_OK;
}

// IGMP Group Del Query Port: Removes a port from the router port list
// Clears router timer and updates router port mask
int32 igmp_group_del_queryPort(int32 pn)
{

    if( pn>=0 )
    {
        igmpRouter.portTimer[pn] = 0;
    }
    else
    {
        return LWPS_ERR;
    }
    
    igmpRouter.router_portMask      &= (~(1UL<<pn));
    
    return LWPS_OK;
}

// IGMP Group Get Query Port: Retrieves the current router port mask
// Combines dynamic and static router ports into output mask
int32 igmp_group_get_queryPort(portmask_type *portmask)
{
    *portmask = igmpRouter.router_portMask;
    *portmask |= igmpRouter.routerStatic_portMask;
    
    return LWPS_OK;
}

// IGMP Group Link Status Change: Handles port link state changes for multicast groups
// Updates group memberships when ports go up/down, removes expired entries
int32 igmp_group_linkStatus_change(portmask_type curr_pm, portmask_type prev_pm)
{
    ipaddr_group_list_t *iter, *iter_next;
    portmask_type xdata port_index, xdata tmp_pm;
    uint16 xdata listNumber;
    uint8 xdata i, xdata j;
    portmask_type changed_if = (curr_pm ^ prev_pm);
    portmask_type link_down_if = (changed_if & prev_pm);
    portmask_type link_up_if = (changed_if & curr_pm);
	portmask_type router_pm;
    #if LWPS_IGMP_TRUNK_SUPPORT
    uint8 xdata k;
    portmask_type trk_pm;
	sal_ipaddr_t    groupAddr;
	sal_lPortmask_t grp_pm;
    sal_enable_t    linkup;

    for (iter=igmpGroupHead.item, j=0; j<igmpGroupHead.listNumber; j++)
    {
        FOREACH_PORTINMASK(i, link_up_if)
        {
            trk_pm = check_trunk_portmask(1UL<<i);
            FOREACH_PORTINMASK(k, trk_pm)
            {
                /* Remove un-linkup ports in trunking group */
                sal_port_link_get(k, &linkup);
                if(SAL_ENABLED != linkup)
                {
                    trk_pm &= ~(1UL<<k);
                }
            }

            if((0 != trk_pm) && (0 != (iter->fwdPortMask[0] & trk_pm)))
            {

                /* Group with new member has to update DB and set to HW
                 * If one of the trunk port join, join all trunk ports
                 */
                iter->fwdPortMask[0] |= (1UL << i);

                /* All the data in the group database should be updated
                 * Get the data from other port in the same trunking group
                 */
                FOREACH_PORTINMASK(k, (iter->fwdPortMask[0] & trk_pm))
                {
                    if(k == i)
                    {
                        continue;
                    }
                    iter->portTimer[i] = iter->portTimer[k];
                    /* Any port in the same group is enough, so break it */
                    break;
                }

                /* Force update HW record */
                groupAddr = ((uint32)iter->groupAddr.addr[0] << 24) |
                           ((uint32)iter->groupAddr.addr[1] << 16) |
                           ((uint32)iter->groupAddr.addr[2] <<  8) |               
                           ((uint32)iter->groupAddr.addr[3]);
                grp_pm.bits[0] = iter->fwdPortMask[0];

                /* Get and Add router ports to hardware port mask */
                igmp_group_get_queryPort(&router_pm);
                grp_pm.bits[0] |= router_pm;
                sal_igmp_ipAddrEntry_add(groupAddr, 0, iter->vid, &grp_pm);
            }
        }
    }
#endif /* end of LWPS_IGMP_TRUNK_SUPPORT */
    for(i=0, port_index=1;i<SAL_PORT_ID_CPU;i++, port_index <<=1 )
    {
        if( port_index&link_down_if )
        {
            igmp_group_del_queryPort(i);
        }
    }

    listNumber = igmpGroupHead.listNumber;
    for (iter=igmpGroupHead.item, j=0; j<listNumber; j++)
    {
        /* delete iter group will change iter->next value */
        iter_next = iter->next;

        tmp_pm = iter->fwdPortMask[0];
        iter->fwdPortMask[0] &= (~link_down_if);
        if( iter->fwdPortMask[0]==tmp_pm )
        {
            continue;
        }
        else
        {
            tmp_pm = iter->fwdPortMask[0]^tmp_pm;
        }

        /* remove timers & queryPort */
        /* only left router port in group->delete this group */
        if( iter->fwdPortMask[0] == 0UL )
        {
            IGMP_MLD_CALLING( iter, \
                (igmp_group_del_group( &(iter->groupAddr), iter->vid, NULL)), \
                (mld_group_del_group( &(iter->groupAddr), iter->vid, NULL))   \
            );
            
            for(i=0; i<SAL_PORT_ID_CPU; i++)
            {
                iter->portTimer[i] = 0;
            }
        }
        else
        {
            for(i=0, port_index=1;i<SAL_PORT_ID_CPU;i++, port_index <<=1 )
            {
                if( port_index&tmp_pm )
                {
                    iter->portTimer[i] = 0;
                    IGMP_MLD_CALLING( iter, \
                        (igmp_group_del_groupMember(&(iter->groupAddr), 1UL<<i, iter->vid, IGMP_ADDR_GROUP_ADDR)), \
                        (mld_group_del_groupMember(&(iter->groupAddr), 1UL<<i, iter->vid, IGMP_ADDR_GROUP_ADDR))   \
                    );
                }
            }
        }

        iter = iter_next;
    }
    
    return LWPS_OK;
}

/* add group member functions */
// IGMP Group Add Group Member: Adds a port to a multicast group membership
// Creates new group entry if needed, updates forwarding database and hardware
int32 igmp_group_add_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode)
{
    int8 xdata pn;
    ip_addr_t  xdata *grupAddr;
    pktBuf_t   xdata *pBuf;
    igmp_hdr_t xdata *pIgmpHdr;
    ipaddr_group_t      xdata  grp;
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
#if !LWPS_IGMP_TRUNK_SUPPORT
    sal_ipaddr_t        xdata  grp_addr;
    sal_lPortmask_t     xdata  grm;
    portmask_type router_pm;
#endif
    pn = u32_mask_to_num(portmask);


    if( pn<0 )
    {
        return LWPS_ERR;
    }

    if( IGMP_ADDR_PBUF==mode )
    {
        pBuf     = (pktBuf_t *) d;
        pIgmpHdr = (igmp_hdr_t *)pBuf->l4;
        grupAddr = &(pIgmpHdr->groupAddr);
    }
    else
    {
        grupAddr = (ip_addr_t *) d;
    }

    /* search procedure in add/leave/del functions
       can be merged in single function */
    igmpdb_get_group(&grp, grupAddr, vid, IGMP_GRP_DB_IPV4);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        iter = igmpdb_get_empty_entry(iter_last);
        if( !iter )
        {
            return LWPS_ERR;
        }
        IPADDR_COPY( iter->groupAddr.addr, grupAddr->addr);
#if CONFIG_IGMP_MLD_ENABLE
        iter->ipvn = IGMP_GRP_DB_IPV4;
#endif /* end CONFIG_IGMP_MLD_ENABLE */
        iter->vid = vid;
    }

#if LWPS_IGMP_TRUNK_SUPPORT
    igmp_add_groupMember_byTrunk(iter, portmask); //add trunking support
#else
    iter->fwdPortMask[0] |= portmask;
    iter->portTimer[pn] = IGMP_GROUP_MEM_INTERVAL;

    grp_addr = ((uint32)grupAddr->addr[0] << 24) |
               ((uint32)grupAddr->addr[1] << 16) |
               ((uint32)grupAddr->addr[2] <<  8) |               
               ((uint32)grupAddr->addr[3]);
    grm.bits[0] = iter->fwdPortMask[0];
    /* Get and Add router ports to hardware port mask */
    igmp_group_get_queryPort(&router_pm);
    grm.bits[0] |= router_pm;

    sal_igmp_ipAddrEntry_add(grp_addr, 0, vid, &grm);
#endif    
    return LWPS_OK;
}

/* add group member functions */
// IGMP Group Del Group Member: Removes a port from a multicast group membership
// Handles fast leave, timer expiration, and updates hardware forwarding
int32 igmp_group_del_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode)
{
    int8  xdata pn;
    igmp_hdr_t xdata *pIgmpHdr;
    ip_addr_t  xdata *grupAddr;
    pktBuf_t   xdata *pBuf;
    ipaddr_group_t      xdata  grp;
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
#if LWPS_IGMP_TRUNK_SUPPORT
    sal_ipaddr_t        xdata  grp_addr;
    sal_lPortmask_t     xdata  grm;
    portmask_type router_pm;
#endif

    pn = u32_mask_to_num(portmask);
    
    if( pn<0 )
    {
        return LWPS_ERR;
    }

    if( IGMP_ADDR_PBUF==mode )
    {
        pBuf     = (pktBuf_t *) d;
        pIgmpHdr = (igmp_hdr_t *)pBuf->l4;
        grupAddr = &(pIgmpHdr->groupAddr);
    }
    else
    {
        grupAddr = (ip_addr_t *) d;
    }

    igmpdb_get_group(&grp, grupAddr, vid, IGMP_GRP_DB_IPV4);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        DBG_IGMPDB_PRINTF(("[%s][%d][Can't find leave member]\n", __FILE__, (uint16)__LINE__));
        return LWPS_OK;
    }

#if !LWPS_IGMP_TRUNK_SUPPORT

    igmp_delete_groupMember_byTrunk(iter, portmask);

#else  /* else of LWPS_IGMP_TRUNK_SUPPORT */

    if ( portmask&igmpFastLeavePort )
    {
        iter->portTimer[pn] = 0;
        iter->fwdPortMask[0] &= ~(1UL<<pn);

        /* only left router port in group->delete this group */
        if( iter->fwdPortMask[0] == 0UL )
        {
            igmp_group_del_group( grupAddr, vid, NULL );
        }
        else
        {
            grp_addr = ((uint32)grupAddr->addr[0] << 24) |
                       ((uint32)grupAddr->addr[1] << 16) |
                       ((uint32)grupAddr->addr[2] <<  8) |               
                       ((uint32)grupAddr->addr[3]);
            grm.bits[0] = iter->fwdPortMask[0];
            /* Get and Add router ports to hardware port mask */
            igmp_group_get_queryPort(&router_pm);
            grm.bits[0] |= router_pm;
        
            sal_igmp_ipAddrEntry_add(grp_addr, 0, vid, &grm);
        }
    }
    else
    {
        iter->portTimer[pn] = IGMP_LEAVE_TIME;
    }
    
#endif /* end of else of LWPS_IGMP_TRUNK_SUPPORT */
    
    return LWPS_OK;
}

/* group member functions */
// IGMP Group Del Group: Deletes an entire multicast group from the database
// Removes group entry from list and updates hardware forwarding entries
int32 igmp_group_del_group(ip_addr_t *grupAddr, uint16 vid, ip_addr_t *sip)
{
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
    ipaddr_group_t xdata grp;
    sal_ipaddr_t    salGroupAddr;

    igmpdb_get_group(&grp, grupAddr, vid, IGMP_GRP_DB_IPV4);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        return LWPS_OK;
    }
    else
    {
        if( iter_last==iter )
        {
            igmpGroupHead.item = igmpGroupHead.item->next;
        }
        iter_last->next = iter->next;
        iter->next = igmpGroupFreeHead.item;
        igmpGroupFreeHead.item = iter;
    }

    igmpGroupHead.listNumber--;
    igmpGroupFreeHead.listNumber++;

    DBG_IGMPDB_PRINTF(("[%s][%d][Work Group:%d]\n", __FILE__, (uint16)__LINE__, \
            (uint16)igmpGroupHead.listNumber ));

    if ( sip ){}

    salGroupAddr = ((uint32)grupAddr->addr[0] << 24) | \
                   ((uint32)grupAddr->addr[1] << 16) | \
                   ((uint32)grupAddr->addr[2] << 8) | \
                   ((uint32)grupAddr->addr[3]);

    sal_igmp_ipAddrEntry_del(salGroupAddr, 0, vid);
    
    return LWPS_OK;
}

#if !defined(CONFIG_IGMP_PURE_DUMB)
// IGMP Get Group List: Returns pointer to the IGMP group list head
// Used for iterating through all multicast groups in the database
ipaddr_group_head_t* igmp_get_groupList(void)
{
    return &igmpGroupHead;
}
#endif /* end of !defined(CONFIG_IGMP_PURE_DUMB) */

#if CONFIG_IGMP_MLD_ENABLE
/* add group member functions */
// MLD Group Add Group Member: Adds a port to IPv6 multicast group membership
// Handles MLD (IPv6) group memberships similar to IGMPv4
int32 mld_group_add_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode)
{
    int8 xdata pn;
    ip_addr_t   xdata grupAddr;
    ipv6_addr_t xdata *tmp;
    pktBuf_t    xdata *pBuf;
    ipaddr_group_t      xdata  grp;
    igmp_mld_hdr_t      xdata *pMldHdr;
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
    sal_ipaddr_t    salGroupAddr;
	sal_lPortmask_t grp_pm;
    portmask_type router_pm;

    pn = u32_mask_to_num(portmask);

    if( pn<0 )
    {
        return LWPS_ERR;
    }

    if( IGMP_ADDR_PBUF==mode )
    {
        pBuf     = (pktBuf_t *) d;
        pMldHdr = (igmp_mld_hdr_t *)pBuf->l4;
        IGMP_MEMCPY(grupAddr.addr, &(pMldHdr->groupAddr.addr[IPV6_ADDR_LEN-4]), 4);
    }
    else
    {
        tmp = (ipv6_addr_t *) d;
        IGMP_MEMCPY(grupAddr.addr, &(tmp->addr[IPV6_ADDR_LEN-4]), 4);
    }

    igmpdb_get_group(&grp, &grupAddr, vid, IGMP_GRP_DB_IPV6);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        iter = igmpdb_get_empty_entry(iter_last);
        if( !iter )
        {
            return LWPS_ERR;
        }
        IPADDR_COPY( iter->groupAddr.addr, grupAddr.addr);
        iter->ipvn = IGMP_GRP_DB_IPV6;
        iter->vid = vid;
    }

    iter->fwdPortMask[0] |= portmask;
    iter->portTimer[pn] = IGMP_GROUP_MEM_INTERVAL;

    salGroupAddr = ((uint32)grupAddr.addr[0] << 24) | \
                   ((uint32)grupAddr.addr[1] << 16) | \
                   ((uint32)grupAddr.addr[2] << 8) | \
                   ((uint32)grupAddr.addr[3]);
    grp_pm.bits[0] = iter->fwdPortMask[0];
    /* Get and Add router ports to hardware port mask */
    igmp_group_get_queryPort(&router_pm);
    grp_pm.bits[0] |= router_pm;

    sal_igmp_ipv6AddrEntry_add(salGroupAddr, 0, vid, &grp_pm);
    
    return LWPS_OK;
}

/* add group member functions */
// MLD Group Del Group Member: Removes a port from IPv6 multicast group membership
// Handles MLD group deletion with fast leave and timer management
int32 mld_group_del_groupMember(void *d, portmask_type portmask, uint16 vid, uint8 mode)
{
    int8 xdata pn;
    ip_addr_t   xdata *v4_tmp;
    ipv6_addr_t xdata *tmp;
    ip_addr_t    xdata grupAddr;
    igmp_mld_hdr_t     xdata *pMldHdr;
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
    pktBuf_t xdata *pBuf;
    ipaddr_group_t xdata grp;
#if !LWPS_IGMP_TRUNK_SUPPORT
    sal_ipaddr_t    salGroupAddr;
    sal_lPortmask_t grp_pm;
    portmask_type router_pm;
#endif
    
    pn = u32_mask_to_num(portmask);

    if( pn<0 )
    {
        return LWPS_ERR;
    }

    if( IGMP_ADDR_PBUF==mode )
    {
        pBuf     = (pktBuf_t *) d;
        pMldHdr  = (igmp_mld_hdr_t *)pBuf->l4;
        IGMP_MEMCPY(grupAddr.addr, &(pMldHdr->groupAddr.addr[IPV6_ADDR_LEN-4]), 4);
    }
    else if( IGMP_ADDR_GROUP_ADDR==mode )
    {
        v4_tmp = (ip_addr_t *) d;
        IGMP_MEMCPY(grupAddr.addr, v4_tmp->addr, 4);
    }
    else /* IGMP_ADDR_GROUP_ADDRV6 */
    {
        tmp = (ipv6_addr_t *) d;
        IGMP_MEMCPY(grupAddr.addr, &(tmp->addr[IPV6_ADDR_LEN-4]), 4);
    }

    igmpdb_get_group(&grp, &grupAddr, vid, IGMP_GRP_DB_IPV6);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        DBG_IGMPDB_PRINTF(("[%s][%d][Can't find leave member]\n", __FILE__, (uint16)__LINE__));
        return LWPS_OK;
    }

#if LWPS_IGMP_TRUNK_SUPPORT

    igmp_delete_groupMember_byTrunk(iter, portmask);

#else  /* else of LWPS_IGMP_TRUNK_SUPPORT */

    if ( portmask&igmpFastLeavePort )
    {
        iter->portTimer[pn] = 0;
        iter->fwdPortMask[0] &= ~(1UL<<pn);

        /* only left router port in group->delete this group */
        if( (iter->fwdPortMask[0]) == 0UL )
        {
            mld_group_del_group( &grupAddr, vid, NULL );
        }
        else
        {
            salGroupAddr = ((uint32)grupAddr.addr[0] << 24) | \
                           ((uint32)grupAddr.addr[1] << 16) | \
                           ((uint32)grupAddr.addr[2] << 8) | \
                           ((uint32)grupAddr.addr[3]);
            grp_pm.bits[0] = iter->fwdPortMask[0];
            /* Get and Add router ports to hardware port mask */
            igmp_group_get_queryPort(&router_pm);
            grp_pm.bits[0] |= router_pm;
            
            sal_igmp_ipv6AddrEntry_add(salGroupAddr, 0, vid, &grp_pm);

        }
    }
    else
    {
        iter->portTimer[pn] = IGMP_LEAVE_TIME;
    }
    
#endif /* end of else of LWPS_IGMP_TRUNK_SUPPORT */
    
    return LWPS_OK;
}

/* group member functions */
// MLD Group Del Group: Deletes an entire IPv6 multicast group from the database
// Removes MLD group entry and updates hardware IPv6 forwarding entries
int32 mld_group_del_group(ip_addr_t *grupAddr, uint16 vid, ip_addr_t *sip)
{
    //uint16 size = igmpGroupHead.listNumber;
    ipaddr_group_list_t *iter;
    ipaddr_group_list_t *iter_last;
    ipaddr_group_t xdata grp;
    sal_ipaddr_t    salGroupAddr;

    igmpdb_get_group(&grp, grupAddr, vid, IGMP_GRP_DB_IPV6);
    iter = grp.iter;
    iter_last = grp.iter_last;

    if ( !iter )
    {
        return LWPS_OK;
    }
    else
    {
        if( iter_last==iter )
        {
            igmpGroupHead.item = igmpGroupHead.item->next;
        }
        iter_last->next = iter->next;
        iter->next = igmpGroupFreeHead.item;
        igmpGroupFreeHead.item = iter;
    }

    igmpGroupHead.listNumber--;
    igmpGroupFreeHead.listNumber++;

    DBG_IGMPDB_PRINTF(("[%s][%d][Work Group:%d]\n", __FILE__, (uint16)__LINE__, \
            (uint16)igmpGroupHead.listNumber ));

    if ( sip ){}

    salGroupAddr = ((uint32)grupAddr->addr[0] << 24) | \
                   ((uint32)grupAddr->addr[1] << 16) | \
                   ((uint32)grupAddr->addr[2] << 8) | \
                   ((uint32)grupAddr->addr[3]);
    
    sal_igmp_ipv6AddrEntry_del(salGroupAddr, 0, vid);
   
    return LWPS_OK;
}
#endif /* end of CONFIG_IGMP_MLD_ENABLE */


#endif /* end of DBG_LWPS_IGMP_ENABLE */