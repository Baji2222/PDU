/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to forwarding function
 *
 * Feature : Response the related request of forwarding function
 *
 */


#ifndef __WEB_FWD_H__
#define __WEB_FWD_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_fwd(struct http_request_s *pReq);
extern int32 get_fwd_jumboframe(struct http_request_s *pReq);
extern int32 get_fwd_stormcontrol(struct http_request_s *pReq);

extern int32 post_fwd_jumboframe(struct http_request_s *pReq);
extern int32 post_fwd_stormcontrol(struct http_request_s *pReq);

#endif  /* __WEB_FWD_H__ */


