#pragma STRING (far)

#include <stdarg.h>
#include <basic_sfr.h>

#include <web/http_debug.h>
#include <web/http_types.h>
#include <web/web_api.h>
#include <lwps/lwps_api.h>
#include <web/httpd.h>

#if WEB_ENABLE

extern uint8  http_request_buf[]; /* with terminated charator */
extern int32  http_request_len;
extern int32  http_request_sid;
extern uint8  http_variable_value[];
extern uint8  http_bufResponse[];
extern uint8 *http_pRespBuf;
extern uint8 *http_pRespHeader;
extern uint8 *http_pRespBody;
extern int32  http_cntRespTimes;
extern uint8  http_bufPrintf[];
extern int32  http_cntPrintf;

static uint8 http_variable_value[64];

#if (defined( LWS_A_SET) || defined( UPGRADE_TFTP))

// ASCII Hex to Value: Converts ASCII hex character to numeric value
// Returns 0-15 for hex digits, 0 for invalid characters
static char asc_hex2val(char hex)
{
    if ((hex >= '0') && (hex <= '9')) {
        return (hex - '0');
    } else if ((hex >= 'A') && (hex <= 'F')) {
        return (hex + 10 - 'A');
    } else if ((hex >= 'a') && (hex <= 'f')) {
        return (hex + 10 - 'a');
    }

    return 0;
}

/*
 * unencode -- convert to ordinary string
 *
 * Parameters:
 *   src: pointer to the start of the string to convert
 *   last: pointer to the end of the string to convert
 *   dest: destination of the converted string to be placed
 */
// Unencode: Converts URL-encoded string to plain text
// Handles '+' to space and '%XX' hex decoding for HTTP form data
static void
unencode (
    char *  src,
    char *  last,
    char *  dest
    )
{
    for(; src != last; src++, dest++) {
        if (*src == '+') {
            *dest = ' ';
        }
        else if (*src == '%') {
#if 1
            *dest = (asc_hex2val(*(src+1)) * 16) + asc_hex2val(*(src+2));
#else
            int code;

            if (sscanf(src+1, "%2x", &code) != 1) {
                code = '?';
            }
            *dest = code;
#endif
            src += 2;
        }
        else {
            *dest = *src;
        }
    }
    *dest = '\0';
}

// HTTP Request Variable: Extracts variable value from HTTP request
// Searches for variable by name and returns decoded value
int32 http_reqVariable(struct http_request_s *pReq, uint8 *pName, uint8 **ppValue)
{
    int32 i;
        
    //if ((pReq == NULL) || (pName == NULL) || (ppValue == NULL))
    if ((pReq == NULL) || (ppValue == NULL))
        return FAILED;

    /* restart? */
    if (pName != NULL) {
        pReq->pVar_Temp = pName;
        pReq->Var_Temp = 0;
    }

    /* Search Variables */
    for (i=pReq->Var_Temp; i<pReq->Var_Count; i++)
    {
         //if (strlen(pReq->pVar_Temp) == pReq->Var[i].nLen)
         if (strlen(pReq->pVar_Temp) == (pReq->Var[i].value_offset - 1))
            //if (0 == strncmp(pReq->pVar_Temp, pReq->Var[i].pName, strlen(pReq->pVar_Temp)))
            if (0 == strncmp(pReq->pVar_Temp, pReq->pMethod + pReq->Var[i].name_offset, strlen(pReq->pVar_Temp)))
            {
#if 1
                //unencode(pReq->Var[i].pValue, pReq->Var[i].pValue + pReq->Var[i].vLen, &http_variable_value[0]);
                unencode(pReq->pMethod + pReq->Var[i].name_offset + pReq->Var[i].value_offset, pReq->pMethod + pReq->Var[i].name_offset + pReq->Var[i].value_offset +  pReq->Var[i].value_length, &http_variable_value[0]);
                *ppValue = &http_variable_value[0];
                pReq->Var_Temp = (i + 1);

                return SUCCESS;
#else
                *ppValue = pReq->Var[i].pValue;

                /* [FIXME] it's fast but must modify the source packet */
                *(pReq->Var[i].pValue + pReq->Var[i].vLen) = '\0';

                pReq->Var_Temp = (i + 1);

                //return (pReq->Var[i].nLen);
                return SUCCESS;
#endif
            }
    }

    *ppValue = NULL;

    //return -1;  /* Not found */
    return FAILED;
}
#endif

// HTTP Response Begin: Initializes HTTP response buffer
// Resets response buffer pointers and counters for new response
int32 http_respBegin(struct http_request_s *pReq)
{
    if (pReq == NULL)
        return FAILED;

#if 0
    /* Initial Response */
    httpd_request.pRespBuf = &httpd_request.bufResponse[0];
    httpd_request.pRespHeader = NULL;
    httpd_request.pRespBody = NULL;
#endif

    http_pRespBuf = &http_bufResponse[0];
    http_pRespHeader = NULL;
    http_pRespBody = NULL;
    http_cntRespTimes = 0;

    return SUCCESS;
}

// HTTP Response End: Finalizes HTTP response transmission
// Sends any remaining buffered data to the client
int32 http_respEnd(struct http_request_s *pReq)
{
    if (pReq == NULL)
        return FAILED;

    /* left the others data? */
    if (http_pRespBuf != &http_bufResponse[0])
    {
        lwps_send(pReq->socket_id, http_bufResponse, (http_pRespBuf - &http_bufResponse[0]));
    }

    return SUCCESS;
}

// HTTP Response Status: Sets HTTP status line in response
// Builds status line with HTTP version and status code/message
int32 http_respStatus(struct http_request_s *pReq, int32 status)
{
    if (pReq == NULL)
        return FAILED;

#if 1
    /* already responsed? */
    if (http_cntRespTimes > 0)
        return FAILED;
#else
    if (http_pRespHeader != NULL) {
        http_pRespBuf = &http_bufResponse[0];
    }
#endif

    http_pRespBuf = &(http_bufResponse[0]);

    /* Resp Protocol */
    http_pRespBuf += sprintf(http_pRespBuf, "HTTP/1.1 ");

    switch (status)
    {
    /* Success */
    case HTTP_STATUS_OK:
        http_pRespBuf += sprintf(http_pRespBuf, "200 OK\r\n");
        break;

    case HTTP_STATUS_CREATED:
        http_pRespBuf += sprintf(http_pRespBuf, "201 Created\r\n");
        break;

    case HTTP_STATUS_ACCEPTED:
        http_pRespBuf += sprintf(http_pRespBuf, "202 Accepted\r\n");
        break;

    case HTTP_STATUS_NO_CONTENT:
        http_pRespBuf += sprintf(http_pRespBuf, "204 No content\r\n");
        break;

    /* Redirection */
    case HTTP_STATUS_MOVED_PERMANENTLY:
        http_pRespBuf += sprintf(http_pRespBuf, "301 Moved permanently\r\n");
        break;

    case HTTP_STATUS_MOVED_TEMPORARILY:
        http_pRespBuf += sprintf(http_pRespBuf, "302 Moved temporarily\r\n");
        break;

    case HTTP_STATUS_NOT_MODIFIED:
        http_pRespBuf += sprintf(http_pRespBuf, "304 Not modified\r\n");
        break;

    /* Client Error */
    case HTTP_STATUS_BAD_REQUEST:
        http_pRespBuf += sprintf(http_pRespBuf, "400 Bad request\r\n");
        break;

    case HTTP_STATUS_UNAUTHORIZED:
        http_pRespBuf += sprintf(http_pRespBuf, "401 Unauthorized\r\n");
        break;

    case HTTP_STATUS_FORBIDDEN:
        http_pRespBuf += sprintf(http_pRespBuf, "403 Forbidden\r\n");
        break;

    case HTTP_STATUS_NOT_FOUND:
        http_pRespBuf += sprintf(http_pRespBuf, "404 Not found\r\n");
        break;

    case HTTP_STATUS_METHOD_NOT_ALLOWED:
        http_pRespBuf += sprintf(http_pRespBuf, "405 Method not allowed\r\n");
        break;

    case HTTP_STATUS_NOT_ACCEPTABLE:
        http_pRespBuf += sprintf(http_pRespBuf, "406 Not acceptable\r\n");
        break;

    /* Server Error */
    case HTTP_STATUS_INTERNAL_SERVER_ERROR:
        http_pRespBuf += sprintf(http_pRespBuf, "500 Internal server error\r\n");
        break;

    case HTTP_STATUS_NOT_IMPLEMENTED:
        http_pRespBuf += sprintf(http_pRespBuf, "501 Not implemented\r\n");
        break;

    case HTTP_STATUS_SERVICE_UNAVAILABLE:
        http_pRespBuf += sprintf(http_pRespBuf, "503 Server unavailable\r\n");
        break;

    default:
        OSAL_PRINTF("%s():%d HTTP_STATUS (%d) is NOT supported!\n", __FILE__, (uint16)__LINE__, status);
        return FAILED;
        break;
    }

    /* Update pointer */
    http_pRespHeader = http_pRespBuf;

    return SUCCESS;
}

// HTTP Response Header: Adds HTTP header field to response
// Appends header line and manages connection close header
int32 http_respHeader(struct http_request_s *pReq, uint8 *pHeader)
{
    if (pReq == NULL)
        return FAILED;

    /* is the status not ready? */
    if (http_pRespHeader == NULL)
        return FAILED;

    /* ever call the respData()? */
    if ((http_pRespBody != NULL) && (http_pRespBody != http_pRespBuf))
        return FAILED;

    /* force close this connection at first time */
    if (http_pRespHeader == http_pRespBuf) {
        http_pRespBuf += sprintf(http_pRespBuf, "Connection: close\r\n");
    }

#if 0
    http_pRespBuf += sprintf(http_pRespBuf, "%s\r\n", pHeader);

    /* [FIXME] only support 1 header now */
    http_pRespBuf += sprintf(http_pRespBuf, "\r\n");
#else
    if (http_pRespBody)
    {   /* append (not first time) */
        http_pRespBuf -= 2;    /* backspace */
    }
    http_pRespBuf += sprintf(http_pRespBuf, "%s\r\n\r\n", pHeader);
#endif

    /* Update pointer */
    http_pRespBody = http_pRespBuf;

    return SUCCESS;
}

// HTTP Response Data: Sends response body data to client
// Handles chunked transmission when data exceeds buffer size
int32 http_respData(struct http_request_s *pReq, const uint8 *pData, int32 length)
{
    int32 size = length;

    if (pReq == NULL)
        return FAILED;
    ET0   = 0;// performance tune, don't remove

    /* is the status/header not ready? */
    if (http_pRespBody == NULL)
        return FAILED;

    /* over a buffer? */
    while ((http_pRespBuf + size) >= &http_bufResponse[HTTPD_RESP_BUF_SIZE])
    {
        DBG_LWPS_HTTP_STACK(( "%s():%d pData = %p, length = %d, pRespBuf = %p, size = %d\n", \
            __FILE__, (uint16)__LINE__, pData, length, http_pRespBuf, size));
            
        /* combine a buffer and send it */
        memcpy(http_pRespBuf, pData + (length - size), (&http_bufResponse[HTTPD_RESP_BUF_SIZE] - http_pRespBuf));
        if (LWPS_OK != lwps_send(pReq->socket_id, http_bufResponse, HTTPD_RESP_BUF_SIZE))
        {
            OSAL_PRINTF("%s():%d FATAL - lwps_send failed!\n", __FILE__, (uint16)__LINE__);
            return FAILED;
        }
        size -= (&http_bufResponse[HTTPD_RESP_BUF_SIZE] - http_pRespBuf);

        /* reset the pointer */
        http_pRespBuf = &http_bufResponse[0];
        http_cntRespTimes += 1;
    }

    DBG_LWPS_HTTP_STACK(( "%s():%d pData = %p, length = %d, pRespBuf = %p, size = %d\n", \
        __FILE__, (uint16)__LINE__, pData, length, http_pRespBuf, size));
        
    if (size) {
        memcpy(http_pRespBuf, pData + (length - size), size);
        http_pRespBuf += size;
    }

    DBG_LWPS_HTTP_STACK(( "%s():%d pData = %p, length = %d, pRespBuf = %p, size = %d\n", \
        __FILE__, (uint16)__LINE__, pData, length, http_pRespBuf, size));

    ET0   = 1;// performance tune, don't remove

#if 0
    if ((http_pRespBuf + length) >= &http_bufResponse[HTTPD_RESP_BUF_SIZE])
    {
        /* send once first */
        memcpy(http_pRespBuf, pData, (&http_bufResponse[HTTPD_RESP_BUF_SIZE] - http_pRespBuf));
        if (LWPS_OK != lwps_send(pReq->socket_id, &http_bufResponse[0], HTTPD_RESP_BUF_SIZE)) {
            OSAL_PRINTF("%s():%d FATAL - lwps_send failed!\n", __FILE__, (uint16)__LINE__);
            return FAILED;
        }

        /* reset the pointer */
        http_pRespBuf = &http_bufResponse[0];
        http_cntRespTimes += 1;
    } else {
        memcpy(http_pRespBuf, pData, length);
        http_pRespBuf += length;
    }
#endif

    return SUCCESS;    
}
    
#if (defined( LWS_A_SET) || defined( UPGRADE_TFTP))

// HTTP Response Printf: Formats and sends printf-style response data
// Uses vsprintf to format string and sends via http_respData
int32 http_respPrintf(struct http_request_s *pReq, const char *format, ...)
{
    va_list args;
    va_start(args, format);

#if 1   /* use the global variable */
    http_cntPrintf = vsprintf(http_bufPrintf, format, args);   
    http_respData(pReq, http_bufPrintf, http_cntPrintf);  
#else
    pReq->cntPrintf = vsprintf(pReq->bufPrintf, format, args);   
    http_respData(pReq, pReq->bufPrintf, pReq->cntPrintf);  
#endif
    va_end(args);

    return SUCCESS;
}
#endif
#endif