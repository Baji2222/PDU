/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file provides internal types for web smart
*$Id: web_api.h,v 1.0 2009/08/03 17:03:40 zhenyang_jin Exp $
*/
#ifndef __WEB_API_H__
#define __WEB_API_H__

#include <common_types.h>
#include "web_err.h"
#define DUMMY_CACHE_SIZE  1024

typedef struct http_request_s http_request_t;

typedef enum http_method_e {
    HTTP_METHOD_GET = 1,
    HTTP_METHOD_POST
} http_method_t;

typedef enum http_status_e {
    /* Success */
    HTTP_STATUS_OK                      = 200,
    HTTP_STATUS_CREATED                 = 201,
    HTTP_STATUS_ACCEPTED                = 202,
    HTTP_STATUS_NO_CONTENT              = 204,
    /* Redirection */
    HTTP_STATUS_MOVED_PERMANENTLY       = 301,
    HTTP_STATUS_MOVED_TEMPORARILY       = 302,
    HTTP_STATUS_NOT_MODIFIED            = 304,
    /* Client Error */
    HTTP_STATUS_BAD_REQUEST             = 400,
    HTTP_STATUS_UNAUTHORIZED            = 401,
    HTTP_STATUS_FORBIDDEN               = 403,
    HTTP_STATUS_NOT_FOUND               = 404,
    HTTP_STATUS_METHOD_NOT_ALLOWED      = 405,
    HTTP_STATUS_NOT_ACCEPTABLE          = 406,
    /* Server Error */
    HTTP_STATUS_INTERNAL_SERVER_ERROR   = 500,
    HTTP_STATUS_NOT_IMPLEMENTED         = 501,
    HTTP_STATUS_SERVICE_UNAVAILABLE     = 503
} http_status_t;

/* API Function */
extern int32 http_reqVariable(http_request_t *pReq, uint8 *pName, uint8 **ppValue);
extern int32 http_respBegin(http_request_t *pReq);
extern int32 http_respEnd(http_request_t *pReq);
extern int32 http_respStatus(http_request_t *pReq, int32 status);
extern int32 http_respHeader(http_request_t *pReq, uint8 *pHeader);
extern int32 http_respData(http_request_t *pReq, const uint8 *pData, int32 length);
extern int32 http_respData_all_config(http_request_t *pReq, const uint8 *pData, int32 length);
extern int32 http_respPrintf(http_request_t *pReq, const char *format, ...);

#define WEB_VAR(pReq,pName,ppValue)     http_reqVariable(pReq, pName, ppValue)
#define WEB_BEGIN(pReq)                 http_respBegin(pReq)
#define WEB_END(pReq)                   http_respEnd(pReq)
#define WEB_STATUS(pReq,status)         http_respStatus(pReq, status)
#define WEB_HEADER(pReq,pHeader)        http_respHeader(pReq, pHeader)
#define WEB_DATA(pReq,pData,length)     http_respData(pReq, pData, length)
#define WEB_DATA_CONFIG(pReq,pData,length)     http_respData_all_config(pReq, pData, length)
#define WEB_PRINTF                      http_respPrintf

#ifndef OSAL_PRINTF
#define OSAL_PRINTF printf
#endif

#define CONFIG_WEB_DEBUG    (0)
#if CONFIG_WEB_DEBUG
  #define DBG_WEB_PRINTF(x)   do { OSAL_PRINTF x;}while(0)
#else
  #define DBG_WEB_PRINTF(x)
#endif
    
#endif

