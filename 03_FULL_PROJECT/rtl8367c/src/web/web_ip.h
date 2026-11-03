/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to IP function
 *
 * Feature : Response the related request of IP function
 *
 */


#ifndef __WEB_IP_H__
#define __WEB_IP_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_ip(struct http_request_s *pReq);
extern int32 post_ip(struct http_request_s *pReq);

#endif  /* __WEB_IP_H__ */


