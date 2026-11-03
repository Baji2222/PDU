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
 * Feature : Realtek Remote Control Protocol Type II
 *
 */
#ifndef __SAL_RRCPV3_H__
#define __SAL_RRCPV3_H__

typedef enum sal_rrcp_macTbl_block_e
{
	SAL_RRCP_MACTBL_DA_BLOCK,
	SAL_RRCP_MACTBL_SA_BLOCK,
	SAL_RRCP_MACTBL_DA_UNBLOCK,
	SAL_RRCP_MACTBL_SA_UNBLOCK,
} sal_rrcp_macTbl_block_t;

typedef enum {
	SAL_RRCP_QOS_PRIO_SELET_PORT,
	SAL_RRCP_QOS_PRIO_SELET_CLASSIFIER,
	SAL_RRCP_QOS_PRIO_SELET_IN_ACL,
	SAL_RRCP_QOS_PRIO_SELET_DSCP,
} sal_rrcp_qos_priSel_t;

typedef enum sal_rrcp_qosMappingType_e
{
    SAL_RRCP3_FUCN_QOS_REMAPPING_8021P = 0,
    SAL_RRCP3_FUCN_QOS_REMAPPING_DSCP,
}sal_rrcp_qosMappingType_t;

typedef enum sal_rrcp_mirror_e
{
	SAL_RRCP_MIRROR_DISABLE,
	SAL_RRCP_MIRROR_RX,
	SAL_RRCP_MIRROR_TX,
	SAL_RRCP_MIRROR_BOTH,
} sal_rrcp_mirror_t;

typedef enum {
	SAL_RRCP3_RLPP_DISABLE_STATE,
	SAL_RRCP3_RLPP_BLOCKING_STATE,
	SAL_RRCP3_RLPP_LISTENING_STATE,
	SAL_RRCP3_RLPP_FORWARDING_STATE,
	SAL_RRCP3_RLPP_TRUNK_MBR_BLOCKING_STATE,
} sal_rrcp_portState_t;



/* Function Name:
 *      sal_rrcp2Slv_macTblEntry_set
 * Description:
 *      Set the mac table entry.
 * Input:
 *      idx    - mac table entry index
 *      l2_tbl - pointer buffer of mac table data
 *      len    - the size of a mac table entry
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTblEntry_set(uint32 idx, const uint8 *l2_tbl, uint8 len);


/* Function Name:
 *      sal_rrcp2Slv_macTblEntry_get
 * Description:
 *      Get the mac table entry.
 * Input:
 *      idx     - mac table entry index
 *      len     - the size of a mac table entry
 * Output: 
 *      l2_tbl  - pointer buffer of mac table data
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTblEntry_get(uint32 idx, uint8 *l2_tbl, uint8 len);


/* Function Name:
 *      sal_rrcp2Slv_bcamTblEntry_set
 * Description:
 *      Set the BCAM part of mac table entry.
 * Input:
 *      idx	    - mac table entry index
 *      l2_tbl  - pointer buffer of mac table data
 *      len	    - the size of a mac table entry
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_bcamTblEntry_set(uint32 idx, uint8 *l2_tbl, uint8 len);


/* Function Name:
 *      sal_rrcp2Slv_bcamTblEntry_get
 * Description:
 *      Get the BCAM part of mac table entry.
 * Input:
 *      idx		- mac table entry index
 *      len		- the size of a mac table entry
 * Output: 
 *      l2_tbl 	- pointer buffer of mac table data
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_bcamTblEntry_get(uint32 idx, uint8 *l2_tbl, uint8 len);


#ifdef __MAC_DUMP__
/* Function Name:
 *      sal_rrcp2Slv_macTbl_dump
 * Description:
 *      Dump the whole mac table.
 * Input:
 *      *pRcvPktHdr - pointer buffer of RRCP packet header
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTbl_dump(rrcpPktHdr_t *pRcvPktHdr);


/* Function Name:
 *      sal_rrcp2Slv_bcamTbl_dump
 * Description:
 *      Dump the BCAM part of mac table.
 * Input:
 *      *pRcvPktHdr - pointer buffer of RRCP packet header
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_bcamTbl_dump(rtkproto_pkt_t *pRcvPkt);
#endif


/* Function Name:
 *      sal_rrcp2Slv_macTbl_clear
 * Description:
 *      Clear the whole mac table (include the BCAM part).
 * Input:
 *      *pRcvPktHdr - pointer buffer of RRCP packet header
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTbl_clear(uint8 ucType);


/* Function Name:
 *      sal_rrcp2Slv_vlanTblEntry_set
 * Description:
 *      Set the vlan table entry.
 * Input: 
 *      idx         - vlan table entry index
 *      pVlan_tbl   - pointer buffer of vlan table data
 *      len         - the size of a vlan table entry
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanTblEntry_set(uint32 idx, const uint8 *pVlan_tbl, uint8 len);


/* Function Name:
 *      sal_rrcp2Slv_vlanTblEntry_get
 * Description:
 *      Get the vlan table entry.
 * Input: 
 *      idx        - vlan table entry index
 *      len        - the size of a vlan table entry
 * Output: 
 *      pVlan_tbl  - pointer buffer of vlan table data
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanTblEntry_get(uint32 idx, uint8 *pVlan_tbl, uint8 len);


/* Function Name:
 *      sal_rrcp2Slv_vlanName_set
 * Description:
 *      Set the name of a vlan entry.
 * Input: 
 *      vid     - vlan id
 *      pName   - vlan name
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanName_set(sal_vlan_t vid, const uint8 *pName);


/* Function Name:
 *      sal_rrcp2Slv_vlanName_get
 * Description:
 *      Get the name of a vlan entry.
 * Input: 
 *      vid     - vlan id
 * Output: 
 *      pName   - vlan name
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanName_get(sal_vlan_t vid, uint8 *pName);


/* Function Name:
 *      sal_rrcp2Slv_vlanStaticEntry_get
 * Description:
 *      Get the the static vlan entry.
 * Input: 
 *      idx         - vlan static entry index
 * Output: 
 *		 pVlan_tbl  - pointer buffer of vlan table data
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanStaticEntry_get(uint8 idx, uint8 *pVlan_tbl);


/* Function Name:
 *      sal_rrcp2Slv_vlanEnable_set
 * Description:
 *      Set the vlan enable status.
 * Input: 
 *      enable		- vlan state
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanEnable_set(sal_enable_t enable);


/* Function Name:
 *      sal_rrcp2Slv_macTblPort_get
 * Description:
 *      Get the port field of a mac table entry.
 * Input: 
 *      fid         - filtering database id
 *      pMac        - mac address
 *      fid_valid   - to indicate that the argument fid is valid
 * Output:
 *      *pPort      - the port that this mac learned on
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTblPort_get(sal_fid_t fid, sal_mac_t *pMac, sal_port_t *pPort, uint32 fid_valid);


/* Function Name:
 *      sal_rrcp2Slv_macTblBlock_set
 * Description:
 *      Set the vlan enable status.
 * Input: 
 *      fid			- filtering database id
 *      pMac		- mac address
 *      block_mode	- the block mode
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_macTblBlock_set(sal_fid_t fid, sal_mac_t *pMac, sal_rrcp_macTbl_block_t block_mode);


/* Function Name:
 *      sal_rrcp2Slv_mibCounter_get
 * Description:
 *      Get the mib counter.
 * Input: 
 *      pSubOID    - the partial OID
 *      port       - port
 * Output: 
 *      pData      - the mib counter value
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_mibCounter_get(uint8 *pSubOID, uint8 port, uint32 *pData);


/* Function Name:
 *      sal_rrcp2Slv_mibCounter_reset
 * Description:
 *      Reset the mib counter.
 * Input: 
 *      pSubOID		- the partial OID
 *      port		- port
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_mibCounter_reset(uint8 *pSubOID, sal_port_t port);


/* Function Name:
 *      sal_rrcp2Slv_phyReg_set
 * Description:
 *      Set the phy register data.
 * Input: 
 *      method		- the access method
 *      port		- port
 *      page		- phy port
 *      addr		- phy addr
 *      data		- data in phy
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_phyReg_set(sal_phy_access_method_t method, sal_port_t port, uint32 page, uint32 addr, uint16 value);


/* Function Name:
 *      sal_rrcp2Slv_phyReg_get
 * Description:
 *      Get the phy register data.
 * Input: 
 *      method	   - the access method
 *      port       - port
 *      page       - phy port
 *      addr       - phy addr
 * Output:
 *      pData      - data in phy
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_phyReg_get(sal_phy_access_method_t method, sal_port_t port, uint32 page, uint32 addr, uint16 *pData);


/* Function Name:
 *      sal_rrcp2Slv_phyAbility_set
 * Description:
 *      Set the phy ability.
 * Input: 
 *		port		- port id
 *		ability		- phy ability code
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_phyAbility_set(sal_port_t port, uint32 ability);


/* Function Name:
 *      sal_rrcp2Slv_phyAbility_get
 * Description:
 *      Get the phy ability.
 * Input: 
 *      port		- port id
 * Output:
 *      pAbility	- phy ability code
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_phyAbility_get(sal_port_t port, uint32 *pAbility);


/* Function Name:
 *      sal_rrcp2Slv_portNegotitation_set
 * Description:
 *      Set the port advertised ability while negotiation.
 * Input: 
 *		 port		- port id
 *		 ability	- port advertised ability code
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_portNegotitation_set(sal_port_t port, uint32 ability);


/* Function Name:
 *      sal_rrcp2Slv_portNegotitation_get
 * Description:
 *      Get the port advertised ability while negotiation.
 * Input: 
 *      port       - port id
 *      pAbility   - port advertised ability
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_portNegotitation_get(sal_port_t port, uint32 *pAbility);


/* Function Name:
 *      sal_rrcp2Slv_portSpeedDuplex_get
 * Description:
 *      Get the speed and duplex of a port.
 * Input: 
 *      port	    - port id
 * Output: 
 *      pAbility    - speed and duplex code
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_portSpeedDuplex_get(sal_port_t port, uint32 *pAbility);


/* Function Name:
 *      sal_rrcp2Slv_portFlowCtrl_set
 * Description:
 *      Set the flow control status of a port.
 * Input: 
 *      port   - port id
 *      fcCode   - flow control status code
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_portFlowCtrl_set(sal_port_t port, uint32 fcCode);


/* Function Name:
 *      sal_rrcp2Slv_portFlowCtrl_get
 * Description:
 *      Get the flow control status of a port.
 * Input: 
 *      port   - port id
 * Output: 
 *		pCode  - flow control status code
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_portFlowCtrl_get(sal_port_t port, uint32 *pCode);


/* Function Name:
 *      sal_rrcp2Slv_vlanIgrFilter_set
 * Description:
 *      Set the vlan ingress filter status of a port.
 * Input: 
 *		port    - port id
 *		enable  - vlan ingress filter status
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanIgrFilter_set(sal_port_t port, sal_enable_t enable);


/* Function Name:
 *      sal_rrcp2Slv_vlanIgrFilter_get
 * Description:
 *      Get the vlan ingress filter status of a port.
 * Input: 
 *		port    - port id
 * Output:
 *      pEnable	- vlan ingress filter status
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanIgrFilter_get(sal_port_t port, sal_enable_t *pEnable);


/* Function Name:
 *      sal_rrcp2Slv_vlanPortCfg_set
 * Description:
 *      Set the vlan pvid, tag mode and frame type of a port.
 * Input: 
 *		port                - port id
 *		vid                 - vlan id
 *		vlan_mode           - vlan tag mode
 *		acceptFrameType   - vlan accept frame type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanPortCfg_set(sal_port_t port, sal_vlan_t vid, sal_vlan_tagMode_t vlan_mode, sal_vlan_acceptFrameType_t acceptFrameType);


/* Function Name:
 *      sal_rrcp2Slv_vlanPortCfg_get
 * Description:
 *      Get the vlan pvid, tag mode and frame type of a port.
 * Input: 
 *      port				- port id
 * Output: 
 *		pVid				- vlan id
 *		pVlan_mode		    - vlan tag mode
 *		pAcceptFrameType    - vlan accept frame type
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_vlanPortCfg_get(sal_port_t port, sal_vlan_t *pVid, sal_vlan_tagMode_t *pVlan_mode, sal_vlan_acceptFrameType_t *pAcceptFrameType);


/* Function Name:
 *      sal_rrcp2Slv_qosQueueNum_set
 * Description:
 *      Set the queue number of a port.
 * Input: 
 *		port    - port
 *		num     - the queue number
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosQueueNum_set(sal_port_t port, uint8 num);


/* Function Name:
 *      sal_rrcp2Slv_qosQueueNum_get
 * Description:
 *      Get the queue number of a port.
 * Input: 
 *		port    - port id
 * Output:
 *      pNum    - the queue number
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosQueueNum_get(sal_port_t port, uint8 *pNum);


/* Function Name:
 *      sal_rrcp2Slv_qosRemapping_set
 * Description:
 *      Set the mapping between specified priority source and the internal priority.
 * Input: 
 *      type		- the priority source to be mapped
 *      pri 		- priority value (range from 0 ~ 7)
 *      intPri		- internal priority value (range from 0 ~ 7)
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosRemapping_set(sal_rrcp_qosMappingType_t type, sal_pri_t pri, sal_pri_t intPri);


/* Function Name:
 *      sal_rrcp2Slv_qosRemapping_get
 * Description:
 *      Get the mapped internal priority from specified priority source and value.
 * Input: 
 *      type		- the priority source to be mapped
 *      pri 		- priority value (range from 0 ~ 7)
 * Output:
 *      intPri		- internal priority value (range from 0 ~ 7)
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosRemapping_get(sal_rrcp_qosMappingType_t type, sal_pri_t pri, sal_pri_t *pIntPri);
int32 SalRrcpv3SetQoSRemarking(uint8 port, uint8 uc1pEbl, uint8 ucDscpEbl);
int32 SalRrcpv3GetQoSRemarking(uint8 port, uint8 *puc1pEbl, uint8 *pucDscpEbl);
int32 SalRrcpv3SetQoSRemarkingParameter(uint8 ucIntPriority, uint8 uc1pVal, uint8 ucDscpVal);
int32 SalRrcpv3GetQoSRemarkingParameter(uint8 ucIntPriority, uint8 *puc1pVal, uint8 *pucDscpVal);


/* Function Name:
 *      sal_rrcp2Slv_qosPriSel_set
 * Description:
 *      Set the priority selection mechanism.
 * Input: 
 *      pri_src		- priority selection source
 *      weight		- the weight
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosPriSel_set(sal_rrcp_qos_priSel_t pri_src, uint8 weight);


/* Function Name:
 *      sal_rrcp2Slv_qosPriSel_get
 * Description:
 *      Get the priority selection mechanism.
 * Input: 
 *		 pri_src    - priority selection source
 * Output: 
 *		 pWeight    - the weight
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_qosPriSel_get(uint32 pri_src, uint8 *pWeight);


/* Function Name:
 *      sal_rrcp2Slv_mirror_set
 * Description:
 *      Set the port mirror.
 * Input: 
 *		 setId			- set id
 *		 direction		- the mirror direction
 *		 mirIso		    - the traffic isolation on monitor port
 *		 mirroredMask	- the mirrored port mask
 *		 monitorPort	- monitor port
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
extern int32 sal_rrcp2Slv_mirror_set(uint8 setId, sal_rrcp_mirror_t direction, uint8 mirIso, sal_lPortmask_t *pMirroredMask, sal_port_t monitorPort);
#ifdef EMBEDDED_SUPPORT
int32 SalRrcpv3GetMirror(uint8 setId, uint8 *pucDir, uint8 *pucMirIso, uint32 *psrcPortMskH, uint32 *psrcPortMskL, uint8 *pmonitorPort);
int32 SalRrcpv3SetTrunk(int8 ucTrkId, uint32 portMskH, uint32 portMskL);
int32 SalRrcpv3GetTrunk(int8 ucTrkId, uint32 *pPortMskH, uint32 *pPortMskL);
#else
int32 SalRrcpv3GetMirror(uint8 setId, uint8 *pucDir, uint8 *pucMirIso, uint64 *psrcPortMsk, uint8 *pmonitorPort);
int32 SalRrcpv3SetTrunk(int8 ucTrkId, uint64 portMsk);
int32 SalRrcpv3GetTrunk(int8 ucTrkId, uint64 *pPortMsk);
#endif
int32 SalRrcpv3SetJumbo(uint16 usSize);
int32 SalRrcpv3GetJumbo(uint16 *pusSize);
int32 SalRrcpv3SetStormCtrl(uint8 port, uint8 ucType, uint32 ulRate);
int32 SalRrcpv3GetStormCtrl(uint8 port, uint8 ucType, uint32 *pulRate);
int32 SalRrcpv3Reset(uint8 ucType);
extern int32 sal_rrcp2Slv_adminPortMask_set(sal_pPortmask_t *pAdmin_mask);
extern int32 sal_rrcp2Slv_adminPortMask_get(sal_pPortmask_t *pAdmin_mask);
extern int32 sal_rrcp2Slv_authPortMask_set(sal_pPortmask_t *pAuth_mask);
extern int32 sal_rrcp2Slv_authPortMask_get(sal_pPortmask_t *pAuth_mask);
int32 SalRrcpv3SetPrivateKey(uint32 ulKey);
int32 SalRrcpv3GetPrivateKey(uint32 *pulKey);
int32 SalRrcpv3SaveSwitchConfig(void);
#ifdef EMBEDDED_SUPPORT
int32 SalRrcpv3SetPortIsolation(int8 port, uint32 portMskH, uint32 portMskL);
int32 SalRrcpv3GetPortIsolation(int8 port, uint32 *pPortMsk, uint32 *pPortMskL);
#else
int32 SalRrcpv3SetPortIsolation(int8 port, uint64 portMsk);
int32 SalRrcpv3GetPortIsolation(int8 port, uint64 *pPortMsk);
#endif
int32 SalRrcpv3SetMacConstraint(int8 port, uint16 usLimitNum, uint8 ucAct);
int32 SalRrcpv3GetMacConstraint(int8 port, uint16 *pusLimitNum, uint8 *pucAct);
int32 SalRrcpv3SetDefenceEngine(int8 ucEnble);
extern int32 sal_rrcp2Slv_defenceEngine_get(sal_enable_t *pEnable);
#ifdef __RLPP__
int32 SalRrcpv3GetRLPPPortState(uint8 port, uint8 *pucByte);
#endif
int32 SalRrcpv3StartCableTest(void);
int32 SalRrcpv3StopCableTest(void);
int32 SalRrcpv3GetCableTest(int8 port, int8 *ucStatus, int32 *ulLength);
int32 SalRrcpv3SetEEPROM(int8 *src, uint32 addr, uint32 size);
int32 SalRrcpv3SetFlash(int8 *src, uint32 addr, uint32 size, int8 bLast);
int32 SalRrcpv3Set8051(int8 *src, uint32 addr, uint32 size);

#endif

