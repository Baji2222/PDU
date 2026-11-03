/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : 
*$Id: 
*/
#pragma STRING (far)

#include <stdio.h>

#include <web/web_api.h>
#include "html_unauth.h" 
#include <syslog/syslogdefs.h>

/*
 * Data Declaration
 */
uint8 const far htm_http_unauthorized[] = "\
<html><head>\n\
<title>401 Authorization Required</title>\n\
</head><body>\n\
<h1>Authorization Required</h1>\n\
<p>This server could not verify that you\n\
are authorized to access the document\n\
requested.  Either you supplied the wrong\n\
credentials (e.g., bad password), or your\n\
browser doesn't understand how to supply\n\
the credentials required.</p>\n\
<hr>\n\
<address>Web-Smart Server</address>\n\
</body></html>\n\
";

// Get Unauthorized: Sends HTTP 401 Unauthorized response to client
// Displays authentication required page when user fails to provide valid credentials
int8 get_unauth(struct http_request_s *pReq)
{
//	syslog_add_new_buf_entry(SYSLOG_MSG_LOGIN_FAILED, 0);

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_UNAUTHORIZED);
    WEB_HEADER(pReq, "WWW-Authenticate: Basic realm=\"Switch\"");
    WEB_DATA(pReq, htm_http_unauthorized, sizeof(htm_http_unauthorized) -1);
    WEB_END(pReq);

    return SUCCESS;
}