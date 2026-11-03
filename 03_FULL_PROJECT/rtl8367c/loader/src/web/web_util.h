
#ifndef __WEB_UTIL_H__
#define __WEB_UTIL_H__

/* Header files */
#include <local/config.h>
#include <web/http_types.h>

/* External functions */
extern void printerr (struct http_request_s *pReq, unsigned short errno);
extern void printmsg (struct http_request_s *pReq, char * msg);

char * strnstr (const char *haystack, const char *needle, uint16 hlen);
#endif  /* __WEB_UTIL_H__ */

