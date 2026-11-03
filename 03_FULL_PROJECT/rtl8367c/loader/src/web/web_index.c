#pragma STRING (far)

#include <local/config.h>

#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_index.h" 

#include <stdio.h>

#if WEB_ENABLE

uint8 const far htm_index[] = "\
<html>\n\
<head>\n\
<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=big5\">\n\
<title>OPL Web-Smart Switch</title>\n\
</head>\n\
<frameset cols=\"0, 250,*\">\n\
<frame name=\"hidden_frame\" frameborder=\"0\" src=\"hidden.cgi\" >\n\
<frame name=\"menu-frame\" target=\"main-frame\" src=\"menu.cgi\" >\n\
<frameset rows=\"130,*\">\
<frame name=\"panel-frame\" scrolling=\"no\" src=\"panel.cgi\" noresize>\n\
<frame name=\"main-frame\" src=\"info.cgi\" noresize>\n\
<noframes>\n\
<body>\n\
<p>This page uses frames, but the browser does not support them.</p>\n\
</body>\n\
</noframes>\n\
</frameset>\n\
</html>\n\
";

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

#endif


