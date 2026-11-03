#pragma STRING (far)

#include <string.h>
#include <stdio.h>

#include <sall2.h>
#include <salflash.h>
#include <salerror.h>
#include <salutil.h>
#include <saltrunk.h>
#include <rtk_api\l2.h>

#include <rtl8367c_asicdrv_lut.h>
#include <rtl8367c_asicdrv_port.h>
#include <l2.h>

#if SAL_ENABLE

int32 _sal_lutTbl_getByIdx(uint32 idx, rtk_l2_ucastAddr_t *l2Param)
{
    rtl8367c_luttb lut_entry;
    rtk_api_ret_t ret;
    
    if ( idx >= RTK_MAX_LUT_ADDRESS )
        return SAL_ERR_PARAMETER;
    
    lut_entry.address = idx;
    
    if ((ret = rtl8367c_getAsicL2LookupTb(LUTREADMETHOD_ADDRESS, &lut_entry)) != RT_ERR_OK)
    {
        return ret;
    }
    
    /* if this is not unicast entry, return failed */
    if ( ((lut_entry.mac.octet[0] & 1) != 0)  || (lut_entry.age == 0))
    {
        return FAILED;
    }
    
    l2Param->auth      = lut_entry.auth;
    l2Param->fid       = lut_entry.fid;
    l2Param->cvid      = lut_entry.cvid_fid;//add
    l2Param->is_static = lut_entry.nosalearn;
    l2Param->port      = lut_entry.spa;
    l2Param->sa_block  = lut_entry.sa_en;
    memcpy(&l2Param->mac, &lut_entry.mac, MAC_ADDR_LEN);
    
    return RT_ERR_OK;    
}

void sal_mac_dbg_dump(void)
{
    sal_flash_l2_sttcEntry_t entry;
    uint8 mac_str[30];
	int i;

    printf("Dump MAC Table:\n");
    printf("Entry Valid         mac         fid   auth  PortMask saBlock daBlock\n");
	
	for (i = 0; i < SAL_MAX_STATIC_MAC; i++) {
        sal_flash_l2_sttcEntry_get(i, &entry);
        mac_to_str(mac_str, entry.mac.octet);
        printf(" %02d  | %02d | %s | %04d |  %02d  |  %02lx   |   %02d   |  %02d \n", \
	         (int16)i, (int16)entry.valid, mac_str, (int16)entry.fid, (int16)entry.auth, \
	         (int32)entry.pPortMask.bits[0], (int16)entry.saBlock, (int16)entry.daBlock);
	}
}

int32 sal_l2_config_restore (void)
{
    uint16			    slot;
    sal_mac_t           empty;
	sal_flash_l2_sttcEntry_t	flshMacSttcEntry;
	uint8           action;
	sal_port_t      port;
	sal_lPortmask_t portmask;
	uint32          mac_limit;
    rtk_api_ret_t      ret;   

	memset(&empty, 0, sizeof(sal_mac_t));
	sal_l2_table_clear(SAL_L2_CLR_DYNAMIC);

	/* insert static MAC entries from flash */
	for (slot = 0; slot < SAL_MAX_STATIC_MAC; ++slot) {
		if (SAL_ERR_SUCCESS == sal_flash_l2_sttcEntry_get(slot, &flshMacSttcEntry)) {
			if (memcmp(flshMacSttcEntry.mac.octet, empty.octet, MAC_ADDR_LEN) != 0) {
			    sal_l2_entry_add(&flshMacSttcEntry.mac, flshMacSttcEntry.fid, TRUE,
                                flshMacSttcEntry.auth, flshMacSttcEntry.saBlock,
                                flshMacSttcEntry.daBlock, &flshMacSttcEntry.pPortMask);                                 
			}
		}
	}

    sal_trunk_logicPort_setAll(&portmask);
	for (port = 0; port < SAL_PORT_ID_CPU; ++port)
	{
		if (SAL_ERR_SUCCESS != sal_lPortMask_chkPort(&portmask, port))
		{
			continue;
		}

		sal_flash_l2_mac_constraint_get(port, &mac_limit);
		sal_l2_limitLearningCnt_set(port, mac_limit);
	}
	sal_flash_l2_mac_constraint_act_get(&action);
    if ((ret = rtk_l2_limitLearningCntAction_set(RTK_WHOLE_SYSTEM, action)) != RT_ERR_OK)
        return ret;	
	return SAL_ERR_SUCCESS;
}

int32 sal_l2_table_clear (sal_l2_clr_t type)
{
	uint16	index;
	uint8	aucMac[MAC_ADDR_LEN];
	sal_flash_l2_sttcEntry_t	flashMacEntry;
    //sal_port_t port;
    //rtk_api_ret_t ret;
    switch (type)
	{
        case SAL_L2_CLR_STATIC:
            break;

        case SAL_L2_CLR_DYNAMIC:
#if 0
            for (port = 0; port < SAL_MAX_PHY_PORT; ++port)
            {
                //if ((ret = rtk_l2_flushType_set(FLUSH_TYPE_BY_PORT, 0, port)) != RT_ERR_OK)
                if ((ret = rtl8367c_setAsicLutFlushMode(FLUSHMDOE_PORT)) != RT_ERR_OK)
                    return ret;
                if ((ret = rtl8367c_setAsicLutForceFlush(1<<port)) != RT_ERR_OK)
                    return ret;                    
            }
#endif            

            /*Restore static entries*/
    	    memset(aucMac, 0, MAC_ADDR_LEN);
    	    for (index = 0; index < SAL_MAX_STATIC_MAC; ++index)
            {
                sal_flash_l2_sttcEntry_get(index, &flashMacEntry);
                if (memcmp(flashMacEntry.mac.octet, aucMac, MAC_ADDR_LEN) != 0)
                {
                    sal_l2_entry_add(&flashMacEntry.mac, flashMacEntry.fid, TRUE,
                        flashMacEntry.auth, flashMacEntry.saBlock, flashMacEntry.daBlock,
                        &flashMacEntry.pPortMask);
                }
    	    }
			break;

        case SAL_L2_CLR_ALL:
			break;
    }

    return SAL_ERR_SUCCESS;
}

int32
sal_l2_entry_add (
	sal_mac_t       *pMac,
	sal_vlan_t		vid,
	uint8		    is_static,
	uint8		    is_auth,
	uint8		    sa_block,
	uint8		    da_block,
	sal_pPortmask_t *pPortMask
	)
{
    int32  ret;
	uint32 ulSpa = 0;
	uint8  ucIndex;
    rtk_mac_t mac;
    rtk_l2_ucastAddr_t lut_entry;

    if(da_block)
        return SAL_ERR_NOT_SUPPORT;
    
	if (pMac->octet[0] & 0x1) /* Multicast entry */
	{
		return SAL_ERR_NOT_SUPPORT;
	}
	else /* L2 Unicast entry */
	{
		/* Transfer port-mask to port number, and get the minimum port number to be spa */
		for (ucIndex = 0; ucIndex < SAL_PORT_ID_CPU; ucIndex++)
		{
			if ((pPortMask->bits[0]) & (1UL << ucIndex))
			{
				ulSpa = ucIndex; /* Port number is computed from 0. */
				break;
			}
		}

        memcpy(mac.octet, pMac->octet, MAC_ADDR_LEN);
        memset(&lut_entry, 0, sizeof(rtk_l2_ucastAddr_t));
		lut_entry.ivl=1;
        lut_entry.auth = is_auth;
        lut_entry.cvid = vid;
        lut_entry.is_static = is_static;
        lut_entry.port = ulSpa;
        memcpy(lut_entry.mac.octet, pMac->octet, MAC_ADDR_LEN);
        lut_entry.sa_block = sa_block;

		if ((ret = rtk_l2_addr_add(&mac, &lut_entry)) != RT_ERR_OK)
			return ret;
	}

	return SAL_ERR_SUCCESS;
}

int32
sal_l2_entry_del(sal_mac_t *pMac, sal_vlan_t vid)
{
	int32 ret;
    rtk_mac_t mac;
    rtk_fid_t fid;
	rtk_l2_ucastAddr_t pL2_data;
	
	pL2_data.cvid=vid;
	pL2_data.ivl=1;
	pL2_data.fid=0;
	pL2_data.efid=0;
    
	if ((pMac->octet[0] & 0x1) == 0) /* L2 Unicast entry */
	{
	    memcpy(mac.octet, pMac->octet, MAC_ADDR_LEN);
        fid = vid;
		if ((ret = rtk_l2_addr_del(&mac, &pL2_data)) != SUCCESS)
			return ret;
	}
	else /* L2 Multicast entry */
	{
		return SAL_ERR_NOT_SUPPORT;
	}

	return SAL_ERR_SUCCESS;
}
  
int32
sal_l2_sramEntry_get (
    uint32 index,
    sal_mac_t  *pMac,
    sal_vlan_t *pVid,
    uint8   *pStatic,
    sal_l2_type_t   *pType,
    sal_pPortmask_t *pPortMask)
{
    rtk_l2_ucastAddr_t l2Param;

    memset(pPortMask, 0, sizeof(sal_pPortmask_t));
    
    if (index >= SAL_MAX_MAC_SRAM_ENTRY)
        return SAL_ERR_PARAMETER;

    if (SUCCESS != _sal_lutTbl_getByIdx(index, &l2Param))
        return SAL_ERR_MAC_ENTRY_INVALID;

    *pType = SAL_L2_UCAST;

    if (l2Param.mac.octet[0] == 0 && l2Param.mac.octet[1] == 0 && l2Param.mac.octet[2] == 0\
        && l2Param.mac.octet[3] == 0 && l2Param.mac.octet[4] == 0 && l2Param.mac.octet[5] == 0)
        return SAL_ERR_MAC_ENTRY_INVALID;
    
    // if (l2Param.sa_block == 1 || l2Param.da_block == 1)
    
    if (l2Param.sa_block == 1) 
    {
            pPortMask->bits[0] = 0;
            *pType = SAL_L2_MAC_FILTER;
    }
    else
    {
            pPortMask->bits[0] = ((uint32)0x1 << l2Param.port);
    }

    memcpy(pMac->octet, l2Param.mac.octet, MAC_ADDR_LEN);
    *pVid = l2Param.cvid;
    *pStatic = l2Param.is_static;
       
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_l2_addr_get
 * Description:
 *      Get a L2 entry from ASIC with adress being L2 unicast.
 * Input:
 *      pMac        -   MAC address
 *      vid         -    VLAN ID 
 * Output:
 *      pEntry      -   Pointer to the L2 entry.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_MAC_ENTRY_NOT_FOUND 
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
int32 sal_l2_addr_get (
    sal_mac_t           *pMac,
    sal_vlan_t          vid,
    sal_l2_ucastAddr_t  *pEntry
    )
{
    rtk_l2_ucastAddr_t l2entry;
    rtk_api_ret_t      ret;   

    l2entry.cvid=vid;
	l2entry.ivl=1;//MAC+VID
	l2entry.fid=0;
	l2entry.efid=0;
    if(SUCCESS != (ret = rtk_l2_addr_get((rtk_mac_t*)pMac, &l2entry)))
    {
        if(RT_ERR_L2_ENTRY_NOTFOUND == ret)
            return SAL_ERR_MAC_ENTRY_NOT_FOUND;        
        else
            return ret;
    }

    memcpy(pEntry->mac.octet, l2entry.mac.octet, SAL_ETHER_ADDR_LEN);
    pEntry->vid = l2entry.cvid;
    pEntry->isStatic  = (uint8)(l2entry.is_static & 0xFF);
    pEntry->srcPort = (sal_port_t)(l2entry.port & 0xFF);
    
    return SAL_ERR_SUCCESS;
}
#if 0 //NOT used
int32 sal_l2_entry_get(uint16 index, sal_l2_sttcEntry_t *pEntry)
{
    return sal_flash_l2_sttcEntry_get(index, (sal_flash_l2_sttcEntry_t*)pEntry);
}

/* Function Name:
 *      _sal_clear_entry
 * Description:
 *      Check whether there exists another entry with the same mac address and fid
 * Input:
 *      index - static mac entry index, range from 0 ~ SAL_MAX_STATIC_MAC - 1
 *      mac - mac address
 *      fid - filtering database id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_MAC_ENTRY_DUPLICATE
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
static int32 _sal_duplicated_entry_check(
    int16       index, 
    sal_mac_t*  mac, 
    uint16      fid
    )
{
    sal_flash_l2_sttcEntry_t	entry;
    int32   ret;
    uint16  i;	

    for (i = 0; i < SAL_MAX_STATIC_MAC; ++i)
    {
        if ((ret = sal_flash_l2_sttcEntry_get(i, &entry)) != SAL_ERR_SUCCESS)
            return ret;
        
        if ((i != index) && (memcmp(entry.mac.octet, mac, SAL_ETHER_ADDR_LEN) == 0) \
            && (entry.fid == fid))
 		{
            printf("duplicated entry!\n");
            return SAL_ERR_MAC_ENTRY_DUPLICATE;
  		}

    }
   
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      _sal_clear_entry
 * Description:
 *      Clear static mac entry. First check whether the static entry is valid. 
 *      If valid, delete it from entry and hardware. Otherwise, delete it from entry.
 * Input:
 *      index - static mac entry index, range from 0 ~ SAL_MAX_STATIC_MAC - 1
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
static int32 _sal_clear_entry(int16 index)
{
    sal_flash_l2_sttcEntry_t	entry;
    int32       ret;
    
    if (index > SAL_MAX_STATIC_MAC-1)
		return SAL_ERR_ENTRY_INDEX;

    if ((ret = sal_flash_l2_sttcEntry_get(index, &entry)) != SAL_ERR_SUCCESS)
        return ret;

    if (entry.valid == 1)
    {
        if ((ret = sal_l2_entry_del(&entry.mac, entry.fid)) != SAL_ERR_SUCCESS)
            return ret;
    }

    sal_flash_l2_sttcEntrySlot_del(index);

    return SAL_ERR_SUCCESS; 
}

/* Function Name:
 *      _sal_entry_set_validate
 * Description:
 *      Validate an mac entry. If pEntry equals NULL, load the indexed content
 *      into hardware, else save pEntry into the indexed entry and set the hardware.
 * Input:
 *      index - static mac entry index, range from 0 ~ SAL_MAX_STATIC_MAC-1
 *      pEntry - mac entry content
 * Output:
 *      No
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 */
static int32 _sal_entry_set_validate(int16 index, sal_flash_l2_sttcEntry_t *pEntry)
{
    int32 ret;
    sal_flash_l2_sttcEntry_t entry;
    sal_mac_t		mac;
	sal_vlan_t		fid;
	uint8		    auth;
	uint8		    sa_block;
	uint8		    da_block;
	sal_pPortmask_t portmask;

    if (pEntry == NULL)
    {
        if ((ret = sal_flash_l2_sttcEntry_get(index, &entry)) != SAL_ERR_SUCCESS)
            return ret;

        if (entry.mac.octet[0] == 0 && entry.mac.octet[1] == 0 && entry.mac.octet[2] == 0 && \
            entry.mac.octet[3] == 0 && entry.mac.octet[4] == 0 && entry.mac.octet[5] == 0)
            return SAL_ERR_MAC_ENTRY_INVALID;

        /* Write the hardware */
        memcpy(mac.octet, entry.mac.octet, SAL_ETHER_ADDR_LEN);
        fid = entry.fid;
        auth  = entry.auth;
        sa_block = entry.saBlock;
        da_block = entry.daBlock;
        memcpy(&portmask, &entry.pPortMask, sizeof(sal_pPortmask_t));

        if ((ret = sal_l2_entry_add(&mac, fid, TRUE, auth, sa_block, da_block, &portmask)) != SAL_ERR_SUCCESS)
            return ret;

        /* Valid the entry if it is an invalid entry */
    	if (entry.valid == 0)
        {
            entry.valid = 1;
    	    sal_flash_l2_sttcEntry_add(index, &entry);    	    
    	}	
    }
	else
	{
	    /* Check whether it will cause a duplicated one */
        if (_sal_duplicated_entry_check(index, &pEntry->mac, pEntry->fid) == SAL_ERR_MAC_ENTRY_DUPLICATE)
            return SAL_ERR_MAC_ENTRY_DUPLICATE; 

        if ((ret = _sal_clear_entry(index)) != SAL_ERR_SUCCESS) 
    	  	return ret;

    	/* Write the hardware */
    	memcpy(mac.octet, pEntry->mac.octet, SAL_ETHER_ADDR_LEN);
        fid = pEntry->fid;
        auth  = pEntry->auth;
        sa_block = pEntry->saBlock;
        da_block = pEntry->daBlock;
        memcpy(&portmask, &pEntry->pPortMask, sizeof(sal_pPortmask_t));

        if ((ret = sal_l2_entry_add(&mac, fid, TRUE, auth, sa_block, da_block, &portmask)) != SAL_ERR_SUCCESS)
            return ret;

        /* Write the flash */
        pEntry->valid = 1;
        sal_flash_l2_sttcEntry_add(index, pEntry);
	}
    	    	
    return SAL_ERR_SUCCESS; 
}

/* Function Name:
 *      _sal_entry_set_keep
 * Description:
 *      If the entry is valid, save pEntry into the indexed entry and set the hardware.
 *      Otherwise, save pEntry into the indexed entry and don't set the hardware.
 * Input:
 *      index - static vlan entry index, range from 0 ~ SAL_MAX_STATIC_MAC-1
 *      pEntry - vlan entry content
 * Output:
 *      No
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 */
static int32 _sal_entry_set_keep(int16 index, sal_flash_l2_sttcEntry_t *pEntry)
{
    int32 ret;
    sal_flash_l2_sttcEntry_t entry;

    if (_sal_duplicated_entry_check(index, &pEntry->mac, pEntry->fid) == SAL_ERR_MAC_ENTRY_DUPLICATE)
            return SAL_ERR_MAC_ENTRY_DUPLICATE; 

    if ((ret = sal_flash_l2_sttcEntry_get(index, &entry)) != SAL_ERR_SUCCESS)
        return ret;

    if (entry.valid == 1)
    {
        _sal_entry_set_validate(index, pEntry);
    }
    else
    {   
  	  	/* Update Entry */
  	  	pEntry->valid = 0; /* Keep valid status*/
        sal_flash_l2_sttcEntry_add(index, pEntry);
    }
	    	
    return SAL_ERR_SUCCESS;        
}

int32 sal_l2_entry_set(uint16 index, sal_l2_sttcEntry_t *pEntry, sal_entry_action_t action) 
{
    int32 ret;

    if (index >= SAL_MAX_STATIC_MAC) 
		return SAL_ERR_ENTRY_INDEX;
	
    switch(action)
    {
        case SAL_ENTRY_ACT_VALID:
            if ((ret = _sal_entry_set_validate(index, (sal_flash_l2_sttcEntry_t*)pEntry)) != SAL_ERR_SUCCESS)
                return ret;                
            break;
        case SAL_ENTRY_ACT_INVALID:
            if ((ret = _sal_clear_entry(index)) != SAL_ERR_SUCCESS)
                return ret;
            break;
        case SAL_ENTRY_ACT_KEEP:
            if ((ret = _sal_entry_set_keep(index, (sal_flash_l2_sttcEntry_t*)pEntry)) != SAL_ERR_SUCCESS)
                return ret;  
            break;
        default:
            return SAL_ERR_PARAMETER;
    }
        
    return SAL_ERR_SUCCESS;
}
#endif

int32 sal_l2_limitLearningCnt_set(sal_port_t port, uint32 mac_cnt)
{
    int32          ret;
	uint8          portId;
	sal_pPortmask_t	portmask;
    rtk_mac_cnt_t   mac_limit;

    sal_trunk_logicId_toPhysicMask(&portmask, port);
    mac_limit = mac_cnt;
    
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portmask, portId))
        {
            if ((ret = rtk_l2_limitLearningCnt_set(portId, mac_limit)) != RT_ERR_OK)
                return ret;
        }
    }

    return sal_flash_l2_mac_constraint_set(port, mac_cnt);
}

int32 sal_l2_limitLearningCnt_get(sal_port_t port, uint32 *pMac_cnt)
{
    int32          ret;
	uint8          portId;
    rtk_mac_cnt_t  mac_limit;

    if (pMac_cnt == NULL)
        return SAL_ERR_NULL_POINTER;
            
	sal_trunk_logicId_toPhysicId(&portId, port);

    *pMac_cnt = 0;
	if ((ret = rtk_l2_limitLearningCnt_get(portId, &mac_limit)) != RT_ERR_OK)
        return ret;

    *pMac_cnt = mac_limit;

	return SAL_ERR_SUCCESS;
}

#if 0//NOT used
/* Function Name:
 *      sal_l2_learnedEntry_flush
 * Description:
 *      Flush leanred entry of a port
 * Input:
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
int32 sal_l2_learnedEntry_flush(sal_port_t port)
{
    if(port >= SAL_MAX_PHY_PORT)
        return SAL_ERR_PARAMETER;

    rtl8367c_setAsicLutForceFlush(1UL << port); 
    
    return SAL_ERR_SUCCESS;
}
#endif

#if 1 /* reserved for future use */
int32 sal_l2_limitLearningCntAction_set(sal_port_t port, sal_l2_limitLearnCntAction_t action)
{
    int32          ret;
    rtk_l2_limitLearnCntAction_t act;

    switch (action)
    {
    case SAL_LIMIT_LEARN_CNT_ACTION_DROP:
        act = LIMIT_LEARN_CNT_ACTION_DROP;
        break;
    case SAL_LIMIT_LEARN_CNT_ACTION_FORWARD:
        act = LIMIT_LEARN_CNT_ACTION_FORWARD;
        break;
    default:
        return SAL_ERR_PARAMETER;
    }
    
    if ((ret = rtk_l2_limitLearningCntAction_set(RTK_WHOLE_SYSTEM, act)) != RT_ERR_OK)
        return ret;
     
    /* Remove warning */
    port = port;

	return sal_flash_l2_mac_constraint_act_set(act);
}

int32 sal_l2_limitLearningCntAction_get(sal_port_t port, sal_l2_limitLearnCntAction_t *pAction)
{
    int32          ret;
    rtk_l2_limitLearnCntAction_t act;

    if (pAction == NULL)
        return SAL_ERR_NULL_POINTER;
            
    *pAction = 0;
	if ((ret = rtk_l2_limitLearningCntAction_get(RTK_WHOLE_SYSTEM, &act)) != RT_ERR_OK)
        return ret;

    switch (act)
    {
    case LIMIT_LEARN_CNT_ACTION_DROP:
        *pAction = SAL_LIMIT_LEARN_CNT_ACTION_DROP;
        break;
    case LIMIT_LEARN_CNT_ACTION_FORWARD:
        *pAction = SAL_LIMIT_LEARN_CNT_ACTION_FORWARD;
        break;
    default:
        return SAL_ERR_FAILED;
    }

    /* Remove warning */
    port = port;

	return SAL_ERR_SUCCESS;
}
#endif

#endif
