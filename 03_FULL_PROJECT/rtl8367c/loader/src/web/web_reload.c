#pragma STRING (far)

#include <stdio.h>

#include <local/config.h>


#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_reload.h"

#if WEB_ENABLE

int32 get_reload(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head><title>Reload</title>\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "function reloadAll()\n");
    WEB_PRINTF(pReq, "{\n");
    WEB_PRINTF(pReq, "    parent.frames[1].location.reload();\n");
    WEB_PRINTF(pReq, "    parent.frames[2].location.reload();\n");
    WEB_PRINTF(pReq, "    parent.frames[3].location.reload();\n");    
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "<body onload=\"reloadAll()\">\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);
    
    return SUCCESS;
}

#endif


