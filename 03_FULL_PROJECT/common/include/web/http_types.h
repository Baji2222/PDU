/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file provides internal types for web smart
*$Id: http_types.h,v 1.0 2009/08/03 17:03:40 zhenyang_jin Exp $
*/

#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <common_types.h>
#include <local/config.h>
#include <local/web_config.h>

/*
 * Symbol Definition
 */
#define HTTPD_REQ_PATH_MAX      (96)
#define HTTPD_VAR_NUM_MAX       (80)
#define HTTPD_RESP_BUF_SIZE     (1460)
#define HTTPD_RESP_PRTBUF_SIZE  (256)
#define HTTPD_MULTIPART_BOUNDARY_MAX (64)

typedef struct http_variable_s {
    int16  name_offset;
    uint8  value_offset;
    uint8  value_length;
}http_variable_t;

struct http_request_s {
    int32  socket_id;  /* [FIXME] */
    int32  method;
    uint8  path[HTTPD_REQ_PATH_MAX];

    uint8  *pMethod;
    uint8  *pPath;
    uint8  *pProtocol;
    uint8  *pHeader;
    uint8  *pBody;

    /* Headers */
    uint8  *pHdrAuthorization;
#if  __WEB_UPLOAD__
    uint8  *pHdrContentType;
    uint8  multipart_boundary_str[HTTPD_MULTIPART_BOUNDARY_MAX];
#endif  /* __WEB_UPLOAD__ */
    uint8  *pHdrCookie;
    int32  hdrContentLength;
    int32  hdrIfModifiedSince;

    /* GET/POST values */
    int16  Var_Count;
    int16  Var_Temp;
    struct http_variable_s Var[HTTPD_VAR_NUM_MAX];
    uint8  *pVar_Temp;

    uint8 *upload_data;
    int32  upload_len;
    int32  upload_offset;   /* sync with client */
    int32  upload_size;     /* sync with client */
    uint8  remoteaddr[4];   /* Remote client IP address */
};

/***************************Image Transfer******************************/

typedef enum http_image_type_e {
    HTTP_IMAGE_CONFIG_RESTORE,      /* restore user configuration file into switch*/
    HTTP_IMAGE_FIRMWARE_UPDATE,     /* bin file update  */
    HTTP_IMAGE_TYPE_END
}http_image_type_t;

typedef enum http_image_stat_e{
    HTTP_IMAGE_IDLE,        /* ready to accept image    */
    HTTP_IMAGE_TRANSFERING, /* transfering image        */
    HTTP_IMAGE_ERROR,       /* an error has occured     */ 
    HTTP_IMAGE_FINISHED,       /* an error has occured     */  
    HTTP_IMAGE_STAT_END
}http_image_stat_t;

#endif
