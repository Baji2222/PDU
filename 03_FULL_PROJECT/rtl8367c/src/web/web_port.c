#pragma STRING(far)

#include <string.h>
#include <stdio.h>

#include <salport.h>
#include <salmirror.h>
#include <salrate.h>
#include <salmib.h>
#include <saltrunk.h>
#include <salutil.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_port.h"
#include "web_util.h"
#include <syslog/syslogdefs.h>

#define PORT_STAT_REFRESH_TIME_MAX      2

static uint8      g_portStat_need_refresh = FALSE;  
static uint8      g_portStat_refresh_cnt  = 0;
static uint8      showRmonCounters = FALSE;
static sal_port_t queryPort;


static sal_phyAbility_t ability_set[SAL_MAX_LOGIC_COMBO_PORT];
static uint8   port_set[SAL_MAX_LOGIC_COMBO_PORT];
static uint8 set_flag = 0;
static uint8 set_count = 0;

/*
 * Function: check_port_setting
 * ----------------------------
 * Delayed port setting application function.
 * Applies saved port settings after a delay count of 2.
 * Called periodically to handle port configuration updates.
 * Resets counters after applying settings.
 */
void check_port_setting()
{
    int16 i;
    static int16 count = 0;
    if(set_flag == 1)
        count++;
    if(count >1)
    {
        for(i = 0; i < set_count; i++)
        {
            sal_port_phyAbility_set(port_set[i], &ability_set[i]); 
        }
        set_count = 0;
        set_flag = 0;
        count = 0;
    }
}

/*
 * Function: get_port
 * ------------------
 * Displays port configuration page with current status.
 * Shows port state, speed/duplex, and flow control settings.
 * Includes form for applying changes to selected ports.
 * Auto-refreshes after port setting changes.
 */
int32 get_port(struct http_request_s *pReq)
{
    uint8   port;
    uint8   logicPortStr[MAX_LOGIC_ID_LEN];

    port = 0;
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Port Setting</title>\n");

    if (g_portStat_need_refresh == TRUE)
    {
        WEB_PRINTF(pReq, "<meta http-equiv=refresh content=2>\n");

        g_portStat_refresh_cnt++;
        if (g_portStat_refresh_cnt == PORT_STAT_REFRESH_TIME_MAX)
            g_portStat_need_refresh = FALSE;
    }

    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");

    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Port Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"port.cgi\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th class=MidSize>Port</th>\n");
    WEB_PRINTF(pReq, "      <th>State</th>\n");
    WEB_PRINTF(pReq, "      <th>Speed/Duplex</th>\n");
    WEB_PRINTF(pReq, "      <th>Flow Control</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"portid\" multiple size=\"6\" >\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        WEB_PRINTF(pReq, "          <option value=\"%bu\">%s\n", port, logicPortStr);
    }
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"state\" class=MidSize>\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Enable\n", (int16)ENABLE);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Disable\n", (int16)DISABLE);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"speed_duplex\" class=MidSize>\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Auto\n", (int16)SAL_PORT_SPEED_DUPLEX_AUTO);
    WEB_PRINTF(pReq, "          <option value=\"%d\">10M/Half\n", (int16)SAL_PORT_SPEED_DUPLEX_10_HALF);
    WEB_PRINTF(pReq, "          <option value=\"%d\">10M/Full\n", (int16)SAL_PORT_SPEED_DUPLEX_10_FULL);
    WEB_PRINTF(pReq, "          <option value=\"%d\">100M/Half\n", (int16)SAL_PORT_SPEED_DUPLEX_100_HALF);
    WEB_PRINTF(pReq, "          <option value=\"%d\">100M/Full\n", (int16)SAL_PORT_SPEED_DUPLEX_100_FULL);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"flow\" class=MidSize>\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Off\n", (int16)SAL_PORT_FLOW_CTRL_DISABLE);
    WEB_PRINTF(pReq, "          <option value=\"%d\">On\n", (int16)SAL_PORT_FLOW_CTRL_ENABLE);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"port\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "<hr>\n");
    
    get_portcurrstatus(pReq);
    
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_portcurrstatus
 * ----------------------------
 * Displays current port status table with actual vs configured values.
 * Shows admin state, configured and actual speed/duplex.
 * Also shows configured and actual flow control status.
 * Called from get_port to show real-time port status.
 */
int32 get_portcurrstatus(struct http_request_s *pReq)
{
    sal_phyAbility_t        ability;
    sal_port_macStatus_t    status;
    sal_enable_t            enable;
    uint8                   port;
    uint8                   logicPortStr[MAX_LOGIC_ID_LEN];

    enable = SAL_DISABLED;
    port = 0;
    
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th rowspan=\"2\" width=\"90\">Port</th>\n");
    WEB_PRINTF(pReq, "    <th rowspan=\"2\" width=\"90\">State</th>\n");
    WEB_PRINTF(pReq, "    <th colspan=\"2\">Speed/Duplex</th>\n");
    WEB_PRINTF(pReq, "    <th colspan=\"2\">Flow Control</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");

    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"90\">Config</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"90\">Actual</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"90\">Config</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"90\">Actual</th>\n");

    WEB_PRINTF(pReq, "  </tr>\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        sal_port_adminEnable_get(port, &enable);
        sal_port_phyAbility_get(port, &ability);
        sal_port_macStatus_get(port, &status);
            
        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <td>%s</td>\n", logicPortStr);
        WEB_PRINTF(pReq, "    <td>");
        if (enable == SAL_ENABLED) {
            WEB_PRINTF(pReq, "Enabled");
        }
        else {
            WEB_PRINTF(pReq, "Disabled");
        }
        WEB_PRINTF(pReq, "</td>\n");

        WEB_PRINTF(pReq, "    <td>");

        // ability
        /* Full_1000 equals 1 only when AutoNegotiation is enabled */
        if (ability.Full_1000== 1) {
            WEB_PRINTF(pReq, "Auto");
        }
        else
        {
            if(ability.Full_100 == 1)
            {
                WEB_PRINTF(pReq, "100 Full");
            }
            else if(ability.Half_100 == 1)
            {
                WEB_PRINTF(pReq, "100 Half");
            }
            else if(ability.Full_10==1)
            {
                WEB_PRINTF(pReq, "10 Full");
            }
            else if(ability.Half_10==1)
            {
                WEB_PRINTF(pReq, "10 Half");
            }
            else
                WEB_PRINTF(pReq, "Unknown");
        }
        WEB_PRINTF(pReq, "</td>\n");
        WEB_PRINTF(pReq, "    <td>");

        // Status
        if (SAL_DISABLED == status.linkup) {
            WEB_PRINTF(pReq, "Link Down");
        }
        else {

            if (status.speed == SAL_PORT_SPEED_10) {
                WEB_PRINTF(pReq, "10");
            }
            else if (status.speed == SAL_PORT_SPEED_100) {
                WEB_PRINTF(pReq, "100");
            }
            else if (status.speed == SAL_PORT_SPEED_1000) {
                WEB_PRINTF(pReq, "1000");
            }
            else
                WEB_PRINTF(pReq, "Unknown");

            if (status.duplex == SAL_PORT_DUPLEX_HALF) {
                WEB_PRINTF(pReq, "Half");
            }
            else if (status.duplex == SAL_PORT_DUPLEX_FULL) {
                WEB_PRINTF(pReq, "Full");
            }
        }

        WEB_PRINTF(pReq, "</td>\n");

        // ability
        WEB_PRINTF(pReq, "    <td>");
        if ((ability.FC & 0x1) == SAL_PORT_FLOW_CTRL_DISABLE) {
            WEB_PRINTF(pReq, "Off");
        }
        else if ((ability.FC & 0x1 )== SAL_PORT_FLOW_CTRL_ENABLE) {
            WEB_PRINTF(pReq, "On");
        }

        WEB_PRINTF(pReq, "</td>\n");
        WEB_PRINTF(pReq, "    <td>");

        // status
        if (status.linkup == ENABLE)
        {
            if (status.fc == SAL_PORT_FLOW_CTRL_DISABLE) {
                WEB_PRINTF(pReq, "Off");
            }
            else if (status.fc == SAL_PORT_FLOW_CTRL_ENABLE) {
                WEB_PRINTF(pReq, "On");
            }
        }
        else
            WEB_PRINTF(pReq, "Off");

        WEB_PRINTF(pReq, "</td>\n");


        WEB_PRINTF(pReq, "  </tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");

    return SUCCESS;
}

/*
 * Function: get_bandwidthcontrolstatus
 * ------------------------------------
 * Displays current bandwidth control status table.
 * Shows ingress and egress rates for each port.
 * Displays "Unlimited" if bandwidth control is disabled.
 * Called from get_port_bandwidthcontrol to show current settings.
 */
int32 get_bandwidthcontrolstatus (struct http_request_s *pReq)
{
    uint8   port = 0;
    uint32  ingress_spd, egress_spd;
    uint8   logicPortStr[MAX_LOGIC_ID_LEN];

    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th width=\"90\">Port</th>\n");
    WEB_PRINTF(pReq, "      <th>Ingress Rate (Kbit/sec)</th>\n");
    WEB_PRINTF(pReq, "      <th>Egress Rate (Kbit/sec) </th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);

        sal_rate_igrBandwidthCtrl_get(port, &ingress_spd);
        sal_rate_egrBandwidthCtrl_get(port, &egress_spd);

        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <td>%s</td>\n", logicPortStr);

        WEB_PRINTF(pReq, "    <td>");


        if (ingress_spd == BWCTRL_DISABLE_THRES) {
            WEB_PRINTF(pReq, "Unlimited");
        }
        else{
            WEB_PRINTF(pReq, "%ld", ingress_spd);
        }


        WEB_PRINTF(pReq, "    </td>\n");
        WEB_PRINTF(pReq, "    <td>");


        if (egress_spd == BWCTRL_DISABLE_THRES) {
            WEB_PRINTF(pReq, "Unlimited");
        }
        else{
            WEB_PRINTF(pReq, "%ld", egress_spd);
        }

        WEB_PRINTF(pReq, "    </td>\n");
        WEB_PRINTF(pReq, "  </tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    return SUCCESS;
}

/*
 * Function: get_port_bandwidthcontrol
 * -----------------------------------
 * Displays bandwidth control configuration page.
 * Shows form to set ingress/egress rates per port.
 * Includes enable/disable toggle with JavaScript.
 * Displays current bandwidth status table below.
 */
int32 get_port_bandwidthcontrol(struct http_request_s *pReq)
{
    uint8   port;
    uint8   logicPortStr[MAX_LOGIC_ID_LEN];

    port = 0;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Bandwidth Control Setting</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "  function chkEnable(value)\n");
    WEB_PRINTF(pReq, "  {\n");
    WEB_PRINTF(pReq, "      if ( value == '1' ) {\n");
    WEB_PRINTF(pReq, "          document.getElementById('rate').value = \'\';\n");
    WEB_PRINTF(pReq, "          document.getElementById('rate').disabled = false;\n");
    WEB_PRINTF(pReq, "      }\n");
    WEB_PRINTF(pReq, "      else { \n");
    WEB_PRINTF(pReq, "          document.getElementById('rate').value = \'Unlimited\';\n");
    WEB_PRINTF(pReq, "          document.getElementById('rate').disabled = true;\n");
    WEB_PRINTF(pReq, "      }\n");
    WEB_PRINTF(pReq, "  }\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");

    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Bandwidth Control Setting</legend>\n");

    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=bwctrl\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th>Port</th>\n");
    WEB_PRINTF(pReq, "      <th>Type</th>\n");
    WEB_PRINTF(pReq, "      <th>State</th>\n");
    WEB_PRINTF(pReq, "      <th nowrap>Rate(Kbit/sec)</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"portid\" multiple size=\"6\">\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", (uint16)port, logicPortStr);
    }
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"type\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Ingress\n", (uint16)0);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Egress\n", (uint16)1);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"state\" onload=\"chkEnable(this.value)\" onchange=\"chkEnable(this.value)\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Disable\n", (uint16)DISABLE);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Enable\n", (uint16)ENABLE);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td><input type=\"text\" size=\"6\" name=\"rate\" id=\"rate\" value=\'Unlimited\' disabled = true>"
    "(0-1000000, multiple of 8)</td>\n");

    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"bandwidthcontrol\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "<hr>");
    WEB_PRINTF(pReq, "<br>\n");
    get_bandwidthcontrolstatus(pReq);
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_port_mirroring
 * ----------------------------
 * Displays port mirroring configuration page.
 * Shows form to set mirror direction, mirroring port, and mirrored ports.
 * Displays current mirroring configuration table.
 * Includes delete button to remove mirroring configuration.
 */
int32
get_port_mirroring(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask;
    sal_port_t      port;
    uint32          mode;
    int16           i;    
    uint8           logicPortStr[MAX_LOGIC_ID_LEN];
    uint8           buf[128];

    port = 0;
    mode = PORT_MIRROR_OFF;

    sal_lPortMask_delAll(&portmask);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Port Monitoring Information</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Port Mirroring Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=mirroring\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" >Mirror Direction</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" >Mirroring Port</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" >Mirrored Port List</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\" width=\"120\">\n");
    WEB_PRINTF(pReq, "        <select name=\"mirror_direction\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Disable\n", (uint16)PORT_MIRROR_OFF);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Rx\n", (uint16)PORT_MIRROR_RX);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Tx\n", (uint16)PORT_MIRROR_TX);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Both\n", (uint16)PORT_MIRROR_BOTH);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"mirroring_port\">\n");
    for (i = 0; i < SAL_PORT_ID_CPU; i++)
    {
        if (!(sal_trunk_logicId_exist(i)))
            continue;
            
        sal_trunk_logicId_toStr(logicPortStr, i);
        WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", (uint16)i, logicPortStr);
    }
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"mirrored_port\">\n");
   
    for (i = 0; i < SAL_PORT_ID_CPU; i++)
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

    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Apply \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"mirror\">\n");
    WEB_PRINTF(pReq, "  <br>\n");
    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "<hr>\n");

    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=delete_mirror\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"120\">Mirror Direction</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"120\">Mirroring Port</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"200\">Mirrored Port List</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
 
    sal_mirror_portMirror_get(0, &mode, &port, &portmask);

    DBG_WEB_PRINTF(("Mode : %ld\n", mode));
    if (mode == PORT_MIRROR_BOTH)
        sprintf(buf, "Both");
    else if(mode == PORT_MIRROR_RX)
        sprintf(buf, "Rx");
    else if(mode == PORT_MIRROR_TX)
        sprintf(buf, "Tx");
    else
        sprintf(buf, "Disabled");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);

    if (mode != PORT_MIRROR_OFF)
    {
        WEB_PRINTF(pReq, "      <td align=\"center\">%d</td>\n", (uint16)(port + 1));
        lmask_to_comma(buf, &portmask);
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", buf);
    }
    else
    {
        WEB_PRINTF(pReq, "      <td align=\"center\"> - </td>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\"> - </td>\n");
    }
    WEB_PRINTF(pReq, "    </tr>\n");
 
    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Delete \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"del_mirror\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "<p>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_port_isolation
 * ----------------------------
 * Displays port isolation configuration page.
 * Shows form to select port and isolation list.
 * Displays current isolation settings for all ports.
 * Allows adding/removing ports from isolation list.
 */
int32 get_port_isolation(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask, isomask;
    uint32 i;
    uint8  port;
    uint8  logicPortStr[MAX_LOGIC_ID_LEN];
    uint8  portMaskStr[256];

    i = 0;

    sal_trunk_logicPort_setAll(&portmask);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Port Isolation</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Port Isolation Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=isolation\">\n");

    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\">Port</th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" width=\"140\">Port Isolation List</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");

    WEB_PRINTF(pReq, "  <tr>\n");

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"port\" multiple size=\"6\">\n");

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

    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"isolationlist\" multiple size=\"6\">\n");

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


    WEB_PRINTF(pReq, "  </tr>\n");

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Apply \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"portisolation\">\n");
    WEB_PRINTF(pReq, "  <br>\n");
    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "<hr>\n");

    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" width=\"80\">Port</th>\n");
    WEB_PRINTF(pReq, "    <th align=\"center\" width=\"200\">Port Isolation List</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&portmask, port))
        {
            continue;
        }

        strcpy(portMaskStr , "");
        sal_port_isolation_get(port, &isomask);
        sal_trunk_logicId_toStr(logicPortStr, port);
        lmask_to_comma(portMaskStr, &isomask);

        WEB_PRINTF(pReq, "    <tr>\n");

        if(strcmp(portMaskStr, "") == 0 )
        {
            WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", logicPortStr);
            WEB_PRINTF(pReq, "      <td align=\"center\">-</td>\n");
        }
        else
        {
            WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", logicPortStr);
            WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", portMaskStr);
        }
        WEB_PRINTF(pReq, "    </tr>\n");
    }


    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_port_statistics
 * -----------------------------
 * Displays port statistics table with Rx/Tx good/bad packets.
 * Shows admin state and link status for each port.
 * Includes clear button to reset all port statistics.
 * Optionally shows RMON MIB counters when queried.
 */
int32 get_port_statistics(struct http_request_s *pReq)
{
#define RMON_COUNTER_NUM    17

    sal_enable_t    enable;
    sal_enable_t    linkup;
    sal_port_t      port;
    uint8           logicPortStr[MAX_LOGIC_ID_LEN];
    uint32          mibHigh;  // it will not displayed
    uint32          rxgood;
    uint32          rxbad;
    uint32          txgood;
    uint32          txbad;
#if __WEB_RMON_COUNTER_QUERY__    
    uint8           i;    
    uint32          cntrh, cntrl;
#endif    

#if __WEB_RMON_COUNTER_QUERY__
    far const sal_stat_port_type_t rmon_cntr[RMON_COUNTER_NUM] = {
        SAL_ETHER_STATS_DROP_EVENTS_INDEX,
        SAL_ETHER_STATS_OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_INDEX,
        SAL_ETHER_STATS_BCAST_PKTS_INDEX,
        SAL_ETHER_STATS_MCAST_PKTS_INDEX,
        SAL_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,
        SAL_ETHER_STATS_UNDER_SIZE_PKTS_INDEX,
        SAL_ETHER_STATS_OVERSIZE_PKTS_INDEX,
        SAL_ETHER_STATS_FRAGMENTS_INDEX,
        SAL_ETHER_STATS_JABBERS_INDEX,
        SAL_ETHER_STATS_COLLISIONS_INDEX,
        SAL_ETHER_STATS_PKTS_64OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_65TO127OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_128TO255OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_256TO511OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_512TO1023OCTETS_INDEX,
        SAL_ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX
    };

    uint8* rmon_cntr_name[RMON_COUNTER_NUM];
#endif


    /* init */
    enable  = SAL_DISABLED;
    port    = 0;
    mibHigh = 0;
    rxgood  = 0;
    rxbad   = 0;
    txgood  = 0;
    txbad   = 0;

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Port Stistics</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Port Statistics Information</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=stats\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th width=\"80\">Port</th>\n");
    WEB_PRINTF(pReq, "      <th width=\"90\">State</th>\n");
    WEB_PRINTF(pReq, "      <th>Link Status</th>\n");
    WEB_PRINTF(pReq, "      <th>TxGoodPkt</th>\n");
    WEB_PRINTF(pReq, "      <th>TxBadPkt</th>\n");
    WEB_PRINTF(pReq, "      <th>RxGoodPkt</th>\n");
    WEB_PRINTF(pReq, "      <th>RxBadPkt</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        sal_port_adminEnable_get(port, &enable);
        sal_port_link_get(port, &linkup);
        sal_mib_rxGoodPkt_get(port, &mibHigh, &rxgood);
        sal_mib_rxBadPkt_get(port, &mibHigh, &rxbad);
        sal_mib_txGoodPkt_get(port, &mibHigh, &txgood);
        sal_mib_txBadPkt_get(port, &mibHigh, &txbad);

        WEB_PRINTF(pReq, "    <tr>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", logicPortStr);
        WEB_PRINTF(pReq, "      <td align=\"center\">");
        if (enable == SAL_ENABLED) {
            WEB_PRINTF(pReq, "Enabled");
        }
        else {
            WEB_PRINTF(pReq, "Disabled");
        }
        WEB_PRINTF(pReq, "</td>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\">");
        if (SAL_DISABLED == linkup) {
            WEB_PRINTF(pReq, "Link Down");
        }
        else {
            WEB_PRINTF(pReq, "Link Up");
        }
        WEB_PRINTF(pReq, "</td>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\">%lu</td>\n", (uint32)txgood);
        WEB_PRINTF(pReq, "      <td align=\"center\">%lu</td>\n", (uint32)txbad);
        WEB_PRINTF(pReq, "      <td align=\"center\">%lu</td>\n", (uint32)rxgood);
        WEB_PRINTF(pReq, "      <td align=\"center\">%lu</td>\n", (uint32)rxbad);
        WEB_PRINTF(pReq, "    </tr>\n");
    }

    WEB_PRINTF(pReq, "  </table>\n");

    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");

    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"submit\" value=\"   Clear   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"stats\">\n");
    
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");

#if __WEB_RMON_COUNTER_QUERY__
    /* port input section to display RMON MIB counters */
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>RMON MIB Counter</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/port.cgi?page=stat_rmon\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" >Port Number</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "    <tr>\n");        
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"rmon_query\">\n");
    for (port = 0; port < SAL_MAX_PHY_PORT; port++)
    {
        WEB_PRINTF(pReq, "          <option value=\"%d\">Port %d\n", (uint16)port, (uint16)(port + 1));
    }
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"submit\" value=\"   Query   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"stat_rmon\">\n");

    if(TRUE == showRmonCounters)
    {
        rmon_cntr_name[0] = "etherStatsDropEvents",
        rmon_cntr_name[1] = "etherStatsOctets",
        rmon_cntr_name[2] = "etherStatsPkts",
        rmon_cntr_name[3] = "etherStatsBroadcastPkts",
        rmon_cntr_name[4] = "etherStatsMulticastPkts",
        rmon_cntr_name[5] = "etherStatsCRCAlignErrors",
        rmon_cntr_name[6] = "etherStatsUndersizePkts",
        rmon_cntr_name[7] = "etherStatsOversizePkts",
        rmon_cntr_name[8] = "etherStatsFragments",
        rmon_cntr_name[9] = "etherStatsJabbers",
        rmon_cntr_name[10] = "etherStatsCollisions",
        rmon_cntr_name[11] = "etherStatsPkts64Octets",
        rmon_cntr_name[12] = "etherStatsPkts65to127Octets",
        rmon_cntr_name[13] = "etherStatsPkts128to255Octets",
        rmon_cntr_name[14] = "etherStatsPkts256to511Octets",
        rmon_cntr_name[15] = "etherStatsPkts512to1023Octets",
        rmon_cntr_name[16] = "etherStatsPkts1024to1518Octets",

        WEB_PRINTF(pReq, "<table border=\"1\">\n"); 
        WEB_PRINTF(pReq, "  <tr><th colspan=2>Port %u</th></tr>\n", (uint16)(queryPort + 1));

        for(i = 0; i < RMON_COUNTER_NUM;i++)
        {
            WEB_PRINTF(pReq, "  <tr>\n");
            WEB_PRINTF(pReq, "    <td width=\"300\">%s</td>\n", rmon_cntr_name[i]);            
            if(SAL_ERR_SUCCESS == sal_mib_port_get(queryPort, rmon_cntr[i], &cntrh, &cntrl))
                WEB_PRINTF(pReq, "    <td width=\"100\">%lu</td>", cntrl);
            else
                WEB_PRINTF(pReq, "    <td>0</td>\n");            
            WEB_PRINTF(pReq, "  </tr>\n");                                    
        }
        
        WEB_PRINTF(pReq, "</table>\n");

        /* disable show RMON counter */
        showRmonCounters = FALSE;
    }
    WEB_PRINTF(pReq, "</fieldset>\n");    
#endif

    
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_port
 * -------------------
 * Processes port configuration form submission.
 * Extracts port list, state, speed/duplex, and flow control settings.
 * Validates and stores settings for delayed application.
 * Triggers auto-refresh to show updated port status.
 */
int32 post_port(struct http_request_s *pReq)
{
    int32   ret;
    int16   i;
    uint8  *pValue;   
    uint32  settingCnt;     // the total port numbers to set
    Tbool   enable;
    uint8   duplex_spd;
    uint8   flowCtrl;

    settingCnt =0;
    enable = DISABLE;
    duplex_spd = 0;
    flowCtrl = 0;

    WEB_VAR(pReq, "portid", &pValue);
    if (pValue) {
        port_set[settingCnt++] = atoi(pValue);
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            port_set[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        enable = atoi(pValue);
    }
    
    WEB_VAR(pReq, "speed_duplex", &pValue);
    if (pValue) {
        duplex_spd = atoi(pValue);
    }
    
    WEB_VAR(pReq, "flow", &pValue);
    if (pValue) {
        flowCtrl = atoi(pValue);
    }

    for (i = 0; i < settingCnt; ++i) {
        if (!(sal_trunk_logicId_exist(port_set[i]))) {
            continue;
        }

        if ((ret = sal_port_adminEnable_set(port_set[i], enable)) != SAL_ERR_SUCCESS)
        {
            printerr(pReq, ret);
            return SUCCESS;    
        }            

        memset(&ability_set[i], 0, sizeof(sal_phyAbility_t));


        /* if configured as auto, all ability is set */
        if (duplex_spd == SAL_PORT_SPEED_DUPLEX_AUTO)
        {
        	ability_set[i].AutoNegotiation = 1;
            ability_set[i].Full_10   = 1;
            ability_set[i].Half_10   = 1;
            ability_set[i].Full_100  = 1;
            ability_set[i].Half_100  = 1;
            ability_set[i].Full_1000 = 1;

            if (flowCtrl == 1)
            {
                ability_set[i].FC = 1;
                ability_set[i].AsyFC = 1;
            }
        }        
        else
        {
            if (duplex_spd == SAL_PORT_SPEED_DUPLEX_10_HALF)
            {
                ability_set[i].Half_10 = 1;
            }
            else if (duplex_spd == SAL_PORT_SPEED_DUPLEX_10_FULL)
            {
                ability_set[i].Full_10 = 1;
            }
            else if (duplex_spd == SAL_PORT_SPEED_DUPLEX_100_HALF)
            {
                ability_set[i].Half_100 = 1;
            }
            else if (duplex_spd == SAL_PORT_SPEED_DUPLEX_100_FULL)
            {
                ability_set[i].Full_100 = 1;
            }

            if (flowCtrl == 1)
            {
                ability_set[i].FC = 1;
                ability_set[i].AsyFC = 1;
            }
        }

        DBG_WEB_PRINTF(("The Port Status setting parameter:\n"));
        DBG_WEB_PRINTF(("Port: %bd, Enable: %bd, Auto: %d, Half_10: %d, Full_10: %d, \
            Half_100: %d,\n Full_100: %d, Full_1000: %d, FC: %d\n", \
            (uint8)port_set[i], (uint8)enable, (uint16)ability_set[i].AutoNegotiation, \
            (uint16)ability_set[i].Half_10, (uint16)ability_set[i].Full_10, (uint16)ability_set[i].Half_100, \
            (uint16)ability_set[i].Full_100, (uint16)ability_set[i].Full_1000, (uint16)ability_set[i].FC));

        set_flag = 1;
        g_portStat_need_refresh = TRUE;
        set_count = settingCnt;
    }
    
    g_portStat_need_refresh = TRUE;
    g_portStat_refresh_cnt  = 0;
	syslog_add_new_buf_entry(SYSLOG_MSG_PORT_STATE_CHANGE, 0);

    get_port(pReq);
    return SUCCESS;
}

/*
 * Function: post_bandwidthcontrol
 * -------------------------------
 * Processes bandwidth control form submission.
 * Extracts port list, type (ingress/egress), state, and rate.
 * Validates rate range (0-1000000 Kbit/sec).
 * Applies settings to selected ports via API calls.
 */
int32 post_bandwidthcontrol(struct http_request_s *pReq)
{
    int32   ret;
    int16   i;
    uint8   *pValue;
    uint8   port[SAL_MAX_LOGIC_COMBO_PORT];
    uint8   settingCnt;     // the total port numbers to set
    uint8   type;
    Tbool   enable;
    uint32  rate = BWCTRL_DISABLE_THRES;

    settingCnt  = 0;
    type    = 0;
    enable  = DISABLE;

    WEB_VAR(pReq, "portid", &pValue);
    if (pValue) {
        port[settingCnt++] = atoi(pValue);
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            port[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "type", &pValue);
    if (pValue) {
        type = atoi(pValue);
    }
    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        enable = atoi(pValue);
    }
    WEB_VAR(pReq, "rate", &pValue);
    if (pValue) {
        rate = atol(pValue);

        if (rate < 0 || rate > 1000000UL)
        {
            DBG_WEB_PRINTF(("The rate is : %ld\n", rate));
            printmsg(pReq, "Invalid Control rate !!");
            return SUCCESS;
        }
    }

    for (i = 0; i < settingCnt; ++i) {

        if (!(sal_trunk_logicId_exist(port[i]))) {
            continue;
        }
        
        if ( enable == DISABLE )
            rate = BWCTRL_DISABLE_THRES;
    
        if ( type == 0 ) /*ingress*/
        {
            if ((ret = sal_rate_igrBandwidthCtrl_set(port[i], rate)) != SAL_ERR_SUCCESS) 
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
        }
        else     /*egress*/
        {
            if ((ret = sal_rate_egrBandwidthCtrl_set(port[i], rate)) != SAL_ERR_SUCCESS) 
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
        }
    }

    get_port_bandwidthcontrol(pReq);
    return SUCCESS;
}

/*
 * Function: post_port_isolation
 * -----------------------------
 * Processes port isolation form submission.
 * Extracts selected port and isolation list from POST data.
 * Handles both Port and Trunk selections.
 * Applies isolation mask to selected ports via API.
 */
int32
post_port_isolation(struct http_request_s *pReq)
{
    sal_lPortmask_t isomask;
    sal_pPortmask_t portMask1, portMask2;
    int32   ret;
    uint32  portlist;
    int16   i;
    uint8   settingPortCnt, port[SAL_MAX_LOGIC_COMBO_PORT];
    uint8   settingTrunkCnt, trunk[SAL_MAX_STATIC_COMBO_TRUNK];
    uint8   *pValue;
    uint8   isolatedPortCnt, isolatedPort[SAL_MAX_LOGIC_COMBO_PORT];
    uint8   isolatedTrunkCnt, isolatedTrunk[SAL_MAX_STATIC_COMBO_TRUNK];

    portlist = 0;
    settingPortCnt   = 0;
    settingTrunkCnt  = 0;
    isolatedPortCnt  = 0;
    isolatedTrunkCnt = 0;

    memset(port, sizeof(port), 0);
    memset(port, sizeof(trunk), 0);    
    memset(port, sizeof(isolatedPort), 0);
    memset(port, sizeof(isolatedTrunk), 0);    

    WEB_VAR(pReq, "port", &pValue);
    if (pValue) {
        if (strncmp(pValue, "Port", 4) == 0)
        {
            port[settingPortCnt++] = atoi(pValue + 5);
        }
        else
        if (strncmp(pValue, "Trunk", 5) == 0)
        {
            trunk[settingTrunkCnt++] = atoi(pValue + 5);
        }
    }


    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            if (strncmp(pValue, "Port", 4) == 0)
            {
                port[settingPortCnt++] = atoi(pValue + 5);
            }
            else
            if (strncmp(pValue, "Trunk", 5) == 0)
            {
                trunk[settingTrunkCnt++] = atoi(pValue + 5);
            }
        }
    }

    WEB_VAR(pReq, "isolationlist", &pValue);
    if (pValue) {
        if (strncmp(pValue, "Port", 4) == 0)
        {
            isolatedPort[isolatedPortCnt++] = atoi(pValue + 5);
        }
        else
        if (strncmp(pValue, "Trunk", 5) == 0)
        {
            isolatedTrunk[isolatedTrunkCnt++] = atoi(pValue + 5);
        }
    }


    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            if (strncmp(pValue, "Port", 4) == 0)
            {
                isolatedPort[isolatedPortCnt++] = atoi(pValue + 5);
            }
            else
            if (strncmp(pValue, "Trunk", 5) == 0)
            {
                isolatedTrunk[isolatedTrunkCnt++] = atoi(pValue + 5);
            }
        }
    }


    /* Get the port */
    /* Get the ports to be set*/
    sal_pPortMask_delAll(&portMask2);
    for (i = 0; i < settingTrunkCnt; i++)
    {
        sal_trunk_port_get(trunk[i] - 1, NULL, &portMask1, NULL);
        sal_pPortMask_or(&portMask2, &portMask2, &portMask1);
    }

    for (i = 0; i < settingPortCnt; i++)
    {
        sal_pPortMask_addPort(&portMask2, port[i] - 1);
    }

    sal_pPortMask_toUL(&portlist, &portMask2);

    /* Get the isolation port mask*/
    sal_pPortMask_delAll(&portMask2);
    for (i = 0; i < isolatedTrunkCnt; i++)
    {
        sal_trunk_port_get(isolatedTrunk[i] - 1, NULL, &portMask1, NULL);
        sal_pPortMask_or(&portMask2, &portMask2, &portMask1);
    }

    for (i = 0; i < isolatedPortCnt; i++)
    {
        sal_pPortMask_addPort(&portMask2, isolatedPort[i] - 1);
    }

    sal_lPortMask_copyPMask(&isomask, &portMask2);

    /* Call API */
    FOREACH_PORTINMASK(settingPortCnt, portlist){
        DBG_WEB_PRINTF(("Setting port number is: %bd\n", settingPortCnt));
        if ((ret = sal_port_isolation_set(settingPortCnt, isomask)) != SAL_ERR_SUCCESS) 
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
    }

    get_port_isolation(pReq);
    
    return SUCCESS;
}

/*
 * Function: post_port_mirroring
 * -----------------------------
 * Processes port mirroring configuration form submission.
 * Extracts mirror direction, mirroring port, and mirrored port list.
 * Handles both Port and Trunk selections for mirrored ports.
 * Applies mirroring configuration via sal_mirror_portMirror_set().
 */
int32
post_port_mirroring(struct http_request_s *pReq)
{
    sal_lPortmask_t lportmask;
    sal_pPortmask_t portmask, tmp;
    int32           ret;
    uint8           mode = PORT_MIRROR_OFF;
    uint8           port = 0, mirrorId = 0;
    uint8          *pValue;

    WEB_VAR(pReq, "mirror_direction", &pValue);
    if (pValue) {
        mode = atoi(pValue);
    }

    WEB_VAR(pReq, "mirroring_port", &pValue);
    if (pValue) {
        port = atoi(pValue);
    }
    
    sal_pPortMask_delAll(&portmask);
    sal_pPortMask_delAll(&tmp);

    WEB_VAR(pReq, "mirrored_port", &pValue);
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

    sal_lPortMask_copyPMask(&lportmask, &portmask);
    if ((ret = sal_mirror_portMirror_set(mirrorId, mode, port, &lportmask)) != SAL_ERR_SUCCESS) 
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

    get_port_mirroring(pReq);
    
    return SUCCESS;
}

/*
 * Function: post_port_mirroring_delete
 * ------------------------------------
 * Deletes current port mirroring configuration.
 * Calls sal_mirror_portMirror_set with PORT_MIRROR_OFF mode.
 * Refreshes mirroring page after deletion.
 */
int32
post_port_mirroring_delete(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask;
    int32           ret;
    
    sal_lPortMask_delAll(&portmask);
    
    if ((ret =  sal_mirror_portMirror_set(0, PORT_MIRROR_OFF, 0, &portmask)) != SAL_ERR_SUCCESS) 
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

    get_port_mirroring(pReq);

    return SUCCESS;
}

/*
 * Function: post_port_statistics_clear
 * ------------------------------------
 * Clears all port statistics counters.
 * Iterates through all ports and calls sal_mib_port_reset().
 * Refreshes statistics page after clearing.
 */
int32
post_port_statistics_clear(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask;
    uint8           port;
        
    sal_trunk_logicPort_setAll(&portmask);
    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        sal_mib_port_reset(port);
    }

    get_port_statistics(pReq);

    return SUCCESS;
}

#if __WEB_RMON_COUNTER_QUERY__
/*
 * Function: post_port_statistics_rmonQuery
 * ----------------------------------------
 * Processes RMON MIB counter query form submission.
 * Extracts port number from POST data.
 * Sets flag to display RMON counters on statistics page.
 * Refreshes statistics page with RMON data.
 */
int32
post_port_statistics_rmonQuery(struct http_request_s *pReq)
{
    uint8          *pValue;

    showRmonCounters = TRUE;

    WEB_VAR(pReq, "rmon_query", &pValue);
    if (pValue) {
        queryPort = atoi(pValue);
    }
    get_port_statistics(pReq);

    return SUCCESS;
}

#endif



