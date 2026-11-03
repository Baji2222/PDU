#pragma STRING(far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to forwarding function
 *
 * Feature : Response the related request of forwarding function
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <salrate.h>
#include <salerror.h>
#include <saltrunk.h>
#include <salportmask.h>
#include <salsys.h>
#include <web/web_api.h>
#include "web_fwd.h"
#include "web_util.h"



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
 * Function: jumboframe_get_actual_size
 * ------------------------------------
 * Converts jumbo frame code to actual byte size.
 * Maps SAL_JUMBO_FRAME_* constants to physical sizes.
 * Returns 0 if frameCode is invalid or unknown.
 */
uint32
jumboframe_get_actual_size(uint32 frameCode)
{
    uint32 frameSize = 0;

    if(SAL_JUMBO_FRAME_1522 == frameCode)
    {
        frameSize = 1522;
    }
    else
    if(SAL_JUMBO_FRAME_1536 == frameCode)
    {
        frameSize = 1536;
    }
    else
    if(SAL_JUMBO_FRAME_1552 == frameCode)
    {
        frameSize = 1552;
    }
    else
    if(SAL_JUMBO_FRAME_16k == frameCode)
    {
        frameSize = 16383;
    }

    return frameSize;
}

/*
 * Function: get_fwd
 * -----------------
 * Placeholder handler for forwarding page.
 * Currently displays simple text response.
 * Can be extended for main forwarding configuration.
 */
int32 get_fwd(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "This is fwd.cgi");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_fwd_jumboframe
 * ----------------------------
 * Displays Jumbo Frame configuration page.
 * Reads current frame size setting from driver.
 * Renders dropdown with all available frame sizes.
 * Highlights currently selected size in dropdown.
 */
int32 get_fwd_jumboframe(struct http_request_s *pReq)
{
    uint32 frameCode;
    uint32 i;

    frameCode = 0;
    sal_rate_maxFrameSize_get(&frameCode);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Jumbo Frame Setting</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Jumbo Frame Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"jumboframe\" action=\"/fwd.cgi?page=jumboframe\">\n");

    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"200\">Jumbo Frame (Bytes)</th>\n");

    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"jumboframe\">\n");

    for (i = SAL_JUMBO_FRAME_1522; i <= SAL_JUMBO_FRAME_16k; i++)
    {
        if (i == frameCode)
        {
            WEB_PRINTF(pReq, "          <option value=\"%d\" selected>%d\n", (uint16)i,
                (uint16)jumboframe_get_actual_size(i));
        }
        else
        {
            WEB_PRINTF(pReq, "          <option value=\"%d\">%d\n", (uint16)i,
                (uint16)jumboframe_get_actual_size(i));
        }
    }

    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "  </tr>\n");
    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"jumboframe\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_fwd_stormcontrol
 * ------------------------------
 * Displays Storm Control configuration page.
 * Shows current storm rates for all ports.
 * Provides form to set storm type, port, and rate.
 * Uses JavaScript to enable/disable rate input field.
 */
 int32 get_fwd_stormcontrol(struct http_request_s *pReq)
{
    uint32   i;
    uint8    logicPortStr[MAX_LOGIC_ID_LEN];
    uint8    port, buf[128];
    uint32   rate; /* Rate in packet per second */
    uint32   rate1; /* Rate in packet per second */
    uint32   rate2; /* Rate in packet per second */
    uint32   rate3; /* Rate in packet per second */
    sal_lPortmask_t portmask;

    /* init */
    i = 0;
    memset(buf, sizeof(buf), 0);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Storm Control</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");

    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "function chkEnable(value)\n");
    WEB_PRINTF(pReq, "{\n");
    WEB_PRINTF(pReq, "  if ( value == '0' ) {\n");
    WEB_PRINTF(pReq, "      document.getElementById('rate').value = \'\';\n");
    WEB_PRINTF(pReq, "      document.getElementById('rate').disabled = true;\n");
    WEB_PRINTF(pReq, "  }\n");
    WEB_PRINTF(pReq, "  else { \n");
    WEB_PRINTF(pReq, "      document.getElementById('rate').value = \'\';\n");
    WEB_PRINTF(pReq, "      document.getElementById('rate').disabled = false;\n");
    WEB_PRINTF(pReq, "  }\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");


    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Storm Control Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/fwd.cgi?page=storm_ctrl\">\n");

    WEB_PRINTF(pReq, "  <table border=\"1\">\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\">Storm Type</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\">Port</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\">State</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\">Rate (kbps)</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"storm_filter\" style=\"width:150px;\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Broadcast\n", (uint16)SAL_STORM_TYPE_BCAST);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Multicast\n", (uint16)SAL_STORM_TYPE_MCAST);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Unknown Unicast\n", (uint16)SAL_STORM_TYPE_UNKNOWN_UCAST);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Unknown Multicast\n", (uint16)SAL_STORM_TYPE_UNKNOWN_MCAST);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"portid\" multiple size=\"6\">\n");

    for (i = 0; i < SAL_PORT_ID_CPU; ++i)
    {
        if (!(sal_trunk_logicId_exist(i)))
        {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, i);
        WEB_PRINTF(pReq, "          <option value=\"%s\">%s\n", logicPortStr, logicPortStr);
    }

    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"action\" onchange=\"chkEnable(this.value)\" style=\"width:80px;\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Off\n", (uint16)FALSE);
    WEB_PRINTF(pReq, "          <option value=\"%d\">On\n", (uint16)TRUE);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" name=\"rate\" id=\"rate\" value = \'\' disabled = true> (8-1000000)</td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Apply \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"storm\">\n");

    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "  <hr>\n");
    WEB_PRINTF(pReq, "  <br>\n");

    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" style=\"width:80px;\"> Port </th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" >Broadcast (kbps)</th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" >Multicast (kbps)</th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" >Unknown Unicast (kbps)</th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" >Unknown Multicast (kbps)</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    sal_trunk_logicPort_setAll(&portmask);

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portmask, port)) {
            continue; /* logical port not selected */
        }

        if (!(sal_trunk_logicId_exist(port))) {
            continue; /* logical port not exist */
        }

        /* Print port info */
        sal_trunk_logicId_toStr(logicPortStr, port);

        WEB_PRINTF(pReq, "    <tr>\n");

        sprintf(buf, "%s", logicPortStr);
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);


        sal_rate_stormControl_get(port, SAL_STORM_TYPE_BCAST, &rate);
        sal_rate_stormControl_get(port, SAL_STORM_TYPE_MCAST, &rate1);
        sal_rate_stormControl_get(port, SAL_STORM_TYPE_UNKNOWN_UCAST, &rate2);
        sal_rate_stormControl_get(port, SAL_STORM_TYPE_UNKNOWN_MCAST, &rate3);

        if (STRM_CTRL_OFF == rate || 0 == rate)
            sprintf(buf, "%s", "Off");
        else
            sprintf(buf, "%ld", (uint32)rate);

        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);

        if (STRM_CTRL_OFF == rate1 || 0 == rate1)
            sprintf(buf, "%s", "Off");
        else
            sprintf(buf, "%ld", (uint32)rate1);

        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);

        if (STRM_CTRL_OFF == rate2 || 0 == rate2)
            sprintf(buf, "%s", "Off");
        else
            sprintf(buf, "%ld", (uint32)rate2);

        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);

        if (STRM_CTRL_OFF == rate3 || 0 == rate3)
            sprintf(buf, "%s", "Off");
        else
            sprintf(buf, "%ld", (uint32)rate3);

        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);
        WEB_PRINTF(pReq, "    </tr>\n");
}

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br>\n");

    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}
 
/*
 * Function: post_fwd_jumboframe
 * -----------------------------
 * Processes Jumbo Frame configuration form submission.
 * Extracts selected frame size from POST data.
 * Calls sal_rate_maxFrameSize_set() to apply change.
 * Redirects back to GET handler to refresh the page.
 */
int32 post_fwd_jumboframe(struct http_request_s *pReq)
{
    int32 ret;
    uint8  *pValue;
    uint32 frameCode;

    frameCode = 0;

    WEB_VAR(pReq, "jumboframe", &pValue);
    if (pValue) {
        frameCode = atoi(pValue);
    }

    if ((ret =  sal_rate_maxFrameSize_set(frameCode)) != SAL_ERR_SUCCESS) 
    {
        printerr(pReq, ret);
        return SUCCESS;
    }
    
    get_fwd_jumboframe(pReq);
    return SUCCESS;
}

/*
 * Function: post_fwd_stormcontrol
 * -------------------------------
 * Processes Storm Control configuration form submission.
 * Parses storm type, port selection, enable/off, and rate.
 * Validates rate range (8-1000000 kbps).
 * Applies setting to all selected ports via sal_rate_stormControl_set().
 */
 int32
post_fwd_stormcontrol(struct http_request_s *pReq)
{
    uint8           port;
    uint8           enable_storm;
    uint8           *pValue;    
    sal_pPortmask_t portmask, tmp;
    int32 ret;
    int32           iValue, stormType;
    uint32          rate;
    

    /* init */
    iValue = 0;
    stormType = 0;
    rate = 0;
    port = 0;

    WEB_VAR(pReq, "storm_filter", &pValue);
    if (pValue) {
        stormType = atoi(pValue);
    }

    WEB_VAR(pReq, "action", &pValue);
    if (pValue) {
        iValue = atoi(pValue);
        if (FALSE == iValue)
        {
            enable_storm = FALSE;
        }
        else
        if (TRUE == iValue)
        {
            enable_storm = TRUE;
        }
    }

    if(TRUE == enable_storm)
    {
        WEB_VAR(pReq, "rate", &pValue);
        if (pValue) 
        {
            iValue = atol(pValue);

            if (iValue < 8 || iValue > 1000000)
            {
                printmsg(pReq, "Invalid Storm Control rate !!");
                return SUCCESS;
            }

            rate = iValue;
        }        
    }
    else
    {
        rate = SAL_RATE_DISABLE;
    }

    sal_pPortMask_delAll(&portmask);

    WEB_VAR(pReq, "portid", &pValue);
    if (pValue) {
        if (strncmp(pValue, "Port", 4) == 0)
        {
            sal_pPortMask_addPort(&portmask, atoi(pValue + 5) - 1);
        }
        else
        if (strncmp(pValue, "Trunk", 5) == 0)
        {
            sal_trunk_port_get(atoi(pValue + 5) - 1, NULL, &portmask, NULL);
        }
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            sal_pPortMask_delAll(&tmp);
            if (strncmp(pValue, "Port", 4) == 0)
            {
                sal_pPortMask_addPort(&tmp, atoi(pValue + 5) - 1);
            }
            else
            if (strncmp(pValue, "Trunk", 5) == 0)
            {
                sal_trunk_port_get(atoi(pValue + 5) - 1, NULL, &tmp, NULL);
            }

            sal_pPortMask_or(&portmask, &portmask, &tmp);
        }
    }

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_FAILED == sal_pPortMask_chkPort(&portmask, port)) 
        {
            continue; /* physical port not selected */
        }
        
        if ((ret = sal_rate_stormControl_set(port, stormType, rate)) != SAL_ERR_SUCCESS) 
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
    }


    get_fwd_stormcontrol(pReq);
    return SUCCESS;
}