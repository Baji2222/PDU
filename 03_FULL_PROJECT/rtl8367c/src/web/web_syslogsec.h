/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to syslog
 *
 * Feature : Response the related request of syslog
 *
 */


#ifndef __WEB_SYSLOGSEC_H__
#define __WEB_SYSLOGSEC_H__

/*
 * Include Files
 */
#include <local/config.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_syslogsec(struct http_request_s *pReq);
extern int32 post_syslogsec(struct http_request_s *pReq);

#endif  /* __WEB_SYSLOGSEC_H__ */



