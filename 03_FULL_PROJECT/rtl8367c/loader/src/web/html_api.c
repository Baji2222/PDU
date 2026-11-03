#pragma STRING (far)

/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : 
*$Id: 
*/
#include <string.h>
#include <stdio.h>
#include <web/web_api.h>
#include "web_tftp.h"
#include "web_hidden.h"
#include "web_index.h"
#include "web_info.h"
#include "web_menu.h"
#include "web_file.h"
#include "web_http.h"
#include "web_reload.h"
#include "web_image.h"
#include "web_sys.h"

#include <salfwug.h>

int8 default_reply_asp_send(struct http_request_s *pReq, uint8  *pPath)  
{
    if (pReq == NULL || pPath == NULL)
        return FAILED;

    switch(pReq->method)
    {
    case HTTP_METHOD_GET:
        if (WEB_STRCMP(pReq->path, "/") == 0)
        {
           return get_menu(pReq);
        }
        else if(WEB_STRCMP(pReq->path, "/hidden.cgi") == 0)
        {
           return get_hidden(pReq);
        }
        else if (WEB_STRCMP(pReq->path, "/panel.cgi") == 0)
        {
            return get_hidden(pReq);
        }
        else if (WEB_STRCMP(pReq->path, "/info.cgi") == 0)
        {
            return get_info(pReq);
        }
        else if(WEB_STRCMP(pReq->path, "/menu.cgi") == 0)
        {
            return get_menu(pReq);
        }
        else if (WEB_STRCMP(pReq->path, "/tree.js") == 0)
        {
            return get_file(pReq, WEB_FILE_TREE_JS);
        }
        else if (WEB_STRCMP(pReq->path, "/style.css") == 0)
        {
           return get_file(pReq, WEB_FILE_STYLE_CSS);
        }
#if defined(UPGRADE_TFTP)        
        else if(WEB_STRCMP(pReq->path, "/tftpug.cgi") == 0)
        {
            return get_tftp(pReq);
        }
#endif        
        else if(WEB_STRCMP(pReq->path, "/httpug.cgi") == 0)
        {
            return get_httpupg(pReq);
        }
        else if (WEB_STRCMP(pReq->path, "/document.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_DOCUMENT_GIF);
        }
        else if (WEB_STRCMP(pReq->path, "/folder_opened.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_FOLDER_OPENED_GIF);
        }
        else if (WEB_STRCMP(pReq->path, "/folder_closed.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_FOLDER_CLOSED_GIF);
        }
        else if (WEB_STRCMP(pReq->path, "/plus.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_PLUS_GIF);
        }
        else if (WEB_STRCMP(pReq->path, "/minus.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_MINUS_GIF);
        }
        else if (WEB_STRCMP(pReq->path, "/reboot.cgi") == 0)
        {
             return get_reboot(pReq);
        }
        else
        {
            return get_info(pReq);        
        }
    case HTTP_METHOD_POST:
#if defined(UPGRADE_TFTP)    
        if (WEB_STRCMP(pReq->path, "/tftpug.cgi") == 0)
        {
           return post_tftp_fwupg(pReq);
        }
        else 
#endif        
        if (WEB_STRCMP(pReq->path, "/httpupg.cgi") == 0)
        {
           return post_http_firmupdate(pReq);
        }
        /* reboot.cgi */
        else if (WEB_STRCMP(pReq->path, "/reboot.cgi") == 0)
        {
             return post_reboot(pReq);
        }
    default:
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_NOT_IMPLEMENTED);
        WEB_HEADER(pReq, "");
        WEB_END(pReq);
        break;
    }

	return  SUCCESS;
}

