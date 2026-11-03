/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to HTTP function
 *
 * Feature : Response the related request of HTTP function
 *
 */


#ifndef __WEB_HTTP_H__
#define __WEB_HTTP_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_httpupg(http_request_t *pReq);
extern int32 post_http_firmupdate(http_request_t *pReq);

#endif  /* __WEB_INFO_H__ */


