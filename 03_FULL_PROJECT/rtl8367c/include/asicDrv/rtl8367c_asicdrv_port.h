#ifndef _RTL8367C_ASICDRV_PORTSECURITY_H_
#define _RTL8367C_ASICDRV_PORTSECURITY_H_

#include <rtl8367c_asicdrv.h>
#include <rtl8367c_asicdrv_unknownMulticast.h>
#include <rtl8367c_asicdrv_phy.h>

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

#define RTL8367C_MAC7       7
#define RTL8367C_EXTNO       3

#define RTL8367C_RTCT_PAGE          (11)
#define RTL8367C_RTCT_RESULT_A_REG  (27)
#define RTL8367C_RTCT_RESULT_B_REG  (28)
#define RTL8367C_RTCT_RESULT_C_REG  (29)
#define RTL8367C_RTCT_RESULT_D_REG  (30)
#define RTL8367C_RTCT_STATUS_REG    (26)

enum L2_SECURITY_BEHAVE
{
    L2_BEHAVE_FLOODING = 0,
    L2_BEHAVE_DROP,
    L2_BEHAVE_TRAP,
    L2_BEHAVE_END
};

enum L2_UNDA_BEHAVE
{
    L2_UNDA_BEHAVE_FLOODING_PMASK = 0,
    L2_UNDA_BEHAVE_DROP,
    L2_UNDA_BEHAVE_TRAP,
    L2_UNDA_BEHAVE_FLOODING,
    L2_UNDA_BEHAVE_END
};

enum L2_SECURITY_SA_BEHAVE
{
    L2_BEHAVE_SA_FLOODING = 0,
    L2_BEHAVE_SA_DROP,
    L2_BEHAVE_SA_TRAP,
    L2_BEHAVE_SA_COPY28051,
    L2_BEHAVE_SA_END
};

/* enum for port current link speed */
enum SPEEDMODE
{
    SPD_10M = 0,
    SPD_100M,
    SPD_1000M,
    SPD_2500M
};

/* enum for mac link mode */
enum LINKMODE
{
    MAC_NORMAL = 0,
    MAC_FORCE,
};

/* enum for port current link duplex mode */
enum DUPLEXMODE
{
    HALF_DUPLEX = 0,
    FULL_DUPLEX
};

/* enum for port current MST mode */
enum MSTMODE
{
    SLAVE_MODE= 0,
    MASTER_MODE
};


enum EXTMODE
{
    EXT_DISABLE = 0,
    EXT_RGMII,
    EXT_MII_MAC,
    EXT_MII_PHY,
    EXT_TMII_MAC,
    EXT_TMII_PHY,
    EXT_GMII,
    EXT_RMII_MAC,
    EXT_RMII_PHY,
    EXT_SGMII,
    EXT_HSGMII,
    EXT_1000X_100FX,
    EXT_1000X,
    EXT_100FX,
    EXT_RGMII_2,
    EXT_MII_MAC_2,
    EXT_MII_PHY_2,
    EXT_TMII_MAC_2,
    EXT_TMII_PHY_2,
    EXT_RMII_MAC_2,
    EXT_RMII_PHY_2,
    EXT_END
};

enum DOSTYPE
{
    DOS_DAEQSA = 0,
    DOS_LANDATTACKS,
    DOS_BLATATTACKS,
    DOS_SYNFINSCAN,
    DOS_XMASCAN,
    DOS_NULLSCAN,
    DOS_SYN1024,
    DOS_TCPSHORTHDR,
    DOS_TCPFRAGERROR,
    DOS_ICMPFRAGMENT,
    DOS_END,

};

typedef struct  rtl8367c_port_ability_s{
    uint16 forcemode;
    uint16 mstfault;
    uint16 mstmode;
    uint16 nway;
    uint16 txpause;
    uint16 rxpause;
    uint16 link;
    uint16 duplex;
    uint16 speed;
}rtl8367c_port_ability_t;

typedef struct  rtl8367c_port_status_s{

    uint16 lpi1000;
    uint16 lpi100;
    uint16 mstfault;
    uint16 mstmode;
    uint16 nway;
    uint16 txpause;
    uint16 rxpause;
    uint16 link;
    uint16 duplex;
    uint16 speed;

}rtl8367c_port_status_t;

typedef struct rtct_result_s
{
    uint32      channelAShort;
    uint32      channelBShort;
    uint32      channelCShort;
    uint32      channelDShort;

    uint32      channelAOpen;
    uint32      channelBOpen;
    uint32      channelCOpen;
    uint32      channelDOpen;

    uint32      channelAMismatch;
    uint32      channelBMismatch;
    uint32      channelCMismatch;
    uint32      channelDMismatch;

    uint32      channelALinedriver;
    uint32      channelBLinedriver;
    uint32      channelCLinedriver;
    uint32      channelDLinedriver;

    uint32      channelALen;
    uint32      channelBLen;
    uint32      channelCLen;
    uint32      channelDLen;
} rtl8367c_port_rtct_result_t;


/****************************************************************/
/* Driver Proto Type Definition                                 */
/****************************************************************/
extern ret_t rtl8367c_setAsicPortUnknownDaBehavior(uint32 port, uint32 behavior);
extern ret_t rtl8367c_getAsicPortUnknownDaBehavior(uint32 port, uint32 *pBehavior);
extern ret_t rtl8367c_setAsicPortUnknownSaBehavior(uint32 behavior);
extern ret_t rtl8367c_getAsicPortUnknownSaBehavior(uint32 *pBehavior);
extern ret_t rtl8367c_setAsicPortUnmatchedSaBehavior(uint32 behavior);
extern ret_t rtl8367c_getAsicPortUnmatchedSaBehavior(uint32 *pBehavior);
extern ret_t rtl8367c_setAsicPortUnmatchedSaMoving(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicPortUnmatchedSaMoving(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicPortUnknownDaFloodingPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicPortUnknownDaFloodingPortmask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicPortUnknownMulticastFloodingPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicPortUnknownMulticastFloodingPortmask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicPortBcastFloodingPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicPortBcastFloodingPortmask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicPortBlockSpa(uint32 port, uint32 block);
extern ret_t rtl8367c_getAsicPortBlockSpa(uint32 port, uint32 *pBlock);
extern ret_t rtl8367c_setAsicPortForceLink(uint32 port, rtl8367c_port_ability_t *pPortAbility);
extern ret_t rtl8367c_getAsicPortForceLink(uint32 port, rtl8367c_port_ability_t *pPortAbility);
extern ret_t rtl8367c_getAsicPortStatus(uint32 port, rtl8367c_port_status_t *pPortStatus);
extern ret_t rtl8367c_setAsicPortForceLinkExt(uint32 id, rtl8367c_port_ability_t *pPortAbility);
extern ret_t rtl8367c_getAsicPortForceLinkExt(uint32 id, rtl8367c_port_ability_t *pPortAbility);
extern ret_t rtl8367c_setAsicPortExtMode(uint32 id, uint32 mode);
extern ret_t rtl8367c_getAsicPortExtMode(uint32 id, uint32 *pMode);
extern ret_t rtl8367c_setAsicPortDos(uint32 type, uint32 drop);
extern ret_t rtl8367c_getAsicPortDos(uint32 type, uint32* pDrop);
extern ret_t rtl8367c_setAsicPortEnableAll(uint32 enable);
extern ret_t rtl8367c_getAsicPortEnableAll(uint32 *pEnable);
extern ret_t rtl8367c_setAsicPortSmallIpg(uint32 port, uint32 enable);
extern ret_t rtl8367c_getAsicPortSmallIpg(uint32 port, uint32* pEnable);
extern ret_t rtl8367c_setAsicPortLoopback(uint32 port, uint32 enable);
extern ret_t rtl8367c_getAsicPortLoopback(uint32 port, uint32 *pEnable);
extern ret_t rtl8367c_setAsicPortRTCTEnable(uint32 portmask);
extern ret_t rtl8367c_setAsicPortRTCTDisable(uint32 portmask);
extern ret_t rtl8367c_getAsicPortRTCTResult(uint32 port, rtl8367c_port_rtct_result_t *pResult);
extern ret_t rtl8367c_sdsReset(uint32 id);
extern ret_t rtl8367c_getSdsLinkStatus(uint32 ext_id, uint32 *pSignalDetect, uint32 *pSync, uint32 *pLink);
extern ret_t rtl8367c_setSgmiiNway(uint32 ext_id, uint32 state);
extern ret_t rtl8367c_getSgmiiNway(uint32 ext_id, uint32 *pState);

#endif /*_RTL8367C_ASICDRV_PORTSECURITY_H_*/

