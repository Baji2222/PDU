#pragma STRING (far)

#include <stdio.h>

#include <local/config.h>


#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_info.h"

#if WEB_ENABLE

uint8 const far htm_msg[] = "\
<html>\n\
\n\
<head>\n\
<title>Reset</title>\n\
<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n\
</head>\n\
\n\
<body>\n\
<center>\n\
\n\
<fieldset>\n\
<legend>Loader Mode</legend>\n\
  <b style=\"font-weight:normal;font-family: Geneva, Arial, Helvetica, sans-serif;letter-spacing:.45pt\">Firmware upgrade mode. Please upload your image by TFTP or HTTP \n\
  <p>\n\
</fieldset>\n\
\n\
</center>\n\
</body>\n\
</html>\n\
";

int32 get_info(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    
    WEB_DATA(pReq, htm_msg, sizeof(htm_msg));
    
    WEB_END(pReq);
    
    return SUCCESS;
}

#endif

