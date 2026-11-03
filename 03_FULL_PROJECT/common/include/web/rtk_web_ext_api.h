/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file specify APIs which require user to implement
*$Id: rtk_web_ext_api.h,v 1.0 2009/08/03 17:03:40 zhenyang_jin Exp $
*/
#ifndef __RTK_WEB_EXT_API_H__
#define __RTK_WEB_EXT_API_H__

#include <common_types.h>
#include <local/web_config.h>  // local/web_config.h should be defined in user directory
#include <rtk_api/rtk_error.h>
//#include <rtk_api/rtk_api_ext.h>
#include <web/web_api.h>
#include "web_err.h"

/*
 *  User can define his own symbolic constant
 */
/* Web username and password*/
#ifndef FLSH_USR_USERNAME_LEN
#define FLSH_USR_USERNAME_LEN  20
#endif

#ifndef FLSH_USR_PASSWORD_LEN
#define FLSH_USR_PASSWORD_LEN  20
#endif

#ifndef WEB_TRK_GRP_TX_EMPTY    
#define WEB_TRK_GRP_TX_EMPTY    TRUE
#endif
 
// general API
extern int32 rtk_user_reply_asp_send(http_request_t *pReq, http_method_t method, uint8  *pPath);

#endif  
