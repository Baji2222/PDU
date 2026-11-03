/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 7005 $
 * $Date: 2009-11-13 15:15:59 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : 1P priority/Priority Selection/Scheduling
 *
 */
#ifndef __SAL_QOS_H__
#define __SAL_QOS_H__

#include <saltype.h>

/*
 * Symbol Definition
 */

/* Priority selection structure */
typedef struct sal_qos_pri_selet_e
{
    uint32 port_pri;
    uint32 dot1q_pri;
    uint32 acl_pri;
    uint32 dscp_pri;
    uint16 cvlan_pri;
    uint32 svlan_pri;
    uint32 dmac_pri;
    uint32 smac_pri;
} sal_qos_pri_selet_t;

/* Priority to queue mapping structure */
typedef struct sal_qos_pri2queue_s
{
    uint32 pri2queue[SAL_MAX_NUM_OF_PRIORITY];
} sal_qos_pri2queue_t;

/* Types of queue scheduling algorithm */
typedef enum sal_qos_scheduling_type_e
{
    SAL_WFQ = 0,    /* Weighted-Fair-Queue */
    SAL_WRR,        /* Weighted-Round-Robin */
    SAL_SCHEDULING_TYPE_END
} sal_qos_scheduling_type_t;

/* Queue weights structure */
typedef struct sal_qos_queue_weights_s
{
    uint32 weights[SAL_MAX_NUM_OF_QUEUE];
} sal_qos_queue_weights_t;

/* Types of trap to CPU */
typedef enum sal_qos_trap_pri_e
{
    QOS_TRAP_PRIORITY_RMA = 0,
    QOS_TRAP_PRIORITY_IPV4_IGMP,
    QOS_TRAP_PRIORITY_IPV6_MLD,
    QOS_TRAP_PRIORITY_EAPOL,
    QOS_TRAP_PRIORITY_VLAN_ERROR,
    QOS_TRAP_PRIORITY_SLP_CHANGE,
    QOS_TRAP_PRIORITY_MCAST_DLF,
    QOS_TRAP_PRIORITY_CFI,
    QOS_TRAP_PRIORITY_UNAUTH,
    QOS_TRAP_PRIORITY_MAX
} sal_qos_trap_pri_t;

/* Types of queue ID */
typedef enum sal_qos_queue_id_e
{
	QOS_QUEUE_0 = 0,
	QOS_QUEUE_1,
	QOS_QUEUE_2,
	QOS_QUEUE_3,
	QOS_QUEUE_MAX
} sal_qos_queue_id_t;



/* Function Name:
 *      sal_qos_config_restore
 * Description:
 *      Restore QoS configurations from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_qos_config_restore(void);


/* Function Name:
 *      sal_qos_portPri_set
 * Description:
 *      Set internal priority of one port.
 * Input:
 *      port    - port id
 *      intPri  - priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */ 
extern int32 sal_qos_portPri_set(sal_port_t port, sal_pri_t intPri);


/* Function Name:
 *      sal_qos_portPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port                - port id
 * Output:
 *      pIntPri             - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *		None
 */
extern int32 sal_qos_portPri_get(sal_port_t port, sal_pri_t *pIntPri);


/* Function Name:
 *      sal_qos_1pPriRemap_set
 * Description:
 *      Set a internal priority for 802.1p priority to re-map.
 * Input:
 *      dot1pPri    - 802.1p priority value (range from 0 ~ 7)
 *      intPri      - internal priority value (range from 0 ~ 7)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_1pPriRemap_set(sal_pri_t dot1pPri, sal_pri_t intPri);


/* Function Name:
 *      sal_qos_1pPriRemap_get
 * Description:
 *      Get the internal priority that 802.1p priority re-map to.
 * Input:
 *      dot1pPri    - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      pIntPri     - internal priority value (range from 0 ~ 7)
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_1pPriRemap_get(sal_pri_t dot1pPri, sal_pri_t *pIntPri);


/* Function Name:
 *      sal_qos_dscpPriRemap_set
 * Description:
 *      Set a internal priority for DSCP value to re-map.
 * Input:
 *      dscp    - DSCP value of receiving frame (0~63)
 *      intPri  - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_dscpPriRemap_set(sal_dscp_t dscp, sal_pri_t intPri);


/* Function Name:
 *      sal_qos_dscpPriRemap_get
 * Description:
 *      Get the internal priority that DSCP value re-map to.
 * Input:
 *      dscp        - DSCP value of receiving frame (0~63)
 * Output:
 *      pIntPri     - priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                            the highest prioirty)
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_dscpPriRemap_get(sal_dscp_t dscp, sal_pri_t *pIntPri);


/* Function Name:
 *      sal_qos_priSel_set
 * Description:
 *      Set the priority among different priority mechanism.
 * Input:
 *      pPriDec    - priority assign for each priority-based selection
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_priSel_set(sal_qos_pri_selet_t *pPriDec);


/* Function Name:
 *      sal_ qos_priSel_get
 * Description:
 *      Get the priority among different priority mechanism.
 * Input:
 *      None
 * Output:
 *      pPriDec    - priority value of each priority-based selection
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_priSel_get(sal_qos_pri_selet_t *pPriDec); 


/* Function Name:
 *      sal_qos_priMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      qNum        - the number of queue (1~8).
 *      pPri2qid    - array of internal priority on a queue
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
*/ 
extern int32 sal_qos_priMap_set(uint8 qNum, sal_qos_pri2queue_t *pPri2qid);                                               


/* Function Name:
 *      sal_qos_priMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      qNum        - the number of queue (1~8).
 * Output:
 *     	pPri2qid    - array of internal priority on a queue
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */ 
extern int32 sal_qos_priMap_get(uint8 qNum, sal_qos_pri2queue_t *pPri2qid);                                              


/* Function Name:
 *      sal_qos_queueNum_set
 * Description:
 *      Set the number of queue for specified port.
 * Input:
 *      port        - port id
 *      qNum        - the number of queue (1~8).
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PORT_RANGE
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_queueNum_set(sal_port_t port, uint8 qNum);                                                           


/* Function Name:
 *      sal_qos_queueNum_get
 * Description:
 *      Get the number of queue for specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pQnum   - the number of queue (1~8).
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_qos_queueNum_get(sal_port_t port, uint8 *pQnum);    


/* Function Name:
 *      sal_qos_schedulingAlgo_set
 * Description:
 *      Set the scheduling algorithm of the port.
 * Input:
 *      port            - port id
 *      scheduling_type - type of scheduling algorithm.
 * Output:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PORT_RANGE 
 *      SAL_ERR_PARAMETER
 * Return:
 *      None
* Note:
 *    None
 */
extern int32 sal_qos_schedulingAlgo_set(sal_port_t port, sal_qos_scheduling_type_t scheduling_type);                                                     


/* Function Name:
 *      sal_qos_schedulingAlgo_get
 * Description:
 *      Get the scheduling algorithm of the port.
 * Input:
 *      port                - port id
 * Output:
 *      pScheduling_type    - type of scheduling algorithm.
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *		None
*/
extern int32 sal_qos_schedulingAlgo_get(sal_port_t port, sal_qos_scheduling_type_t *pScheduling_type);                                                   


/* Function Name:
 *      sal_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      port        - port id
 *      pQweights   - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PORT_RANGE 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
extern int32 sal_qos_schedulingQueue_set(sal_port_t port, sal_qos_queue_weights_t *pQweights);                               


/* Function Name:
 *      sal_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      port        - port id
 * Output:
 *      pQweights   - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
extern int32 sal_qos_schedulingQueue_get(sal_port_t port, sal_qos_queue_weights_t *pQweights); 


/* Function Name:
 *      sal_qos_trapCpuPri_set
 * Description:
 *      Set the priority value of specific trap to CPU reason.
 * Input:
 *      type    - the type of 
 *      pri     - priority value (range from 0 ~ 7)
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
extern int32 sal_qos_trapCpuPri_set(sal_qos_trap_pri_t type, sal_pri_t pri);                         


/* Function Name:
 *      sal_qos_trapCpuPri_get
 * Description:
 *      Get the priority value of specific trap to CPU reason.
 * Input:
 *      type    - the type of 
 * Output:
 *      pPri    - priority value (range from 0 ~ 7)
 * Return:
 *      SAL_ERR_SUCCESS 
 *      SAL_ERR_PARAMETER
 * Note:
 *    None
 */
extern int32 sal_qos_trapCpuPri_get(sal_qos_trap_pri_t type, sal_pri_t *pPri);

/* Function Name:
 *      sal_qos_port2queue_set
 * Description:
 *      Set port to queue mapping 
 * Input:
 *      ucPortId - port id
 *      ucQid    - queue id
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS 
 * Note:
 *    None
 */
extern int32 sal_qos_port2queue_set(uint8 ucPortId, uint8 ucQid);

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
extern int32 sal_qos_dscpRemarking_set(uint8 qid, uint8 dscp);

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
extern int32 sal_qos_dscpRemarking_get(uint8 qid, uint8 *pDscp);

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
extern int32 sal_qos_dscpRemarkingEnable_set(uint8 enable);

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
extern int32 sal_qos_dscpRemarkingEnable_get(uint8 *pEnable);

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
extern int32 sal_qos_priSel_set(sal_qos_pri_selet_t *pPriDec);

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
extern int32 sal_qos_priSel_get(sal_qos_pri_selet_t *pPriDec);

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
extern int32 sal_qos_dscpPriRemap_set(sal_dscp_t dscp, sal_pri_t int_pri);

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
extern int32 sal_qos_dscpPriRemap_get(sal_dscp_t dscp, sal_pri_t *pInt_pri);


#endif /* end of __SAL_QOS_H__ */

