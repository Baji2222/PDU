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
 * Feature : Port abilitiy/Port isolation/Port security
 *
 */
#ifndef __SAL_PORT_H__
#define __SAL_PORT_H__

#include <saltype.h>

typedef enum sal_port_nego_e {
	SAL_PORT_NEGO_FORCE = 0,
	SAL_PORT_NEGO_AUTO
} sal_port_nego_t;

typedef enum sal_port_speed_e {
	SAL_PORT_SPEED_10 = 0,
	SAL_PORT_SPEED_100,
	SAL_PORT_SPEED_1000,
	SAL_PORT_SPEED_END
} sal_port_speed_t;

typedef enum sal_port_duplex_e {
	SAL_PORT_DUPLEX_HALF = 0,
	SAL_PORT_DUPLEX_FULL
} sal_port_duplex_t;

typedef enum sal_port_speed_duplex_e {
	SAL_PORT_SPEED_DUPLEX_AUTO,
	SAL_PORT_SPEED_DUPLEX_10_HALF,
	SAL_PORT_SPEED_DUPLEX_10_FULL,
	SAL_PORT_SPEED_DUPLEX_100_HALF,
	SAL_PORT_SPEED_DUPLEX_100_FULL,
	SAL_PORT_SPEED_DUPLEX_1000_FULL
} sal_port_speed_duplex_t;

typedef enum sal_port_fc_e {
	SAL_PORT_FLOW_CTRL_DISABLE = 0,
	SAL_PORT_FLOW_CTRL_ENABLE
} sal_port_fc_t ;

typedef enum sal_phy_access_method_e
{
    SAL_PHY_METHOD_PHY = 0,
    SAL_PHY_METHOD_SDS,
    SAL_PHY_METHOD_MDX
} sal_phy_access_method_t;

typedef struct sal_phyAbility_s
{
    uint8   AutoNegotiation:1;      /* 0: Force mode; 1: NWay mode*/
    uint8   Half_10:1;		/*In NWay mode: PHY Pregister 4.5 setting for 10BASE-TX half duplex capable*/
							/*In Force mode: set Register 0.6 and Register 0.10 for speed; Register 0.8 for duplex*/
    uint8   Full_10:1;		/*PHY register 4.6 setting for 10BASE-TX full duplex capable*/
							/*In Force mode: set Register 0.6 and Register 0.10 for speed; Register 0.8 for duplex*/
    uint8   Half_100:1;		/*PHY register 4.7 setting for 100BASE-TX half duplex capable*/
							/*In Force mode: set Register 0.6 and Register 0.10 for speed; Register 0.8 for duplex*/
    uint8   Full_100:1;		/*PHY register 4.8 setting for 100BASE-TX full duplex capable*/
							/*In Force mode: set Register 0.6 and Register 0.10 for speed; Register 0.8 for duplex*/
    uint8   Full_1000:1;		/*PHY register 9.9 setting for 1000BASE-T full duplex capable*/
							/*In Force mode: not support*/
    uint8   FC:1;			/*PHY register 4.10 setting for flow control capability*/
    uint8   AsyFC:1;		/*PHY register 4.11 setting for  asymmetric flow control capability*/		
} sal_phyAbility_t;

typedef struct sal_port_macStatus_s
{
    uint8               linkup;     /* link up / link down status */
    sal_port_duplex_t   duplex;     /* duplex */
    sal_port_speed_t    speed;      /* speed */
    sal_port_fc_t       fc;         /* flow control status */    
} sal_port_macStatus_t;

#if SAL_RTCT

typedef struct sal_rtctResult_s
{
    sal_port_speed_t linkType;
    struct ge_result_s
    {
        uint8  channelAShort;
        uint8  channelBShort;
        uint8  channelCShort;
        uint8  channelDShort;

        uint8  channelAOpen;
        uint8  channelBOpen;
        uint8  channelCOpen;
        uint8  channelDOpen;

        uint8  channelAMismatch;
        uint8  channelBMismatch;
        uint8  channelCMismatch;
        uint8  channelDMismatch;

        uint8  channelALinedriver;
        uint8  channelBLinedriver;
        uint8  channelCLinedriver;
        uint8  channelDLinedriver;

        /* length unit: cm */
        uint16  channelALen;
        uint16  channelBLen;
        uint16  channelCLen;
        uint16  channelDLen;
    } ge_result;
} sal_rtctResult_t;
#endif /* SAL_RTCT */


/* Function Name:
 *      sal_port_config_restore
 * Description:
 *      Restore port configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_port_config_restore(void);


/* Function Name:
 *      sal_port_rxEnable_set
 * Description:
 *      Set port RX enable status of the specific port
 * Input:
 *      port    - port ID
 *      enable  - RX status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_port_rxEnable_set(sal_port_t port, sal_enable_t enable);


/* Function Name:
 *      sal_port_adminEnable_set
 * Description:
 *      Set port enable status of the specific port
 * Input:
 *      port    - port ID
 *      enable  - port status
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_adminEnable_set(sal_port_t port, sal_enable_t enable);


/* Function Name:
 *      sal_port_adminEnable_get
 * Description:
 *      Get port enable status of the specific port
 * Input:
 *      port    - port ID
 * Output:
 *      pEnable - Pointer to the enable status of port
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_adminEnable_get(sal_port_t port, sal_enable_t *pEnable);



/* Function Name:
 *      sal_port_link_get
 * Description:
 *      Get link status of the specific port
 * Input:
 *      port    - port ID
 * Output:
 *      pEnable - pointer to link status of port
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_link_get(sal_port_t port, sal_enable_t *pEnable);


/* Function Name:
 *      sal_port_speedDuplex_get
 * Description:
 *      Get speed/duplex of the specific port
 * Input:
 *      port    - Port ID
 * Output:
 *      pSpeed  - Pointer to speed of port
 *      pDuplex - Pointer to duplex of port
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_speedDuplex_get(sal_port_t port, uint32 *pSpeed, uint32 *pDuplex);


/* Function Name:
 *      sal_port_isolation_set
 * Description:
 *      Set the port isolation mask of the specific port
 * Input:
 *      port        - port ID
 *      portMask    - port isolation mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_port_isolation_set(sal_port_t port, sal_lPortmask_t portMask);


/* Function Name:
 *      sal_port_isolation_get
 * Description:
 *      Get the port isolation mask of the specific port
 * Input:
 *      port        - port ID
 * Output:
 *      pPortMask   - pointer to the port isolation mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_port_isolation_get(sal_port_t port, sal_lPortmask_t *pPortMask);

#if defined(RLPP_PATCH_8367N)
/* Function Name:
 *      sal_port_isolation_set_old
 * Description:
 *      Set the port isolation mask of the specific port
 * Input:
 *      port        - port ID
 *      portMask    - port isolation mask
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_port_isolation_set_old(sal_port_t port, sal_lPortmask_t portMask);


/* Function Name:
 *      sal_port_isolation_get_old
 * Description:
 *      Get the port isolation mask of the specific port
 * Input:
 *      port        - port ID
 * Output:
 *      pPortMask   - pointer to the port isolation mask
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 sal_port_isolation_get_old(sal_port_t port, sal_lPortmask_t *pPortMask);
#endif

/* Function Name:
 *      sal_port_phyAbility_set
 * Description:
 *      Set the port ability of the specific port
 * Input:
 *      port        - port ID
 *      pPhyAbility - pointer to the port ability
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_phyAbility_set(sal_port_t port, sal_phyAbility_t *pPhyAbility);


/* Function Name:
 *      sal_port_phyAutoNegoAbility_get
 * Description:
 *      Get the port ability of the specific port
 * Input:
 *      port        - port ID
 * Output:
 *      pPhyAbility - Pointer to the port ability
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_phyAbility_get(sal_port_t port, sal_phyAbility_t *pPhyAbility);

/* Function Name:
 *      sal_port_flowCtrlNegoResult_get
 * Description:
 *      Get the flow control negotiation result of the specific port
 * Input:
 *      port    - port ID
 * Output:
 *      pEnable - pointer to the flow control negotiation result
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_flowCtrlNegoResult_get(sal_port_t port, sal_enable_t *pEnable);

/* Function Name:
 *      sal_port_macStatus_set
 * Description:
 *      Get MAC status.
 * Input:
 *      port    - port ID
 * Output:
 *      pStatus - pointer to the mac status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
extern int32 sal_port_macStatus_get(sal_port_t port, sal_port_macStatus_t *pStatus);


/* Function Name:
 *      sal_port_security_set
 * Description:
 *      Set the limited L2 learning entry count of the specific port for port security
 * Input:
 *      port    - port ID
 *      num     - The number of limited L2 learning entry count.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_port_security_set(sal_port_t port, uint32 num);


/* Function Name:
 *      sal_port_security_set
 * Description:
 *      Get the limited L2 learning entry count of the specific port
 * Input:
 *      port    - port ID
 * Output:
 *      pNum    - pointer to the number of limited L2 learning entry count.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_port_security_get(sal_port_t port, uint32 *pNum);


/* Function Name:
 *      sal_port_securityAction_set
 * Description:
 *      Set the port security action of the specific port
 * Input:
 *      port    - port ID
 *      act     - the action of port security
 * Output:
 *      None.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_port_securityAction_set(sal_port_t port, uint32 act);


/* Function Name:
 *      sal_port_securityAction_get
 * Description:
 *      Get the port security action of the specific port
 * Input:
 *      port    - port ID
 * Output:
 *      pAct    - pointer to the action of port security
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_port_securityAction_get(sal_port_t port, uint32 *pAct);


/* Function Name:
 *      sal_port_flush_set
 * Description:
 *      Flush packets of a port
 * Input:
 *      port    - port ID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PORT_RANGE
 * Note:
 *      None
 */
extern int32 sal_port_flush_set(sal_port_t port);

#if SAL_RTCT
/* Function Name:
 *      sal_port_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result 
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 *      SAL_ERR_NOT_SUPPORT
 * Note:
 *      (1) If linkType is PORT_SPEED_1000M, test result will be stored in ge_result. 
 *      (2) If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
extern int32 sal_port_rtctResult_get(sal_port_t port, sal_rtctResult_t *pRtctResult);

/* Function Name:
 *      sal_port_rtct_start
 * Description:
 *      Start RTCT for ports. 
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      port        - the physical port for retriving RTCT test result
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_PARAMETER
 * Note:
 *      None
 */
extern int32 sal_port_rtct_start(sal_port_t port);
#endif /* SAL_RTCT */


#endif

