#pragma STRING (far)

#include <salmirror.h>
#include <salflash.h>
#include <salportmask.h>
#include <saltrunk.h>
#include <salport.h>
#include <string.h>
#include <stdio.h>
#include <trunk.h>
#include <rtl8367c_asicdrv_trunking.h>
#include <l2.h>
#include <port.h>

extern uint16 getReg(uint16);
extern uint16 phyAllPortMask;

// Sal Trunk Logic Port Overlap: Checks if two logical port masks have any overlapping ports
// Returns TRUE if any port is present in both masks, FALSE otherwise
bool
_sal_trunk_logicPort_overlap(
	sal_pPortmask_t *	pstPortMask,
	sal_pPortmask_t *	pstPortMask1
	)
{
	int	i;

	for (i = 0; i < SAL_PORT_ID_CPU; ++i) {
		if ((SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask, i)) &&  \
		    (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask1, i))) {
			return TRUE;
		}
	}

	return FALSE;
}

// Sal Trunk Physic Mask Conflict: Checks if two physical port masks have inconsistent overlap pattern
// Returns TRUE if port membership pattern differs across the mask
bool
_sal_trunk_physicMask_conflict (
	sal_pPortmask_t *	pstPortMask,
	sal_pPortmask_t *	pstPortMask1
	)
{
	int	i;
	bool bOverlapped = FALSE, bOldOverlapped = FALSE, bFirst = TRUE;

	for (i = 0; i < SAL_MAX_PHY_PORT; ++i) {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask, i))
		{
			if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask1, i))
				bOverlapped = TRUE;
			else
				bOverlapped = FALSE;

			if (bFirst == FALSE) {
				if (bOldOverlapped != bOverlapped)
					return TRUE;
				else
					bOldOverlapped = bOverlapped;
			}
			else {
				bOldOverlapped = bOverlapped;
				bFirst = FALSE;
			}
		}
	}

	return FALSE;
}

// Sal Trunk Port Used By Trunk: Checks if any port in a mask is already used by another trunk
// Returns TRUE if port conflict exists with existing trunk groups
static bool
_sal_trunk_portUsedByTrunk (
	uint8		ucTrkId,
	sal_pPortmask_t *	pstPortMask
	)
{
	uint8		ucId;
	uint8		bLacp;
	sal_pPortmask_t	portMask;
	sal_pPortmask_t	actMask;

	for (ucId = 0; ucId < SAL_MAX_STATIC_TRUNK; ucId++) {
		if (ucId == ucTrkId) {
			continue; /* skip the current trunk ID */
		}

		sal_flash_trunk_port_get(ucId, &bLacp, &portMask, &actMask);
		if (SAL_ERR_SUCCESS != sal_pPortMask_isZero(&portMask)) {
			if (_sal_trunk_logicPort_overlap(&portMask, pstPortMask)) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Sal Trunk Port VLAN Conflict: Checks if trunk ports conflict with VLAN configurations
// Returns TRUE if any port in mask has inconsistent VLAN membership
static bool
_sal_trunk_portVlanConflict (
	sal_pPortmask_t *	pstPortMask
	)
{
	sal_flash_vlanEntry_t	stEntry;
	uint16			usSlot;
	sal_flash_vlanDb_t	flashVlanDb;

	sal_flash_vlan_db_get(&flashVlanDb);
    
	for (usSlot = 0; ((usSlot < SAL_MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[usSlot] >= 0)); ++usSlot)
	{
		sal_flash_vlan_sttcEntry_get(flashVlanDb.sortedSlot[usSlot], &stEntry);
		if (_sal_trunk_physicMask_conflict(pstPortMask, &stEntry.p_portMask)) {
			return TRUE;
		}
	}

	return FALSE;
}

// Sal Trunk Port In Mirror: Checks if any trunk port is used in mirroring configuration
// Returns TRUE if any port in mask is being mirrored
static bool
_sal_trunk_portInMirror(
	sal_pPortmask_t *	pstPortMask
	)
{
	uint8 ucIndex;
	uint32 mode = 0;
	uint8 ucPortId = 0;
    sal_lPortmask_t lPortMask;

	for (ucIndex = 0; ucIndex < SAL_MAX_MIRROR_GROUP; ucIndex++) {
		sal_mirror_portMirror_get(ucIndex, &mode, &ucPortId, &lPortMask);

		if ((mode != 0) && (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask, ucPortId))) 
        {
            return TRUE;
        }
	}

	return FALSE;
}

// Sal Trunk Port Rate Limited: Checks if ports in trunk have consistent rate limiting settings
// Returns TRUE if ingress/egress rates differ between ports in the trunk
static bool
_sal_trunk_portRateLimited(
	sal_pPortmask_t *	pstPortMask
	)
{
    uint8 ucPortId, isFirst = TRUE;
    uint32 usInRate, usERate, usFirstInRate = 0xffff, usFirstERate = 0xffff;

	for (ucPortId = 0 ; ucPortId < SAL_PORT_ID_CPU; ucPortId ++) {
		if(SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pstPortMask, ucPortId))
		{
			sal_rate_igrBandwidthCtrl_get(ucPortId, &usInRate);
			sal_rate_egrBandwidthCtrl_get(ucPortId, &usERate);
            if(isFirst == TRUE)
            {
                isFirst = FALSE;
                usFirstInRate = usInRate;
                usFirstERate = usERate;
            }
            else
            {
            
                if((usFirstInRate != usInRate) || (usFirstERate != usERate))
				return TRUE;
		}
	}
    }

	return FALSE;
}

// Sal Trunk Config Restore: Restores all trunk configurations from flash memory
// Reads trunk settings from flash and applies them to hardware
int32
sal_trunk_config_restore (void)
{
	uint8   trkId;
	bool    is_lacp;
	sal_pPortmask_t	portMask;
	sal_pPortmask_t	actMask;
	uint32 algo_bitmask;
    algo_bitmask=0x7f;
	for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {
		sal_flash_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);
		sal_trunk_port_set(trkId, is_lacp, &portMask, &actMask);
	}
    rtk_trunk_distributionAlgorithm_set((rtk_trunk_group_t)RTK_WHOLE_SYSTEM, algo_bitmask);
    return SAL_ERR_SUCCESS;
}

// Sal Trunk Port Set: Configures a trunk group with specified ports and settings
// Validates port speed/duplex consistency and applies trunk configuration
int32
sal_trunk_port_set(
    uint8 trkId, 
    uint8 is_lacp, 
    sal_pPortmask_t *pPortMask, 
    sal_pPortmask_t *pActMask
    )
{    
    int32      ret;
	Tbool		bFirst;
	uint8		ucTrkPorts;
	uint8      ucNego;
	uint8		ucSpeed=SAL_PORT_SPEED_1000;
	uint8		ucPorts;
	sal_port_t	ucPortId;

	uint8      ucFirstNego=FALSE;
	uint8      ucFirstSpeed=SAL_PORT_SPEED_1000;
	uint8      ucDuplex=SAL_PORT_DUPLEX_FULL;
    sal_phyAbility_t ability;
   	rtk_portmask_t portmask;

	is_lacp = 0; // kill compiler warning
	pActMask = NULL;  // kill compiler warning
	if (trkId >= SAL_MAX_STATIC_TRUNK) {
		return SAL_ERR_TRUNK_TRK_RANGE;
	}
	ucPorts = 0;
	for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ++ucPortId) {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pPortMask, ucPortId)) {
			++ucPorts;
		}
	}
    if (ucPorts < 2)
        return SAL_ERR_TRUNK_PORT_NUM_TOO_LOW;

	if (ucPorts > SAL_MAX_PORTS_PER_TRK_DUMB) {
		return SAL_ERR_TRUNK_PORT_NUM_TOO_HIGH;
	}
	if (_sal_trunk_portUsedByTrunk(trkId, pPortMask)) {
		return SAL_ERR_TRUNK_PORT_USED_BY_TRK;
	}
	if (_sal_trunk_portVlanConflict(pPortMask)) {
		return SAL_ERR_TRUNK_PORT_VLAN_CONFLICT;
	}
	if (_sal_trunk_portInMirror(pPortMask)) {
		return SAL_ERR_TRUNK_PORT_IN_MIRROR;
	}
	if (_sal_trunk_portRateLimited(pPortMask)) {
		return SAL_ERR_TRUNK_PORT_RATE_LIMITED;
	}
	ucTrkPorts = 0;
	bFirst = TRUE;

	for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ++ucPortId) {

		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(pPortMask, ucPortId)) {
			++ucTrkPorts;

            /*Change trunk speed/duplex decision */
            //sal_port_linkAbility_get(ucPortId, &ability);
            sal_port_phyAbility_get(ucPortId, (sal_phyAbility_t*)&ability);

            /*Parse speed/duplex from phyAbility*/
			ucNego = ability.AutoNegotiation;

			if (ucNego == SAL_PORT_NEGO_FORCE) /*force mode*/
			{
				if ( ability.Full_10 | ability.Half_10 )
					ucSpeed = SAL_PORT_SPEED_10;
				if ( ability.Full_100 | ability.Half_100 )
					ucSpeed = SAL_PORT_SPEED_100;
				if ( ability.Half_10 | ability.Half_100)
					ucDuplex = SAL_PORT_DUPLEX_HALF;
				if ( ability.Full_10 | ability.Full_100)
					ucDuplex = SAL_PORT_DUPLEX_FULL;
			}

			if (bFirst) {
				bFirst = FALSE;
				ucFirstNego = ucNego;
				if (ucNego == SAL_PORT_NEGO_FORCE)
					ucFirstSpeed = ucSpeed;
			}
			else {
				if (ucFirstNego != ucNego)
				{
					return SAL_ERR_TRUNK_PORT_NEGO;
				}

				if ((ucNego == SAL_PORT_NEGO_FORCE) && (ucFirstSpeed != ucSpeed)) {
					return SAL_ERR_TRUNK_PORT_SPEED;
				}
			}
			if ((ucNego == SAL_PORT_NEGO_FORCE) && (ucDuplex != SAL_PORT_DUPLEX_FULL)) {
                return SAL_ERR_TRUNK_PORT_DUPLEX;
			}
		}
	}

    memset(&portmask, 0, sizeof(rtk_portmask_t));
    portmask.bits[0] = pPortMask->bits[0];
	if ((ret = rtk_trunk_port_set(trkId, &portmask)) != RT_ERR_OK)
		return ret; 

    sal_flash_trunk_port_set (trkId, FALSE, pPortMask, pPortMask);

	return SAL_ERR_SUCCESS;
}

// Sal Trunk Port Del: Deletes a trunk group and cleans up associated resources
// Removes trunk from hardware, flushes MAC entries, and updates flash
int32
sal_trunk_port_del (
	uint8	trkId
	)
{
    sal_port_t   portId;
    int32   ret;
	bool    is_lacp;
	sal_pPortmask_t	portMask;
	sal_pPortmask_t	actMask;
	rtk_portmask_t portmask;
	bool    firstPort=0;
	uint8   logicalId=0;
    rtk_l2_flushCfg_t flushCfg;


	if (trkId >= SAL_MAX_STATIC_TRUNK) {
		return SAL_ERR_TRUNK_TRK_RANGE;
	}

	sal_flash_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);

	for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId) {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, portId)) {
			if (! firstPort) {
				logicalId = portId;
				break;
			}
		}
	}
	
	/* delete trunk */
    memset(&portmask, 0, sizeof(rtk_portmask_t));
	if ((ret = rtk_trunk_port_set(trkId, &portmask)) != RT_ERR_OK)
		return ret;

	/* clear MAC dynamic entries */
    memset(&flushCfg, 0x00, sizeof(rtk_l2_flushCfg_t));
    flushCfg.flushByPort = ENABLED;
    flushCfg.port = logicalId;
	if (ret = rtk_l2_ucastAddr_flush(&flushCfg) != SAL_ERR_SUCCESS)
        return ret;

    /* write to flash */
	sal_flash_trunk_port_del (trkId);
    
	return SAL_ERR_SUCCESS;
}

// Sal Trunk Port Get: Retrieves trunk configuration for a specific trunk ID
// Returns port mask and LACP status from hardware
int32
sal_trunk_port_get(
    uint8 trkId, 
    uint8 *pIs_lacp, 
    sal_pPortmask_t *pPortMask, 
    sal_pPortmask_t *pActMask
    )
{
	int32 ret;
    rtk_portmask_t portmask;

	pIs_lacp = NULL; // kill compiler warning
	pActMask = NULL; // kill compiler warning
	
    if(trkId >= SAL_MAX_STATIC_TRUNK)
		return SAL_ERR_TRUNK_TRK_RANGE;
   
    if ((ret = rtk_trunk_port_get(trkId, &portmask)) != RT_ERR_OK)
        return ret;

    memset(pPortMask, 0, sizeof(sal_pPortmask_t));
    pPortMask->bits[0] = portmask.bits[0];
    
    return SAL_ERR_SUCCESS;
}

// Sal Trunk Logic ID to String: Converts logical port ID to human-readable string
// Returns string representation for port, CPU, or trunk ID
int32
sal_trunk_logicId_toStr (
	int8 *pLogicIdStr, 
	sal_port_t logicId
	)
{
    if (logicId > SAL_PORT_ID_CPU - 1) {
		return SAL_ERR_PORT_RANGE;
	}

	if (logicId < SAL_MAX_PHY_PORT) {
		sprintf(pLogicIdStr, "Port %bd", (uint8)(logicId + 1));
	}
	else if (logicId < SAL_MAX_PHY_N_CPU_PORT) {
		strcpy(pLogicIdStr, "CPU");
	}
	else if (logicId < SAL_MAX_PHY_N_CPU_PORT+ SAL_MAX_TRK_PORT) {
		sprintf(pLogicIdStr, "Trunk%bd", (uint8)(logicId + 1 - SAL_MAX_PHY_N_CPU_PORT));
    }	
    else if (logicId < SAL_PORT_ID_CPU) {
#if defined(LWS_8G_2F)    
        if(logicId == EXT_PORT0)
            sprintf(pLogicIdStr, "Port %bd", 9);
        if(logicId == EXT_PORT1)
            sprintf(pLogicIdStr, "Port %bd", 10);
#elif defined(LWS_5G_1F) 
        if(logicId == EXT_PORT0)
            sprintf(pLogicIdStr, "Port %bd", 6);
#else
#endif
	}
	else {
		strcpy(pLogicIdStr, "Port?");
	}
	
    return SAL_ERR_SUCCESS;
}

// Sal Trunk Logic ID to String RLPP: Converts logical ID to string for RLPP display
// Similar to logicId_toStr but optimized for RLPP protocol display
int32
sal_trunk_logicId_toStr_rlpp (
	int8 *pLogicIdStr, 
	sal_port_t logicId
	)
{
    if (logicId > SAL_PORT_ID_CPU - 1) {
		return SAL_ERR_PORT_RANGE;
	}

	if (logicId < SAL_MAX_PHY_PORT - SAL_MAX_EXT_PORT) {
		sprintf(pLogicIdStr, "Port %bd", (uint8)(logicId + 1));
	}
    else{
#if defined(LWS_8G_2F)
        if(logicId == EXT_PORT0)
            sprintf(pLogicIdStr, "Port %bd", 9);
        if(logicId == EXT_PORT1)
            sprintf(pLogicIdStr, "Port %bd", 10);
#elif defined(LWS_5G_1F)  
        if(logicId == EXT_PORT0)
            sprintf(pLogicIdStr, "Port %bd", 6);
#else
#endif
	}
    
    return SAL_ERR_SUCCESS;
}

// Sal Trunk Logic ID to Physic ID: Converts logical port ID to physical port ID
// Returns first linked port in trunk, or first port if none linked
int32
sal_trunk_logicId_toPhysicId (
    sal_port_t *pPhysicId, 
    sal_port_t logicId
    )
{
    //Tuint32         ulLink;
    uint8   is_lacp;
    int32   ret;
	uint8   trkId;
	uint8   portId, min = 0;
    bool    flag = 0;
    sal_pPortmask_t	portMask;
    sal_pPortmask_t	actMask;
    rtk_port_mac_ability_t stat;

#if defined(LWS_8G_2F)
    if (logicId < 8) {
        *pPhysicId = logicId;
        return SAL_ERR_SUCCESS;
    }
    if(logicId == EXT_PORT0){
        *pPhysicId = logicId;
        return SAL_ERR_SUCCESS;
    }
    if(logicId == EXT_PORT1){
        *pPhysicId = logicId;
        return SAL_ERR_SUCCESS;
    }
#elif defined(LWS_5G_1F)
    if (logicId < 5) {
        *pPhysicId = logicId;
        return SAL_ERR_SUCCESS;
    }
    if(logicId == EXT_PORT0){
        *pPhysicId = logicId;
        return SAL_ERR_SUCCESS;
    }        
#else
	if (logicId < SAL_MAX_PHY_N_CPU_PORT) {
		*pPhysicId = logicId;
		return SAL_ERR_SUCCESS;
	}
#endif

	trkId = logicId - SAL_MAX_PHY_N_CPU_PORT;
	sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);
    if (SAL_ERR_SUCCESS == sal_pPortMask_isZero(&portMask))
		return SAL_ERR_TRUNK_TRK_NO_EXIST;

	for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId) {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, portId)) 
		{
            if ((ret = rtk_port_macStatus_get(portId, &stat)) != RT_ERR_OK)
                return ret;

            if (flag == 0)
            {
                min = portId;
                flag = TRUE;
            }

            if (stat.link)
            {
			    *pPhysicId = portId;
			    return SAL_ERR_SUCCESS;
            }
		}
	}

    *pPhysicId = min;
    return SAL_ERR_SUCCESS;
}

// Sal Trunk Physic ID to Logic ID: Converts physical port ID to logical port ID
// Returns logical ID for port or trunk containing the physical port
int32
sal_trunk_physicId_toLogicId (
    sal_port_t *pLogicId, 
    sal_port_t physicId
	)
{
	sal_lPortmask_t l_PortMask;
    uint8   is_lacp;
	uint8   trkId;
	sal_pPortmask_t portMask;
    sal_pPortmask_t actMask;

	sal_trunk_logicPort_setAll(&l_PortMask);
	if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&l_PortMask, physicId)) {
		*pLogicId = physicId;
		return SAL_ERR_SUCCESS;
	}

	/* port belongs to a trunk */
	for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {
		sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);

		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, physicId)) {
			*pLogicId = SAL_MAX_PHY_N_CPU_PORT + trkId;
			return SAL_ERR_SUCCESS;
		}
	}

	/* not found */
	return SAL_ERR_PORT_RANGE;
}

// Sal Trunk Logic Mask to Physic Mask: Converts logical port mask to physical port mask
// Expands trunk logical ports to their physical port members
int32
sal_trunk_logicMask_toPhysicMask (
	sal_pPortmask_t *pPPortMask, 
	sal_lPortmask_t *pLPortMask
	)
{
	uint8	trkId;

	/* set physical mask: all ports with CPU port */
	sal_pPortMask_copyLMask(pPPortMask, pLPortMask);
	/* set physical mask: trunks */
	for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {
        uint8  is_lacp;
		sal_pPortmask_t	portMask;
        sal_pPortmask_t	actMask;

		if (SAL_ERR_SUCCESS != sal_lPortMask_chkPort(pLPortMask, SAL_MAX_PHY_N_CPU_PORT + trkId)) {
			continue; /* this trunk is not considered */
		}
		sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);

		sal_pPortMask_addPorts(pPPortMask, &portMask);
	}
	return SAL_ERR_SUCCESS;
}

// Sal Trunk Physic Mask to Logic Mask: Converts physical port mask to logical port mask
// Collapses trunk physical ports into single logical trunk entries
int32
sal_trunk_physicMask_toLogicMask (
	sal_lPortmask_t *	pLPortMask,
	sal_pPortmask_t *	pPPortMask
	)
{
    uint8   is_lacp;
    uint8   portId;
    uint8   trkId;
    sal_lPortmask_t	l_PortMask;
    sal_pPortmask_t	portMask;
    sal_pPortmask_t	actMask;

    memset(&l_PortMask, 0, sizeof(sal_lPortmask_t));
    memset(&portMask, 0, sizeof(sal_pPortmask_t));

	/* set logical mask: all ports with CPU */
	sal_lPortMask_copyPMask(pLPortMask, pPPortMask);
	for (portId = SAL_MAX_PHY_N_CPU_PORT; portId < SAL_MAX_PHY_N_CPU_PORT+SAL_MAX_TRK_PORT; ++portId) {
		sal_lPortMask_delPort(pLPortMask, portId);
	}
	/* set logical mask: trunks */
	for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {

		sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);

		if (SAL_ERR_SUCCESS == sal_pPortMask_isZero(&portMask)) {
			continue; /* trunk does not exist */
		}

		sal_pPortMask_and(&portMask, &portMask, pPPortMask);
		if (SAL_ERR_SUCCESS == sal_pPortMask_isZero(&portMask)) {
			continue; /* this trunk is not considered */
		}

        sal_lPortMask_copyPMask(&l_PortMask, &portMask);
		sal_lPortMask_delPorts(pLPortMask, &l_PortMask);
		sal_lPortMask_addPort(pLPortMask, SAL_MAX_PHY_N_CPU_PORT + trkId);
	}
	return SAL_ERR_SUCCESS;
}

// Sal Trunk Logic ID to Physic Mask: Converts logical port ID to physical port mask
// Returns physical port mask for a single logical port or trunk
int32
sal_trunk_logicId_toPhysicMask (
    sal_pPortmask_t *pPPortMask, 
    sal_port_t logicId
	)
{
    uint8   is_lacp;    
	uint8   trkId;
	sal_pPortmask_t	portMask;
    sal_pPortmask_t	actMask;

	sal_pPortMask_delAll(pPPortMask);
	//printf("logicId is : %d\n", (uint16)logicId);
#if defined(LWS_8G_2F)
	if (logicId < 8) {
		sal_pPortMask_addPort(pPPortMask, logicId);
		//printf("pPPortMask after Add is : %lx\n", pPPortMask->bits[0]);
		return SAL_ERR_SUCCESS;
	}
	if (logicId == 8 || logicId == 16){
	    pPPortMask->bits[0] |= (1UL << EXT_PORT0);
		return SAL_ERR_SUCCESS;
	}
	if (logicId == 9 || logicId == 17){
	    pPPortMask->bits[0] |= (1UL << EXT_PORT1);
		return SAL_ERR_SUCCESS;
	}    	
#elif defined(LWS_5G_1F)
	if (logicId < 5) {
		sal_pPortMask_addPort(pPPortMask, logicId);
		return SAL_ERR_SUCCESS;
	}
	if (logicId == 5 || logicId == 16){
	    pPPortMask->bits[0] |= (1UL << EXT_PORT0);
		return SAL_ERR_SUCCESS;
	}
	if (logicId == 17){
	    pPPortMask->bits[0] |= (1UL << EXT_PORT1);
		return SAL_ERR_SUCCESS;
	}	
#else
	if (logicId < SAL_MAX_PHY_N_CPU_PORT) {
		sal_pPortMask_addPort(pPPortMask, logicId);
		//printf("pPPortMask after Add is : %lx\n", pPPortMask->bits[0]);
		return SAL_ERR_SUCCESS;
	}
#endif

    memset(&portMask, 0, sizeof(sal_pPortmask_t));
	trkId = logicId - SAL_MAX_PHY_N_CPU_PORT;
	sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);
    //printf("ucTrkId : %bd, portmask[0]: %lx\n", trkId, portMask.bits[0]);
	if (SAL_ERR_SUCCESS != sal_pPortMask_isZero(&portMask)) {
		sal_pPortMask_copy(pPPortMask, &portMask);
		return SAL_ERR_SUCCESS;
	}

	return SAL_ERR_PORT_RANGE;
}

// Sal Trunk Logic Port Set All: Builds complete logical port mask with all active trunks
// Creates logical port mask containing all physical ports and trunk groups
int32
sal_trunk_logicPort_setAll (
	sal_lPortmask_t *	pstLPortMask
	)
{
    uint8   is_lacp;
	uint8   portId;
	uint8   trkId;
    sal_lPortmask_t	l_PortMask;
	sal_pPortmask_t	portMask;
    sal_pPortmask_t	actMask;

    sal_pPortMask_delAll(&portMask);

	sal_lPortMask_addAll(pstLPortMask);
	for (portId = SAL_MAX_PHY_PORT; portId < SAL_MAX_PHY_N_CPU_PORT + SAL_MAX_TRK_PORT; ++portId) {
		sal_lPortMask_delPort(pstLPortMask, portId);
	}

	for (trkId = 0; trkId < SAL_MAX_STATIC_TRUNK; ++trkId) {
		sal_trunk_port_get(trkId, &is_lacp, &portMask, &actMask);

		if (SAL_ERR_SUCCESS != sal_pPortMask_isZero(&portMask)) {
			sal_lPortMask_addPort(pstLPortMask, SAL_MAX_PHY_N_CPU_PORT + trkId);
            sal_lPortMask_copyPMask(&l_PortMask, &portMask);
			sal_lPortMask_delPorts(pstLPortMask, &l_PortMask);
		}
	}

	return SAL_ERR_SUCCESS;
}

// Sal Trunk Logic ID Exist: Checks if a logical port ID exists in the system
// Returns TRUE if the logical ID represents a valid port or trunk
int32
sal_trunk_logicId_exist (
	sal_port_t logicId
	)
{
	sal_lPortmask_t	stLPortMask;

	sal_trunk_logicPort_setAll(&stLPortMask);
	if (SAL_ERR_SUCCESS != sal_lPortMask_chkPort(&stLPortMask, logicId)) {
		return FALSE;
	}

	return TRUE;
}

/* Function Name:
 *      sal_trunk_representPort_get
 * Description:
 *      Get a represent port of the trunk of a physical port
 * Input:
 *      port - Physical port number
 * Output:
 *      pLport - Port number of the represent port.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
// Sal Trunk Represent Port Get: Gets a representative port for a trunk group
// Returns first linked port in trunk group, or port itself if not in trunk
int32 sal_trunk_representPort_get(sal_port_t port, sal_port_t *pLport)
{
    uint8  linkUp, found;
    uint16 i, trk_grp_start;
    uint32 pmsk;

    if(port >= SAL_PORT_ID_CPU)
        return SAL_ERR_PARAMETER;

    /* get trunk group) */
    rtl8367c_getAsicTrunkingGroup(port >> 2, &pmsk);

    if(port >= 4)
        pmsk = pmsk << 4;

    /* if port in trunk */
    if((1 << port) & (pmsk & 0xFFFF))
    {
        /* find the first linked port number in group */
        trk_grp_start = port & ~(0x3);

        found = FALSE;
        for(i = trk_grp_start;i < trk_grp_start + 4;i++)
        {
            if(pmsk & (1 << i))
            {
                /* check if port is linked up */
                linkUp = getReg(0x1352 + i) & 0x10;
                if(linkUp)
                {
                    found = TRUE;
                    break;
                }
            }
        }

        /* if cannot find represent port, then the represent port is itself */
        if(TRUE == found)
            *pLport = i;
        else
            *pLport = port;
    }
    /* else, the represent is itself */
    else
    {    
        *pLport = port;
    }
    
    return SAL_ERR_SUCCESS;
}