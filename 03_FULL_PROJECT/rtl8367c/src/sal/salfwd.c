//#pragma STRING (far)

#include <string.h>
#include <stdio.h>

#include <salflash.h>
#include <salerror.h>
#include <saltrunk.h>

#include <salportmask.h>
#include <salmirror.h>
#include <salrate.h>
#include <mirror.h>
#include <rtk_switch.h>
#include <rtk_types.h>
#include <rate.h>

#include <rtl8367c_asicdrv_meter.h>
//LWS_5G_1F   
/* meter allocation:
            bc      mc      unmc    unda
   port0    4       5       6       7
   port1    8       9       10      11
   ...
   port4    20      21      22      23
   port5    x       x       x       x
   port6    24      25      26      27
*/

//Else
/* meter allocation:
            bc      mc      unmc    unda
   port0    8      9        10      11
   port1    12      13        14      15
   ...
   port4    24      25        26      27
   port5    28      29        30      31
   port6    32      33        34      35
   port7    36      37        38      39   
   port8    40      41        42      43
   port9    44      45        46      47
*/
#ifdef LWS_5G_1F    
#define BCAST_METER(port)    ((port) * 4 + 4)
#define MCAST_METER(port)    ((port) * 4 + 5)
#define UNMC_METER(port)     ((port) * 4 + 6)
#define UNDA_METER(port)     ((port) * 4 + 7)
#else
#define BCAST_METER(port)    ((port) * 4 + 8)
#define MCAST_METER(port)    ((port) * 4 + 9)
#define UNMC_METER(port)     ((port) * 4 + 10)
#define UNDA_METER(port)     ((port) * 4 + 11)
#endif

typedef enum maxPktLen_e
{
    MAXPKTLEN_1522B = 0,
    MAXPKTLEN_1536B,
    MAXPKTLEN_1552B,
    MAXPKTLEN_16000B,   
    MAXPKTLEN_END   
} maxPktLen_t;



/****************************** Salmirror.c *********************/

// Sal Mirror Config Restore: Restores mirroring configuration from flash
// Reads mirror settings from flash and applies to hardware
int32
sal_mirror_config_restore(void)
{
	uint32		        mirrId, mode;
	sal_port_t          portId;
	sal_lPortmask_t     portMask;

	for (mirrId = 0; mirrId < SAL_MAX_MIRROR_GROUP; mirrId++)
	{
		sal_flash_mirror_portMirror_get(mirrId, &mode, &portId, &portMask);
		sal_mirror_portMirror_set(mirrId, mode, portId, &portMask);
	}

    return SAL_ERR_SUCCESS;
}

// Sal Mirror Port Mirror Set: Configures port mirroring settings
// Sets mirroring mode, monitoring port, and mirrored ports for RX/TX traffic
int32
sal_mirror_portMirror_set(
	uint32		mirrId,
	uint32		mode,
	sal_port_t		port,
	sal_lPortmask_t *pPortMask)
{
	int32                      ret;
    sal_pPortmask_t                portMask;
    rtk_portmask_t Mirrored_rx_msk;
    rtk_portmask_t Mirrored_tx_msk;

    sal_trunk_logicMask_toPhysicMask(&portMask, pPortMask);
    
    memset(&Mirrored_rx_msk, 0, sizeof(rtk_portmask_t));
    memset(&Mirrored_tx_msk, 0, sizeof(rtk_portmask_t));
    if (mode == PORT_MIRROR_OFF)
	{
	    port = 0;
	}
    else if (mode == PORT_MIRROR_RX)
        Mirrored_rx_msk.bits[0] = portMask.bits[0];
    else if (mode == PORT_MIRROR_TX)
        Mirrored_tx_msk.bits[0] = portMask.bits[0];
    else
    {
        Mirrored_rx_msk.bits[0] = portMask.bits[0];
        Mirrored_tx_msk.bits[0] = portMask.bits[0];
    }
    
    if((ret = rtk_mirror_portBased_set(port, &Mirrored_rx_msk, &Mirrored_tx_msk)) != RT_ERR_OK)
        return ret;
        
    return sal_flash_mirror_portMirror_set(mirrId, mode, port, pPortMask);
}

// Sal Mirror Port Mirror Get: Retrieves current mirroring configuration
// Gets mirroring mode, monitoring port, and mirrored ports from hardware
int32
sal_mirror_portMirror_get(
	uint32		mirrId,
	uint32 *	pMode,
	sal_port_t *	pPort,
	sal_lPortmask_t *	pPortMask)
{
	int32  ret;
    sal_pPortmask_t    mask;
    rtk_port_t Mirroring_port;
    rtk_portmask_t Mirrored_rx_msk;
    rtk_portmask_t Mirrored_tx_msk;

    mirrId = 0;
    memset(&mask, 0, sizeof(sal_pPortmask_t));
    memset(&Mirrored_rx_msk, 0, sizeof(rtk_portmask_t));
    memset(&Mirrored_tx_msk, 0, sizeof(rtk_portmask_t));
    
    if((ret = rtk_mirror_portBased_get(&Mirroring_port, &Mirrored_rx_msk, &Mirrored_tx_msk)) != RT_ERR_OK)
        return ret;

#if defined(LWS_8G_2F)
    if(Mirroring_port == EXT_PORT0){
        *pPort = 8;
    }
    else if(Mirroring_port == EXT_PORT1){
        *pPort = 9;
    }
    else{
        *pPort = Mirroring_port;
    }
#elif defined(LWS_5G_1F)  
    if(Mirroring_port == EXT_PORT0){
        *pPort = 5;
    }
    else{
        *pPort = Mirroring_port;
    }        
#else
    *pPort = Mirroring_port;
#endif

    if (Mirrored_rx_msk.bits[0] == 0 && Mirrored_tx_msk.bits[0] == 0)
    {
        *pMode = PORT_MIRROR_OFF;
    }
    else if (Mirrored_rx_msk.bits[0] != 0 && Mirrored_rx_msk.bits[0] != Mirrored_tx_msk.bits[0])
    {
        *pMode = PORT_MIRROR_RX;
        mask.bits[0] = Mirrored_rx_msk.bits[0];
    }
    else if (Mirrored_tx_msk.bits[0] != 0 && Mirrored_tx_msk.bits[0] != Mirrored_rx_msk.bits[0])
    {
        *pMode = PORT_MIRROR_TX;
        mask.bits[0] = Mirrored_tx_msk.bits[0];
    }
    else
    {
        *pMode = PORT_MIRROR_BOTH;
        mask.bits[0] = Mirrored_rx_msk.bits[0];
    }

    sal_trunk_physicMask_toLogicMask(pPortMask, &mask);

    SAL_ERR_CODE(ret, SAL_ERR_MIRROR_GET_FAILED);

    return SAL_ERR_SUCCESS;
}

/****************************** Salrate.c *********************/

// Sal Rate Config Restore: Restores rate control configuration from flash
// Restores storm control, bandwidth control, and jumbo frame settings
int32
sal_rate_config_restore(void)
{
    uint32          rate = 0;
    uint32          frameSize;
	uint32		    rate1;
    uint8           storm;
	sal_port_t      port;
	sal_lPortmask_t allPortMask;

    //sal_flash_rate_igrIncludeIFG_get(&include);
	//sal_rate_igrIncludeIFG_set(include);
	//sal_flash_rate_egrIncludeIFG_get(&include);
	//sal_rate_egrIncludeIFG_set(include);

    sal_trunk_logicPort_setAll(&allPortMask);
	for (port = 0; port < SAL_PORT_ID_CPU; port++)
	{
    	if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&allPortMask, port))
    	{
    		continue;
    	}

        /* Initialize storm control */
	    for (storm = SAL_STORM_TYPE_UNKNOWN_UCAST; storm <= SAL_STORM_TYPE_BCAST; storm++)
	    {
            sal_flash_rate_stormCtrl_get(port, storm, &rate);
            sal_rate_stormControl_set(port, storm, rate);
        }

        sal_flash_rate_igrBandwidthCtrl_get(port, &rate1);
		sal_rate_igrBandwidthCtrl_set(port, rate1);
		sal_flash_rate_egrBandwidthCtrl_get(port, &rate1);
		sal_rate_egrBandwidthCtrl_set(port, rate1);
	}

    /*Defence Engine*/
    //sal_flash_rate_cpuEgrCtrl_get(&enable);
    //sal_rate_cpuEgrCtrl_set(enable);

    /* Initialize jumbo frame setting */
    sal_flash_rate_maxFrameSize_get(&frameSize);
    sal_rate_maxFrameSize_set(frameSize);

    return SAL_ERR_SUCCESS;
}

// Sal Rate Storm Control Set: Configures storm control rate for specific traffic type
// Sets rate limit for broadcast, multicast, unknown multicast, or unknown unicast
int32
sal_rate_stormControl_set(
	sal_port_t	portId,
	sal_stormCtrl_type_t	type,
	uint32 rate)
{
	sal_port_t          port, portLogic;
	sal_pPortmask_t	    portMask;
	rtk_enable_t Ifg_include = DISABLED;

    SAL_ERR_CHK_EXPR((type > SAL_STORM_TYPE_BCAST), SAL_ERR_STRM_CTRL_UNKNOWN_FILTER);

	sal_trunk_logicId_toPhysicMask(&portMask, portId);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_SUCCESS == sal_pPortMask_chkPort(&portMask, port))
        {
#if defined(LWS_8G_2F)
            if(port == EXT_PORT0)
                portLogic = 8;
            else if(port == EXT_PORT1)
                portLogic = 9;
            else
                portLogic = port;
#elif defined(LWS_5G_1F)
            if(port == EXT_PORT0)
                portLogic = 5; //tmp use 5 instead of 6 to make meter in index 31
            else
                portLogic = port;
#else
            portLogic = port;
#endif
            switch(type)
            {
            case SAL_STORM_TYPE_BCAST:
                rtl8367c_setAsicShareMeter(BCAST_METER(portLogic), rate/SAL_RATE_GRANULARITY_IN_KB, 0);
                break;
            case SAL_STORM_TYPE_MCAST:
                rtl8367c_setAsicShareMeter(MCAST_METER(portLogic), rate/SAL_RATE_GRANULARITY_IN_KB, 0);
                break;
            case SAL_STORM_TYPE_UNKNOWN_MCAST:
                rtl8367c_setAsicShareMeter(UNMC_METER(portLogic), rate/SAL_RATE_GRANULARITY_IN_KB, 0);
                break;
            case SAL_STORM_TYPE_UNKNOWN_UCAST:
                rtl8367c_setAsicShareMeter(UNDA_METER(portLogic), rate/SAL_RATE_GRANULARITY_IN_KB, 0);
                break;
            default:
                /* exception */
                return SAL_ERR_FAILED;
            }            
        }
    }

    return sal_flash_rate_stormCtrl_set(portId, type, rate);
}

// Sal Rate Storm Control Get: Retrieves storm control rate for specific traffic type
// Gets rate limit from hardware for specified storm type
int32
sal_rate_stormControl_get(
	sal_port_t	portId,
	sal_stormCtrl_type_t	type,
	uint32 *pRate)
{
	int32       ret;
	sal_port_t  port;
	uint32      Ifg_include;
    uint32 rate;

    SAL_ERR_CHK_EXPR((type > SAL_STORM_TYPE_BCAST), SAL_ERR_STRM_CTRL_UNKNOWN_FILTER);

    sal_trunk_logicId_toPhysicId(&port, portId);
    
#if defined(LWS_8G_2F)
    if(port == EXT_PORT0)
        port = 8;
    else if(port == EXT_PORT1)
        port = 9;
#elif defined(LWS_5G_1F)
    if(port == EXT_PORT0)
        port = 5; //tmp use 5 instead of 6 to make meter in index 31
#endif

    switch(type)
    {
    case SAL_STORM_TYPE_BCAST:
        if((ret = rtl8367c_getAsicShareMeter(BCAST_METER(port), &rate, &Ifg_include))!=0)
            return ret;
        break;
    case SAL_STORM_TYPE_MCAST:
        if((ret = rtl8367c_getAsicShareMeter(MCAST_METER(port), &rate, &Ifg_include))!=0)
            return ret;
        break;        
    case SAL_STORM_TYPE_UNKNOWN_MCAST:
        if((ret = rtl8367c_getAsicShareMeter(UNMC_METER(port), &rate, &Ifg_include))!=0)
            return ret;
        break;        
    case SAL_STORM_TYPE_UNKNOWN_UCAST:
        if((ret = rtl8367c_getAsicShareMeter(UNDA_METER(port), &rate, &Ifg_include))!=0)
            return ret;
        break;        
    }
    rate = rate * 8;

    SAL_ERR_CODE(ret, SAL_ERR_STRM_CTRL);

    *pRate = rate;

    return SAL_ERR_SUCCESS;
}

// Sal Rate Max Frame Size Set: Configures maximum frame size (jumbo frame) setting
// Sets the maximum allowable frame size for the switch
int32
sal_rate_maxFrameSize_set(uint32 size)
{
	int32 ret;
	uint32 pktLen=0;
	sal_port_t          port;
	sal_pPortmask_t	    portMask;

	//ret = rtk_switch_maxPktLen_set(size);
	switch (size)
	{
	    case MAXPKTLEN_1522B:
	        pktLen = 1522;
	    break;
	    case MAXPKTLEN_1536B:
	        pktLen = 1536;
	    break;
	    case MAXPKTLEN_1552B:
	        pktLen = 1552;
	    break;
	    case MAXPKTLEN_16000B:
	        pktLen = 16383;
	    break;
	    default:
	        pktLen = 1522;
	    break;	    
	}
	ret = rtk_switch_maxPktLenCfg_set(0, pktLen);
	SAL_ERR_CODE(ret, SAL_ERR_JUMBO_FRAME_SIZE);

    sal_trunk_logicId_toPhysicMask(&portMask, port);
    
    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, port))
        {
            ret = rtk_switch_portMaxPktLen_set(port, MAXPKTLEN_LINK_SPEED_FE, 0);
            SAL_ERR_CODE(ret, SAL_ERR_JUMBO_FRAME_SIZE);

            ret = rtk_switch_portMaxPktLen_set(port, MAXPKTLEN_LINK_SPEED_GE, 0);   
            SAL_ERR_CODE(ret, SAL_ERR_JUMBO_FRAME_SIZE);
        }
    }

	return sal_flash_rate_maxFrameSize_set(size);
}

// Sal Rate Max Frame Size Get: Retrieves current maximum frame size setting
// Gets jumbo frame size configuration from hardware
int32
sal_rate_maxFrameSize_get(uint32 *pSize)
{
    int32  ret;
    uint32 pktLen;

	ret = rtk_switch_maxPktLenCfg_get(0, &pktLen);
	SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);

	switch (pktLen)
	{
	    case 1522:
	        *pSize = MAXPKTLEN_1522B;
	    break;
	    case 1536:
	        *pSize = MAXPKTLEN_1536B;
	    break;
	    case 1552:
	        *pSize = MAXPKTLEN_1552B;
	    break;
	    case 16383:
	        *pSize = MAXPKTLEN_16000B;
	    break;
	    default:
	        *pSize = MAXPKTLEN_1522B;
	    break;	    
	}

	return SAL_ERR_SUCCESS;
}

// Sal Rate Ingress Bandwidth Control Set: Configures ingress bandwidth limit for port
// Sets the incoming traffic rate limit for specified port
int32
sal_rate_igrBandwidthCtrl_set (
	sal_port_t	port,
	uint32	rate
	)
{
    int32          ret;
	uint8          portId;
	sal_pPortmask_t	portMask;
    rtk_enable_t Ifg_include = FALSE;
    
    if ((port > SAL_PORT_ID_CPU - 1)/* || (port == SAL_MAX_PHY_PORT)*/) {
        return SAL_ERR_PARAMETER;
    }

    sal_trunk_logicId_toPhysicMask(&portMask, port);
    
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {
            rate &= ~(SAL_RATE_GRANULARITY_IN_KB - 1);
            if ((ret = rtk_rate_igrBandwidthCtrlRate_set(portId, rate, Ifg_include,DISABLE)) != RT_ERR_OK)
                return ret;
        }
    }

    return sal_flash_rate_igrBandwidthCtrl_set(port, rate);
}

// Sal Rate Ingress Bandwidth Control Get: Retrieves ingress bandwidth limit for port
// Gets incoming traffic rate limit from hardware
int32
sal_rate_igrBandwidthCtrl_get (
	sal_port_t	port,
	uint32 *pRate
	)
{
	int32  ret;
	uint8  portId;
    rtk_enable_t Ifg_include = FALSE;
    //rtk_rate_t rate = 0;
    uint32 rate = 0;
	rtk_enable_t pFc_enable = FALSE;

	if ((port > SAL_PORT_ID_CPU - 1)/* || (port == SAL_MAX_PHY_PORT)*/) {
		return SAL_ERR_PORT_RANGE;
	}

    sal_trunk_logicId_toPhysicId(&portId, port);
	ret = rtk_rate_igrBandwidthCtrlRate_get(portId, &rate , &Ifg_include,&pFc_enable);
    SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);
    *pRate = rate;
    
    return SAL_ERR_SUCCESS;
}

// Sal Rate Egress Bandwidth Control Set: Configures egress bandwidth limit for port
// Sets the outgoing traffic rate limit for specified port
int32
sal_rate_egrBandwidthCtrl_set (
	sal_port_t	port,
	uint32	rate
	)
{
    int32          ret;
	uint8          portId;
	sal_pPortmask_t	portMask;
    rtk_enable_t Ifg_include = FALSE;

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PORT_RANGE;
	}

	sal_trunk_logicId_toPhysicMask(&portMask, port);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        if (SAL_ERR_SUCCESS  == sal_pPortMask_chkPort(&portMask, portId))
        {
            rate &= ~(SAL_RATE_GRANULARITY_IN_KB - 1);
            if ((ret = rtk_rate_egrBandwidthCtrlRate_set(portId, rate, Ifg_include)) != RT_ERR_OK)
                return ret;
        }
    }

    return sal_flash_rate_egrBandwidthCtrl_set(port, rate);
}

// Sal Rate Egress Bandwidth Control Get: Retrieves egress bandwidth limit for port
// Gets outgoing traffic rate limit from hardware
int32
sal_rate_egrBandwidthCtrl_get (
	sal_port_t	port,
	uint32 *pRate
	)
{
	int32 ret;
	sal_port_t          portId;
    rtk_enable_t Ifg_include = FALSE;
    //rtk_rate_t rate = 0;
    uint32 rate = 0;

	if ((port > SAL_PORT_ID_CPU - 1)) {
		return SAL_ERR_PARAMETER;
	}

    sal_trunk_logicId_toPhysicId(&portId, port);
	ret = rtk_rate_egrBandwidthCtrlRate_get( portId, &rate, &Ifg_include);
    SAL_ERR_CODE(ret, SAL_ERR_PARAMETER);

    *pRate = rate;

    return SAL_ERR_SUCCESS;
}