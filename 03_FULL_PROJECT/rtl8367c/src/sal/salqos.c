#pragma STRING (far)

#include <stdio.h>
#include <string.h>

#include <salqos.h>
#include <salerror.h>
#include <salflash.h>
#include <saltrunk.h>
#include <salportmask.h>
#include <qos.h>
#include <rtk_error.h>
#include <rtl8367c_asicdrv.h>

// Sal QoS Config Restore: Restores all QoS settings from flash configuration
// Restores queue number, port priority, scheduling weights, DSCP remarking, and priority mapping
int32 sal_qos_config_restore(void)
{		
    uint8          portId, pPortId;
    uint8          index;
    uint8          qNum;
#if SAL_DSCP_REMARKING
    uint8          qid, dscp, enable;
#endif
    sal_lPortmask_t    lPortMask;
    sal_qos_queue_weights_t qweights;
    sal_flash_qos_port_table_t qosPortTbl;    
#if defined(INTERNAL_PRIORITY)
    sal_flash_qos_pri_selet_t priSel;
    sal_dscp_t dscp;
    sal_pri_t int_pri;
    sal_flash_qos_pri2queue_t pri2queue;
#endif
	sal_flash_qos_queueNum_get(0, &qNum);
    if (qNum > SAL_MAX_NUM_OF_QUEUE)
        qNum = SAL_MAX_NUM_OF_QUEUE;

    _sal_flash_qos_portTable_get(&qosPortTbl);
	sal_trunk_logicPort_setAll(&lPortMask); /* Normal port 0~23 */
    
    for (portId = 0; portId < SAL_PORT_ID_CPU; portId++)
    {
		if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&lPortMask, portId))
			continue;
			
        sal_trunk_logicId_toPhysicId(&pPortId, portId);

        sal_qos_queueNum_set(portId, qNum);
        sal_qos_portPri_set(portId, qosPortTbl.qosPort[pPortId].pri);

        for (index = 0; index < qNum; ++index) 
            qweights.weights[index] = qosPortTbl.qosPort[pPortId].weight[index];
        sal_qos_schedulingQueue_set(portId, &qweights);
    }

#if SAL_DSCP_REMARKING
    for(qid = 0; qid < SAL_MAX_NUM_OF_QUEUE; qid++)
    {
        sal_flash_qos_dscpRemarking_get(qid, &dscp, &enable);
        sal_qos_dscpRemarking_set(qid, dscp);
    }
    sal_qos_dscpRemarkingEnable_set(enable);
#endif    
    
#if defined(INTERNAL_PRIORITY)
    sal_flash_qos_priSel_get(&priSel);
    sal_qos_priSel_set((sal_qos_pri_selet_t*)&priSel);

    for(dscp = 0; dscp < SAL_MAX_VALUE_OF_DSCP; dscp++)
    {
        sal_flash_qos_dscpPriRemap_get(dscp, &int_pri);
        sal_qos_dscpPriRemap_set(dscp, int_pri);
    }

    sal_qos_queueNum_get(0, &qNum);
    sal_flash_qos_priMap_get(&pri2queue);
    sal_qos_priMap_set(qNum, (sal_qos_pri2queue_t*)&pri2queue);
#endif
    
    return SAL_ERR_SUCCESS;
}

// Sal QoS Port Pri Set: Sets the internal priority for a port
// Configures the default priority for packets received on the specified port
int32
sal_qos_portPri_set(sal_port_t port, sal_pri_t intPri)
{
	uint8          portId;
#if defined(LWS_8G_2F) || defined(LWS_5G_1F)
    uint8          portIdLogic;
#endif
	int32          ret;
	sal_pPortmask_t	pPortMask;
	rtk_pri_t       rtk_pri;

	sal_trunk_logicId_toPhysicMask(&pPortMask, port);
    rtk_pri = intPri;
    
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId) 
    {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, portId)) 
        {
            if ((ret = rtk_qos_portPri_set(portId, rtk_pri)) != RT_ERR_OK)
                return ret;    		
                
#if defined(LWS_8G_2F)
        if(portId == EXT_PORT0)
            portIdLogic = 8;
        else if(portId == EXT_PORT1)
            portIdLogic = 9;
        else
            portIdLogic = portId;
            
        rtl8367c_setAsicRegBits(0x0851+portIdLogic/4, 0x7 << ((portIdLogic%4)*4), intPri);
#elif defined(LWS_5G_1F)
        if(portId == EXT_PORT0)
            portIdLogic = 6;
        else
            portIdLogic = portId;
            
        rtl8367c_setAsicRegBits(0x0851+portIdLogic/4, 0x7 << ((portIdLogic%4)*4), intPri);
#else
        rtl8367c_setAsicRegBits(0x0851+portId/4, 0x7 << ((portId%4)*4), intPri);
#endif
		}
	}

    return sal_flash_qos_portPri_set(port, intPri);
}

// Sal QoS Port Pri Get: Retrieves the internal priority for a port
// Gets the default priority configured for the specified port
int32 
sal_qos_portPri_get(sal_port_t port, sal_pri_t *pIntPri)
{
    int32      ret;
	uint8      portId;
	rtk_pri_t  pri;

    sal_trunk_logicId_toPhysicId(&portId, port);
    	
    if ((ret = rtk_qos_portPri_get(portId, &pri)) != RT_ERR_OK)
        return ret;    
    			
    *pIntPri = pri;
    
	return SAL_ERR_SUCCESS;
}

// Sal QoS Pri Map Get: Retrieves the priority to queue mapping table
// Gets the mapping of internal priorities to queue IDs
int32
sal_qos_priMap_get(uint8 qNum, sal_qos_pri2queue_t *pPri2qid)
{
    int32 ret;
    rtk_qos_pri2queue_t map;

    if (RTK_MAX_NUM_OF_PRIORITY != SAL_MAX_NUM_OF_PRIORITY)
        printf("Macro RTK_MAX_NUM_OF_PRIORITY != SAL_MAX_NUM_OF_PRIORITY");
        
    if ((ret = rtk_qos_priMap_get(qNum, &map)) != SAL_ERR_SUCCESS)
        return ret;
    
    memcpy(pPri2qid, &map, sizeof(rtk_qos_pri2queue_t));        
    
    return SAL_ERR_SUCCESS;
}

// Sal QoS Queue Num Set: Sets the number of queues for a port
// Configures how many transmit queues are available on the specified port
int32
sal_qos_queueNum_set(sal_port_t port, uint8 qNum)
{
	uint8          portId;
	int32          ret;
	sal_pPortmask_t	pPortMask;

	sal_trunk_logicId_toPhysicMask(&pPortMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId) 
    {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, portId)) 
        {
			ret = rtk_qos_queueNum_set(portId, qNum);
            SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);
		}
	}
    return sal_flash_qos_queueNum_set(port, qNum);
}

// Sal QoS Queue Num Get: Retrieves the number of queues for a port
// Gets the queue count configured for the specified port
int32
sal_qos_queueNum_get(sal_port_t port, uint8 *pQnum)
{
    int32      ret;
	uint8      portId;
	rtk_queue_num_t queue_num;

    sal_trunk_logicId_toPhysicId(&portId, port);

    if ((ret = rtk_qos_queueNum_get(portId, &queue_num)) != RT_ERR_OK)
        return ret;
        
    *pQnum = queue_num;

	return SAL_ERR_SUCCESS;
}

// Sal QoS Scheduling Queue Set: Configures queue scheduling weights for a port
// Sets the WRR (Weighted Round Robin) weights for each queue
int32
sal_qos_schedulingQueue_set(sal_port_t port, sal_qos_queue_weights_t *pQweights)
{
	uint8          portId, index;
    int32          ret;
	sal_pPortmask_t	pPortMask;
	rtk_qos_queue_weights_t weight;
	
    sal_trunk_logicId_toPhysicMask(&pPortMask, port);

    if (RTK_MAX_NUM_OF_QUEUE < SAL_MAX_NUM_OF_QUEUE)
    {
        printf("RTK_MAX_NUM_OF_QUEUE < SAL_MAX_NUM_OF_QUEUE");
        return SAL_ERR_FAILED;
    }     

    memset(&weight, 0, sizeof(rtk_qos_queue_weights_t));
    for (index = 0; index < SAL_MAX_NUM_OF_QUEUE; index++)
    {
        weight.weights[index] = pQweights->weights[index];
        
    }
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId) 
    {
		if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&pPortMask, portId)) 
        {
            if ((ret = rtk_qos_schedulingQueue_set(portId, &weight)) != RT_ERR_OK)
                return ret;
		}
	}
    
    return sal_flash_qos_schedulingQueue_set(port, pQweights);                               
}

// Sal QoS Scheduling Queue Get: Retrieves queue scheduling weights for a port
// Gets the WRR weights configured for each queue on the specified port
int32
sal_qos_schedulingQueue_get(sal_port_t port, sal_qos_queue_weights_t *pQweights)
{
    int32      ret;
	uint8      portId, index;
	rtk_qos_queue_weights_t weight;

    if (RTK_MAX_NUM_OF_QUEUE < SAL_MAX_NUM_OF_QUEUE)
    {
        printf("RTK_MAX_NUM_OF_QUEUE < SAL_MAX_NUM_OF_QUEUE");
        return SAL_ERR_FAILED;
    } 
    
    sal_trunk_logicId_toPhysicId(&portId, port);
    if ((ret = rtk_qos_schedulingQueue_get(portId, &weight)) != RT_ERR_OK)
        return ret;

    memset(pQweights, 0, sizeof(sal_qos_queue_weights_t));
    for (index = 0; index < SAL_MAX_NUM_OF_QUEUE; index++)
    {
        pQweights->weights[index] = weight.weights[index];		
    }
    
	return SAL_ERR_SUCCESS;
}

#if 0
int32 sal_qos_port2queue_set(
    uint8 port, 
    uint8 queue   // port -> priority -> queue 
)
{
    int32 ret;
    uint8 ucQNum;
    rtk_qos_pri2queue_t map;
    uint8 i;

    //printf("Setting Port ID: %bx, queue: %bx!\n", port, queue);

    sal_qos_queueNum_get(0, &ucQNum);
    if ((ret = rtk_qos_priMap_get(ucQNum, &map)) != RT_ERR_OK)
        return ret;
    
    for (i = 0; i < RTK_MAX_NUM_OF_PRIORITY; i++)
    {
        //printf("Mapping Table: pri: %bd ==> queue: %bd\n", (uint8)i, (uint8)map.pri2queue[i]);
        if (map.pri2queue[i] == queue)
        {
            //printf("Setting Port ID: %bx, priority: %bx\n", port, i);
            if ((ret = sal_qos_portPri_set(port, i)) != SAL_ERR_SUCCESS)
                return ret;
            
            break;
        }
    }

    return SAL_ERR_SUCCESS;    
}
#endif

#if SAL_DSCP_REMARKING
/* Function Name:
 *      sal_qos_dscpRemarking_set
 * Description:
 *      Set queue id to DSCP remarking mapping
 * Input:
 *      qid  - queue id
 *      dscp - remarked DSCP value 
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
// Sal QoS DSCP Remarking Set: Configures DSCP remarking for a queue
// Sets the DSCP value to be used when remarking packets from the specified queue
int32 sal_qos_dscpRemarking_set(uint8 qid, uint8 dscp)
{
    rtk_api_ret_t ret;
    uint8         tmp, dscp_enable;

    if(qid >= SAL_MAX_NUM_OF_QUEUE || dscp > SAL_MAX_VALUE_OF_DSCP)
        return SAL_ERR_PARAMETER;

    ret = rtk_qos_dscpRemark_set(qid, dscp);
    if(RT_ERR_OK != ret)
        return ret;

    /* get dscp enable then write back */
    sal_flash_qos_dscpRemarking_get(0, &tmp, &dscp_enable);

    return sal_flash_qos_dscpRemarking_set(qid, dscp, dscp_enable);
}


/* Function Name:
 *      sal_qos_dscpRemarking_get
 * Description:
 *      Get queue id to DSCP remarking mapping
 * Input:
 *      qid  - queue id
 * Output:
 *      pDscp - pointer to remarked DSCP value 
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
// Sal QoS DSCP Remarking Get: Retrieves DSCP remarking value for a queue
// Gets the DSCP value configured for remarking packets from the specified queue
int32 sal_qos_dscpRemarking_get(uint8 qid, uint8 *pDscp)
{
    rtk_dscp_t    dscp;
    rtk_api_ret_t ret;

    if(qid >= SAL_MAX_NUM_OF_QUEUE)
        return SAL_ERR_PARAMETER;

    ret = rtk_qos_dscpRemark_get(qid, &dscp);
    if(ret)
        return ret;

    *pDscp = dscp & 0xFF;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_qos_dscpRemarkingEnable_set
 * Description:
 *      Enable/Disable DSCP Remarking function
 * Input:
 *      enable - enable/disable DSCP Remarking
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
// Sal QoS DSCP Remarking Enable Set: Enables or disables DSCP remarking globally
// Toggles the DSCP remarking feature for the entire system
int32 sal_qos_dscpRemarkingEnable_set(sal_enable_t enable)
{
    rtk_api_ret_t ret;
    uint8         tmp1, tmp2;

    if(enable >= SAL_ENABLE_END)
        return SAL_ERR_PARAMETER;

    ret = rtk_qos_dscpRemarkEnable_set(RTK_WHOLE_SYSTEM, enable);
    if(ret)
        return ret;

    sal_flash_qos_dscpRemarking_get(0, &tmp1, &tmp2);

    return sal_flash_qos_dscpRemarking_set(0, tmp1, enable);
}

/* Function Name:
 *      sal_qos_dscpRemarkingEnable_get
 * Description:
 *      Get DSCP Remarking state
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer to state of DSCP Remarking state
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
// Sal QoS DSCP Remarking Enable Get: Retrieves DSCP remarking enable status
// Gets whether DSCP remarking is globally enabled or disabled
int32 sal_qos_dscpRemarkingEnable_get(sal_enable_t *pEnable)
{
    rtk_enable_t  enable;
    rtk_api_ret_t ret;
    
    ret = rtk_qos_dscpRemarkEnable_get(RTK_WHOLE_SYSTEM, &enable);
	if(ret)
        return ret;
    
    *pEnable = enable & 0xFF;
    return SAL_ERR_SUCCESS;
    
}

#endif

/* Function Name:
 *      sal_qos_priSel_set
 * Description:
 *      set QoS internal priority selection.
 * Input:
 *      pPriDec - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_OK              - OK
 *      SAL_ERR_FAILED          - FAILED
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame. 
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to 
 *      assign queue priority to receiving frame.  
 *      The priority sources are:
 *      PRIDEC_PORT
 *      PRIDEC_ACL
 *      PRIDEC_DSCP
 *      PRIDEC_1Q
 *      PRIDEC_1AD
 *      PRIDEC_CVLAN
 *      PRIDEC_DA
 *      PRIDEC_SA 
 */
// Sal QoS Pri Sel Set: Sets priority selection order for QoS
// Configures which priority sources (port, DSCP, 802.1p, etc.) take precedence
int32 sal_qos_priSel_set(sal_qos_pri_selet_t *pPriDec)
{
    int32 ret;
    rtk_priority_select_t priDec;
    
    priDec.port_pri = pPriDec->port_pri;
    priDec.dot1q_pri = pPriDec->dot1q_pri;
    priDec.acl_pri = pPriDec->acl_pri;
    priDec.dscp_pri = pPriDec->dscp_pri;
    priDec.cvlan_pri = pPriDec->cvlan_pri;
    priDec.svlan_pri = pPriDec->svlan_pri;
    priDec.dmac_pri = pPriDec->dmac_pri;
    priDec.smac_pri = pPriDec->smac_pri;

    if (ret = rtk_qos_priSel_set(PRIDECTBL_IDX0, &priDec) != RT_ERR_OK)
        return ret;
#if defined(INTERNAL_PRIORITY)
    return sal_flash_qos_priSel_set((sal_flash_qos_pri_selet_t*)pPriDec);
#else
    return SAL_ERR_SUCCESS;
#endif
}

/* Function Name:
 *      sal_qos_priSel_get
 * Description:
 *      Get the priority order configuration among different priority mechanism.
 * Input:
 *      None
 * Output:
 *      pPriDec - Priority assign for port, dscp, 802.1p, cvlan, svlan, acl based priority decision .
 * Return:
 *      SAL_ERR_OK              - OK
 *      SAL_ERR_FAILED          - FAILED
 * Note:
 *      ASIC will follow user priority setting of mechanisms to select mapped queue priority for receiving frame. 
 *      If two priority mechanisms are the same, the ASIC will chose the highest priority from mechanisms to 
 *      assign queue priority to receiving frame. 
 *      The priority sources are:
 *      PRIDEC_PORT,
 *      PRIDEC_ACL,
 *      PRIDEC_DSCP,
 *      PRIDEC_1Q,
 *      PRIDEC_1AD,
 *      PRIDEC_CVLAN,
 *      PRIDEC_DA,
 *      PRIDEC_SA,
 */
// Sal QoS Pri Sel Get: Retrieves the priority selection order
// Gets the current configuration of which priority sources are enabled
int32 sal_qos_priSel_get(sal_qos_pri_selet_t *pPriDec)
{
    int32 ret;
    rtk_priority_select_t priDec;

    if ((ret = rtk_qos_priSel_get(PRIDECTBL_IDX0, &priDec)) != RT_ERR_OK)
        return ret;
    pPriDec->port_pri = priDec.port_pri;
    pPriDec->dot1q_pri = priDec.dot1q_pri;
    pPriDec->acl_pri = priDec.acl_pri;
    pPriDec->dscp_pri = priDec.dscp_pri;
    pPriDec->cvlan_pri = priDec.cvlan_pri & 0xffff;
    pPriDec->svlan_pri = priDec.svlan_pri;
    pPriDec->dmac_pri = priDec.dmac_pri;
    pPriDec->smac_pri = priDec.smac_pri;
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_qos_dscpPriRemap_set
 * Description:
 *      Map dscp value to internal priority.
 * Input:
 *      dscp - Dscp value of receiving frame
 *      int_pri - internal priority value .
 * Output:
 *      None
 * Return:
 *      SAL_ERR_OK              - OK
 *      SAL_ERR_FAILED          - FAILED
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically 
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of 
 *      DSCP are carefully chosen then backward compatibility can be achieved.    
 */
// Sal QoS DSCP Pri Remap Set: Maps DSCP value to internal priority
// Configures how incoming DSCP values are translated to internal priority levels
int32 sal_qos_dscpPriRemap_set(sal_dscp_t dscp, sal_pri_t int_pri)
{
    int32 ret;

    if ((ret = rtk_qos_dscpPriRemap_set(dscp,int_pri))!=RT_ERR_OK)
        return ret;
#if defined(INTERNAL_PRIORITY)
    return sal_flash_qos_dscpPriRemap_set(dscp, int_pri);
#else
    return SAL_ERR_SUCCESS;
#endif
}

/* Function Name:
 *      sal_qos_dscpPriRemap_get
 * Description:
 *      Get dscp value to internal priority.
 * Input:
 *      dscp - Dscp value of receiving frame
 * Output:
 *      pInt_pri - internal priority value. 
 * Return:
 *      SAL_ERR_OK              - OK
 *      SAL_ERR_FAILED          - FAILED
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviors. As a selector, there is no implication that a numerically 
 *      greater DSCP implies a better network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS. So if values of 
 *      DSCP are carefully chosen then backward compatibility can be achieved.    
 */
// Sal QoS DSCP Pri Remap Get: Retrieves DSCP to internal priority mapping
// Gets the internal priority value mapped from the specified DSCP
int32 sal_qos_dscpPriRemap_get(sal_dscp_t dscp, sal_pri_t *pInt_pri)
{
    rtk_api_ret_t ret;
    rtk_pri_t int_pri;

    if ((ret = rtk_qos_dscpPriRemap_get((rtk_dscp_t)dscp, &int_pri)!=RT_ERR_OK))
        return ret;
    *pInt_pri = int_pri & 0xff;
    
    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_qos_priMap_set
 * Description:
 *      Set internal priority to queue id based on different queue number.
 * Input:
 *      qNum - Queue number usage.
 *      pPri2qid - Priority mapping to queue ID.
 * Output:
 *      None 
 * Return:
 *      SAL_ERR_OK              - OK
 *      SAL_ERR_FAILED          - FAILED
 * Note:
 *      ASIC supports priority mapping to queue with different queue number from 1 to 8.
 *      For different queue numbers usage, ASIC supports different internal available queue IDs.
 */
// Sal QoS Pri Map Set: Sets the priority to queue mapping table
// Configures how internal priorities are mapped to hardware queues
int32 sal_qos_priMap_set(uint8 qNum, sal_qos_pri2queue_t *pPri2qid)
{
    rtk_api_ret_t ret;

    if ((ret = rtk_qos_priMap_set(qNum, pPri2qid)!=RT_ERR_OK))
        return ret;
        
#if defined(INTERNAL_PRIORITY)
    return sal_flash_qos_priMap_set(pPri2qid); //only save current use queue number to FLASH
#else
    return SAL_ERR_SUCCESS;
#endif
    
}