#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to System function
 *
 * Feature : Response the request related to System function
 *
 */

/*
 * Include Files
 */
#include <stdio.h>

#include <web/web_api.h>
#include "web_sys.h"

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
uint8 const far htm_reboot[] = "\
<html><head><title>Reboot</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:300px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
input{background:#f44336;color:#fff;border:0;padding:8px 16px;cursor:pointer;border-radius:3px;margin-top:10px;font-weight:bold}\
.m{margin-bottom:15px;color:#444}\
</style></head>\
<body>\
<fieldset>\
<legend>System Reboot</legend>\
<form method=post action=reboot.cgi>\
<div class=\"m\">Reboot the switch to apply changes</div>\
<input type=submit value=\"Reboot Now\">\
<input type=hidden name=cmd value=reboot>\
</form>\
</fieldset>\
</body></html>\n\
";


uint8 const far htm_rebooting[] = "\
<html><head><title>Rebooting</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:300px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
.p{height:5px;margin:15px 0;background:#eee}\
.b{height:100%;width:0;background:#1e88e5;display:block;animation:f 8s forwards}\
@keyframes f{to{width:100%}}\
.m{margin-bottom:15px;color:#444}\
.s{font-size:12px;color:#777;margin-top:10px}\
</style>\
<script>setTimeout(function(){location.href='/'},12000)</script></head>\
<body>\
<fieldset>\
<legend>System Reboot</legend>\
<div class=\"m\">Please wait while the device reboots...</div>\
<div class=\"p\"><span class=\"b\"></span></div>\
<div class=\"s\">The page will refresh automatically</div>\
</fieldset>\
</body></html>\n\
";

/*
 * Function Declaration
 */
int32 get_reboot(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_reboot, sizeof(htm_reboot));

    WEB_END(pReq);

    return SUCCESS;
}

int32 post_reboot(struct http_request_s *pReq)
{   

    /* Reboot */    
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_rebooting, sizeof(htm_rebooting));

    WEB_END(pReq);
		
    sal_sys_system_reboot();

    return SUCCESS;
}

