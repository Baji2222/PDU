#include <string.h>
#include <common.h>
#include <salflash.h>
#include <saltrunk.h>
#include <salerror.h>
#include <salportmask.h>
#include <salport.h>
#include <rtl8367c_asicdrv_phy.h>
#include <port.h>

/* macro for RTCT */
#define PHY_REG_ADDR_BASE        0x2000

#define PHY_PAGE_REG(port)       (PHY_REG_ADDR_BASE + 0x1F + (port) * 0x20)
#define RTCT_PHY_PAGE            11

#define RTCT_ACT_REG             0x1B21
#define RTCT_ACT_GLOBAL_OFF      5
#define RTCT_ACT_REG_OFF         15

#define RTCT_ACT_PMSK_REG        0x1B22

#define RTCT_TIMEOUT_CNT         0x20000UL

typedef enum rtct_channel
{
    RTCT_CHANNEL_A = 0,
    RTCT_CHANNEL_B,
    RTCT_CHANNEL_C,
    RTCT_CHANNEL_D,
    RTCT_CHANNEL_END
} rtct_channel_t;

#define RTCT_STS_PHY_REG(port)             (PHY_REG_ADDR_BASE + 26 + (port) * 0x20)
#define RTCT_STS_SHORT_STS_OFF(channel)    (12 + channel - RTCT_CHANNEL_A)
#define RTCT_STS_OPEN_STS_OFF(channel)     (8  + channel - RTCT_CHANNEL_A) 
#define RTCT_STS_MISM_STS_OFF(channel)     (4  + channel - RTCT_CHANNEL_A) 
#define RTCT_STS_LDRV_STS_OFF(channel)     (0  + channel - RTCT_CHANNEL_A) 



#define RTCT_RDY_PHY_REG(port)             (PHY_REG_ADDR_BASE + 27 + (port) * 0x20)
#define RTCT_RDY_PHY_REG_OFF               14

#define RTCT_RST_PHY_REG(port)             (PHY_REG_ADDR_BASE + 26 + (port) * 0x20)

#define RTCT_LEN_PHY_REG(port, channel)    (PHY_REG_ADDR_BASE + 27 + (port) * 0x20 + (channel))

#define RTCT_ACT_PHY_REG(port)             (PHY_REG_ADDR_BASE + 25 + (port) * 0x20)
#define RTCT_ACT_PHY_CMD_OFF               15

#if SAL_ENABLE
int32
sal_port_config_restore(void)
{
    uint8                   port;
    sal_lPortmask_t         allPortMask, isolationMask;
    sal_enable_t	        bPortEbl;
	Tbool           	    bFlowEbl;
    sal_phyAbility_t        phyAbility;        
    //rtk_port_phy_ability_t  phyAbility;

    sal_trunk_logicPort_setAll(&allPortMask);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
    	if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&allPortMask, port))
    	{
    		continue;
    	}

        //memset(&ability, 0, sizeof(sal_phyAbility_t));
        memset(&phyAbility, 0, sizeof(sal_phyAbility_t));
    	sal_flash_port_adminEnable_get(port, &bPortEbl);
        sal_flash_port_phyAbility_get(port, &phyAbility);
        sal_flash_port_flowCtrl_get(port, &bFlowEbl);
        sal_flash_port_isolation_get(port, &isolationMask);

        phyAbility.FC = bFlowEbl;
        sal_port_adminEnable_set(port, bPortEbl);
        sal_port_phyAbility_set(port, &phyAbility);
        //sal_port_linkAbility_set(port, &phyAbility);
        sal_port_isolation_set(port, isolationMask);
    }
    
    return SAL_ERR_SUCCESS;
}

int32 
sal_port_adminEnable_set(
    sal_port_t port, 
    sal_enable_t enable
     )
{
	int32          ret;
	uint8          portId;
	sal_pPortmask_t	p_portMask;

	sal_trunk_logicId_toPhysicMask(&p_portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&p_portMask, portId))
        {
            if ((ret = rtk_port_adminEnable_set(portId, enable)) != RT_ERR_OK)
                return ret;
        }
    }

    return sal_flash_port_adminEnable_set(port, enable);
}

int32 
sal_port_adminEnable_get(
    sal_port_t port, 
    sal_enable_t *pEnable
    )
{      
    int32 ret;
    rtk_enable_t en;
    sal_port_t  p_portId;

    sal_trunk_logicId_toPhysicId(&p_portId, port);
    if ((ret = rtk_port_adminEnable_get(p_portId, &en)) != RT_ERR_OK)
        return ret;

    if (en == DISABLED)
        *pEnable = SAL_DISABLED;
    else
        *pEnable = SAL_ENABLED;

    return SAL_ERR_SUCCESS;
}

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
int32 sal_port_link_get(sal_port_t port, sal_enable_t *pEnable)
{
    static rtk_port_mac_ability_t macAbt;
    memset(&macAbt, 0x00, sizeof(macAbt));
    rtk_port_macStatus_get(port, &macAbt);

    *pEnable = (uint8)(macAbt.link & 0xFF);

	return SAL_ERR_SUCCESS;
}

#if 0
int32 
sal_port_speedDuplex_get(
    sal_port_t port, 
    uint32 *pSpeed, 
    uint32 *pDuplex
    )
{
	int32  ret;
	uint8  ucPPortId;
    rtk_port_linkStatus_t LinkStatus;
    rtk_port_speed_t spd;
    rtk_port_duplex_t duplex;

    
	SalTrkLId2PId(&ucPPortId, port);
    if ((ret = rtk_port_phyStatus_get(ucPPortId, &LinkStatus, &spd, &duplex)) !=SUCCESS)
		return ret;

	*pSpeed = spd;
	*pDuplex = duplex;

    return SAL_ERR_SUCCESS;
}
#endif

int32 
sal_port_isolation_set(
    sal_port_t port, 
    sal_lPortmask_t portMask
    )
{
	int32          ret;
	uint8          portId;
	sal_pPortmask_t	stPPortMask1, stPPortMask2;
    rtk_portmask_t  portmask2;

    sal_trunk_logicMask_toPhysicMask(&stPPortMask1, &portMask);
    sal_trunk_logicId_toPhysicMask(&stPPortMask2, port);

    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&stPPortMask2, portId))
        {
            memset(&portmask2, 0, sizeof(rtk_portmask_t));
            portmask2.bits[0] = stPPortMask1.bits[0] | ((uint32)1 << SAL_PORT_ID_CPU);
            if ((ret = rtk_port_isolation_set(portId, &portmask2)) != RT_ERR_OK)
                return ret;
        }
    }
    
#if defined(LWS_8G_2F)
	if (port == 8)
        port = EXT_PORT0;
	if (port == 9)
        port = EXT_PORT1;
#elif defined(LWS_5G_1F)
	if (port == 6)
        port = EXT_PORT0;
#endif

	return sal_flash_port_isolation_set(port, portMask);
}

int32 
sal_port_isolation_get(
    sal_port_t port, 
    sal_lPortmask_t *pPortMask
    )
{
	int32          ret;
	uint8          ucPPortId;
    sal_pPortmask_t    stPPortMask;
    rtk_portmask_t  portmask;

    SAL_ERR_CHK_EXPR(NULL == pPortMask, SAL_ERR_NULL_POINTER);

    memset(&portmask, 0, sizeof(rtk_portmask_t));
    memset(&stPPortMask, 0, sizeof(sal_pPortmask_t));
    
	sal_trunk_logicId_toPhysicId(&ucPPortId, port);
    ret = rtk_port_isolation_get(ucPPortId, &portmask);
    SAL_ERR_CODE(ret, SAL_ERR_PORT_ISOLATION_GET);

    stPPortMask.bits[0] = portmask.bits[0];

    sal_trunk_physicMask_toLogicMask(pPortMask, &stPPortMask);

	return SAL_ERR_SUCCESS;
}

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
int32 sal_port_phyAbility_set(sal_port_t port, sal_phyAbility_t *pPhyAbility)
{        
    int32          ret;
	uint8          portId;
    sal_pPortmask_t  portmask;
    rtk_port_phy_ability_t ability;

    if (pPhyAbility == NULL)
        return SAL_ERR_NULL_POINTER;
        
    ability.AutoNegotiation = pPhyAbility->AutoNegotiation & 0x0001;
	ability.Half_10         = pPhyAbility->Half_10 & 0x0001;
	ability.Full_10         = pPhyAbility->Full_10 & 0x0001;
	ability.Half_100        = pPhyAbility->Half_100 & 0x0001;
	ability.Full_100        = pPhyAbility->Full_100 & 0x0001;
	ability.Full_1000       = pPhyAbility->Full_1000 & 0x0001;
	ability.FC              = pPhyAbility->FC & 0x0001;
	ability.AsyFC           = pPhyAbility->AsyFC & 0x0001;
	sal_trunk_logicId_toPhysicMask(&portmask, port);
	
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portmask, portId))
        {
            if(ability.AutoNegotiation == 1)
            {
                if ((ret = rtk_port_phyAutoNegoAbility_set(portId, &ability)) != RT_ERR_OK)
                    return ret;
            }
            else
            {
                if ((ret = rtk_port_phyForceModeAbility_set(portId, &ability)) != RT_ERR_OK)
                    return ret;            
            }
        }
    }
    
    return sal_flash_port_phyAbility_set(port, pPhyAbility);
}


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
int32 sal_port_phyAbility_get(sal_port_t port, sal_phyAbility_t *pPhyAbility)
{
//    rtk_api_ret_t ret;
    sal_phyAbility_t ability;
    sal_port_t  portId;

    if (pPhyAbility == NULL)
        return SAL_ERR_NULL_POINTER;

    sal_trunk_logicId_toPhysicId(&portId, port);
    
#if 1
        sal_flash_port_phyAbility_get(port, &ability);
        memcpy(pPhyAbility, &ability, sizeof(sal_phyAbility_t));
#else
        if(ability.AutoNegotiation == 1)
        if ((ret = rtk_port_phyAutoNegoAbility_get(portId, &ability)) != RT_ERR_OK)
            return ret;
     
        pPhyAbility->AutoNegotiation = (uint8)(ability.AutoNegotiation & 0x1);
        pPhyAbility->Half_10         = (uint8)(ability.Half_10 & 0x1);
        pPhyAbility->Full_10         = (uint8)(ability.Full_10 & 0x1);
        pPhyAbility->Half_100        = (uint8)(ability.Half_100 & 0x1);
        pPhyAbility->Full_100        = (uint8)(ability.Full_100 & 0x1);
        pPhyAbility->Full_1000       = (uint8)(ability.Full_1000 & 0x1);
        pPhyAbility->FC              = (uint8)(ability.FC & 0x1);
        pPhyAbility->AsyFC           = (uint8)(ability.AsyFC & 0x1);
#endif	
	
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_port_macStatus_get
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
int32 sal_port_macStatus_get(sal_port_t port, sal_port_macStatus_t *pStatus)
{
    int32 ret;
    sal_port_t  portId;
    rtk_port_mac_ability_t status;
    if (pStatus == NULL)
        return SAL_ERR_NULL_POINTER;

    sal_trunk_logicId_toPhysicId(&portId, port);

    if((ret = rtk_port_macStatus_get(portId, &status)) != SUCCESS)
        return ret;


    pStatus->duplex = (sal_port_duplex_t)status.duplex;
    pStatus->fc     = (sal_port_fc_t)status.rxpause & (sal_port_fc_t)status.txpause;
    pStatus->linkup = status.link;
    pStatus->speed  = (sal_port_speed_t)status.speed;

    return SAL_ERR_SUCCESS;
}

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
int32 sal_port_flush_set(sal_port_t port)
{
    port = 0;

    return SAL_ERR_SUCCESS;
}

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
 *      SAL_ERR_PARAMETER
 *      SAL_ERR_NULL_POINTER 
 * Note:
 *      (1) If linkType is PORT_SPEED_1000M, test result will be stored in sal_ge_result. 
 *      (2) If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32 sal_port_rtctResult_get(sal_port_t port, sal_rtctResult_t *pRtctResult)
{
    uint32 i;
    uint32 cnt;
    int32 ret;
    rtk_rtctResult_t rtk_rtctResult;

    if(port > SAL_MAX_PHY_PORT - SAL_MAX_EXT_PORT)
        return SAL_ERR_PARAMETER;

    if(NULL == pRtctResult)
        return SAL_ERR_NULL_POINTER;

    pRtctResult->linkType = SAL_PORT_SPEED_1000;

    cnt = 0;
    memset(&rtk_rtctResult, 0, sizeof(rtk_rtctResult));
    while(1)
    {
        ret = rtk_port_rtctResult_get(port, &rtk_rtctResult);
        if ( ret == RT_ERR_OK)
            break;
        for(i=0;i<0x2fff;i++);
        cnt++;
    
        if ( cnt == 30 )
            return SAL_ERR_FAILED;
    }    

    pRtctResult->ge_result.channelALen = rtk_rtctResult.result.rtk_ge_result.channelALen & 0xffff;
    pRtctResult->ge_result.channelBLen = rtk_rtctResult.result.rtk_ge_result.channelBLen & 0xffff;
    pRtctResult->ge_result.channelCLen = rtk_rtctResult.result.rtk_ge_result.channelCLen & 0xffff;
    pRtctResult->ge_result.channelDLen = rtk_rtctResult.result.rtk_ge_result.channelDLen & 0xffff;

    pRtctResult->ge_result.channelALinedriver = rtk_rtctResult.result.rtk_ge_result.channelALinedriver & 0xff;
    pRtctResult->ge_result.channelBLinedriver = rtk_rtctResult.result.rtk_ge_result.channelBLinedriver & 0xff;
    pRtctResult->ge_result.channelCLinedriver = rtk_rtctResult.result.rtk_ge_result.channelCLinedriver & 0xff;
    pRtctResult->ge_result.channelDLinedriver = rtk_rtctResult.result.rtk_ge_result.channelDLinedriver & 0xff;

    pRtctResult->ge_result.channelAMismatch = rtk_rtctResult.result.rtk_ge_result.channelAMismatch & 0xff;
    pRtctResult->ge_result.channelBMismatch = rtk_rtctResult.result.rtk_ge_result.channelBMismatch & 0xff;
    pRtctResult->ge_result.channelCMismatch = rtk_rtctResult.result.rtk_ge_result.channelCMismatch & 0xff;
    pRtctResult->ge_result.channelDMismatch = rtk_rtctResult.result.rtk_ge_result.channelDMismatch & 0xff;

    pRtctResult->ge_result.channelAOpen = rtk_rtctResult.result.rtk_ge_result.channelAOpen & 0xff;
    pRtctResult->ge_result.channelBOpen = rtk_rtctResult.result.rtk_ge_result.channelBOpen & 0xff;
    pRtctResult->ge_result.channelCOpen = rtk_rtctResult.result.rtk_ge_result.channelCOpen & 0xff;
    pRtctResult->ge_result.channelDOpen = rtk_rtctResult.result.rtk_ge_result.channelDOpen & 0xff;

    pRtctResult->ge_result.channelAShort = rtk_rtctResult.result.rtk_ge_result.channelAShort & 0xff;
    pRtctResult->ge_result.channelBShort = rtk_rtctResult.result.rtk_ge_result.channelBShort & 0xff;
    pRtctResult->ge_result.channelCShort = rtk_rtctResult.result.rtk_ge_result.channelCShort & 0xff;
    pRtctResult->ge_result.channelDShort = rtk_rtctResult.result.rtk_ge_result.channelDShort & 0xff;    

    return SAL_ERR_SUCCESS;

}

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
int32 sal_port_rtct_start(sal_port_t port)
{
    int32 ret;
    rtk_portmask_t portmask;
    
    if(port > SAL_MAX_PHY_PORT - SAL_MAX_EXT_PORT)
        return SAL_ERR_PARAMETER;
    portmask.bits[0] = 1UL << port;
    if ((ret = rtk_port_rtctEnable_set(&portmask)) != RT_ERR_OK)
        return ret;
    
    return SAL_ERR_SUCCESS;
}

#endif
#endif

