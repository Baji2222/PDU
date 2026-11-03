/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to MAC function
 *
 * Feature : Response the related request of MAC function
 *
 */


#ifndef __WEB_MAC_H__
#define __WEB_MAC_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <web/http_types.h>


/*
 * Function Declaration
 */
extern int32 get_mac(struct http_request_s *pReq);
extern int32 get_mac_forwardingtable(struct http_request_s *pReq);
extern int32 get_mac_staticmac(struct http_request_s *pReq);
extern int32 get_mac_search(struct http_request_s *pReq);

extern int32 post_mac_del(struct http_request_s *pReq);
extern int32 post_mac_add(struct http_request_s *pReq);
extern int32 post_mac_tblclear(struct http_request_s *pReq);
extern int32 post_mac_search(struct http_request_s *pReq);

extern int32 get_mac_constraint(struct http_request_s *pReq);
extern int32 post_mac_constraint(struct http_request_s *pReq);
extern int32 post_mac_constraint_action(struct http_request_s *pReq);

#endif  /* __WEB_MAC_H__ */


