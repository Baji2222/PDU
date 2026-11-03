#pragma STRING (far)

#include <stdio.h>
#include <string.h>

#include <local/web_config.h>


#include <salsys.h>
#include <saltrunk.h>
#include <salerror.h>
#include <salutil.h>


#include <web/web_api.h>
#include <web/http_debug.h>

#include "web_info.h"
#include "web_util.h"

/*
 * Function: get_info
 * ------------------
 * Displays system information page with device, network, and software details.
 * Retrieves MAC, IP, netmask, gateway, firmware version, and hardware info.
 * Renders responsive HTML cards with badges and styled layout.
 * Uses modern CSS with flexbox for clean, professional appearance.
 */
int32 get_info(struct http_request_s *pReq)
{
    sal_mac_t       mac;
    uint8           macStr[18];
    uint8           firmVer[SAL_SYS_STRING_LEN];
    uint8           firmDate[FIRMWARE_DATE_LEN];
    uint8           hdrVer[SAL_SYS_STRING_LEN];
    uint8           devDesc[SAL_SYS_STRING_LEN];    
    sal_ipaddr_t    ip, netmask, gateway;
    uint8           ipStr[16], netMaskStr[16], gatewayStr[16];

    DBG_LWPS_HTTP_PRINTF(("-------get_info()-------\n"));

    sal_sys_mac_get(&mac);    
    mac_to_str(macStr, mac.octet);    
    sal_sys_ip_get(&ip, &netmask);
    ip_to_str(ipStr, (uint8 *)&ip);
    ip_to_str(netMaskStr, (uint8 *)&netmask);
    sal_sys_gateway_get(&gateway);
    ip_to_str(gatewayStr, (uint8 *)&gateway);

    sal_sys_firmVersion_get(firmVer);
    sal_sys_firmDate_get(firmDate);
    sal_sys_hardwareVersion_get(hdrVer);
    sal_sys_dev_type_get(devDesc);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html lang=\"en\">\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<meta charset=\"UTF-8\">\n");
    WEB_PRINTF(pReq, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    WEB_PRINTF(pReq, "<title>System Information</title>\n");
    WEB_PRINTF(pReq, "<style>\n");
    WEB_PRINTF(pReq, "* { box-sizing: border-box; margin: 0; padding: 0; }\n");
    WEB_PRINTF(pReq, "body { font-family: Arial, sans-serif; background-color: #f4f7fa; }\n");
    WEB_PRINTF(pReq, ".container { max-width: 900px; margin: 20px auto; padding: 20px; }\n");
    WEB_PRINTF(pReq, ".page-title { text-align: center; margin-bottom: 30px; color: #333; font-size: 28px; }\n");
    WEB_PRINTF(pReq, ".dashboard { display: flex; flex-wrap: wrap; gap: 20px; justify-content: center; }\n");
    WEB_PRINTF(pReq, ".card { background: white; border-radius: 10px; box-shadow: 0 5px 15px rgba(0,0,0,0.08); padding: 20px; flex: 1 1 300px; margin-bottom: 20px; }\n");
    WEB_PRINTF(pReq, ".card-header { margin-bottom: 15px; padding-bottom: 10px; border-bottom: 1px solid #eee; display: flex; justify-content: space-between; align-items: center; }\n");
    WEB_PRINTF(pReq, ".card-title { font-size: 18px; font-weight: 600; color: #333; }\n");
    WEB_PRINTF(pReq, ".card-content { display: flex; flex-direction: column; gap: 15px; }\n");
    WEB_PRINTF(pReq, ".info-row { display: flex; align-items: baseline; margin-bottom: 10px; }\n");
    WEB_PRINTF(pReq, ".info-label { font-weight: bold; width: 140px; color: #555; }\n");
    WEB_PRINTF(pReq, ".info-value { color: #333; flex-grow: 1; }\n");
    WEB_PRINTF(pReq, ".badge { display: inline-block; padding: 5px 10px; border-radius: 20px; font-size: 14px; font-weight: 500; background-color: #e0e0e0; color: #333; }\n");
    WEB_PRINTF(pReq, ".badge-primary { background-color: #e3f2fd; color: #1565c0; }\n");
    WEB_PRINTF(pReq, ".badge-success { background-color: #e8f5e9; color: #2e7d32; }\n");
    WEB_PRINTF(pReq, ".badge-info { background-color: #e0f7fa; color: #00838f; }\n");
    WEB_PRINTF(pReq, "</style>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<div class=\"container\">\n");
    WEB_PRINTF(pReq, "  <h1 class=\"page-title\">System Information</h1>\n");
    
    WEB_PRINTF(pReq, "  <div class=\"dashboard\">\n");
    
    // Device Card
    WEB_PRINTF(pReq, "    <div class=\"card\">\n");
    WEB_PRINTF(pReq, "      <div class=\"card-header\">\n");
    WEB_PRINTF(pReq, "        <h2 class=\"card-title\">Device Information</h2>\n");
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "      <div class=\"card-content\">\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Device Type</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\"><span class=\"badge badge-primary\">OPL SMART PDU RACK</span></div>\n");
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">MAC Address</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\">%s</div>\n", macStr);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Hardware Version</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\">06062025</div>\n");
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "    </div>\n");
    
    // Network Card
    WEB_PRINTF(pReq, "    <div class=\"card\">\n");
    WEB_PRINTF(pReq, "      <div class=\"card-header\">\n");
    WEB_PRINTF(pReq, "        <h2 class=\"card-title\">Network Configuration</h2>\n");
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "      <div class=\"card-content\">\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">IP Address</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\"><span class=\"badge badge-info\">%s</span></div>\n", ipStr);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Netmask</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\">%s</div>\n", netMaskStr);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Gateway</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\">%s</div>\n", gatewayStr);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "    </div>\n");
    
    // Firmware Card
    WEB_PRINTF(pReq, "    <div class=\"card\">\n");
    WEB_PRINTF(pReq, "      <div class=\"card-header\">\n");
    WEB_PRINTF(pReq, "        <h2 class=\"card-title\">Software Information</h2>\n");
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "      <div class=\"card-content\">\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Firmware Version</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\"><span class=\"badge badge-success\">%s</span></div>\n", firmVer);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "        <div class=\"info-row\">\n");
    WEB_PRINTF(pReq, "          <div class=\"info-label\">Firmware Date</div>\n");
    WEB_PRINTF(pReq, "          <div class=\"info-value\">%s</div>\n", firmDate);
    WEB_PRINTF(pReq, "        </div>\n");
    
    WEB_PRINTF(pReq, "      </div>\n");
    WEB_PRINTF(pReq, "    </div>\n");
    
    WEB_PRINTF(pReq, "  </div>\n"); // End of dashboard
    WEB_PRINTF(pReq, "</div>\n"); // End of container
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

#if __WEB_CABLE_DIAGNOSTIC__

/*
 * Function: get_cable_diag
 * ------------------------
 * Performs cable diagnostic test on selected ports.
 * Reads checkbox values to determine which ports to test.
 * Starts RTCT (Real-Time Cable Test) on selected ports.
 * Displays test results: Normal, Open, Short, or Mismatch.
 * Shows cable fault distance in meters for each pair (A, B, C, D).
 */
int32 get_cable_diag(http_request_t *pReq)
{
    int8                i;
	char                strPort[4];
    char                strCheck[48];
    char                strResult[12];
    char                strDescription[80];
    uint8               *pValue;
    sal_rtctResult_t    rtctResult;
    sal_pPortmask_t     selectPmsk;
    int32               ret;
    uint32 j;

    /* get selected portmask */
    selectPmsk.bits[0] = 0;
    for (i = 0; i < (MAX_PORT_ONBOARD - SAL_MAX_EXT_PORT); i++)
    {
        sprintf(strPort, "P%d", (int16)(i+1));    
        WEB_VAR(pReq, strPort, &pValue);
        if (pValue) /* enable */
            selectPmsk.bits[0] |= 1UL << i;
    }

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Port Setting</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Cable Diagnostic</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"get\" action=\"poe.cgi\">\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"60\">Check</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"80\">Port</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"100\">Test Result</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"440\">Cable Fault Distance</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");

    for (i = 0; i < (MAX_PORT_ONBOARD - SAL_MAX_EXT_PORT); i++)
    {
        if (selectPmsk.bits[0] & (1UL << i))
        {
            sal_port_rtct_start(i);

        }
    }
    if(selectPmsk.bits[0] != 0)
    {
        for(j=0;j<0x9ffff;j++);
    }

    for (i = 0; i < (MAX_PORT_ONBOARD - SAL_MAX_EXT_PORT); i++)
    {
        if (selectPmsk.bits[0] & (1UL << i))
        {
            memset(&rtctResult, 0, sizeof(sal_rtctResult_t));
            
            ret = sal_port_rtctResult_get(i, &rtctResult);

            sprintf(strCheck, "<input type=checkbox name=P%d checked>", (int16)(i+1));

            if (SAL_ERR_FAILED == ret)
            {
                sprintf(strResult, "ERROR");
                sprintf(strDescription, "Failed");
            }
            else if (SAL_ERR_NOT_SUPPORT == ret)
            {
                sprintf(strResult, "-");
                sprintf(strDescription, "Not support.");
            }
            else
            {
                if (SAL_PORT_SPEED_1000 == rtctResult.linkType)
                {
                    if (rtctResult.ge_result.channelAShort | \
                        rtctResult.ge_result.channelBShort)
                    {
                        sprintf(strResult, "Short");
                    }
                    else if (rtctResult.ge_result.channelAOpen | \
                             rtctResult.ge_result.channelBOpen)
                    {
                        sprintf(strResult, "Open");
                    }
                    else if (rtctResult.ge_result.channelAMismatch | \
                             rtctResult.ge_result.channelBMismatch)
                    {
                        sprintf(strResult, "Mismatch");
                    }
                    else
                    {
                        sprintf(strResult, "Normal");
                    }
                    sprintf(strDescription, "PairA:%u(M), PairB:%u(M), PairC:%u(M), PairD:%u(M)\n", \
                        (rtctResult.ge_result.channelALen / 40), \
                        (rtctResult.ge_result.channelBLen / 40), \
                        (rtctResult.ge_result.channelCLen / 40), \
                        (rtctResult.ge_result.channelDLen / 40));
                }
            }
        }
        else
        {
            sprintf(strCheck, "<input type=checkbox name=P%d>", (uint16)(i+1));
            sprintf(strResult, "-");
            sprintf(strDescription, "-");
        }

        /* Display */
        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <td>%s</td>\n", strCheck);
        WEB_PRINTF(pReq, "    <td>Port %d</td>\n", (int16)(i + 1));
        WEB_PRINTF(pReq, "    <td>%s</td>\n", strResult);
        WEB_PRINTF(pReq, "    <td>%s</td>\n", strDescription);
        WEB_PRINTF(pReq, "  </tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"page\" value=\"cable_diag\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

#endif


