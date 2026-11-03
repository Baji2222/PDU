#pragma STRING (far)

#include <string.h> 

#include <salerror.h>

#include <web/web_api.h>
#include "web_util.h"

/*
 * Function: errMsg_get
 * --------------------
 * Retrieves error message string from error code.
 * Wrapper around sal_err_msg_get() function.
 * Returns human-readable error description.
 * Used by printerr to display error messages.
 */
const char *
errMsg_get(int32 errCode)
{
    const char* errStr;

    errStr = sal_err_msg_get(errCode);

    return errStr;
    /*
    if (memcmp(errStr, "Unknown error", sizeof("Unknown error")) != 0)
        return errStr;
        */
}

/*
 * Function: printerr
 * ------------------
 * Displays error message page and redirects back.
 * Shows error alert in popup window via JavaScript.
 * Redirects to /goback.cgi to return to previous page.
 * Used to display API/function errors to user.
 */
void
printerr (
    struct http_request_s *pReq,
    unsigned short  errno
    )
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Message</title>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "window.location.replace(\"/goback.cgi\");\n");
    WEB_PRINTF(pReq, "window.open(\'/alert.cgi?alertmsg=Error: %s\',\'hidden_frame\',\'\');\n", 
            errMsg_get(errno));
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);

}

/*
 * Function: printmsg
 * ------------------
 * Displays custom message page and redirects back.
 * Removes newline characters from message for safe display.
 * Shows message alert in popup window via JavaScript.
 * Redirects to /goback.cgi after showing message.
 */
void
printmsg (
    struct http_request_s *pReq,
    char *  msg
    )
{
    int i;
    char msg_temp[128];

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Message</title>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    i = 0;
    strncpy(msg_temp, msg, 128);
    while(msg_temp[i] != '\0')
    {
            if(msg_temp[i] == '\n')
            {
            msg_temp[i] = ' ';
            }
            i ++;
    }
    WEB_PRINTF(pReq, "window.location.replace(\"/goback.cgi\");\n");
    WEB_PRINTF(pReq, "window.open(\'/alert.cgi?alertmsg=%s\',\'hidden_frame\',\'\');\n", msg_temp);
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);
}

/*
 * Function: strnstr
 * -----------------
 * Finds first occurrence of substring needle in haystack.
 * Searches within first 'hlen' characters of haystack.
 * Returns pointer to beginning of match or NULL if not found.
 * Safe version of strstr with length limit.
 */
char *
strnstr (const char *haystack, const char *needle, uint16 hlen)
{
    uint16 nlen;
                                                                                
    if (!needle || !*needle)
        return (char *)haystack;
    else
        nlen = strlen (needle);
                                                                                
    while (hlen >= nlen) {
        if (*needle == *haystack && !strncmp (haystack + 1, needle + 1,
                                              nlen - 1))
            return (char *) haystack;
        haystack++; hlen--;
    }
                                                                                
    return NULL;
}