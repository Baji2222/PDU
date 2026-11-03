
#ifndef __WEB_IMAGE_H__
#define __WEB_IMAGE_H__

/* Header files */
#include <common_types.h>
#include <web/http_types.h>

typedef enum web_image_e {
    WEB_IMAGE_NONE = 0,
    WEB_IMAGE_LOGO_PNG,
    WEB_IMAGE_RJ45_DOWN_0_PNG,
    WEB_IMAGE_RJ45_UP_0_PNG,
    WEB_IMAGE_RJ45_DOWN_1_PNG,
    WEB_IMAGE_RJ45_UP_1_PNG,
    WEB_IMAGE_RJ45_LOOP_RLDP_UP_PNG,
    WEB_IMAGE_RJ45_LOOP_RLPP_UP_PNG,
    WEB_IMAGE_DOCUMENT_GIF,
    WEB_IMAGE_FOLDER_OPENED_GIF,
    WEB_IMAGE_FOLDER_CLOSED_GIF,
    WEB_IMAGE_PLUS_GIF,
    WEB_IMAGE_MINUS_GIF,
    WEB_IMAGE_TRANSPARENT_GIF,
    
    WEB_IMAGE_MAX   /* last one */
} web_image_t;

/* External functions */
extern int8 get_image(struct http_request_s *pReq, int32 image_idx);

#endif  /* __WEB_IMAGE_H__ */

