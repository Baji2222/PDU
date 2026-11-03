#pragma STRING (far)

#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_index.h" 

/*
 * Array: htm_index
 * ----------------
 * HTML page that auto-redirects to menu.cgi.
 * Uses meta refresh tag (0 second delay).
 * Sets charset to big5 for Chinese character support.
 * Displays brief "Redirecting..." message while redirecting.
 */
uint8 const far htm_index[] = "\
<html>\n\
<head>\n\
<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=big5\">\n\
<title>Original Web-Smart Switch</title>\n\
<meta http-equiv=\"refresh\" content=\"0; URL=menu.cgi\">\n\
</head>\n\
<body>\n\
<p>Redirecting to menu page...</p>\n\
</body>\n\
</html>\n\
";

/*
 * Function: get_index
 * -------------------
 * Handles root path request ("/" or "/index.cgi").
 * Serves index page that auto-redirects to menu.
 * Sets proper Content-Type header for HTML.
 * Logs debug message when accessed.
 */
int8 get_index(struct http_request_s *pReq)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_index()-------\n"));

    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
   
    WEB_DATA(pReq, htm_index, sizeof(htm_index) - 1);
    
    WEB_END(pReq);

    return SUCCESS;
}