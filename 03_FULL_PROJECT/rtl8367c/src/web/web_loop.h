/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to loop prevention and detection
 *
 * Feature : Response the related request of loop prevention and detection
 *
 */


#ifndef __WEB_LOOP_PREVENT_H__
#define __WEB_LOOP_PREVENT_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>

#define STP_PRI_UNIT_BEGIN  0
#define STP_PRI_UNIT_END    15
#define STP_PRI_UNIT_SIZE   4096

/*
 * Function Declaration
 */
extern int32 get_loop(struct http_request_s *pReq);
extern int32 post_loop(struct http_request_s *pReq);
extern int32 post_loop_port (http_request_t *pReq);

extern int32 get_stp_global(struct http_request_s *pReq);
extern int32 post_stp_global(struct http_request_s *pReq);

extern int32 get_stp_port(struct http_request_s *pReq);
extern int32 post_stp_port(struct http_request_s *pReq);


#endif  /* __WEB_FWD_H__ */

