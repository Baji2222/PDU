/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to system function
 *
 * Feature : Response the related request of system function
 *
 */


#ifndef __WEB_SYS_H__
#define __WEB_SYS_H__

/*
 * Include Files
 */
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_reboot(struct http_request_s *pReq);
extern int32 get_reset(struct http_request_s *pReq);
extern int32 get_save(struct http_request_s *pReq);

extern int32 post_reboot(struct http_request_s *pReq);
extern int32 post_factdefault_load(struct http_request_s *pReq);

#endif  /* __WEB_SYS_H__ */

