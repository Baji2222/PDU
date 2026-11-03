/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to loop prevention and detection
 *
 * Feature : Response the related request of loop prevention and detection
 *
 */


#ifndef __WEB_EEE_H__
#define __WEB_EEE_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_eee(struct http_request_s *pReq);
extern int32 post_eee(struct http_request_s *pReq);
extern int32 get_mac_setup(struct http_request_s *pReq);

#endif  /* __WEB_EEE_H__ */


