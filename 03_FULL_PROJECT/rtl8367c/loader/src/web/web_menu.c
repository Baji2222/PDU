#pragma STRING (far)

#include <local/config.h>

#include <web/web_api.h>
#include <web/web_err.h>
#include <web/http_debug.h>

#include "web_menu.h"

#if WEB_ENABLE
#if defined(UPGRADE_TFTP)
uint8 const far htm_menu[] = "\
    <html>\n\
    <head>\n\
    <title>Firmware Upgrade</title>\n\
    <STYLE type=\"text/css\">\n\
    body {\n\
        font-size: 13px;\n\
        font-family: sans-serif, Geneva, Arial, Helvetica;\n\
        background: white;\n\
        margin: 0;\n\
        padding: 0;\n\
    }\n\
    .header {\n\
        background-color: #1e88e5;\n\
        padding: 15px 20px;\n\
        color: white;\n\
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n\
        display: flex;\n\
        justify-content: space-between;\n\
        align-items: center;\n\
    }\n\
    .header h2 {\n\
        margin: 0;\n\
        font-size: 18px;\n\
    }\n\
    .nav-menu {\n\
        display: flex;\n\
        align-items: center;\n\
    }\n\
    .nav-menu a {\n\
        color: white;\n\
        text-decoration: none;\n\
        padding: 8px 15px;\n\
        margin-left: 5px;\n\
        border-radius: 3px;\n\
        font-weight: bold;\n\
        transition: background-color 0.2s;\n\
    }\n\
    .nav-menu a:hover {\n\
        background-color: rgba(255,255,255,0.2);\n\
    }\n\
    .upgrade-link {\n\
        background-color: white;\n\
        color: #1e88e5 !important;\n\
    }\n\
    .reboot-link {\n\
        background-color: #f44336;\n\
    }\n\
    .content {\n\
        padding: 20px;\n\
    }\n\
    </STYLE>\n\
    </head>\n\
    <body>\n\
    <div class=\"header\">\n\
        <h2>Firmware Upgrade Mode</h2>\n\
        <div class=\"nav-menu\">\n\
            <a href=\"httpug.cgi\" target=\"main-frame\" class=\"upgrade-link\">HTTP Upgrade</a>\n\
            <a href=\"reboot.cgi\" target=\"main-frame\" class=\"reboot-link\">Reboot</a>\n\
        </div>\n\
    </div>\n\
    <div class=\"content\">\n\
        <iframe id=\"contentFrame\" name=\"main-frame\" src=\"httpug.cgi\" style=\"width:100%; height:600px; border:none;\"></iframe>\n\
    </div>\n\
    </body>\n\
    </html>\n";
#else  

uint8 const far htm_menu[] = "\
    <html>\n\
    <head>\n\
    <title>Firmware Upgrade</title>\n\
    <STYLE type=\"text/css\">\n\
    body {\n\
        font-size: 13px;\n\
        font-family: sans-serif, Geneva, Arial, Helvetica;\n\
        background: white;\n\
        margin: 0;\n\
        padding: 0;\n\
    }\n\
    .header {\n\
        background-color: #1e88e5;\n\
        padding: 15px 20px;\n\
        color: white;\n\
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n\
        display: flex;\n\
        justify-content: space-between;\n\
        align-items: center;\n\
    }\n\
    .header h2 {\n\
        margin: 0;\n\
        font-size: 18px;\n\
    }\n\
    .nav-menu {\n\
        display: flex;\n\
        align-items: center;\n\
    }\n\
    .nav-menu a {\n\
        color: white;\n\
        text-decoration: none;\n\
        padding: 8px 15px;\n\
        margin-left: 5px;\n\
        border-radius: 3px;\n\
        font-weight: bold;\n\
        transition: background-color 0.2s;\n\
    }\n\
    .nav-menu a:hover {\n\
        background-color: rgba(255,255,255,0.2);\n\
    }\n\
    .upgrade-link {\n\
        background-color: white;\n\
        color: #1e88e5 !important;\n\
    }\n\
    .reboot-link {\n\
        background-color: #f44336;\n\
    }\n\
    .content {\n\
        padding: 20px;\n\
    }\n\
    </STYLE>\n\
    </head>\n\
    <body>\n\
    <div class=\"header\">\n\
        <h2>Firmware Upgrade Mode</h2>\n\
        <div class=\"nav-menu\">\n\
            <a href=\"httpug.cgi\" target=\"main-frame\" class=\"upgrade-link\">HTTP Upgrade</a>\n\
            <a href=\"reboot.cgi\" target=\"main-frame\" class=\"reboot-link\">Reboot</a>\n\
        </div>\n\
    </div>\n\
    <div class=\"content\">\n\
        <iframe id=\"contentFrame\" name=\"main-frame\" src=\"httpug.cgi\" style=\"width:100%; height:600px; border:none;\"></iframe>\n\
    </div>\n\
    </body>\n\
    </html>\n";

#endif

int8 get_menu(struct http_request_s *pReq)
{
    DBG_LWPS_HTTP_PRINTF(("-------get_menu()-------\n"));
    
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_DATA(pReq, htm_menu, sizeof(htm_menu) - 1);

    WEB_END(pReq);

    return SUCCESS;
}

#endif


