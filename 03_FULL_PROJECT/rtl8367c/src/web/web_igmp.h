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


#ifndef __WEB_IGMP_H__
#define __WEB_IGMP_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_igmp_dump(struct http_request_s *pReq);
extern int32 post_igmp_state(struct http_request_s *pReq);
extern int32 post_igmp_static_router(struct http_request_s *pReq);

#endif  /* __WEB_IGMP_H__ */


