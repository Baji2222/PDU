
#ifndef __WEB_INFO_H__
#define __WEB_INFO_H__

/* Header files */
#include <common_types.h>
#include <web/http_types.h>

#define FIRMWARE_VER_LEN        24
#define FIRMWARE_DATE_LEN       24
#define HARDWARE_DATE_LEN       24

/* External functions */
extern int32 get_info(struct http_request_s *pReq);
extern int32 get_cable_diag(struct http_request_s *pReq);

#endif  /* __WEB_INFO_H__ */


