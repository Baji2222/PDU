/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to QoS function
 *
 * Feature : Response the related request of QoS function
 *
 */


#ifndef __WEB_QOS_H__
#define __WEB_QOS_H__

/*
 * Include Files
 */
#include <local/web_config.h>
#include <web/http_types.h>

/*
 * Function Declaration
 */
extern int32 get_qos(struct http_request_s *pReq);
extern int32 get_qos_portbasedpriority(struct http_request_s *pReq);
extern int32 get_qos_packetscheduling(struct http_request_s *pReq);
extern int32 get_qos_internalPriority(struct http_request_s *pReq);
extern int32 get_qos_dscpremapping(struct http_request_s *pReq);
extern int32 get_qos_pri2queueid(struct http_request_s *pReq);

extern int32 post_qos_portbasedpriority(struct http_request_s *pReq);
extern int32 post_qos_packetscheduling(struct http_request_s *pReq);
extern int32 post_qos_queueweight(struct http_request_s *pReq);
extern int32 post_qos_internalPriority(struct http_request_s *pReq);
extern int32 post_qos_dscpremapping(struct http_request_s *pReq);
extern int32 post_qos_pri2queueid(struct http_request_s *pReq);

#if __WEB_DSCP__
extern int32 get_qos_dscpremarking(struct http_request_s *pReq);
extern int32 post_qos_dscpremarking(struct http_request_s *pReq);
#endif

#endif  /* __WEB_QOS_H__ */

