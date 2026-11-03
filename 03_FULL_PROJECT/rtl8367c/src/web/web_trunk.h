
#ifndef __WEB_TRUNK_H__
#define __WEB_TRUNK_H__

/* Header files */
#include <web/http_types.h>

/* External functions */
extern int32 get_trunk(struct http_request_s *pReq);
extern int32 get_trunk_distributionalgorithm(struct http_request_s *pReq);
extern int32 post_trunk (struct http_request_s *pReq);
extern int32 post_trunk_remove (struct http_request_s *pReq);
extern int32 post_trunk_distribution_algorithm (struct http_request_s *pReq);

#endif  /* __WEB_TRUNK_H__ */


