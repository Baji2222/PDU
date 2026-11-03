#pragma STRING (far)

#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_hidden.h"

/*
 * Array: htm_hidden
 * -----------------
 * Empty HTML page used as hidden redirect target.
 * Contains minimal HTML structure with no visible content.
 * Used as interim page before redirecting to another URL.
 */
uint8 const far htm_hidden[] = "\
<html>\n\
<body>\n\
</body>\n\
</html>\n\
";

/*
 * Array: htm_goback
 * -----------------
 * JavaScript page that triggers browser back navigation.
 * Uses history.go(-1) to go back one page.
 * Useful for returning to previous page after actions.
 */
uint8 const far htm_goback[] = "\
<script type=\"text/javascript\">\n\
 history.go(-1);\n\
</script>\n\
";

/*
 * Function: get_hidden
 * --------------------
 * Serves empty hidden HTML page to the client.
 * Used as a blank landing page for redirects.
 * Logs debug message when accessed.
 */
int8 get_hidden(struct http_request_s *pReq)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_hidden()-------\n"));

    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_hidden, sizeof(htm_hidden) - 1);
    
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_goback
 * --------------------
 * Serves JavaScript page that performs browser back navigation.
 * Useful for returning to previous page after form submissions.
 * Logs debug message and sends content with proper headers.
 */
int8 get_goback(struct http_request_s *pReq)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_goback()-------\n"));
    
    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_goback, sizeof(htm_goback) - 1);
    
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_alert
 * -------------------
 * Generates HTML page with JavaScript alert dialog.
 * Displays custom message passed as parameter (pMsg).
 * After alert, redirects to /hidden.cgi page.
 */
int8 get_alert(struct http_request_s *pReq, uint8 *pMsg)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_alert()-------\n"));

    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Message</title>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">");
    WEB_PRINTF(pReq, "alert(\"%s\");\n", pMsg);
    WEB_PRINTF(pReq, "window.location.replace(\'/hidden.cgi\')\n");
    WEB_PRINTF(pReq, "</script>");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");
    
    WEB_END(pReq);

    return SUCCESS;
}