/*
 *  Http Daemon for LwPS
 */
#pragma STRING (far)


/*
 * Include Files
 */

#include <common_types.h>
#include <lwps/lwps_api.h>

#include <common_types.h>
#include <lwps/lwps_api.h>

#include <web/http_debug.h>
#include <web/http_types.h>
#include <web/httpd.h>
#include <web/rtk_web_ext_api.h>
#include "Base64.h" 
#include <html_api.h>
#include <salsys.h>
#include <salflash.h>
#include <web_util.h>
#include <local/web_config.h>
#include "syslog/syslogdefs.h"


/*
 * Symbol Definition
 */
typedef struct httpd_client_s {
    int32  socket_id;
    http_request_t request;
#if  __WEB_UPLOAD__
    int32  upload_offset;
    int32  upload_size;
#endif
} httpd_client_t;

#if 0
typedef struct http_info_s {
    uint8  mothod[16];         /* GET, POST */
    uint8  path[128];          /* URL */
    uint8  protocol[32];       /* HTTP proto */
    uint8  content_type[32];   /* Content-Type */
    uint16 bound_len;
    uint8  boundary[124];      /* Multipart, form-data */
    uint8  cookie[64];         /* Cookie */
    uint8  *post_data;
    uint8  host[32];           /* HOST */
} http_info_t;
#endif
/*
 * Data Declaration
 */
httpd_client_t xdata httpd_client_entry[HTTPD_CLIENT_MAX];
uint8  http_request_buf[HTTPD_REQ_BUF_SIZE + 1]; /* with terminated charator */
int32  http_request_len;
int32  http_request_sid;
uint8  http_variable_value[64];
uint8  http_bufResponse[HTTPD_RESP_BUF_SIZE];
uint8 *http_pRespBuf;
uint8 *http_pRespHeader;
uint8 *http_pRespBody;
int32  http_cntRespTimes;
uint8  http_bufPrintf[HTTPD_RESP_PRTBUF_SIZE];
int32  http_cntPrintf;

/*
 * Function Declaration
 */

// HTTP Request Parse: Parses HTTP request from raw data into structured format
// Extracts method, path, headers, and GET/POST variables from HTTP request
static Tint32 http_request_parse(struct http_request_s *pReq, Tuint8 *pData, Tint32 length)
{
    uint8 *pPos = pData;
    int32 i;

    if ((pReq == NULL) || (pData == NULL) || (length <= 0))
        return FAILED;

    /* PASS 1 */
    pReq->path[0] = '\0';
    pReq->Var_Count = 0;

    /* Parsing */
    pReq->pMethod = pPos;
    for (i=0; i<length; i++) {
        if (*pPos == ' ') {
            if (strncmp(pReq->pMethod, "GET", 3) == 0) {
                pReq->method = HTTP_METHOD_GET;
            } else if (strncmp(pReq->pMethod, "POST", 4) == 0) {
                pReq->method = HTTP_METHOD_POST;
            } else {
                pReq->method = -1;  /* Unknown */
                return FAILED;
            }
            break;
        }
        pPos++;
    }

    pReq->pPath = ++pPos;
    for (; i<length; i++) {
        if (*pPos == '?') {
            if (pReq->path[0] == '\0') {
                if ((pPos - pReq->pPath) < sizeof(pReq->path)) {
                    //memset(&pReq->path[0], 0, sizeof(pReq->path));
                    strncpy(pReq->path, pReq->pPath, (pPos - pReq->pPath));
                    pReq->path[(pPos - pReq->pPath)] = '\0';
                } else {
                    OSAL_PRINTF("Path is too long (%d)\n", (pPos - pReq->pPath));
                    return FAILED;
                }
            }

            /* parse Get variables */
            pPos++;
            //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
            for (; i<length; i++) {
                if (*pPos == ' ') {
                    //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                    break;
                }

                /* has Name */
                //pReq->Var[pReq->Var_Count].pName = pPos;
                pReq->Var[pReq->Var_Count].name_offset = (pPos - pReq->pMethod);
                for (; i<length; i++) {
                    //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                    if (*pPos == '=') {
                        //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                        //pReq->Var[pReq->Var_Count].nLen = (pPos - pReq->Var[pReq->Var_Count].pName);
                        break;
                    }
                    pPos++;
                }
                //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                /* Value */
                //pReq->Var[pReq->Var_Count].pValue = ++pPos;
                pReq->Var[pReq->Var_Count].value_offset = (++pPos) - (pReq->pMethod + pReq->Var[pReq->Var_Count].name_offset);
                for (; i<length; i++) {
                    //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                    if (*pPos == '&' || *pPos == ' ') {
                        //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                        //pReq->Var[pReq->Var_Count].vLen = (pPos - pReq->Var[pReq->Var_Count].pValue);
                        pReq->Var[pReq->Var_Count].value_length = \
                            (pPos - (pReq->pMethod + pReq->Var[pReq->Var_Count].name_offset + pReq->Var[pReq->Var_Count].value_offset));
                        pReq->Var_Count++; /* add var */
                        break;
                    }
                    pPos++;
                }

                //DBG_PRINT(0, "%s():%d pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                if (*pPos == ' ') {
                    break;
                }

                pPos++;
            }
            break;
        } else if (*pPos == ' ') {
            if (pReq->path[0] == '\0') {
                if ((pPos - pReq->pPath) < sizeof(pReq->path)) {
                    strncpy(pReq->path, pReq->pPath, (pPos - pReq->pPath));
                    pReq->path[(pPos - pReq->pPath)] = '\0';
                } else {
                    OSAL_PRINTF("Path is too long (%d)\n", (pPos - pReq->pPath));
                    return FAILED;
                }
            }
            break;
        }
        pPos++;
    }

    pReq->pProtocol = ++pPos;
    for (; i<length; i++) {
        if (*pPos == '\r') {
            break;
        }
        pPos++;
    }

    pReq->pHeader = ++pPos;
    for (; i<length; i++) {
        if (strncmp(pPos - 3, "\r\n\r\n", 4) == 0) {
            break;
        }
        pPos++;
    }

#if 1
    pReq->pBody = ++pPos;

    /* PASS 2: Headers */
    pReq->pHdrAuthorization = NULL;
#if __WEB_UPLOAD__
    pReq->pHdrContentType = NULL;
#endif
    pReq->hdrContentLength = 0;
    pReq->hdrIfModifiedSince = 0;
    for (pPos=pReq->pHeader; pPos<pReq->pBody; pPos++)
    {
        if (strncmp(pPos, "Authorization: ", 15) == 0)
        {
            pReq->pHdrAuthorization = (pPos += 15);
            while (*pPos != '\n')
            {
                pPos++;
            }
        }
#if __WEB_UPLOAD__
        else if (strncmp(pPos, "Content-Type: ", 14) == 0)
        {
            pReq->pHdrContentType = (pPos += 14);
            while (*pPos != '\n')
            {
                pPos++;
            }
        }
#endif
        else if (strncmp(pPos, "Content-Length: ", 16) == 0)
        {
            pPos += 16;
            while (*pPos != '\n')
            {
                if (*pPos != '\r')
                {
                    pReq->hdrContentLength *= 10;
                    pReq->hdrContentLength += (*pPos - '0');
                }
                pPos++;
            }
        }
        /* HTTP Caching */
        else if (strncmp(pPos, "If-Modified-Since: ", 19) == 0)
        {
            pPos += 19;
            while (*pPos != '\n')
            {
                if (*pPos != '\r')
                {
                    pReq->hdrIfModifiedSince = -1;  /* != 0 */
                }
                pPos++;
            }
        }
    }

#if __WEB_UPLOAD__
    pReq->upload_data = NULL;
    pReq->upload_len = 0;
    if ((pReq->pHdrContentType != NULL) && (strncmp(pReq->pHdrContentType, "multipart", 9) == 0))
    {
        int32 boundary_len = strnstr(pReq->pHdrContentType, "\r\n", (pReq->pBody - pReq->pHdrContentType)) \
                  - strnstr(pReq->pHdrContentType, "boundary=", (pReq->pBody - pReq->pHdrContentType)) - 9;
               
        if (HTTPD_MULTIPART_BOUNDARY_MAX <= boundary_len)
        {
            OSAL_PRINTF("%s():%d - Error: HTTPD_MULTIPART_BOUNDARY_MAX is too small!\n", __FILE__, (uint16)__LINE__);
        }
        else
        {
            uint8 *pBegin;

            strncpy(pReq->multipart_boundary_str, strstr(pReq->pHdrContentType, "boundary=") + 9, boundary_len);
            pReq->multipart_boundary_str[boundary_len] = '\0';
            //printf("%s():%d - multipart_boundary_str: %s\n", __FUNCTION__, __LINE__, pReq->multipart_boundary_str);

            if ((pBegin = strnstr(pReq->pBody, pReq->multipart_boundary_str, (pData + length) - pReq->pBody)) != NULL)
            {
                pBegin += boundary_len;
                if ((pBegin = strnstr(pReq->pBody, "\r\n\r\n", (pData + length) - pReq->pBody)) != NULL)
                {
                    pBegin += 4;    /* data begin */

                    pReq->upload_data = pBegin;
                    pReq->upload_len = (pData + length) - pBegin;
                    pReq->upload_offset = 0;
                    pReq->upload_size = pReq->hdrContentLength - (pBegin - pReq->pBody);
                }
            }
        }

        /*printf("%s():%d - @@@ pReq->upload_data = %p, pReq->upload_len = %ld, pReq->upload_offset = %ld, pReq->upload_size = %ld\n", \
            __FILE__, (uint16)__LINE__, pReq->upload_data, pReq->upload_len, pReq->upload_offset, pReq->upload_size);
        */
    }
#endif  /* __WEB_UPLOAD__ */

#if __WEB_UPLOAD__
    if ((pReq->method == HTTP_METHOD_POST) && 
        (0 != strncmp(pReq->pHdrContentType, "multipart", 9)))
#else
    if (pReq->method == HTTP_METHOD_POST)
#endif
    {
        /* parse Post variables */
        //DBG_PRINT(0, "%s():%d [POST_START] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
        for (; i<length; i++) {
            //if (*pPos == ' ') {
            if (*pPos == '\0') {
                //DBG_PRINT(0, "%s():%d [empty?] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                break;
            }
        
            /* has Name */
            //pReq->Var[pReq->Var_Count].pName = pPos;
            pReq->Var[pReq->Var_Count].name_offset = (pPos - pReq->pMethod);
            for (; i<length; i++) {
                //DBG_PRINT(0, "%s():%d [NAME] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                if (*pPos == '=') {
                    //DBG_PRINT(0, "%s():%d [NAME] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                    //pReq->Var[pReq->Var_Count].nLen = (pPos - pReq->Var[pReq->Var_Count].pName);
                    break;
                }
                pPos++;
            }
            //DBG_PRINT(0, "%s():%d [=?] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
            /* Value */
            //pReq->Var[pReq->Var_Count].pValue = ++pPos;
            pReq->Var[pReq->Var_Count].value_offset = (++pPos) - (pReq->pMethod + pReq->Var[pReq->Var_Count].name_offset);
            for (; i<length; i++) {
                //DBG_PRINT(0, "%s():%d [VALUE] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                if (*pPos == '&' || (pPos >= (pData + length))) {
                    //DBG_PRINT(0, "%s():%d [VALUE] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
                    //pReq->Var[pReq->Var_Count].vLen = (pPos - pReq->Var[pReq->Var_Count].pValue);
                    pReq->Var[pReq->Var_Count].value_length = \
                        (pPos - (pReq->pMethod + pReq->Var[pReq->Var_Count].name_offset + pReq->Var[pReq->Var_Count].value_offset));
                    pReq->Var_Count++; /* add var */
                    break;
                }
                pPos++;
            }
        
            //DBG_PRINT(0, "%s():%d [END?] pPos = '%1c'\n", __FUNCTION__, __LINE__, *pPos);
            if (pPos >= (pData + length)) {
                break;
            }
        
            pPos++;
        }
    }
#endif

    //printf("==========================================================\n");
//    OSAL_PRINTF("##########################################################\n");
    //printf("method: %.40s (%d)\n", (pReq->method == 1)? "GET" : (pReq->method == 2)? "POST" : "Unknown", pReq->method);
    //printf("path: %.40s\n", pReq->path);
    //printf("----------------------------------------------------------\n");
    //OSAL_PRINTF("pMethod: %.40s\n", pReq->pMethod);
    //OSAL_PRINTF("pPath: %.40s\n", pReq->pPath);
    //OSAL_PRINTF("pProtocol: %.40s\n", pReq->pProtocol);
    //OSAL_PRINTF("pHeader: \n%.40s\n", pReq->pHeader);
    //OSAL_PRINTF("pBody: \n%.40s\n\n", pReq->pBody);
    //OSAL_PRINTF("--[GET/POST VARs]-----------------------------------------\n");
    //OSAL_PRINTF("Var Count: %d\n", pReq->Var_Count);
    for (i=0; i<pReq->Var_Count; i++) {
        uint8 bufName[128];
        uint8 bufValue[128];

        //strncpy(bufName, pReq->Var[i].pName, pReq->Var[i].nLen);
        strncpy(bufName, pReq->pMethod + pReq->Var[i].name_offset, (pReq->Var[i].value_offset - 1));
        //bufName[pReq->Var[i].nLen] = '\0';
        bufName[(pReq->Var[i].value_offset - 1)] = '\0';

        //strncpy(bufValue, pReq->Var[i].pValue, pReq->Var[i].vLen);
        strncpy(bufValue, pReq->pMethod + pReq->Var[i].name_offset + pReq->Var[i].value_offset, pReq->Var[i].value_length);
        //bufValue[pReq->Var[i].vLen] = '\0';
        bufValue[pReq->Var[i].value_length] = '\0';

        OSAL_PRINTF("%s: %s\n", bufName, bufValue);
    }
    //OSAL_PRINTF("----------------------------------------------------------\n");

    return SUCCESS;
}


/*
 * Function Declaration
 */

// LWPS HTTP Connect: Handles new HTTP client connection requests
// Allocates client entry and associates socket with client context
void lwps_httpd_connect(int32 parentSocketId, int32 socketId, ip_addr_t ip, uint16 port) reentrant
{
    uint32 i;
    if (parentSocketId); // kill compiler warning
    if (ip.addr[0]); // kill compiler warning
    if (port); // kill compiler warning

    DBG_LWPS_HTTP_CONNECT(("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, (uint16)socketId));

    DBG_LWPS_HTTP_CONNECT(("%s():%d parentSocketId = %d, socketId = %d, remote_host = (%d.%d.%d.%d:%d)\n", \
        __FILE__, (uint16)__LINE__, (uint16)parentSocketId, (uint16)socketId, \
        (uint16)ip.addr[3], (uint16)ip.addr[2], (uint16)ip.addr[1], (uint16)ip.addr[0], (uint16)port));
    

    /* ban the bad host? */

    /* Bind user info */
    for (i=0; i<HTTPD_CLIENT_MAX; i++)
    {
        if (httpd_client_entry[i].socket_id < 0)
        {
            httpd_client_entry[i].socket_id = socketId;
#if __WEB_UPLOAD__
            httpd_client_entry[i].upload_offset = -1;
            httpd_client_entry[i].upload_size = -1;
#endif
            lwps_socket_setCookie(socketId, (void *)&httpd_client_entry[i]);
            DBG_LWPS_HTTP_CONNECT(("%s():%d lwps_socket_setCookie(socketId = %d, pHttpdUser = %p)\n", \
                __FILE__, (uint16)__LINE__, (uint16)socketId, &httpd_client_entry[i]));

            return; /* ACCEPT: accept this connection */
        }
    }
    
    printf("%s():%d HTTPD_CLIENT_MAX is too small\n", __FILE__, (uint16)__LINE__);
    /* REJECT: connection is full -> close this session */
    lwps_close(socketId);
}

// LWPS HTTP Disconnect: Handles HTTP client disconnection
// Cleans up client entry and releases associated resources
void lwps_httpd_disconnect(int32 socketId) reentrant
{
    httpd_client_t *pHttpdUser = (httpd_client_t *)lwps_socket_getCookie(socketId);

    DBG_LWPS_HTTP_CONNECT(("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, (uint16)socketId));

    DBG_LWPS_HTTP_CONNECT(("%s():%d lwps_socket_getCookie(socketId = %d) = pHttpdUser = %p\n", \
        __FILE__, (uint16)__LINE__, (uint16)socketId, pHttpdUser));

    /* Clear request buffer */
    if (http_request_sid == socketId)
    {
        http_request_sid = -1;
        http_request_len = 0;
    }

    /* Passive CLOSE */
    pHttpdUser->socket_id = -1;
}
    
// Web Process: Main HTTP request processing entry point
// Handles authentication and routes request to appropriate handler
int32 web_process(struct http_request_s *pReq)
{
    int32 ret = SUCCESS;
#if WEB_AUTHORIZATION_NEEDED
    Tuint8 *pValue = NULL;
    Tuint8 username[SAL_FLSH_USR_USERNAME_LEN + 1];
    Tuint8 password[SAL_FLSH_USR_PASSWORD_LEN + 1];
    Tuint8 bufAuth[40];

    sal_flash_sysAccount_get(username, password);

    sprintf(bufAuth, "%s:%s", username, password);
    sprintf(bufAuth, "Basic %s", base64_encode(bufAuth, strlen(bufAuth)));
    
    /* Check Authentication (always check) */
#if __WEB_UPLOAD__
    if (((pReq->pHdrAuthorization == NULL) || (strncmp(pReq->pHdrAuthorization, bufAuth, strlen(bufAuth)) != 0)) &&
        (0 != strncmp(pReq->pPath, "/httpupg.cgi", 12)))
#else
    if ((pReq->pHdrAuthorization == NULL) || (strncmp(pReq->pHdrAuthorization, bufAuth, strlen(bufAuth)) != 0))
#endif    
    {
        /* HTTP/1.1 401 Authorization Required */
        DBG_LWPS_HTTP_AUTH(("Entering Authorization:\n", __FILE__, (uint16)__LINE__));
        DBG_LWPS_HTTP_AUTH(("%s():%d bufAuth = [%s]\n", __FILE__, (uint16)__LINE__, bufAuth));
        DBG_LWPS_HTTP_AUTH(("%s():%d Authori = [%s]\n", __FILE__, (uint16)__LINE__, pReq->pHdrAuthorization));
        DBG_LWPS_HTTP_AUTH(("%s():%d Len of bufAuth is : %d\n", __FILE__, (uint16)__LINE__, strlen(bufAuth)));

        default_reply_asp_send(pReq, "Unauth");
        
        return SUCCESS;
    }
#endif
    default_reply_asp_send(pReq, pReq->path);

    return SUCCESS;
}

/*  1: incomplete packet
 *  0: complete packet
 * -1: error packet
 */
// HTTP Packet Check: Checks if HTTP packet is complete or incomplete
// Validates packet structure and content length for proper HTTP framing
static int32 http_packet_check(httpd_client_t *pClient, uint8 *pData, uint32 length)
{
    uint8 *pBoundary;
    uint8 *pContentType;
    uint8 *pContentLength;
    int32 iContentLength;
    int32 i;

    pClient = NULL; // kill compiler warning
    iContentLength = -1;    /* not found yet */

    if ((pData == NULL) || (length == 0) || \
        ((pBoundary = strnstr(pData, "\r\n\r\n", length)) == NULL))
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - incomplete HTTP packet!\n", __FILE__, (uint16)__LINE__));
        return 1;   /* incomplete */
    }

    /* try to find `Content-Type` */
    if ((pContentType = strnstr(pData, "Content-Type: ", pBoundary - pData)) == NULL)
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - complete HTTP packet (GOOD).\n", __FILE__, (uint16)__LINE__));
        return 0;   /* complete */
    }
    
    /* try to find `Content-Length` */
    if ((pContentLength = strnstr(pData, "Content-Length: ", pBoundary - pData)) == NULL)
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - error packet!\n", __FILE__, (uint16)__LINE__));
        return -1;  /* error */
    }

    pContentLength += 16;   /* jump to digital position */
    iContentLength = 0;
    for (i=0; i<32; i++)
    {
        if (*(pContentLength + i) == '\r')
            break;

        iContentLength *= 10;
        iContentLength += (*(pContentLength + i) - '0');
    }
    DBG_LWPS_HTTP_PRINTF(("%s():%d - pClient->ContentLength = %d\n", __FILE__, (uint16)__LINE__, iContentLength));

#if 0
    if (iContentLength >= length)
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - error packet!\n", __FILE__, (uint16)__LINE__));
        return -1;  /* error */
    }
#endif

    DBG_LWPS_HTTP_PRINTF(("%s():%d - pBoundary = %p, pData = %p, length = %d\n", __FILE__, (uint16)__LINE__, pBoundary, pData, length));

    if (iContentLength == ((pData + length) - (pBoundary + 4)))
    {
        DBG_LWPS_HTTP_PRINTF(( "%s():%d - complete HTTP packet (GOOD).\n", __FILE__, (uint16)__LINE__));
        return 0;   /* complete */
    }
    else if (iContentLength > ((pData + length) - (pBoundary + 4)))
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - incomplete HTTP packet!\n", __FILE__, (uint16)__LINE__));
        return 1;   /* incomplete */
    }

    DBG_LWPS_HTTP_PRINTF(("%s():%d - error packet!\n", __FILE__, (uint16)__LINE__));
    return -1;  /* error */
}

// LWPS HTTP Receive: Handles incoming HTTP data from client
// Processes packet assembly, parsing, and routing to web_process
void lwps_httpd_receive(int32 socketId, uint8 *pData, uint32 length) reentrant
{    
    httpd_client_t *pClient = (httpd_client_t *)lwps_socket_getCookie(socketId);
    int32 pkt_check = 0;
    uint32 push_len = 0;

    DBG_LWPS_HTTP_PRINTF(("%s():%d socketId = %d\n", __FILE__, (uint16)__LINE__, socketId));

    DBG_LWPS_HTTP_PRINTF(("%s():%d lwps_socket_getCookie(socketId = %ld) = pClient = %p\n", \
        __FILE__, (uint16)__LINE__, socketId, pClient));
 
    /* complete packet ? */
    if ((socketId != http_request_sid) &&
        (0 == (pkt_check = http_packet_check(pClient, pData, length))))
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - debug\n", __FILE__, (uint16)__LINE__));

        /* parsing and web_process directly */
        pClient->request.socket_id = socketId;
        if (SUCCESS != http_request_parse(&pClient->request, pData, length))
        {
            DBG_LWPS_HTTP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
            goto _recv_close;
        }
       
        if (SUCCESS != web_process(&pClient->request))
        {
            OSAL_PRINTF("%s():%d web_process failed!\n", __FILE__, (uint16)__LINE__);
        }

        goto _recv_close;
    }
    else if (-1 == pkt_check)
    {   /* error packet */
        DBG_LWPS_HTTP_PRINTF(( "%s():%d - debug (ERROR HTTP PACKET)\n", __FILE__, (uint16)__LINE__));
        goto _recv_close;
    }
    else /* Waiting or incomplete (first) packet */
    {
        DBG_LWPS_HTTP_PRINTF(("%s():%d - debug (Waiting or incomplete packet)\n", __FILE__, (uint16)__LINE__));

        /* try to take http_request_buf ? */
        if (http_request_sid < 0)
        {   /* take http_request_buf (first packet) */
            DBG_LWPS_HTTP_PRINTF(("%s():%d - take http_request_buf (first packet).\n", __FILE__, (uint16)__LINE__));
            http_request_sid = pClient->socket_id;
            http_request_len = 0;   /* clear length */
        }
        else if (http_request_sid != socketId)
        {
            DBG_LWPS_HTTP_PRINTF(("%s():%d - http_request_buf is already used!\n", __FILE__, (uint16)__LINE__));
            goto _recv_close;
        }

        /* http_request_buf process */
        if ((http_request_len + length) > HTTPD_REQ_BUF_SIZE)
        {
            /* push the part of data into the request buffer */
            push_len = (HTTPD_REQ_BUF_SIZE - http_request_len);
        }
        else
        {   
            /* push data into the request buffer */
            push_len = length;
        }
        memcpy(http_request_buf + http_request_len, pData, push_len);
        http_request_len += push_len;
        http_request_buf[http_request_len] = '\0';  /* terminated */
   
        /* to do parsing */
        pkt_check = http_packet_check(pClient, http_request_buf, http_request_len);
        if (-1 == pkt_check)
        {
            DBG_LWPS_HTTP_PRINTF(("%s():%d - debug (ERROR HTTP PACKET)\n", __FILE__, (uint16)__LINE__));
            goto _recv_close;
        }
        else if (0 == pkt_check)
        {
            DBG_LWPS_HTTP_PRINTF(("%s():%d - debug (COMPLETED PACKET)\n", __FILE__, (uint16)__LINE__));
            
            /* parsing and web_process directly */
            pClient->request.socket_id = socketId;
            if (SUCCESS != http_request_parse(&pClient->request, http_request_buf, http_request_len))
            {
                DBG_LWPS_HTTP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
                goto _recv_close;
            }

            if (SUCCESS != web_process(&pClient->request))
            {
                OSAL_PRINTF("%s():%d web_process failed!\n", __FILE__, (uint16)__LINE__);
            }

            http_request_sid = -1;  /* release this buffer */

            goto _recv_close;
        }
        else
        {

           /* keep waiting the next packet (incomplete packet) */
            DBG_LWPS_HTTP_PRINTF(("%s():%d - debug (keep waiting the next packet)\n", __FILE__, (uint16)__LINE__));

            /* buffer full? */
            if (http_request_len == HTTPD_REQ_BUF_SIZE)
            {
#if __WEB_UPLOAD__
                if (pClient->upload_offset < 0)
                {   /* first time */

                    DBG_LWPS_HTTP_PRINTF(("%s():%d pClient->request: upload_data = %p, upload_len = %d, upload_offset = %d\n", \
                        __FILE__, (uint16)__LINE__, pClient->request.upload_data, pClient->request.upload_len, pClient->request.upload_offset));

                    /* parsing and web_process directly */
                    pClient->request.socket_id = socketId;
                    if (SUCCESS != http_request_parse(&pClient->request, http_request_buf, http_request_len))
                    {
                        DBG_LWPS_HTTP_PRINTF(("%s():%d\n", __FILE__, (uint16)__LINE__));
                        goto _recv_close;
                    }

                    if (SUCCESS != web_process(&pClient->request))
                    {
                        OSAL_PRINTF("%s():%d web_process failed!\n", __FILE__, (uint16)__LINE__);
                    }

                    pClient->upload_offset = pClient->request.upload_len;
                    pClient->upload_size = pClient->request.upload_size;

                    /* fragment? */
                    if ((push_len > 0) && (push_len != length))
                    {   /* fragment process */
                        pClient->request.upload_data = pData + push_len;
                        pClient->request.upload_len = length - push_len;
                        pClient->request.upload_offset = pClient->upload_offset;
                        DBG_LWPS_HTTP_PRINTF(("%s():%d pClient->request: upload_data = %p, upload_len = %d, upload_offset = %d\n", \
                            __FILE__, (uint16)__LINE__, pClient->request.upload_data, pClient->request.upload_len, pClient->request.upload_offset));
                        if (SUCCESS != web_process(&pClient->request))
                        {
                            OSAL_PRINTF("%s():%d web_process failed!\n", __FILE__, (uint16)__LINE__);
                        }
                        
                        pClient->upload_offset = (pClient->request.upload_offset + pClient->request.upload_len);
                    }
                }
                else
                {   /* non-first time */
                    pClient->request.upload_data = pData;
                    pClient->request.upload_len = length;
                    pClient->request.upload_offset = pClient->upload_offset;
                    pClient->request.upload_size = pClient->upload_size;
                    DBG_LWPS_HTTP_PRINTF(("%s():%d pClient->request: upload_data = %p, upload_len = %d, upload_offset = %d\n", \
                        __FILE__, (uint16)__LINE__, pClient->request.upload_data, pClient->request.upload_len, pClient->request.upload_offset));
                    if (SUCCESS != web_process(&pClient->request))
                    {
                        OSAL_PRINTF("%s():%d web_process failed!\n", __FILE__, (uint16)__LINE__);
                    }
                    
                    pClient->upload_offset = (pClient->request.upload_offset + pClient->request.upload_len);
                }

                /* goto recv_close when ending */
                if ((pClient->request.upload_offset + pClient->request.upload_len) >= pClient->request.upload_size)
                {
                    goto _recv_close;
                }
#endif  /* __WEB_UPLOAD__ */
            }   /* end of `if (http_request_len == HTTPD_REQ_BUF_SIZE)` */

            return;
        }
    }

_recv_close:
    /* Active CLOSE */
    //pClient->socket_id = -1;
    lwps_close(socketId);
   
    return;
}

// HTTP Daemon Init: Initializes HTTP server
// Creates socket, binds to port, listens, and prepares for client connections
int32 httpd_init(void) large
{
    uint32 i;
    int32 socketId;
    lwps_handler_t httpdHandler;

    /* Initialize private struct */
    for (i=0; i<HTTPD_CLIENT_MAX; i++)
    {
        httpd_client_entry[i].socket_id = -1;
    }

    /* Alloc a socket for HTTP daemon */
    httpdHandler.event_connect    = lwps_httpd_connect;
    httpdHandler.event_disconnect = lwps_httpd_disconnect;
    httpdHandler.event_recv    = lwps_httpd_receive;
    
    socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_TCP, httpdHandler);
    if (socketId < 0)
    {
        OSAL_PRINTF("httpd: Error - alloc socket failed!\n");

        return FAILED;
    }
    
    if (LWPS_OK != lwps_bind(socketId, ip_addr_any, HTTPD_LISTEN_PORT))
    {
        OSAL_PRINTF("httpd: Error - bind failed!\n");

        lwps_close(socketId);
        return FAILED;
    }


    if (LWPS_OK != lwps_listen(socketId, 2))    /* backlog = 2 */
    {
        OSAL_PRINTF("httpd: Error - listen failed!\n");

        lwps_close(socketId);
        return FAILED;
    }

    http_request_sid = -1;

    return SUCCESS;
}