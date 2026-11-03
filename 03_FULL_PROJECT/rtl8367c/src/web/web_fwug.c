
#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to TFTP function
 *
 * Feature : Response the request related to TFTP function
 *
 * NOTE: htm_loader + htm_enter_loader (incl. NUL terminators) = 957 bytes,
 *       fitted to a 960 byte code-memory budget. Only 3 bytes of headroom
 *       left; re-check total size before editing either string.
 */
/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>
#include <salsys.h>
#include <web/http_types.h>
#include <web/web_api.h>
/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */
/*
 * Array: htm_loader
 * -----------------
 * HTML page for firmware upgrade loader mode entry.
 * Displays "Enter Loader Mode" button with confirmation.
 * Minimal CSS, widened container, to fit tight code-memory budget.
 */
uint8 const far htm_loader[] = "\
<html><head><title>FW</title>\
<style>\
body{font:20px Arial;text-align:center;padding-top:30vh}\
fieldset{width:800px;margin:auto;border:3px solid #364961;padding:20px}\
.s{background:#0f1a2b;color:#fff;border:0;padding:12px 28px;margin-top:10px}\
</style>\
<body>\
<fieldset><legend>Firmware Upgrade</legend>\
<form method=post action=fwug.cgi onsubmit=\"return confirm('Enter loader mode to upgrade')\">\
<div>Enter loader mode to upgrade</div>\
<input type=submit class=s value=\"Enter Loader Mode\">\
<input type=hidden name=cmd value=enter_loader>\
</form></fieldset>\
</body></html>\
";
/*
 * Array: htm_enter_loader
 * -----------------------
 * HTML page shown while entering loader mode.
 * Displays 3-second animated progress bar using CSS keyframes.
 * Auto-refreshes after 3 seconds to trigger loader mode.
 */
uint8 const far htm_enter_loader[] = "\
<html><head><title>Loader</title>\
<style>\
body{font:20px Arial;text-align:center;padding-top:30vh}\
fieldset{width:800px;margin:auto;border:3px solid #364961;padding:10px}\
p{height:25px}\
b{height:20px;width:0;background:#0f1a2b;display:block;animation:f 3s}\
@keyframes f{to{width:100%}}\
</style>\
<script>setTimeout(\"top.location.reload()\",3000)</script>\
<body>\
<fieldset><legend>Loader Mode</legend>\
<div>Entering Loader Mode</div>\
<p><b></b></p>\
</fieldset>\
</body></html>\
";
/*
 * Function Declaration
 */
/*
 * Function: get_fwug
 * ------------------
 * Serves the firmware upgrade page (GET request).
 * Displays HTML with "Enter Loader Mode" button.
 * No parameters needed, simply returns the loader page.
 */
int32 get_fwug(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_DATA(pReq, htm_loader, sizeof(htm_loader));
    WEB_END(pReq);
    return SUCCESS;
}
/*
 * Function: post_fwug
 * -------------------
 * Handles firmware upgrade form submission (POST request).
 * Shows "Entering Loader Mode" progress page.
 * Calls sal_sys_loaderMode_set() to switch to bootloader mode.
 * Device reboots and enters loader mode after this function.
 */
int32 post_fwug(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_DATA(pReq, htm_enter_loader, sizeof(htm_enter_loader));
    WEB_END(pReq);
    sal_sys_loaderMode_set();
    return SUCCESS;
}



