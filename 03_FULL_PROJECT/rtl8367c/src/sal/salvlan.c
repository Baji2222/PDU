#pragma STRING (far)

#include <string.h>
#include <stdio.h>

#include <salvlan.h>			  
#include <salflash.h>
#include <saltrunk.h>
#include <salerror.h>
#include <salportmask.h>
#include <rtl8367c_asicdrv_vlan.h>
#include <vlan.h>


#if SAL_ENABLE
#if 0//Not used
void sal_vlan_dbg_dump(void)
{
    sal_flash_vlanEntry_t   entry;
    sal_flash_vlanDb_t      vlanDb;
	int16 i;

    printf("Dump VLAN:\n");
    printf("Sidx is the member sortedSlot in the sorted-index-database\n");
    printf("The value of Sidx means sorted entry number. \n");
    printf("SVid is sorted VID in a direct view\n");
    printf("      Data Base                 Static Entries      \n");
    printf("Entry SIdx Vid(DB) SVid Valid Vid  Member tag      Name\n");
	sal_flash_vlan_db_get(&vlanDb);

	for (i = 0; i < SAL_MAX_STATIC_VLAN; ++i) {
	    printf("%02d   | %02d | %04d | %04d |", \
	        (int16)i, (int16)vlanDb.sortedSlot[i], (int16)vlanDb.slotVid[i], \
            vlanDb.sortedSlot[i] < 0 ? (int16)-1 : (int16)vlanDb.slotVid[vlanDb.sortedSlot[i]]);

        sal_flash_vlan_sttcEntry_get(i, &entry);
        printf(" %1d | %04d | %02lx | %02lx | %s\n", \
	         (int16)entry.valid, entry.vlan_id, entry.p_portMask.bits[0], \
	        entry.p_tagMask.bits[0], entry.name);
	}
}
#endif

// Sal VLAN Entry Insert: Inserts a VLAN entry into hardware with member and tag port masks
// Converts logical masks to physical and configures VLAN in ASIC
int32
sal_vlan_entry_insert (
	sal_vlan_t vlan_id, 
	sal_lPortmask_t *pPortMask, 
	sal_lPortmask_t *pTagMask
	)
{
    int32           ret;
    sal_pPortmask_t p_PortMask, p_tagMask;
    uint32  max_portmask;
    rtk_vlan_cfg_t vlanCfg;

    //if (vlan_id != 1)
    //    max_portmask = ((uint32)1 << SAL_PORT_SKIP_START) - 1;
    //else
#if defined(LWS_8G_2F)
    max_portmask = 0xff | (1UL << SAL_PORT_ID_CPU) | (1UL << EXT_PORT0) | (1UL << EXT_PORT1);
#elif defined(LWS_5G_1F) 
    max_portmask = 0x1f | (1UL << SAL_PORT_ID_CPU) | (1UL << EXT_PORT0);
#else
    max_portmask = ((1UL << SAL_MAX_PHY_PORT) - 1) | (1UL << SAL_PORT_ID_CPU);
#endif
    sal_trunk_logicMask_toPhysicMask(&p_PortMask, pPortMask);
    sal_trunk_logicMask_toPhysicMask(&p_tagMask, pTagMask);
    
    //if (vlan_id == 1)
		p_PortMask.bits[0] |= 1UL << SAL_PORT_ID_CPU;  /* add CPU port */  
    
    memset(&vlanCfg, 0, sizeof(vlanCfg));
    vlanCfg.mbr.bits[0] = (p_PortMask.bits[0]/* & max_portmask*/);
    vlanCfg.untag.bits[0]  = (~(p_tagMask.bits[0] | (1UL << SAL_PORT_ID_CPU)) & max_portmask);
    vlanCfg.ivl_en = 1;
    if ((ret = rtk_vlan_set(vlan_id, &vlanCfg)) != RT_ERR_OK)
        return ret;
    
	return SAL_ERR_SUCCESS; 
}

// Sal VLAN Entry Remove: Removes a VLAN entry from hardware
// Clears VLAN configuration from ASIC for specified VLAN ID
int32
sal_vlan_entry_remove (uint16 usVlanId)
{
	int32           ret;
    rtk_vlan_cfg_t vlanCfg;

    memset(&vlanCfg, 0, sizeof(vlanCfg));

    if ((ret = rtk_vlan_set(usVlanId, &vlanCfg)) != RT_ERR_OK)
        return ret;

	return SAL_ERR_SUCCESS;
}
#if 0//Not used
int32 sal_vlan_entry_get(
    sal_vlan_t vlan_id, 
    sal_lPortmask_t *pLPortMask, 
    sal_lPortmask_t *pLTagMask, 
    sal_fid_t *pFId, 
    sal_stg_t *pMsti)
{
	int32           ret;   
    sal_pPortmask_t portmask;
	sal_pPortmask_t tagmask;
    rtk_fid_t       fid;
    uint32          max_portmask;
    rtk_vlan_cfg_t vlanCfg;

    memset(&vlanCfg, 0, sizeof(vlanCfg));
    if ((ret = rtk_vlan_get(vlan_id, &vlanCfg)) != RT_ERR_OK)
        return ret;

    memset(&portmask, 0, sizeof(sal_pPortmask_t));
    portmask.bits[0] = vlanCfg.mbr.bits[0];

    if (vlan_id != 1)
        max_portmask = ((uint32)1 << SAL_PORT_SKIP_START) - 1;
    else
        max_portmask = (((uint32)1 << SAL_PORT_SKIP_START) - 1) | ((uint32)1 << SAL_PORT_ID_CPU);
    
    memset(&tagmask, 0, sizeof(sal_pPortmask_t));
    tagmask.bits[0] = (~vlanCfg.untag.bits[0]) & max_portmask;     

    sal_trunk_physicMask_toLogicMask(pLPortMask, &portmask);
    sal_trunk_physicMask_toLogicMask(pLTagMask, &tagmask);

    *pFId = fid;
    *pMsti = 0; // not used;
    
	return SAL_ERR_SUCCESS;
}
#endif

// VLAN PVID Exist: Checks if any port has the specified VLAN as its PVID
// Returns TRUE if PVID exists on any port, FALSE otherwise
static int32 _sal_vlan_pvid_exist (sal_vlan_t  vlan_id)
{
    sal_lPortmask_t portmask;
    sal_vlan_t      pvid;
    sal_port_t      port;
    rtk_vlan_cfg_t vlanCfg;
    
    
    memset(&vlanCfg, 0, sizeof(vlanCfg));
    rtk_vlan_get(vlan_id, &vlanCfg);
    vlanCfg.mbr.bits[0] &= ~ (1<<SAL_PORT_ID_CPU);
    portmask.bits[0] = vlanCfg.mbr.bits[0];
    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&portmask, port)) {
            continue;
        }

        sal_vlan_portPvid_get(port, &pvid);
        if (pvid == vlan_id) {
            return TRUE;
        }
    }
    return FALSE;
}

// Sal VLAN Config Restore: Restores all VLAN configuration from flash
// Restores VLAN enable, entries, PVID, tag mode, and accept frame type
int32 sal_vlan_config_restore(void)
{
    sal_flash_vlanDb_t          flashVlanDb;
    sal_flash_vlanEntry_t       flshVlanEntry;

    sal_vlan_t                  vlan_id;
    sal_lPortmask_t             portmask;
    sal_lPortmask_t             tagmask;
    sal_vlan_acceptFrameType_t  type;
    sal_vlan_tagMode_t          vlanMode;
      
    sal_enable_t    enable;
    sal_port_t      port;
    uint16          i;
    
    sal_flash_vlan_enable_get(&enable);
    sal_vlan_enable_set(enable);
    if (enable)
    {
        sal_flash_vlan_db_get(&flashVlanDb);
        for (i = 0; ((i < SAL_MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[i] >= 0)); ++i)
        {
            sal_flash_vlan_sttcEntry_get(flashVlanDb.sortedSlot[i], &flshVlanEntry);
            sal_trunk_physicMask_toLogicMask(&portmask, &flshVlanEntry.p_portMask);
            sal_trunk_physicMask_toLogicMask(&tagmask, &flshVlanEntry.p_tagMask);
            sal_vlan_sttcEntry_add(flshVlanEntry.vlan_id, flshVlanEntry.name, &portmask, &tagmask);
            
        }
    
        /* set PVID, mode and accept frame type configurations */
        sal_trunk_logicPort_setAll(&portmask);
        for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
            if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portmask, port)) {
                continue;
            }
    
            sal_flash_vlan_portPvid_get(port, &vlan_id);
			sal_vlan_portPvid_set(port, vlan_id);
    
            /*Configure tag/untag mode*/
            sal_flash_vlan_tagMode_get(port, &vlanMode);
            sal_vlan_tagMode_set(port, vlanMode);
    
            /*add init about accept type*/
            sal_flash_vlan_portAcceptFrameType_get(port, &type);
            sal_vlan_port_AcceptFrameType_set(port, type);
        }
    }
    
    return SAL_ERR_SUCCESS;
}

// Sal VLAN Enable Set: Enables or disables VLAN functionality
// Initializes VLAN hardware and saves enable state to flash
int32 sal_vlan_enable_set(sal_enable_t enable)
{    
    rtk_vlan_init();
       
    return sal_flash_vlan_enable_set(enable);
}

// Sal VLAN Enable Get: Retrieves VLAN enable status
// Returns current VLAN enable/disable state from flash
int32 sal_vlan_enable_get(sal_enable_t *pEnable)
{
    sal_flash_vlan_enable_get(pEnable);
    return SAL_ERR_SUCCESS;
}

#if 0
int32 sal_vlan_exist(sal_vlan_t vlan_id)
{
    uint16          usSortedSlotIndex;
    uint16          usSlotVidIndex;
    
    return sal_vlan_db_search(vlan_id, &usSortedSlotIndex, &usSlotVidIndex);
}

int32 sal_vlan_sttcEntry_create(sal_vlan_t vlan_id, int8 *pName)
{
    sal_enable_t    enable;
	bool            bFound;
	uint16          sortedSlotIndex;
	uint16          slotVidIndex;
	sal_lPortmask_t portMask;
	sal_lPortmask_t tagMask;
	sal_flash_vlanEntry_t   newEntry;

    if(strlen(pName) >= SAL_MAX_VLAN_NAME_LEN)
        return SAL_ERR_INPUT_STR_LEN;
 
	sal_vlan_enable_get(&enable);

	if (enable == DISABLE) {
		return SAL_ERR_VLAN_DISABLE;
	}

	if ( (!vlan_id) || (vlan_id > 4094)) {
		return SAL_ERR_VLAN_ID_INVALID;
	}

	bFound = sal_vlan_db_search(vlan_id, &sortedSlotIndex, &slotVidIndex);

	if (sortedSlotIndex > SAL_MAX_STATIC_VLAN - 1) {
		return SAL_ERR_VLAN_FULL;
	}

	if (bFound)
		return SAL_ERR_VLAN_EXIST;


	newEntry.vlan_id = vlan_id;
	strncpy(newEntry.name, pName, SAL_MAX_VLAN_NAME_LEN - 1);
	newEntry.name[SAL_MAX_VLAN_NAME_LEN-1] = '\0';

	sal_lPortMask_delAll(&portMask);
	sal_lPortMask_delAll(&tagMask);
    sal_pPortMask_delAll(&newEntry.p_portMask);
	sal_pPortMask_delAll(&newEntry.p_tagMask);

	if (enable == ENABLE) {
		/* insert */
		if (sal_vlan_entry_insert(vlan_id, &portMask, &tagMask) != SAL_ERR_SUCCESS) {
			return SAL_ERR_VLAN_INS_VLAN_ENTRY;
		}
	}

	/* write to flash */
	sal_vlan_db_insert(vlan_id, sortedSlotIndex, slotVidIndex);
	sal_flash_vlan_sttcEntry_create(slotVidIndex, &newEntry);

    return SAL_ERR_SUCCESS;
}

int32 sal_vlan_sttcEntry_set(sal_vlan_t vlan_id, sal_lPortmask_t *pPortMask, sal_lPortmask_t *pTagMask)
{
	sal_enable_t    enable;
	bool    found;
	uint16  sortedSlotIndex;
	uint16  slotVidIndex;

	sal_flash_vlanEntry_t   newEntry;


	sal_vlan_enable_get(&enable);
	if (enable == DISABLE) {
		return SAL_ERR_VLAN_DISABLE;
	}

	if ( (!vlan_id) || (vlan_id > 4094)) {
		return SAL_ERR_VLAN_ID_INVALID;
	}

	found = sal_vlan_db_search(vlan_id, &sortedSlotIndex, &slotVidIndex);

	if (sortedSlotIndex > SAL_MAX_STATIC_VLAN - 1) {
		return SAL_ERR_VLAN_FULL;
	}

	if (!found)
		return SAL_ERR_VLAN_NOT_EXIST;

	newEntry.vlan_id = vlan_id;
	if (vlan_id == 1)
		sal_lPortMask_addPort(pPortMask, SAL_PORT_ID_CPU);/* add CPU port */

	if (enable == ENABLE) {
		if (sal_vlan_entry_insert(vlan_id, pPortMask, pTagMask) != SAL_ERR_SUCCESS) {
			return SAL_ERR_VLAN_INS_VLAN_ENTRY;
		}
	}

	/* write to flash */
    sal_trunk_logicMask_toPhysicMask(&newEntry.p_portMask, pPortMask);
    sal_trunk_logicMask_toPhysicMask(&newEntry.p_tagMask, pTagMask);
	sal_flash_vlan_sttcEntry_set(slotVidIndex, &newEntry);

	return SAL_ERR_SUCCESS;
}

int32 sal_vlan_name_change(sal_vlan_t vlan_id, int8 *pName)
{
	sal_flash_vlanEntry_t   oldEntry;
    sal_lPortmask_t portMask;
    sal_lPortmask_t tagMask;
    sal_enable_t    enable;
	uint16          sortedSlotIndex;
	uint16          slotVidIndex;
	uint8           found;
	
	sal_vlan_enable_get(&enable);
	if (enable == DISABLE) {
		return SAL_ERR_VLAN_DISABLE;
	}

	if ( (!vlan_id) || (vlan_id > 4094)) {
		return SAL_ERR_VLAN_ID_INVALID;
	}

	found = sal_vlan_db_search(vlan_id, &sortedSlotIndex, &slotVidIndex);

	if (sortedSlotIndex > SAL_MAX_STATIC_VLAN-1) {
		return SAL_ERR_VLAN_FULL;
	}

	if (!found)
		return SAL_ERR_VLAN_NOT_EXIST;

	oldEntry.vlan_id = vlan_id;
	sal_vlan_sttcEntry_get(vlan_id, &portMask, &tagMask);
	strncpy(oldEntry.name, pName, SAL_MAX_VLAN_NAME_LEN);
	oldEntry.name[SAL_MAX_VLAN_NAME_LEN] = '\0';

	/* write to flash */
	sal_flash_vlan_sttcEntry_create (slotVidIndex, &oldEntry);

	return SAL_ERR_SUCCESS;
}

int32 sal_vlan_sttcEntry_get(sal_vlan_t vlan_id, sal_lPortmask_t *pPortMask, sal_lPortmask_t *pTagMask)
{
    sal_flash_vlanEntry_t   entry;
    uint16  sortedSlotIndex;
    uint16  slotVidIndex;
    
    if (!(sal_vlan_db_search(vlan_id, &sortedSlotIndex, &slotVidIndex))) {
        return SAL_ERR_VLAN_NOT_EXIST;
    }

    sal_flash_vlan_sttcEntry_get(slotVidIndex, &entry);

    sal_trunk_physicMask_toLogicMask(pPortMask, &entry.p_portMask);
    sal_trunk_physicMask_toLogicMask(pTagMask, &entry.p_tagMask);
    return SAL_ERR_SUCCESS;
}
#endif

// Sal VLAN Static Entry Remove: Removes a static VLAN entry
// Checks if VLAN is used as PVID, removes from hardware and flash database
int32 sal_vlan_sttcEntry_remove(sal_vlan_t vlan_id)
{
    uint16  sortedSlotIndex;
	uint16  slotVidIndex;
	sal_enable_t    enable;

	sal_vlan_enable_get(&enable);
	if (enable == DISABLE) {
		return SAL_ERR_VLAN_DISABLE;
	}

	if (_sal_vlan_pvid_exist(vlan_id)) {
		return SAL_ERR_VLAN_EXIST_PVID;
	}

	if (!(sal_vlan_db_search(vlan_id, &sortedSlotIndex, &slotVidIndex))) {
		return SAL_ERR_VLAN_NOT_EXIST;
	}
	
    /* set hardware */
    if (sal_vlan_entry_remove(vlan_id) != SAL_ERR_SUCCESS) {
        return SAL_ERR_VLAN_DEL_VLAN_ENTRY;
	}

	/* write to flash */
	sal_vlan_db_del(sortedSlotIndex);
	sal_flash_vlan_sttcEntry_remove(slotVidIndex);
    
	return SAL_ERR_SUCCESS;
}

// Sal VLAN DB Insert: Inserts a VLAN entry into the sorted index database
// Updates the sorted database with new VLAN entry at the correct position
void sal_vlan_db_insert(sal_vlan_t vlan_id, uint16 sorted_slot_idx, uint16 slot_idx)
{
	sal_flash_vlanDb_t	flashVlanDb;
	int16   i;
   
	sal_flash_vlan_db_get(&flashVlanDb);
	for (i = 0; ((i < SAL_MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[i] >= 0)); ++i) {
	}
	
	for (; i > sorted_slot_idx; --i) {
		flashVlanDb.sortedSlot[i] = flashVlanDb.sortedSlot[i-1];
	}
	
    flashVlanDb.slotVid[slot_idx] = vlan_id;
	flashVlanDb.sortedSlot[i] = slot_idx;	
	
	sal_flash_vlan_db_set(&flashVlanDb);
}

// Sal VLAN DB Search: Searches for a VLAN ID in the sorted index database
// Returns TRUE with indices if found, FALSE with insertion position if not
int32 sal_vlan_db_search(sal_vlan_t vlan_id, uint16 *pSorted_slot_idx, uint16 *pSlot_idx)
{
	sal_flash_vlanDb_t  vlanDb;
	int16   i;
	int16   j;

	sal_flash_vlan_db_get(&vlanDb);
	for (i = 0; i < SAL_MAX_STATIC_VLAN; ++i) {
		if ((vlanDb.sortedSlot[i] < 0) || (vlanDb.slotVid[vlanDb.sortedSlot[i]] > vlan_id)) {
			/* not found */
			*pSorted_slot_idx = i;
			for (j = 0; ((j < SAL_MAX_STATIC_VLAN) && (vlanDb.slotVid[j] >= 0)); ++j) {
			}
			if (j >= SAL_MAX_STATIC_VLAN) {
				*pSorted_slot_idx = SAL_MAX_STATIC_VLAN; /* no vacancy return value */
			}
			*pSlot_idx = j;
			return FALSE;
		}

		if (vlanDb.slotVid[vlanDb.sortedSlot[i]] == vlan_id) {
			/* found */
			*pSorted_slot_idx = i;
			*pSlot_idx = vlanDb.sortedSlot[i];
			return TRUE;
		}
	}
	/* end of all slots, not found */
	*pSorted_slot_idx = SAL_MAX_STATIC_VLAN; /* no vacancy return value */
	*pSlot_idx = SAL_MAX_STATIC_VLAN; /* no vacancy return value */
	return FALSE;
}

// Sal VLAN DB Delete: Deletes a VLAN entry from the sorted index database
// Removes entry and shifts subsequent entries to fill the gap
void
sal_vlan_db_del (
	uint16	usSortedSlotIndex
	)
{
	sal_flash_vlanDb_t  flashVlanDb;
	int16   i;

	sal_flash_vlan_db_get(&flashVlanDb);
	//printf("slotVid index is: %d\n", flashVlanDb.sortedSlot[usSortedSlotIndex]);
	flashVlanDb.slotVid[flashVlanDb.sortedSlot[usSortedSlotIndex]] = -1;
	for (i = usSortedSlotIndex; ((i < SAL_MAX_STATIC_VLAN-1) && (flashVlanDb.sortedSlot[i+1] >= 0)); ++i)
    {
		flashVlanDb.sortedSlot[i] = flashVlanDb.sortedSlot[i+1];
	}
	flashVlanDb.sortedSlot[i] = -1;
	sal_flash_vlan_db_set(&flashVlanDb);
}

// Sal VLAN Port PVID Set: Sets the Port VLAN ID (PVID) for a port
// Configures default VLAN for untagged frames received on the port
int32 sal_vlan_portPvid_set(sal_port_t port, sal_vlan_t vlan_id)
{
    int32   ret;
    uint8   portId;
    sal_pPortmask_t	portmask;
    sal_pri_t intPri;

    sal_trunk_logicId_toPhysicMask(&portmask, port);
    for (portId = 0; portId <= SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portmask, portId))
        {
            sal_qos_portPri_get(portId, &intPri);
	        if ((ret = rtk_vlan_portPvid_set(portId, vlan_id, intPri)) != RT_ERR_OK)
            {
                SAL_PRINT("Error in setting pvid: %lx", ret);
                return ret;
            }
            
        }
    }

    return sal_flash_vlan_portPvid_set(port, vlan_id);
}

// Sal VLAN Port PVID Get: Retrieves the Port VLAN ID (PVID) for a port
// Gets the default VLAN ID configured for untagged frames on the port
int32 sal_vlan_portPvid_get(sal_port_t port, sal_vlan_t *pVlan_id)
{
    int32       ret;
    uint32      vlan_tmp;
    sal_port_t  portId;
    uint32      pri;

    sal_trunk_logicId_toPhysicId(&portId, port);
    if ((ret = rtk_vlan_portPvid_get(portId, &vlan_tmp, &pri)) != RT_ERR_OK)
    {
         printf("Error in getting pvid: %lx", ret);
         return ret;
    }

    *pVlan_id = vlan_tmp;

	return SAL_ERR_SUCCESS;
}

// Sal VLAN Tag Mode Set: Sets the VLAN tag processing mode for a port
// Configures how VLAN tags are handled: original, keep format, or priority only
int32 sal_vlan_tagMode_set(sal_port_t port, sal_vlan_tagMode_t mode)
{
    int32              ret;
    rtk_vlan_tagMode_t rtk_tag_mode;
    sal_pPortmask_t    portmask;
    uint8              portId;    

    switch(mode)
    {  
    case SAL_VLAN_TAG_MODE_ORIGINAL:
        rtk_tag_mode = VLAN_TAG_MODE_ORIGINAL;
        break;
    case SAL_VLAN_TAG_MODE_KEEP_FORMAT:
        rtk_tag_mode = VLAN_TAG_MODE_REAL_KEEP_FORMAT;
        break;
    case SAL_VLAN_TAG_MODE_PRI:
        rtk_tag_mode = VLAN_TAG_MODE_PRI;
        break;
    default:
        SAL_PRINT("unknown mode!\n");
        return RT_ERR_FAILED;
    }
    
    sal_trunk_logicId_toPhysicMask(&portmask, port);
    
    for (portId = 0; portId <= SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portmask, portId))
        {
            if ((ret = rtk_vlan_tagMode_set(portId, rtk_tag_mode)) != SAL_ERR_SUCCESS)
            {
                SAL_PRINT("Set vlan tag mode fail\n");
                return ret;            
            }
        }
    }
    return sal_flash_vlan_tagMode_set(port, mode);

}
#if 0//Not used
int32 sal_vlan_tagMode_get(sal_port_t port, sal_vlan_tagMode_t *pMode)
{
    int32               ret;
    rtk_vlan_tagMode_t  vlan_tag_mode;
    uint8               portId;
    
    sal_trunk_logicId_toPhysicId(&portId, port);
    if ((ret = rtk_vlan_tagMode_get(portId, &vlan_tag_mode)) != RT_ERR_OK)
    {
         SAL_PRINT("Error: %lx", ret);
         return ret;
    }

    switch(vlan_tag_mode)
    {
    case VLAN_TAG_MODE_ORIGINAL:
        *pMode = SAL_VLAN_TAG_MODE_ORIGINAL;
        break;
    case VLAN_TAG_MODE_KEEP_FORMAT:
    case VLAN_TAG_MODE_REAL_KEEP_FORMAT:
        *pMode = SAL_VLAN_TAG_MODE_KEEP_FORMAT;
        break;
    case VLAN_TAG_MODE_PRI:
        *pMode = SAL_VLAN_TAG_MODE_PRI;
       break;
    default:
        SAL_PRINT("Get vlan tag mode error\n");
        return SAL_ERR_FAILED;
    }
 
    return SAL_ERR_SUCCESS;
}
#endif

// Sal VLAN Port Accept Frame Type Set: Sets the acceptable frame types for a port
// Configures whether the port accepts tagged, untagged, or all frames
int32 sal_vlan_port_AcceptFrameType_set(sal_port_t port, sal_vlan_acceptFrameType_t type)
{
    int32           ret;
    sal_pPortmask_t stPPortMask;
    uint8           portId;    

    sal_trunk_logicId_toPhysicMask(&stPPortMask, port);
    for (portId = 0; portId <= SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&stPPortMask, portId))
        {
            //printf("AcceptFrameType set: port: %d, type: %d\n", (uint16)portId, (uint16)type);
            ret = rtk_vlan_portAcceptFrameType_set(portId, type);
            SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);
        }
    }
    return sal_flash_vlan_portAcceptFrameType_set(port, type);
}

// Sal VLAN Port Accept Frame Type Get: Retrieves the acceptable frame types for a port
// Gets whether the port accepts tagged, untagged, or all frames
int32 sal_vlan_port_AcceptFrameType_get(sal_port_t port, sal_vlan_acceptFrameType_t *pType)
{
    rtk_vlan_acceptFrameType_t type;
    int32      ret;
    uint8      portId;
    
    sal_trunk_logicId_toPhysicId(&portId, port);
    ret = rtk_vlan_portAcceptFrameType_get(portId, &type);
    *pType = type;

    SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);
    return SAL_ERR_SUCCESS;

}

/* Function Name:
 *      sal_vlan_sttcEntry_add
 * Description:
 *      Add an static vlan entries 
 * Input:
 *      vlan_id - VLAN ID
 *      pName  - string of VLAN name
 *      pPortMask - Member port mask
 *      pTagMask  - tag mask
 * Output:
 *      No
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_VLAN_FULL
 * Note:
 *      If an entry with the same vid exists, update the fields in the entry.
 *      Otherwise, a new static entry and one corresponding index entry 
 *      in the sorted-index-database are created. 
 */
// Sal VLAN Static Entry Add: Adds or updates a static VLAN entry
// Creates new VLAN or updates existing, writes to hardware and flash database
int32 sal_vlan_sttcEntry_add(
    sal_vlan_t vlan_id, 
    int8       *pName, 
    sal_lPortmask_t *pPortMask, 
    sal_lPortmask_t *pTagMask
    ) 
{
    int32   ret;
	sal_flash_vlanEntry_t entry; /* one static entrie                   */
	uint16  entry_index;          /* The entry index range from 0 ~ 31   */
	uint16  sorted_idx;           /* Index in the sorted database        */
    bool    found;
    if ( (!vlan_id) || (vlan_id > 4094)) {
		return SAL_ERR_VLAN_ID_INVALID;
	}
	
	found = sal_vlan_db_search(vlan_id, &sorted_idx, &entry_index);
	
	if (entry_index == SAL_MAX_STATIC_VLAN) {
		return SAL_ERR_VLAN_FULL;
	}
    /* Set the asic */
    if ((ret = sal_vlan_entry_insert(vlan_id, pPortMask, pTagMask)) != SAL_ERR_SUCCESS) {
		return ret;
    }
     /* update the entry in the 32 static VLAN entries no matter 
        the vlan id is exist previously */
	entry.vlan_id = vlan_id;
	strncpy(entry.name, pName, SAL_MAX_VLAN_NAME_LEN - 1);
	entry.name[SAL_MAX_VLAN_NAME_LEN-1] = '\0';
    entry.valid = 1;
    
    sal_trunk_logicMask_toPhysicMask(&entry.p_portMask, pPortMask);
    sal_trunk_logicMask_toPhysicMask(&entry.p_tagMask, pTagMask);

	sal_flash_vlan_sttcEntry_set(entry_index, &entry);

    /* If the vlan id doesn't exist, create a new entry in the index 
        database which points to the entry in the 32 static VLAN entries */
    if (found == FALSE)
	    sal_vlan_db_insert(vlan_id, sorted_idx, entry_index);

    return SAL_ERR_SUCCESS;
}
#if 0
/* Function Name:
 *      _sal_clear_entry
 * Description:
 *      Check whether there exists another entry with the same vid
 * Input:
 *      index - static vlan entry index, range from 0 ~ 31
 *      vlan_id - vlan id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_VLAN_ENTRY_DUPLICATE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
static int32 _sal_duplicated_entry_check(int16 index, sal_vlan_t vlan_id)
{
    sal_flash_vlanEntry_t entry;
    int16   i;

    for (i = 0; i < SAL_MAX_STATIC_VLAN; i++)
    {
        sal_flash_vlan_sttcEntry_get(i, &entry);

        if (i != index && entry.vlan_id == vlan_id)
        {
            printf("duplicated entry!\n");
            return SAL_ERR_VLAN_ENTRY_DUPLICATE;
        }
    }    

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      _sal_clear_entry
 * Description:
 *      Clear static vlan entry. First check whether the static entry is referenced 
 *      in the index-database. If found, delete it from database, entry and hardware. 
 *      Otherwise, delete it from entry.
 * Input:
 *      index - static vlan entry index, range from 0 ~ 31
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
static int32 _sal_clear_entry(int16 index)
{
    sal_flash_vlanDb_t      vlanDb;
    sal_flash_vlanEntry_t   entry;
    sal_vlan_t  vlan_id;
	int16       i;
    bool        found;

	sal_flash_vlan_db_get(&vlanDb);

	found = FALSE;
	for (i = 0; i < SAL_MAX_STATIC_VLAN; ++i) 
	{
	    if (vlanDb.sortedSlot[i] < 0)
	        break;

	    if (vlanDb.sortedSlot[i] == index)
	    {
	        found = TRUE;  /* referenced */
	        vlan_id = vlanDb.slotVid[index];
	    }
    }
    
    if (found == TRUE)  /* an valid entry */
    {
        return sal_vlan_sttcEntry_remove(vlan_id);
    }
    else    /* an invalid entry, an invalid entry won't be set into the hardware */
    {
        sal_vlan_index_entry_get(index, (sal_vlan_entry_t*)&entry);
        if (entry.valid == 1)
        {
            printf("Shouldn't be here, valid entry will be found in the database in the previous look up!");
            return SAL_ERR_FAILED;
        }

        sal_flash_vlan_sttcEntry_remove(index);
    }

    return SAL_ERR_SUCCESS; 
}

/* Function Name:
 *      _sal_vlan_index_entry_set_validate
 * Description:
 *      Validate an VLAN entry. If pEntry equals NULL, load the indexed content
 *      into hardware, else save pEntry into the indexed entry and set the hardware.
 * Input:
 *      index - static vlan entry index, range from 0 ~ 31
 *      pEntry - vlan entry content
 * Output:
 *      No
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 */
static int32 _sal_vlan_index_entry_set_validate(int16 index, sal_flash_vlanEntry_t *pEntry)
{
    int32   ret;
    sal_flash_vlanEntry_t entry;
    sal_lPortmask_t portmask;
    sal_lPortmask_t tagmask; 
    uint16  entry_index;          /* The entry index range from 0 ~ 31   */
    uint16  sorted_idx;           /* Index in the sorted database        */
    bool    found;
       
    if (pEntry == NULL) /* Load the indexed content into hardware*/
    {
        if ((ret = sal_flash_vlan_sttcEntry_get(index, &entry)) != SAL_ERR_SUCCESS)
            return ret;

        if ((!entry.vlan_id) || (entry.vlan_id > 4094)) 
	    	return SAL_ERR_VLAN_ENTRY_INVALID;
	
        sal_trunk_physicMask_toLogicMask(&portmask, &entry.p_portMask);
        sal_trunk_physicMask_toLogicMask(&tagmask, &entry.p_tagMask);

        if ((ret = sal_vlan_entry_insert(entry.vlan_id, &portmask, &tagmask)) != SAL_ERR_SUCCESS) 
    	  	return ret;

        /* Valid the entry if it is an invalid entry */
    	if (entry.valid == 0)
        {
            entry.valid = 1;
    	    sal_flash_vlan_sttcEntry_set(index, &entry);    	    
    	}
    }
    else    /* Save pEntry into the indexed entry and set the hardware*/
    {
        /* Check whether it will cause a duplicated one */
        if (_sal_duplicated_entry_check(index, pEntry->vlan_id) == SAL_ERR_VLAN_ENTRY_DUPLICATE)
            return SAL_ERR_VLAN_ENTRY_DUPLICATE;
        
        if ((ret = _sal_clear_entry(index)) != SAL_ERR_SUCCESS) 
    	  	return ret;

  	  	/* Write the hardware and entry */
        sal_trunk_physicMask_toLogicMask(&portmask, &pEntry->p_portMask);
        sal_trunk_physicMask_toLogicMask(&tagmask, &pEntry->p_tagMask);

        if ((ret = sal_vlan_entry_insert(pEntry->vlan_id, &portmask, &tagmask)) != SAL_ERR_SUCCESS) 
    	  	return ret;

        pEntry->valid = 1;
        sal_flash_vlan_sttcEntry_set(index, pEntry);
        
    	/* Update sorted index-database */
    	found = sal_vlan_db_search(pEntry->vlan_id, &sorted_idx, &entry_index);
        if (found)
        {
            printf("Shouldn't found the same vlan id in the database!\n");
            return SAL_ERR_FAILED;
        }

        sal_vlan_db_insert(pEntry->vlan_id, sorted_idx, index); 	
    }
	    	
    return SAL_ERR_SUCCESS; 
}

/* Function Name:
 *      _sal_vlan_index_entry_set_keep
 * Description:
 *      If the entry is valid, save pEntry into the indexed entry and set the hardware.
 *      Otherwise, save pEntry into the indexed entry and don't set the hardware.
 * Input:
 *      index - static vlan entry index, range from 0 ~ 31
 *      pEntry - vlan entry content
 * Output:
 *      No
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 */
static int32 _sal_vlan_index_entry_set_keep(int16 index, sal_flash_vlanEntry_t *pEntry)
{
    int32 ret;
    sal_flash_vlanEntry_t entry;

    if (_sal_duplicated_entry_check(index, pEntry->vlan_id) == SAL_ERR_VLAN_ENTRY_DUPLICATE)
        return SAL_ERR_VLAN_ENTRY_DUPLICATE;

    if ((ret = sal_vlan_index_entry_get(index, (sal_vlan_entry_t*)&entry)) != SAL_ERR_SUCCESS)
        return ret;

    if (entry.valid == 1)
    {
        _sal_vlan_index_entry_set_validate(index, pEntry);
    }
    else
    {   
  	  	/* Update Entry */
  	  	pEntry->valid = 0; /* Keep valid status*/
        sal_flash_vlan_sttcEntry_set(index, pEntry);
    }
	    	
    return SAL_ERR_SUCCESS;        
}
int32 sal_vlan_index_entry_set(int16 index, sal_vlan_entry_t *pEntry, sal_entry_action_t action) 
{
    int32 ret;

    if (index >= SAL_MAX_STATIC_VLAN) 
		return SAL_ERR_ENTRY_INDEX;
	
    switch(action)
    {
        case SAL_ENTRY_ACT_VALID:
            if ((ret = _sal_vlan_index_entry_set_validate(index, (sal_flash_vlanEntry_t*)pEntry)) != SAL_ERR_SUCCESS)
                return ret;                
            break;
        case SAL_ENTRY_ACT_INVALID:
            if ((ret = _sal_clear_entry(index)) != SAL_ERR_SUCCESS)
                return ret;
            break;
        case SAL_ENTRY_ACT_KEEP:
            if ((ret = _sal_vlan_index_entry_set_keep(index, (sal_flash_vlanEntry_t*)pEntry)) != SAL_ERR_SUCCESS)
                return ret;  
            break;
        default:
            return SAL_ERR_PARAMETER;
    }
        
    return SAL_ERR_SUCCESS;
}

int32 sal_vlan_index_entry_get(int16 index, sal_vlan_entry_t *pEntry)
{
    return sal_flash_vlan_sttcEntry_get(index, (sal_flash_vlanEntry_t*)pEntry);
}
#endif

#endif