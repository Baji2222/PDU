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
 */


/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>

#include <local/config.h> 
#include <local/flash_api.h>

#include <salutil.h>
#include <salfwug.h>

#include <web/web_api.h>
#include "web_util.h"
#include "web_tftp.h"

#if WEB_ENABLE

static uint8 get_tftp_status = 0;

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

uint8 const far htm_tftp[] = "\
    <html>\n\
    \n\
    <head>\n\
    <title>TFTP Upload</title>\n\
    <link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n\
    </head>\n\
    <script language=\"JavaScript\">\n\
    function ConfirmUpgradeFW(s) {\n\
        if (confirm('Firmware upgrade will take several seconds to complete, continue?')) {\n\
            return true;\n\
        } else {\n\
            return false;\n\
        }\n\
    }\n\
    </script>\n\
    <body>\n\
    <center>\n\
    \n\
    <fieldset>\n\
    <legend>TFTP Firmware Upgrade</legend>\n\
    <form method=\"post\" action=\"tftpug.cgi\">\n\
      <table border=\"1\">\n\
        <tr>\n\
          <th width=\"200\">TFTP Server IP Address</th>\n\
          <td><input type=\"text\" name=\"ip\" value=\"192.168.1.2\" size=\"20\" maxlength=\"15\"></td>\n\
        </tr>\n\
        <tr>\n\
          <th>Image File Name</th>\n\
          <td><input type=\"text\" name=\"file\" value=\"final.bin\"maxlength=\"32\"></td>\n\
        </tr>\n\
      </table>\n\
      <br style=\"line-height:50%\">\n\
      <input type=\"submit\" value=\"Download Image\">\n\
      <input type=\"hidden\" name=\"cmd\" value=\"fwupg\">\n\
    </form>\n\
    </fieldset>\n\
    \n\
    </center>\n\
    </body>\n\
    </html>\n\
";


/*
 * Function Declaration
 */
int32 get_tftp(struct http_request_s *pReq)
{
    sal_fwug_status_t fwug_status;
    char              *statusStr;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");


    if(get_tftp_status == 0)
    {
        WEB_DATA(pReq, htm_tftp, sizeof(htm_tftp) - 1);
    }
    else
    {
        sal_fwug_status_get(&fwug_status);

        switch(fwug_status)
        {
        case SAL_FWUG_STATUS_FILE_NOT_FOUND:
            get_tftp_status = 0;
            statusStr = "Cannot find file, please refresh web and upload image agagin";
            break;
        case SAL_FWUG_STATUS_SERVER_NOT_FOUND:
            get_tftp_status = 0;
            statusStr = "Cannot find server, please refresh web and upload image agagin";
            break;
        case SAL_FWUG_STATUS_IMAGE_ERR:
            get_tftp_status = 0;            
            statusStr = "Image error, please refresh web and upload image again";
            break;
        case SAL_FWUG_STATUS_TRANSMITTING:
            statusStr = "Firmware upgrade is processing....";
            break;
        case SAL_FWUG_STATUS_NORMAL:
            get_tftp_status = 0;
            statusStr = "Firmware upgrade is not started.";
            break;
        case SAL_FWUG_STATUS_SEARCHING_SERVER:
            get_tftp_status = 1;
            statusStr = "Searching server, please wait";
            break;
        case SAL_FWUG_STATUS_SOCKET_ALLOCATE_FAIL:
            get_tftp_status = 0;
            statusStr = "Socket allcation fail.";
            break;
        default:
            get_tftp_status = 0;
            statusStr = "Unknown error. Please refresh web and upload again.";
        }        

        WEB_PRINTF(pReq, "<html>\n");
        WEB_PRINTF(pReq, "<head>\n");
        WEB_PRINTF(pReq, "<title>Firmware Upgrade Status</title>\n");
        WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
        if(get_tftp_status)
            WEB_PRINTF(pReq, "<meta http-equiv=refresh content=3>\n");      
        WEB_PRINTF(pReq, "</head>\n");
        WEB_PRINTF(pReq, "<body>\n");
        WEB_PRINTF(pReq, "<center>\n");
        WEB_PRINTF(pReq, "<fieldset>\n");
        WEB_PRINTF(pReq, "<legend>Firmware Upgrade Status</legend>\n");
        WEB_PRINTF(pReq, "  <b style=\"font-weight:normal;font-family: Geneva, Arial, Helvetica, sans-serif;letter-spacing:.45pt\">%s \n", statusStr);
        WEB_PRINTF(pReq, "  <p>\n");
        WEB_PRINTF(pReq, "</fieldset>\n");
        WEB_PRINTF(pReq, "</center>\n");
        WEB_PRINTF(pReq, "</body>\n");
        WEB_PRINTF(pReq, "</html>\n");
    }

    WEB_END(pReq);

    return SUCCESS;
}

int32 post_tftp_fwupg(struct http_request_s *pReq)
{
    uint8        *pValue;
    uint8        ipStr[16];
    uint8        str_file[32 + 1];
    sal_ipaddr_t ip;

    memset(ipStr, 0, sizeof(ipStr));
    memset(str_file, 0, sizeof(str_file));

    WEB_VAR(pReq, "ip", &pValue);
    if (pValue)
    {
        WEB_MEMCPY(ipStr, pValue, 15);
    }
    WEB_VAR(pReq, "file", &pValue);
    if (pValue)
    {
        WEB_MEMCPY(str_file, pValue, 15);
    }

    if (str_to_ip(&ip, ipStr) != 0) {
        printmsg(pReq, "Invalid IP Address.");
        return SUCCESS;
    }

    sal_fwug_tftp_init(ip, str_file);
    
    get_tftp_status = TRUE;

    get_tftp(pReq);

    return SUCCESS;
}

#endif

