#ifndef _RTL8367C_ASICDRV_EAV_H_
#define _RTL8367C_ASICDRV_EAV_H_

#include <rtl8367c_asicdrv.h>

typedef enum RTL8367C_PTP_TIME_CMD_E
{
    PTP_TIME_READ = 0,
    PTP_TIME_WRITE,
    PTP_TIME_INC,
    PTP_TIME_DEC,
    PTP_TIME_CMD_END
}RTL8367C_PTP_TIME_CMD;

typedef enum RTL8367C_PTP_TIME_ADJ_E
{
    PTP_TIME_ADJ_INC = 0,
    PTP_TIME_ADJ_DEC,
    PTP_TIME_ADJ_END
}RTL8367C_PTP_TIME_ADJ;

typedef enum RTL8367C_PTP_TIME_CTRL_E
{
    PTP_TIME_CTRL_STOP = 0,
    PTP_TIME_CTRL_START,
    PTP_TIME_CTRL_END
}RTL8367C_PTP_TIME_CTRL;

typedef enum RTL8367C_PTP_INTR_IMRS_E
{
    PTP_IMRS_TX_SYNC,
    PTP_IMRS_TX_DELAY_REQ,
    PTP_IMRS_TX_PDELAY_REQ,
    PTP_IMRS_TX_PDELAY_RESP,
    PTP_IMRS_RX_SYNC,
    PTP_IMRS_RX_DELAY_REQ,
    PTP_IMRS_RX_PDELAY_REQ,
    PTP_IMRS_RX_PDELAY_RESP,
    PTP_IMRS_END,
}RTL8367C_PTP_INTR_IMRS;


typedef enum RTL8367C_PTP_PKT_TYPE_E
{
    PTP_PKT_TYPE_TX_SYNC,
    PTP_PKT_TYPE_TX_DELAY_REQ,
    PTP_PKT_TYPE_TX_PDELAY_REQ,
    PTP_PKT_TYPE_TX_PDELAY_RESP,
    PTP_PKT_TYPE_RX_SYNC,
    PTP_PKT_TYPE_RX_DELAY_REQ,
    PTP_PKT_TYPE_RX_PDELAY_REQ,
    PTP_PKT_TYPE_RX_PDELAY_RESP,
    PTP_PKT_TYPE_END,
}RTL8367C_PTP_PKT_TYPE;

typedef struct  rtl8367c_ptp_time_stamp_s{
    uint32 sequence_id;
    uint32 second;
    uint32 nano_second;
}rtl8367c_ptp_time_stamp_t;

#define RTL8367C_PTP_INTR_MASK        0xFF

#define RTL8367C_PTP_PORT_MASK        0x3FF

extern ret_t rtl8367c_setAsicEavMacAddress(ether_addr_t mac);
extern ret_t rtl8367c_getAsicEavMacAddress(ether_addr_t *pMac);
extern ret_t rtl8367c_setAsicEavTpid(uint32 outerTag, uint32 innerTag);
extern ret_t rtl8367c_getAsicEavTpid(uint32* pOuterTag, uint32* pInnerTag);
extern ret_t rtl8367c_setAsicEavSysTime(uint32 second, uint32 nanoSecond);
extern ret_t rtl8367c_getAsicEavSysTime(uint32* pSecond, uint32* pNanoSecond);
extern ret_t rtl8367c_setAsicEavSysTimeAdjust(uint32 type, uint32 second, uint32 nanoSecond);
extern ret_t rtl8367c_setAsicEavSysTimeCtrl(uint32 control);
extern ret_t rtl8367c_getAsicEavSysTimeCtrl(uint32* pControl);
extern ret_t rtl8367c_setAsicEavInterruptMask(uint32 imr);
extern ret_t rtl8367c_getAsicEavInterruptMask(uint32* pImr);
extern ret_t rtl8367c_getAsicEavInterruptStatus(uint32* pIms);
extern ret_t rtl8367c_setAsicEavPortInterruptStatus(uint32 port, uint32 ims);
extern ret_t rtl8367c_getAsicEavPortInterruptStatus(uint32 port, uint32* pIms);
extern ret_t rtl8367c_setAsicEavPortEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicEavPortEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_getAsicEavPortTimeStamp(uint32 port, uint32 type, rtl8367c_ptp_time_stamp_t* timeStamp);

extern ret_t rtl8367c_setAsicEavTrap(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicEavTrap(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicEavEnable(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsicEavEnable(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsicEavPriRemapping(uint32 srcpriority, uint32 priority);
extern ret_t rtl8367c_getAsicEavPriRemapping(uint32 srcpriority, uint32 *pPriority);

#endif /*#ifndef _RTL8367C_ASICDRV_EAV_H_*/

