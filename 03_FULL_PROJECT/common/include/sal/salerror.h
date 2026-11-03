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
 * Feature : Error Code
 *
 */
#ifndef __SAL_ERROR_H__
#define __SAL_ERROR_H__

#include <saltype.h>

typedef enum {
    SAL_ERR_FAILED = -1,
    SAL_ERR_SUCCESS = 0,

    SAL_ERR_INPUT,
    SAL_ERR_PORT_ID,
    SAL_ERR_SMI,
    SAL_ERR_FILTER_ENTRYIDX,
    SAL_ERR_FILTER_INACL_ACT_NOT_SUPPORT,
    SAL_ERR_FILTER_INACL_RULE_NOT_SUPPORT,
    SAL_ERR_TBL_FULL,
    
    SAL_ERR_ENTRY_INDEX  = 7000,
    SAL_ERR_PORT_RANGE,
    SAL_ERR_PORT_MASK,                          /* Invalid port mask */
    SAL_ERR_TLV_DATA_LENGTH,                    /* The length of data filed in TLV is not accepted */
    SAL_ERR_NOT_SUPPORT,                        /* Action not supported */
    SAL_ERR_ENTRY_NOTFOUND,
    SAL_ERR_NO_EMPTY_ENTRY,
    SAL_ERR_PARAMETER,
    SAL_ERR_NULL_POINTER,
    SAL_ERR_INVALID_RATE,                       /* Invalid rate */

    /* MAC Table */
    SAL_ERR_MAC_ENTRY_INVALID = 7100,            /* Invalid(Empty) MAC entry */    
    SAL_ERR_MAC_ENTRY_NOT_FOUND,                /* MAC entry not found */
    SAL_ERR_NO_EMPTY_MAC_STATIC_ENTRY,         /* No empty static MAC entry */
    SAL_ERR_FLTR_ENTRY_NOT_FOUND,               /* Filter entry not found */
    SAL_ERR_MAC_ENTRY_ADD_FAILED,               /* MAC entry add failed */
    SAL_ERR_MAC_ENTRY_DEL_FAILED,               /* MAC entry delete failed */
    SAL_ERR_FLTR_ENTRY_ADD_FAILED,              /* Filter entry add failed */
    SAL_ERR_FLTR_ENTRY_DEL_FAILED,              /* Filter entry delete failed */
    SAL_ERR_NOT_FLTR_ENTRY,                     /* Not a filter entry */
    SAL_ERR_NOT_MAC_ENTRY,                      /* Not a MAC entry */
    SAL_ERR_MAC_ENTRY_DUPLICATE,                /* There exists another entry with the same mac address and fid */
    
    /* Port Mirror */
    SAL_ERR_MIRROR_GET_FAILED = 7200,                  /* Failed to get port mirror */
    SAL_ERR_MIRROR_SET_FAILED,                  /* Failed to set port mirror */
    SAL_ERR_MIRROR_DIRECTION,			        /* Mirror direction not supported */
    SAL_ERR_MIRROR_SAME_EGRESS,		            /* Two egress mirror set with the same mirroring port */
    SAL_ERR_MIRROR_OVERLAP_SET,	                /* Two mirror set with the same mirroring port should put ingress mirroring in set 0
                                                   and egress mirroring in set 1 */

    /* Storm Control */
    SAL_ERR_STRM_CTRL = 7300,                          /* Storm control error */
    SAL_ERR_STRM_CTRL_INVALID_RATE,             /* Storm control invalid rate */
    SAL_ERR_STRM_CTRL_UNKNOWN_FILTER,           /* Unknown storm control filter */

    /* Trunk */
    SAL_ERR_TRUNK_PORT_USED_BY_TRK = 7400,             /* Port already used by another trunk */
	SAL_ERR_TRUNK_PORT_VLAN_CONFLICT,           /* Joined VLAN of trunk member ports should be the same */
	SAL_ERR_TRUNK_PORT_IN_MIRROR,               /* Mirroring port cannot be a trunk member port */
	SAL_ERR_TRUNK_PORT_RATE_LIMITED,            /* Port with ingress/egress rate limiting cannot be a trunk member port */
	SAL_ERR_TRUNK_PORT_NEGO,                     /* Trunk member ports must all have the same negotiation mode */
	SAL_ERR_TRUNK_PORT_SPEED,                    /* Trunk member ports must all have the same speed */
	SAL_ERR_TRUNK_PORT_DUPLEX,                   /* Trunk member ports must all be full duplex */
	SAL_ERR_TRUNK_TRK_NO_EXIST,                  /* Trunk does not exist */
	SAL_ERR_TRUNK_TRK_RANGE,
	SAL_ERR_TRUNK_NOT_MEMBER_PORT,               /* The port is not a member port of the trunk */
	SAL_ERR_TRUNK_LESS_THAN_2_PORTS,             /* 2 trunk member ports at least */
	SAL_ERR_TRUNK_MORE_THAN_4_PORTS,             /* 4 trunk member ports at most */
	SAL_ERR_TRUNK_PORT_IN_TRUNK,                /* This is link aggregation member port */
    SAL_ERR_TRUNK_PORT_NUM_TOO_LOW,             /* At least two ports should be added */
    SAL_ERR_TRUNK_PORT_NUM_TOO_HIGH,            /* The total port number exceed the limits */

    /*VLAN*/
    SAL_ERR_INPUT_STR_LEN = 7500,                /* Input string length error */
    SAL_ERR_DUPLICATE_NAME,                     /* The name is already existed*/
    SAL_ERR_VLAN_DISABLE,                       /* VLAN is disabled */
    SAL_ERR_VLAN_ID_INVALID,                    /* Invalid VLAN ID */
    SAL_ERR_VLAN_ENTRY_INDEX,                   /* VLAN table index error */
    SAL_ERR_VLAN_FULL,                          /* VLAN table is full */
    SAL_ERR_VLAN_EXIST_PVID,                    /* Port-based VLAN ID is existed*/
    SAL_ERR_VLAN_EXIST,                         /* VLAN is existed */
    SAL_ERR_VLAN_NOT_EXIST,                     /* VLAN is not existed */
    SAL_ERR_VLAN_INS_VLAN_ENTRY,                /* Insert VLAN entry error */
    SAL_ERR_VLAN_DEL_VLAN_ENTRY,                /* Delete VLAN entry error */
    SAL_ERR_VLAN_DEL_DEFAULT_VLAN,              /* Default VLAN can not be deleted */
    SAL_ERR_VLAN_ENTRY_INVALID,                 /* Invalid VLAN entry, mostly due to empty entry */
    SAL_ERR_VLAN_ENTRY_DUPLICATE,               /* There exists another entry with the same vid */

    /*Private VLAN*/
    SAL_ERR_PRIMARY_VLAN_NOT_EXIST = 7550,     /* No primary vlan */
    SAL_ERR_COMMUNITY_VLAN_FULL,               /* Community vlan is full */
    SAL_ERR_PVLAN_ENABLE_CHECK,                /*pvlan should be checked enabled*/
    SAL_ERR_PVLAN_MEMBER_CHECK,                /*At least on port should be included */

    /*SVLAN*/
    SAL_ERR_SVLAN_ENTRY_INDEX = 7600,           /* Invalid svid entry NO.           */                                       
    SAL_ERR_SVLAN_ETHER_TYPE,                   /* Invalid SVLAN ether type         */                                 
    SAL_ERR_SVLAN_TABLE_FULL,                   /* No empty entry in SVLAN table    */                                  
    SAL_ERR_SVLAN_ENTRY_NOT_FOUND,              /* Specified svlan entry not found  */                                 
    SAL_ERR_SVLAN_EXIST,                        /* SVLAN entry is exist             */                               
    SAL_ERR_SVLAN_VID,                          /* Invalid SVID                     */
    SAL_ERR_SVLAN_ENTRY_INVALID,                /* Invalid SVLAN entry, mostly due to empty entry */
    SAL_ERR_SVLAN_ENTRY_DUPLICATE,              /* There exists another entry with the same svid */
    SAL_ERR_SVLAN_ENTRY_NULL,                   /* The operation will cause svlan entry to be null */    
    
    /* Jumbo frame */
    SAL_ERR_JUMBO_FRAME_SIZE = 7700,                   /* Jumbo frame size not supported */
    SAL_ERR_JUMBO_FRAME_GET,                    /* Failed to get jumbo frame size */

    /* Port isolation */
    SAL_ERR_PORT_ISOLATION_SET = 7800,                 /* Failed to set port isolation */
    SAL_ERR_PORT_ISOLATION_GET,                 /* Failed to get port isolation */

    SAL_ERR_UNKNOWN/*Keep Last*/
} SAL_ERR;

/* Function Name:
 *      sal_err_msg_get
 * Description:
 *      Get error message string from error code.
 * Input:
 *      errCode
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
const char *sal_err_msg_get(int32 errCode);

/*
 * This macro checks the return(stmt) value, if it is not SUCCESS then returns the
 * user supplied error code(errCode)
 */
#define SAL_ERR_CODE(stmt, errCode) \
do { \
    if (stmt != SAL_ERR_SUCCESS) { \
        return errCode; \
    } \
} while(0)

#define SAL_ERR_CODE_TRANS(stmt, oldErr, errCode) \
do { \
    if (stmt == oldErr) { \
        return errCode; \
    } \
} while(0)

/*
 * This macro checks the 'expr', if it is TRUE then it returns the
 * user supplied 'errCode'
 */
#define SAL_ERR_CHK_EXPR(expr, errCode)\
do {\
    if ((int32)(expr)) {\
        return errCode; \
    }\
} while(0)

#endif/*__SAL_ERROR_H__*/

