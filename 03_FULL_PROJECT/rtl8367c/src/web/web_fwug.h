/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to entering loader mode to upgrade firmware
 *
 * Feature : System. Entering Loader Mode
 *
 */


#ifndef __WEB_FWUG_H__
#define __WEB_FWUG_H__

/*
 * Include Files
 */
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_fwug(struct http_request_s *pReq);
extern int32 post_fwug(struct http_request_s *pReq);

#endif  /* __WEB_FWUG_H__ */

