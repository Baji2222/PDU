
#ifndef __WEB_FILE_H__
#define __WEB_FILE_H__

/* Header files */
#include <common_types.h>
#include <web/http_types.h>

typedef enum web_file_e {
    WEB_FILE_NONE = 0,
    WEB_FILE_STYLE_CSS,
    WEB_FILE_XP_PROGRESS_JS,
    WEB_FILE_TREE_JS,
    
    WEB_FILE_MAX   /* last one */
} web_file_t;

/* External functions */
extern int8 get_file(struct http_request_s *pReq, int32 file_idx);

#endif  /* __WEB_FILE_H__ */


