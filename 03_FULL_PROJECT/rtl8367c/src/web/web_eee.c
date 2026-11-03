/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to EEE function
 *
 * Feature : Response the related request of EEE function
 *
 */
#pragma STRING (far)

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <salerror.h>
#include <saleee.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_util.h"

#if __WEB_EEE_ENABLE__

/*
 * Function Declaration
 */

/*
 * Function: get_eee
 * -----------------
 * Displays EEE (Energy Efficient Ethernet) configuration page.
 * Reads current EEE state and renders HTML with select dropdown.
 * Highlights the currently selected state (Enable/Disable).
 */
int32 get_eee(struct http_request_s *pReq)
{
    //sal_enable_t eee_enable;
    
//    WEB_BEGIN(pReq);

//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: text/html");

//    WEB_PRINTF(pReq, "<html>\n");
//    WEB_PRINTF(pReq, "<head>\n");
//    WEB_PRINTF(pReq, "<title>EEE Setting</title>\n");
//    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
//    WEB_PRINTF(pReq, "</head>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<body>\n");
//    WEB_PRINTF(pReq, "<center>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<fieldset>\n");
//    WEB_PRINTF(pReq, "<legend>EEE Setting</legend>\n");
//    WEB_PRINTF(pReq, "<form method=\"post\" name=\"eee\" action=\"/eee.cgi\">\n");

//    WEB_PRINTF(pReq, "<br>\n");
//    WEB_PRINTF(pReq, "<table border=\"1\">\n");
//    WEB_PRINTF(pReq, "  <tr>\n");
//    WEB_PRINTF(pReq, "    <th width=\"200\">EEE function</th>\n");

//    WEB_PRINTF(pReq, "      <td width=\"150\">\n");
//    WEB_PRINTF(pReq, "        <select name=\"func_type\" style=\"width:150\">\n");

//    sal_eee_get(&eee_enable);

//    if(SAL_ENABLED == eee_enable)
//    {
//        WEB_PRINTF(pReq, "      <option value=\"0\" >Disable \n");                
//        WEB_PRINTF(pReq, "      <option value=\"1\" selected >Enable \n");
//    }
//    else
//    {
//        WEB_PRINTF(pReq, "      <option value=\"0\" selected >Disable \n");                
//        WEB_PRINTF(pReq, "      <option value=\"1\" >Enable \n");    
//    }
    
//    WEB_PRINTF(pReq, "        </select>\n");
//    WEB_PRINTF(pReq, "      </td>\n");

//    WEB_PRINTF(pReq, "  </tr>\n");
//    WEB_PRINTF(pReq, "</table>\n");
//    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
//    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
//    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"loop\">\n");
//    WEB_PRINTF(pReq, "</form>\n");
//    WEB_PRINTF(pReq, "</fieldset>\n");
//    WEB_PRINTF(pReq, "</center>\n");
//    WEB_PRINTF(pReq, "</body>\n");
//    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_eee
 * ------------------
 * Processes EEE configuration form submission.
 * Extracts 'func_type' value from POST data, converts to integer.
 * Calls sal_eee_set() to enable/disable EEE feature.
 * Redirects back to GET handler to refresh the page.
 */
int32 post_eee(struct http_request_s *pReq)
{
    uint8           *pValue;
    uint8           enable;

    WEB_VAR(pReq, "func_type", &pValue);
    if (pValue) {
        enable = atoi(pValue);
    }

    if(enable)
        sal_eee_set(SAL_ENABLED);
    else
        sal_eee_set(SAL_DISABLED);

    
    get_eee(pReq);

    return SUCCESS;
}

#endif

/*
 * Function: get_mac_setup
 * -----------------------
 * Serves MAC address setup page with HTML/JavaScript.
 * Allows user to enter last 2 bytes of MAC (XX:XX format).
 * Validates input format, sends AJAX POST to /set_mac.
 * Displays success/error messages based on server response.
 */
int32 get_mac_setup(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_HEADER(pReq, "Access-Control-Allow-Origin: *");
    WEB_HEADER(pReq, "Access-Control-Allow-Methods: GET, POST, OPTIONS");
    WEB_HEADER(pReq, "Access-Control-Allow-Headers: Content-Type");

    /* Ultra-minimized HTML content for MAC setup page */
    WEB_PRINTF(pReq, "<html><head><title>MAC Setup</title>");
    WEB_PRINTF(pReq, "<style>body{font:13px Arial;background:#f4f4f4;margin:0;padding-top:50px;text-align:center}");
    WEB_PRINTF(pReq, ".c{background:#fff;padding:20px;border-radius:5px;box-shadow:0 1px 3px rgba(0,0,0,.1);width:300px;margin:0 auto}");
    WEB_PRINTF(pReq, "h1{font-size:20px;margin:0 0 15px}");
    WEB_PRINTF(pReq, "input{width:100%%;padding:8px;margin:10px 0;border:1px solid #ddd;border-radius:3px}");
    WEB_PRINTF(pReq, "button{background:#1e88e5;color:#fff;border:0;padding:8px 15px;border-radius:3px;cursor:pointer}");
    WEB_PRINTF(pReq, "#s{margin-top:15px;font-size:13px}.e{color:red}.g{color:green}</style></head>");
    WEB_PRINTF(pReq, "<body><div class=\"c\"><h1>Set MAC Address</h1>");
    WEB_PRINTF(pReq, "<p>Fixed prefix: A4:DA:22:52:</p>");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"m\" placeholder=\"Enter XX:XX\" maxlength=\"5\">");
    WEB_PRINTF(pReq, "<br><button id=\"b\">Set MAC</button><div id=\"s\"></div></div>");
    WEB_PRINTF(pReq, "<script>");
    WEB_PRINTF(pReq, "const p=\"A4:DA:22:52:\";");
    WEB_PRINTF(pReq, "document.getElementById(\"b\").onclick=function(){");
    WEB_PRINTF(pReq, "const i=document.getElementById(\"m\"),v=i.value.trim().toUpperCase(),s=document.getElementById(\"s\");");
    WEB_PRINTF(pReq, "if(!/^[0-9A-F]{2}:[0-9A-F]{2}$/.test(v)){s.textContent=\"Invalid format\";s.className=\"e\";return}");
    WEB_PRINTF(pReq, "s.textContent=\"Sending...\";s.className=\"\";");
    WEB_PRINTF(pReq, "var x=new XMLHttpRequest();");
    WEB_PRINTF(pReq, "x.open(\"POST\",\"/set_mac\");");
    WEB_PRINTF(pReq, "x.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");");
    WEB_PRINTF(pReq, "x.onload=function(){");
    WEB_PRINTF(pReq, "if(x.status==200){");
    WEB_PRINTF(pReq, "if(x.responseText==\"MAC_SET_SUCCESS\"){s.textContent=\"Success!\";s.className=\"g\";i.value=\"\"}");
    WEB_PRINTF(pReq, "else if(x.responseText==\"MAC_SET_NOT_ALLOWED\"){s.textContent=\"Not allowed\";s.className=\"e\"}");
    WEB_PRINTF(pReq, "else{s.textContent=\"Error\";s.className=\"e\"}}");
    WEB_PRINTF(pReq, "else{s.textContent=\"Error\";s.className=\"e\"}};");
    WEB_PRINTF(pReq, "x.onerror=function(){s.textContent=\"Failed\";s.className=\"e\"};");
    WEB_PRINTF(pReq, "x.send(\"mac=\"+p+v);};");
    WEB_PRINTF(pReq, "</script></body></html>");

    WEB_END(pReq);
    return SUCCESS;
}