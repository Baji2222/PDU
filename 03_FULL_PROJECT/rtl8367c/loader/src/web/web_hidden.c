#pragma STRING (far)

#include <local/config.h>

#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_hidden.h"

#if WEB_ENABLE

uint8 const far htm_hidden[] = "\
<html>\n\
<body>\n\
</body>\n\
</html>\n\
";

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

#endif

