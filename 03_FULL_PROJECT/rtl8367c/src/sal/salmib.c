#pragma STRING (far)

#include <saltrunk.h>
#include <salportmask.h>
#include <salmib.h>
#include <salerror.h>
#include <stdio.h>
#include <salrate.h>
#include <stat.h>
#include <rtk_error.h>

#if 0//NOT used
/* Function Name:
 *      sal_mib_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_mib_global_reset(void)
{
    return rtk_stat_global_reset();
}
#endif

int32 sal_mib_port_reset(sal_port_t port)
{
    int32 ret;

    if ((port >= SAL_MAX_PHY_N_CPU_PORT)) {
		return SAL_ERR_PORT_RANGE;
	}
	
    if ((ret = rtk_stat_port_reset(port)) != RT_ERR_OK)
        return ret;
    
    return SAL_ERR_SUCCESS;
} 

#if 1
/* Function Name:
 *      sal_mib_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port        - port id
 *      cntr_idx    - specified port counter index
 * Output:
 *      pCntH       - MSB of the counter value
 *      pCntL       - LSB of the counter value
 * Return:
 *      SAL_ERR_NOT_SUPPORT
 *      SAL_ERR_SUCCESS
 * Note:
 *      The value of *pCntH means only for 64-bit MIBs. for 32-bit MIBs, 
 *      the value of *pCntH should be 0.
 *
 *      The value of MIB counters can be retrieved by arithmatic expression 
 *      (((uint64)*pCntH << 32) | *pCntL) if 64-bit data type is supported.       
 */
int32 sal_mib_port_get(sal_port_t port, sal_stat_port_type_t cntrIdx, uint32 *pCntH, uint32 *pCntL)
{
    rtk_api_ret_t ret;
    uint32        tmpCntr1, tmpCntr2;

    if(cntrIdx >= SAL_MIB_PORT_CNTR_END)
        return SAL_ERR_PARAMETER;

    switch(cntrIdx)
    {
    case SAL_IF_OUT_OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfOutOctets, pCntH, pCntL);
        break;
    case SAL_IF_IN_OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfInOctets, pCntH, pCntL);
        break;
    case SAL_DOT3_OUT_PAUSE_FRAMES_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3OutPauseFrames, pCntH, pCntL);
        break;
    case SAL_DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsExcessiveCollisions, pCntH, pCntL);
        break;
    case SAL_DOT3_STATS_LATE_COLLISIONS_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsLateCollisions, pCntH, pCntL);        
        break;
    case SAL_DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsDeferredTransmissions, pCntH, pCntL);        
        break;
    case SAL_DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsMultipleCollisionFrames, pCntH, pCntL);        
        break;
    case SAL_DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsSingleCollisionFrames, pCntH, pCntL);        
        break;
    case SAL_DOT3_IN_PAUSE_FRAMES_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3InPauseFrames, pCntH, pCntL);
        break;
    case SAL_DOT3_STATS_SYMBOL_ERRORS_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsSymbolErrors, pCntH, pCntL);        
        break;
    case SAL_DOT3_STATS_FCS_ERRORS_INDEX:
        ret = rtk_stat_port_get(port, STAT_Dot3StatsFCSErrors, pCntH, pCntL);        
        break;
    case SAL_ETHER_STATS_JABBERS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsJabbers, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_COLLISIONS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsCollisions, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_MCAST_PKTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsMulticastPkts, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_BCAST_PKTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsBroadcastPkts, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_FRAGMENTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsFragments, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_64OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts64Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_65TO127OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts65to127Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_128TO255OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts128to255Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_256TO511OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts256to511Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_512TO1023OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts512to1023Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsPkts1024to1518Octets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_PKTS_INDEX:
        *pCntL = 0;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts64Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts65to127Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts128to255Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts256to511Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts512to1023Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        if(SUCCESS == rtk_stat_port_get(port, STAT_EtherStatsPkts1024to1518Octets, &tmpCntr1, &tmpCntr2))
            *pCntL += tmpCntr2;
        *pCntH = 0;
        break;
    case SAL_ETHER_STATS_OVERSIZE_PKTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherOversizeStats, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_OCTETS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsOctets, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_UNDER_SIZE_PKTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsUnderSizePkts, pCntH, pCntL);
        break;
    case SAL_OUT_UCAST_PKTS_CNT_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfOutUcastPkts, pCntH, pCntL);
        break;
    case SAL_OUT_MCAST_PKTS_CNT_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfOutMulticastPkts, pCntH, pCntL);
        break;
    case SAL_OUT_BCAST_PKTS_CNT_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfOutBroadcastPkts, pCntH, pCntL);
        break;
    case SAL_IN_UCAST_PKTS_CNT_INDEX:
        ret = rtk_stat_port_get(port, STAT_IfInUcastPkts, pCntH, pCntL);
        break;
    case SAL_ETHER_STATS_DROP_EVENTS_INDEX:
        ret = rtk_stat_port_get(port, STAT_EtherStatsDropEvents, pCntH, pCntL);
        break;
    default:
        return SAL_ERR_NOT_SUPPORT;
    }
    return SAL_ERR_SUCCESS;
}
#endif

void _counter_add(uint32 *pOrigH, uint32 *pOrigL, uint32 marginH, uint32 marginL)
{        
    if (*pOrigL > (uint32)0xFFFFFFFF - marginL) // Overflow 
        *pOrigH += 1;
        
    *pOrigH += marginH;
    *pOrigL += marginL;
}

int32 _counter_sub(uint32 *pOrigH, uint32 *pOrigL, uint32 marginH, uint32 marginL)
{
    if (*pOrigH < marginH || (*pOrigH == marginH && *pOrigL < marginL)) 
    {
        printf("Error in %s, %d\n", __FILE__, (uint16)__LINE__);
        return SAL_ERR_FAILED;
    }

    *pOrigH -= marginH;
    if (*pOrigL > marginL)
        *pOrigL  -= marginL;
    else
    {
        *pOrigH -= 1;
        *pOrigL = *pOrigL + ((uint32)0xffffffff - marginL + 1);
    }

    return SAL_ERR_SUCCESS;
}

int32 sal_mib_rxGoodPkt_get(sal_port_t port, uint32* high, uint32 *low)
{
	sal_port_t      portId;
	sal_pPortmask_t	portMask;

    rtk_stat_counter_t cnth,cnth2,cnth3;
    rtk_stat_counter_t cntl,cntl2,cntl3; 

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PORT_RANGE;
	}
    
    *high = 0, *low  = 0;

	sal_trunk_logicId_toPhysicMask(&portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {
            cnth = cntl = 0;
        	rtk_stat_port_get(portId, STAT_IfInUcastPkts, &cnth, &cntl);
            _counter_add(high, low, cnth, cntl);

            cnth3 = cntl3 = 0;
            rtk_stat_port_get(portId, STAT_IfOutMulticastPkts,  &cnth3, &cntl3);

            cnth = cntl = 0;
        	rtk_stat_port_get(portId, STAT_EtherStatsMulticastPkts, &cnth, &cntl);
            _counter_add(high, low, cnth, cntl);
            
			/*avoid mib counter too big to subtract*/
            cnth2 = cntl2 = 0;
            rtk_stat_port_get(portId, STAT_IfOutBroadcastPkts,  &cnth2, &cntl2);

            cnth = cntl = 0;
        	rtk_stat_port_get(portId, STAT_EtherStatsBroadcastPkts, &cnth, &cntl);
            _counter_add(high, low, cnth, cntl);
            
			/*avoid mib counter too big to subtract*/
            _counter_sub(high, low, cnth2, cntl2);  
            _counter_sub(high, low, cnth3, cntl3);  
        }
    }    

	return SAL_ERR_SUCCESS;
}

int32 sal_mib_rxBadPkt_get(sal_port_t port, uint32* high, uint32 *low)
{
	sal_port_t      portId;
	sal_pPortmask_t	portMask;
	uint32 frameCode;

    rtk_stat_counter_t cnth;
    rtk_stat_counter_t cntl; 

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PORT_RANGE;
	}
    
    *high = 0, *low  = 0;

    sal_rate_maxFrameSize_get(&frameCode);
    
	sal_trunk_logicId_toPhysicMask(&portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {
            if (SAL_JUMBO_FRAME_16k != frameCode)
            {
                cnth = cntl = 0;
                rtk_stat_port_get(portId, STAT_EtherOversizeStats, &cnth, &cntl);
        	    _counter_add(high, low, cnth, cntl);
        	}
            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_EtherStatsUnderSizePkts, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_Dot3StatsFCSErrors, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_EtherStatsJabbers, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
            
            cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_EtherStatsFragments, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
        }
    }    

	return SAL_ERR_SUCCESS;
}

int32 sal_mib_txGoodPkt_get(sal_port_t port, uint32* high, uint32 *low)
{
	sal_port_t      portId;
	sal_pPortmask_t	portMask;

    rtk_stat_counter_t cnth;
    rtk_stat_counter_t cntl; 

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PORT_RANGE;
	}
    
    *high = 0, *low  = 0;

	sal_trunk_logicId_toPhysicMask(&portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {
            cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_IfOutUcastPkts, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_IfOutMulticastPkts, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_IfOutBroadcastPkts, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
        }
    }    

	return SAL_ERR_SUCCESS;
}

int32 sal_mib_txBadPkt_get(sal_port_t port, uint32* high, uint32 *low)
{
	sal_port_t      portId;
	sal_pPortmask_t	portMask;

    rtk_stat_counter_t cnth;
    rtk_stat_counter_t cntl; 

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PORT_RANGE;
	}

    //printf("Please find TxCrcCheckFailCnt Mib API!\n");
    *high = 0, *low  = 0;

	sal_trunk_logicId_toPhysicMask(&portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {            
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_Dot3StatsExcessiveCollisions, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);

            /*
        	cnth = cntl = 0;
            rtk_stat_port_get(portId, STAT_TXCRCCheckFailCnt, &cnth, &cntl);
        	_counter_add(high, low, cnth, cntl);
        	*/
        }
    }    

	return SAL_ERR_SUCCESS;
}

