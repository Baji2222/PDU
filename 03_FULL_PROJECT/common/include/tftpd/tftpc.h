/*
 *  TFTP Client for LwPS
 */

#ifndef __LWPS_TFTPC_H__
#define __LWPS_TFTPC_H__

/*
 * Include Files
 */
//#include <rtk_type.h>
#include <common_types.h>


/*
 * Symbol Definition
 */
#define UDP_PORT_TFTP       (69)
#define TFTPC_CLIENT_MAX    (1)
#define TFTPC_TIMEOUT       (3)
#define TFTPC_BLKSIZE       (1500 - 20 - 8 - 4)     /* MTU - IPHDR - UDPHDR - TFTPHDR */
#define TFTPC_RETRIES       (5)
typedef enum tftpc_status_e {
    TFTPC_STATUS_NONE       = 0x00000000,
    TFTPC_STATUS_RUNNING    = 0x00000001,
    TFTPC_STATUS_TIMEOUT    = 0x00000002,   
    TFTPC_STATUS_RETRY      = 0x00000004, /* retry will be dec 1 if timeout reset */
    TFTPC_STATUS_SUCCESS    = 0x00000020,
    TFTPC_STATUS_FAILED     = 0x00000040,
    TFTPC_STATUS_COMPLETED  = 0x00000080  /* mission completed */
} tftpc_status_t;
typedef enum tftp_opcode_e {
    TFTP_OPCODE_NONE = 0,
    TFTP_OPCODE_RRQ,            /* Read request */
    TFTP_OPCODE_WRQ,            /* Write request */
    TFTP_OPCODE_DATA,           /* Data */
    TFTP_OPCODE_ACK,            /* Acknowledgment */
    TFTP_OPCODE_ERROR,          /* Error */
    TFTP_OPCODE_OACK            /* Option Acknowledgement */
} tftp_opcode_t;
typedef enum tftp_error_e {
    TFTP_ERROR_UNDEF = 0,       /* Undefined error code */
    TFTP_ERROR_NOTFOUND,        /* File not found */
    TFTP_ERROR_ACCESS,          /* Access violation */
    TFTP_ERROR_NOSPACE,         /* Disk full or allocation exceeded */
    TFTP_ERROR_BADOP,           /* Illegal TFTP operation */
    TFTP_ERROR_BADID,           /* Unknown transfer ID */
    TFTP_ERROR_EXISTS,          /* File already exists */
    TFTP_ERROR_NOUSER           /* No such user */
} tftp_error_t;

/*
 * Macro Definition
 */

#define DBG_LWPS_TFTPC_ENABLE    (0)
#define DBG_LWPS_TFTPC_PRINTF    
//#define DBG_LWPS_TFTPC_PRINTF   printf

/*
 * Function Declaration
 */
extern int32 tftpc_init(void) large;
extern int32 tftpc_routine(void) large;
extern void tftpc_timer(uint32 milli_second) large;
extern int32 tftpc_read(uint8 *pIp, uint8 *pFile, uint8 *pDstAddr, uint32 dstSize) large;
extern int32 tftpc_close(int32 slot_id) large;
//extern int32 tftpc_state(int32 slot_id) large;
extern int32 tftpc_status(int32 slot_id) large;
extern uint32 tftpc_cntsize(int32 slot_id) large;

#endif  /* __LWPS_TFTPC_H__ */

