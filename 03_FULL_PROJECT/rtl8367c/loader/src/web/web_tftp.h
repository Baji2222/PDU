/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to TFTP function
 *
 * Feature : Response the related request of TFTP function
 *
 */


#ifndef __WEB_TFTP_H__
#define __WEB_TFTP_H__

/*
 * Include Files
 */
#include <local/config.h>
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_tftp(struct http_request_s *pReq);
extern int32 get_tftp_fwupg_status(struct http_request_s *pReq);

extern int32 post_tftp_fwupg(struct http_request_s *pReq);

#endif  /* __WEB_INFO_H__ */


