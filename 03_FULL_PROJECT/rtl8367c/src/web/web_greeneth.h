/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to green ethernet and detection
 *
 * Feature : Response the related request of green ethernet and detection
 *
 */


#ifndef __WEB_GREENETH_H__
#define __WEB_GREENETH_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_greeneth(http_request_t *pReq);
extern int32 post_greeneth(http_request_t *pReq);

#endif  /* __WEB_GREENETH_H__ */


