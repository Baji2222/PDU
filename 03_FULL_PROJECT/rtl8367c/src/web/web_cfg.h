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


#ifndef __WEB_CFG_H__
#define __WEB_CFG_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_http_config(http_request_t *pReq);
extern int32 post_http_restore(http_request_t *pReq);

#endif  /* __WEB_FWD_H__ */


