
#ifndef __WEB_HIDDEN_H__
#define __WEB_HIDDEN_H__

/* Header files */
#include <common_types.h>
#include <web/http_types.h>

/* External functions */
extern int8 get_hidden(struct http_request_s *pReq);
extern int8 get_goback(struct http_request_s *pReq);
extern int8 get_alert(struct http_request_s *pReq, uint8 *pMsg);

#endif  /* __WEB_HIDDEN_H__ */

