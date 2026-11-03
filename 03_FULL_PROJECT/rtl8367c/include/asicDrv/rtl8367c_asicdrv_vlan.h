#ifndef _RTL8367C_ASICDRV_VLAN_H_
#define _RTL8367C_ASICDRV_VLAN_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <rtl8367c_asicdrv.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL8367C_PROTOVLAN_GIDX_MAX 3
#define RTL8367C_PROTOVLAN_GROUPNO  4

#define RTL8367C_VLAN_BUSY_CHECK_NO     (10)

#define RTL8367C_VLAN_MBRCFG_LEN    (4)
#define RTL8367C_VLAN_4KTABLE_LEN   (3)

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef struct  VLANCONFIGUSER
{
    uint16  evid;
    uint16  mbr;
    uint16  fid_msti;
    uint16  envlanpol;
    uint16  meteridx;
    uint16  vbpen;
    uint16  vbpri;
}rtl8367c_vlanconfiguser;

typedef struct  USER_VLANTABLE{

    uint16  vid;
    uint16  mbr;
    uint16  untag;
    uint16  fid_msti;
    uint16  envlanpol;
    uint16  meteridx;
    uint16  vbpen;
    uint16  vbpri;
    uint16  ivl_svl;

}rtl8367c_user_vlan4kentry;

typedef enum
{
    FRAME_TYPE_BOTH = 0,
    FRAME_TYPE_TAGGED_ONLY,
    FRAME_TYPE_UNTAGGED_ONLY,
    FRAME_TYPE_MAX_BOUND
} rtl8367c_accframetype;

typedef enum
{
    EG_TAG_MODE_ORI = 0,
    EG_TAG_MODE_KEEP,
    EG_TAG_MODE_PRI_TAG,
    EG_TAG_MODE_REAL_KEEP,
    EG_TAG_MODE_END
} rtl8367c_egtagmode;

typedef enum
{
    PPVLAN_FRAME_TYPE_ETHERNET = 0,
    PPVLAN_FRAME_TYPE_LLC,
    PPVLAN_FRAME_TYPE_RFC1042,
    PPVLAN_FRAME_TYPE_END
} rtl8367c_provlan_frametype;

enum RTL8367C_STPST
{
    STPST_DISABLED = 0,
    STPST_BLOCKING,
    STPST_LEARNING,
    STPST_FORWARDING
};

enum RTL8367C_RESVIDACT
{
    RES_VID_ACT_UNTAG = 0,
    RES_VID_ACT_TAG,
    RES_VID_ACT_END
};

typedef struct
{
    rtl8367c_provlan_frametype  frameType;
    uint32                      etherType;
} rtl8367c_protocolgdatacfg;

typedef struct
{
    uint32 valid;
    uint32 vlan_idx;
    uint32 priority;
} rtl8367c_protocolvlancfg;

extern ret_t rtl8367c_setAsicVlanMemberConfig(uint32 index, rtl8367c_vlanconfiguser *pVlanCg);
extern ret_t rtl8367c_getAsicVlanMemberConfig(uint32 index, rtl8367c_vlanconfiguser *pVlanCg);
extern ret_t rtl8367c_setAsicVlan4kEntry(rtl8367c_user_vlan4kentry *pVlan4kEntry );
extern ret_t rtl8367c_getAsicVlan4kEntry(rtl8367c_user_vlan4kentry *pVlan4kEntry );
extern ret_t rtl8367c_setAsicVlanAccpetFrameType(uint32 port, rtl8367c_accframetype frameType);
extern ret_t rtl8367c_getAsicVlanAccpetFrameType(uint32 port, rtl8367c_accframetype *pFrameType);
extern ret_t rtl8367c_setAsicVlanIngressFilter(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicVlanIngressFilter(uint32 port, uint32 *pEnable);
extern ret_t rtl8367c_setAsicVlanEgressTagMode(uint32 port, rtl8367c_egtagmode tagMode);
extern ret_t rtl8367c_getAsicVlanEgressTagMode(uint32 port, rtl8367c_egtagmode *pTagMode);
extern ret_t rtl8367c_setAsicVlanPortBasedVID(uint32 port, uint32 index, uint32 pri);
extern ret_t rtl8367c_getAsicVlanPortBasedVID(uint32 port, uint32 *pIndex, uint32 *pPri);
extern ret_t rtl8367c_setAsicVlanProtocolBasedGroupData(uint32 index, rtl8367c_protocolgdatacfg *pPbCfg);
extern ret_t rtl8367c_getAsicVlanProtocolBasedGroupData(uint32 index, rtl8367c_protocolgdatacfg *pPbCfg);
extern ret_t rtl8367c_setAsicVlanPortAndProtocolBased(uint32 port, uint32 index, rtl8367c_protocolvlancfg *pPpbCfg);
extern ret_t rtl8367c_getAsicVlanPortAndProtocolBased(uint32 port, uint32 index, rtl8367c_protocolvlancfg *pPpbCfg);
extern ret_t rtl8367c_setAsicVlanFilter(uint32 enabled);
extern ret_t rtl8367c_getAsicVlanFilter(uint32* pEnabled);

extern ret_t rtl8367c_setAsicPortBasedFid(uint32 port, uint32 fid);
extern ret_t rtl8367c_getAsicPortBasedFid(uint32 port, uint32* pFid);
extern ret_t rtl8367c_setAsicPortBasedFidEn(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicPortBasedFidEn(uint32 port, uint32* pEnabled);
extern ret_t rtl8367c_setAsicSpanningTreeStatus(uint32 port, uint32 msti, uint32 state);
extern ret_t rtl8367c_getAsicSpanningTreeStatus(uint32 port, uint32 msti, uint32* pState);
extern ret_t rtl8367c_setAsicVlanUntagDscpPriorityEn(uint32 enabled);
extern ret_t rtl8367c_getAsicVlanUntagDscpPriorityEn(uint32* enabled);
extern ret_t rtl8367c_setAsicVlanTransparent(uint32 port, uint32 portmask);
extern ret_t rtl8367c_getAsicVlanTransparent(uint32 port, uint32 *pPortmask);
extern ret_t rtl8367c_setAsicVlanEgressKeep(uint32 port, uint32 portmask);
extern ret_t rtl8367c_getAsicVlanEgressKeep(uint32 port, uint32* pPortmask);
extern ret_t rtl8367c_setReservedVidAction(uint32 vid0Action, uint32 vid4095Action);
extern ret_t rtl8367c_getReservedVidAction(uint32 *pVid0Action, uint32 *pVid4095Action);
extern ret_t rtl8367c_setRealKeepRemarkEn(uint32 enabled);
extern ret_t rtl8367c_getRealKeepRemarkEn(uint32 *pEnabled);
extern ret_t rtl8367c_resetVlan(void);

#endif /*#ifndef _RTL8367C_ASICDRV_VLAN_H_*/

