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


#ifndef __WEB_SYSLOGBUFSEC_H__
#define __WEB_SYSLOGBUFSEC_H__

/*
 * Include Files
 */
#include <local/config.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_syslogbufsec(struct http_request_s *pReq);
extern int32 post_syslogbufsec(struct http_request_s *pReq);

#endif  /* __WEB_SYSLOGBUFSEC_H__ */



