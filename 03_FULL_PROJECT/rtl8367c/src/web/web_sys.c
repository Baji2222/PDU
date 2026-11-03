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

#include <salsys.h>
#include <salflash.h>

#include <web/web_api.h>
#include "web_sys.h"
#include <salsys.h>
#include <syslog/syslogdefs.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Array: htm_reboot
 * -----------------
 * HTML page for system reboot confirmation.
 * Displays reboot button with warning message.
 * Uses POST method to trigger reboot action.
 * Styled with clean, minimal design.
 */
uint8 const far htm_reboot[] = "\
<html><head><title>Reboot</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:350px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
input[type=submit]{background:#f44336;color:#fff;border:0;padding:8px 16px;cursor:pointer;border-radius:3px;font-weight:bold;margin-top:15px}\
.msg{color:#444;margin-bottom:15px}\
</style></head>\
<body>\
<fieldset>\
<legend>System Reboot</legend>\
<form method=\"post\" action=\"reboot.cgi\">\
<div class=\"msg\">Reboot the switch to apply changes</div>\
<input type=\"submit\" value=\"Reboot Now\">\
<input type=\"hidden\" name=\"cmd\" value=\"reboot\">\
</form>\
</fieldset>\
</body></html>\n\
";

/*
 * Array: htm_rebooting
 * --------------------
 * HTML page shown during system reboot process.
 * Displays progress bar animation using CSS keyframes.
 * Auto-refreshes to home page after 12 seconds.
 * Indicates device is restarting with visual feedback.
 */
uint8 const far htm_rebooting[] = "\
<html><head><title>Rebooting</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:350px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
.p{height:5px;margin:15px 0;background:#eee;border-radius:3px;overflow:hidden}\
.b{height:100%;width:0;background:#1e88e5;display:block;animation:f 8s forwards}\
@keyframes f{to{width:100%}}\
.msg{color:#444;margin-bottom:10px}\
.note{font-size:12px;color:#777;margin-top:10px}\
</style>\
<script>setTimeout(function(){location.href='/'},12000)</script></head>\
<body>\
<fieldset>\
<legend>System Reboot</legend>\
<div class=\"msg\">Please wait while the device reboots...</div>\
<div class=\"p\"><span class=\"b\"></span></div>\
<div class=\"note\">The page will refresh automatically</div>\
</fieldset>\
</body></html>\n\
";

/*
 * Array: htm_reset
 * ----------------
 * HTML page for factory reset confirmation.
 * Displays reset button with warning message.
 * Warns user that all configuration will be lost.
 * Uses POST method to trigger factory reset.
 */
uint8 const far htm_reset[] = "\
<html><head><title>Factory Reset</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:350px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
input[type=submit]{background:#f44336;color:#fff;border:0;padding:8px 16px;cursor:pointer;border-radius:3px;font-weight:bold;margin-top:15px}\
.msg{color:#444;margin-bottom:15px}\
.warning{color:#f44336;font-size:12px;margin-top:10px}\
</style></head>\
<body>\
<fieldset>\
<legend>Factory Reset</legend>\
<form method=\"post\" action=\"reset.cgi\">\
<div class=\"msg\">Reset to default factory settings and restart the system.</div>\
<input type=\"submit\" value=\"Factory Default\">\
<input type=\"hidden\" name=\"cmd\" value=\"factory_default\">\
<div class=\"warning\">Warning: All configuration will be lost!</div>\
</form>\
</fieldset>\
</body></html>\n\
";

/*
 * Array: htm_reset_process
 * ------------------------
 * HTML page shown during factory reset process.
 * Displays progress bar animation.
 * Auto-refreshes to home page after 15 seconds.
 * Indicates device is resetting to defaults.
 */
uint8 const far htm_reset_process[] = "\
<html><head><title>Factory Reset</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:350px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
.p{height:5px;margin:15px 0;background:#eee;border-radius:3px;overflow:hidden}\
.b{height:100%;width:0;background:#1e88e5;display:block;animation:f 8s forwards}\
@keyframes f{to{width:100%}}\
.msg{color:#444;margin-bottom:10px}\
.note{font-size:12px;color:#777;margin-top:10px}\
</style>\
<script>setTimeout(function(){location.href='/'},15000)</script></head>\
<body>\
<fieldset>\
<legend>Factory Reset</legend>\
<div class=\"msg\">Resetting to factory defaults...</div>\
<div class=\"p\"><span class=\"b\"></span></div>\
<div class=\"note\">The device will restart automatically</div>\
</fieldset>\
</body></html>\n\
";

/*
 * Array: htm_save
 * ---------------
 * HTML page shown after configuration save.
 * Displays success message to confirm save operation.
 * Simple, clean design with green success indicator.
 * No auto-redirect, user can navigate manually.
 */
uint8 const far htm_save[] = "\
<html><head><title>Configuration Saved</title><style>\
body{font:13px Arial;background:#f9f9f9;margin:0;padding:25px 0;text-align:center}\
fieldset{width:350px;border:1px solid #ddd;padding:20px;margin:0 auto;background:#fff;border-radius:4px;box-shadow:0 1px 4px rgba(0,0,0,0.05)}\
legend{font:bold 16px Arial;color:#1e88e5;padding:0 10px}\
.success{color:#4caf50;font-weight:bold;margin:10px 0}\
</style></head>\
<body>\
<fieldset>\
<legend>Save Configuration</legend>\
<div class=\"success\">Configuration has been saved successfully.</div>\
</fieldset>\
</body></html>\n\
";


/*
 * Function Declaration
 */

/*
 * Function: get_reboot
 * --------------------
 * Serves the system reboot confirmation page.
 * Displays HTML with reboot button and warning message.
 * GET handler for reboot.cgi page.
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

/*
 * Function: get_reset
 * -------------------
 * Serves the factory reset confirmation page.
 * Displays HTML with reset button and warning about data loss.
 * GET handler for reset.cgi page.
 */
int32 get_reset(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_reset, sizeof(htm_reset));

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_save
 * ------------------
 * Saves current configuration to flash and displays confirmation.
 * Calls sal_flash_cfg_save() to persist configuration.
 * Shows success page after save operation completes.
 * GET handler for save.cgi page.
 */
int32 get_save(struct http_request_s *pReq)
{
    sal_flash_cfg_save();

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_save, sizeof(htm_save));

    WEB_END(pReq);

    return SUCCESS;
}

//extern Tint32 lws_reboot_timer;

/*
 * Function: post_reboot
 * ---------------------
 * Handles system reboot request from form submission.
 * Logs reboot event to syslog.
 * Shows rebooting progress page to user.
 * Calls sal_sys_system_reboot() to restart the device.
 */
int32 post_reboot(struct http_request_s *pReq)
{   
	syslog_add_new_buf_entry(SYSLOG_MSG_SYSTEM_REBOOTED, 0);

    /* Reboot */    
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_rebooting, sizeof(htm_rebooting));

    WEB_END(pReq);

    sal_sys_system_reboot();
    printf("Rebooting....\n");

    return SUCCESS;
}

/*
 * Function: post_factdefault_load
 * -------------------------------
 * Handles factory reset request from form submission.
 * Logs factory reset event to syslog.
 * Calls sal_flash_cfg_reset() to restore default config.
 * Shows reset progress page and reboots the system.
 */
int32 post_factdefault_load(struct http_request_s *pReq)
{
	syslog_add_new_buf_entry(SYSLOG_MSG_FACTORY_RESET, 0);

    sal_flash_cfg_reset();
    
    /* Reboot */
    //lws_reboot_timer = 2;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_reset_process, sizeof(htm_reset_process));

    WEB_END(pReq);

    sal_sys_system_reboot();
    
    return SUCCESS;
}