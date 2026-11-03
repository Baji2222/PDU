#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to port function
 *
 * Feature : Response the request related to port function
 *
 */


/*
 * Include Files
 */
#include <stdio.h>

#include <salport.h>
#include <salloop.h>

#include <web/web_api.h>
#include <web/web_err.h>
#include <web/http_debug.h>

#include "web_panel.h"
#include "web_menu.h"


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/*
 * Function: get_panel
 * -------------------
 * Displays the main panel page with "OPL SMART PDU RACK" title.
 * Uses clean, centered design with beetroot red colored heading.
 * Sets up full viewport height with flexbox centering.
 * Serves as the default landing/display page for the switch.
 */
int32 get_panel(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
   WEB_PRINTF(pReq, "<html>\n");
WEB_PRINTF(pReq, "<head>\n");
WEB_PRINTF(pReq, "<style>\n");
WEB_PRINTF(pReq, "body {\n");
WEB_PRINTF(pReq, "font-family: 'Arial', sans-serif;\n");
WEB_PRINTF(pReq, "display: flex;\n");
WEB_PRINTF(pReq, "justify-content: center;\n");
WEB_PRINTF(pReq, "align-items: center;\n");
WEB_PRINTF(pReq, "height: 100vh;\n");
WEB_PRINTF(pReq, "margin: 0;\n");
WEB_PRINTF(pReq, "background-color: #f4f4f9;\n");
WEB_PRINTF(pReq, "}\n");
WEB_PRINTF(pReq, "h1 {\n");
WEB_PRINTF(pReq, "color: #8a0303; /* Beetroot red */\n");
WEB_PRINTF(pReq, "font-size: 3rem;\n");
WEB_PRINTF(pReq, "text-align: center;\n");
WEB_PRINTF(pReq, "text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);\n");
WEB_PRINTF(pReq, "letter-spacing: 2px;\n");
WEB_PRINTF(pReq, "font-weight: bold;\n");
WEB_PRINTF(pReq, "}\n");
WEB_PRINTF(pReq, "</style>\n");
WEB_PRINTF(pReq, "</head>\n");
WEB_PRINTF(pReq, "<body>\n");
WEB_PRINTF(pReq, "<h1>OPL SMART PDU RACK</h1>\n");
WEB_PRINTF(pReq, "</body>\n");
WEB_PRINTF(pReq, "</html>\n");
WEB_PRINTF(pReq, "\n");
    WEB_END(pReq);
    return SUCCESS;
}