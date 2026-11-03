#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to IP function
 *
 * Feature : Response the request related to IP function
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>

#include <salutil.h>
#include <salsys.h>
#include <salflash.h>
#include <flash_config.h>
#include <syslog/syslog.h>
#include <syslog/syslogdefs.h>
#include <sal/salsyslog.h>

#include <web/web_api.h>
#include "web_ip.h"
#include "web_util.h"   
#include <lib/lib_time.h>
#if 1
/*
 * Symbol Definition
 */
extern volatile uint32 data current_time;
/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/*
 * Function: int_to_str_syslog_severity
 * ------------------------------------
 * Converts syslog severity numeric value to display string.
 * Maps severity codes (Emergency, Alert, Error, etc.) to readable text.
 * Prints formatted table cell with severity name.
 * Used for syslog buffer display formatting.
 */
static void int_to_str_syslog_severity(uint8 severity, struct http_request_s *pReq)
{
	switch(severity)
	{
		case SYSLOG_SEV_EMERG:
			 WEB_PRINTF(pReq, "    <td width=\"15\">%s</td>\n", "Emergency");
			break;
		case SYSLOG_SEV_ALERT:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Alert");
			break;
		case SYSLOG_SEV_ERR:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Error");
			break;
		case SYSLOG_SEV_CRIT:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Critical");
			break;
		case SYSLOG_SEV_WARNING:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Warning");
			break;
		case SYSLOG_SEV_NOTICE:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Notice");
			break;
		case SYSLOG_SEV_INFO:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Info");
			break;
		case SYSLOG_SEV_DEBUG:
			WEB_PRINTF(pReq, "	  <td width=\"15\">%s</td>\n", "Debug");
			break;
		default:
			break;
	}
	return;
}

/*
 * Function: get_syslogbufsec
 * --------------------------
 * Displays syslog buffer contents in a formatted table.
 * Shows log ID, timestamp, severity, and message content.
 * Reads buffer entries from flash and converts timestamps.
 * Provides option to clear syslog messages.
 */
int32 get_syslogbufsec(struct http_request_s *pReq)
{
	sal_enable_t	syslogStatus = 0;
	uint16 headIndex = 0;
	uint16 tailIndex = 0;
	sal_flash_syslog_buf_entry_t sysLogBufEntry;
	uint16 logId = 1;
	struct tm ts;
	uint32 timestamp;
	char timeStr[30]={""};

//	syslog_head_idx_get(&headIndex);	
//	syslog_head_idx_get(&tailIndex);
	sal_syslog_buf_head_idx_get(&headIndex);
	sal_syslog_buf_tail_idx_get(&tailIndex);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
	WEB_HEADER(pReq, "Content-Type: text/html");

	WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<head>\n");
	WEB_PRINTF(pReq, "<title>Syslog Buffer</title>\n");
	WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
	WEB_PRINTF(pReq, "</head>\n");
	WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<body>\n");
	WEB_PRINTF(pReq, "<center>\n");
	WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<fieldset>\n");
	WEB_PRINTF(pReq, "<legend>System Logs</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"syslogbufsec\" action=\"/syslogbuf.cgi?page=syslogbufsec\">\n");

	if(headIndex == tailIndex)
	{
		WEB_PRINTF(pReq, "<legend>No Syslog message to show..</legend>\n");
	}
	else
	{
		WEB_PRINTF(pReq, "<table border=\"1\">\n");

		WEB_PRINTF(pReq, "	<tr>\n");
		WEB_PRINTF(pReq, "	  <th width=\"200\">Syslog Clear</th>\n");

		WEB_PRINTF(pReq, "		<td width=\"150\">\n");
		WEB_PRINTF(pReq, "		  <select name=\"state\" style=\"width:150\">\n");

		WEB_PRINTF(pReq, "		<option value=\"0\" selected >No \n");				  
		WEB_PRINTF(pReq, "		<option value=\"1\" >Yes \n");

		WEB_PRINTF(pReq, "		  </select>\n");
		WEB_PRINTF(pReq, "		</td>\n");

		WEB_PRINTF(pReq, "	<tr>\n");
		WEB_PRINTF(pReq, "</table>\n");

		WEB_PRINTF(pReq, "	<input type=\"submit\" value=\"   Apply   \">\n");
		WEB_PRINTF(pReq, "	<input type=\"hidden\" name=\"cmd\" value=\"syslogbuf\">\n");
		WEB_PRINTF(pReq, "	<br style=\"line-height:50%%\">\n");


		WEB_PRINTF(pReq, "<br>\n");
		WEB_PRINTF(pReq, "<table border=\"1\">\n");

	//	WEB_PRINTF(pReq, "	  <th width=\"15\">%d</th>\n", (uint16)headIndex);
	//	WEB_PRINTF(pReq, "	  <th width=\"15\">%d</th>\n", (uint16)tailIndex);
		WEB_PRINTF(pReq, "	  <th width=\"15\">ID</th>\n");
		WEB_PRINTF(pReq, "	  <th width=\"45\">Time</th>\n");
		WEB_PRINTF(pReq, "	  <th width=\"15\">Severity</th>\n");
		WEB_PRINTF(pReq, "	  <th width=\"450\">Message Content</th>\n");
	//headIndex = 0;
//	tailIndex = 1;


		while(headIndex != tailIndex)
		{
			memset(&sysLogBufEntry, 0x00, sizeof(sal_flash_syslog_buf_entry_t));
			sal_flash_syslog_buf_get(&sysLogBufEntry, headIndex);
			timestamp = sysLogBufEntry.timestamp;

			// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
			calcDate(&ts, timestamp);
			sprintf(timeStr, "%s %d %02d:%02d:%02d	  \0", months[ts.tm_mon], (uint16)ts.tm_mday, (uint16)ts.tm_hour, (uint16)ts.tm_min, (uint16)ts.tm_sec);

			WEB_PRINTF(pReq, "  <tr>\n");
	//		WEB_PRINTF(pReq, "    <td width=\"15\">%d</td>\n", (uint16)sysLogBufEntry.sysLogIdBuf);
			WEB_PRINTF(pReq, "    <td width=\"15\">%d</td>\n", (uint16)logId);
			WEB_PRINTF(pReq, "    <td width=\"45\">%s</td>\n", timeStr);
			int_to_str_syslog_severity(sysLogBufEntry.sysLogSevBuf, pReq);

			if(sysLogBufEntry.sysLogIdBuf == SYSLOG_MSG_PORT_LINK_DOWN || sysLogBufEntry.sysLogIdBuf == SYSLOG_MSG_PORT_LINK_UP)
			{
				WEB_PRINTF(pReq, "    <td width=\"450\">Port %d %s</td>\n", (uint16)sysLogBufEntry.addInfo, syslogmsgcontent[sysLogBufEntry.sysLogIdBuf]);
			}
			else
			{
				WEB_PRINTF(pReq, "    <td width=\"450\">%s</td>\n", syslogmsgcontent[sysLogBufEntry.sysLogIdBuf]);
			}
			
	//		WEB_PRINTF(pReq, "    <td align=\"left\" style=\"width:250px;\">%s</td align=\"center\">\n", "HAPPY");
			WEB_PRINTF(pReq, "  </tr>\n");
			logId++;
			headIndex++;
			if(headIndex == SAL_SYS_LOG_BUF_MAX_ENTRY)
			{
				headIndex = 0; //wrap around
			}
		}
		WEB_PRINTF(pReq, "</table>\n");
	}

	WEB_PRINTF(pReq, "</form>\n");
	WEB_PRINTF(pReq, "</fieldset>\n");
	WEB_PRINTF(pReq, "</center>\n");
	WEB_PRINTF(pReq, "</body>\n");
	WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;

}

/*
 * Function: post_syslogbufsec
 * ---------------------------
 * Processes syslog buffer clear form submission.
 * Extracts clear state (Yes/No) from POST data.
 * If Yes, resets head/tail indices and clears buffer.
 * Saves configuration to flash and shows confirmation message.
 */
int32 post_syslogbufsec(struct http_request_s *pReq)
{
    uint8           *pValue;
	sal_enable_t	syslogBufClear = 0;

    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        syslogBufClear = atoi(pValue);
    }
	
    if (syslogBufClear == 1)
    {
		sal_sys_syslog_headIdx_set(DFLT_SYSLOG_BUF_HEAD_IDX, 1);
		sal_sys_syslog_tailIdx_set(DFLT_SYSLOG_BUF_TAIL_IDX, 1);

		//Clear buffer
		sal_syslog_buf_head_idx_set(DFLT_SYSLOG_BUF_HEAD_IDX);	
		sal_syslog_buf_tail_idx_set(DFLT_SYSLOG_BUF_TAIL_IDX);
		
		sal_flash_cfg_save();

		printmsg(pReq, "Syslog Messages Cleared..");
    }
      
    get_syslogbufsec(pReq);

    return SUCCESS;
}
#endif








///*
// * Function: get_pdu
// * -----------------
// * Displays PDU (Power Distribution Unit) Rack 8 ports control panel page.
// * Auto-refreshes status every 7 seconds via JavaScript.
// * Fetches voltage, current, and port status from MCU.
// *
// * Visual rules:
// *   - OFF                     ? complete card gray
// *   - ON  (current = 0.1 A)   ? card normal/clear, socket gray
// *   - ON  + current > 0.1 A   ? card normal/clear, socket GREEN
// */
//int32 get_pdu(struct http_request_s *pReq)
//{
//    int port = 0;
//    WEB_BEGIN(pReq);
//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: text/html");
//    WEB_PRINTF(pReq, "<html lang=\"en\">\n");
//    WEB_PRINTF(pReq, "<head>\n");
//    WEB_PRINTF(pReq, "<meta charset=\"UTF-8\">\n");
//    WEB_PRINTF(pReq, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
//    WEB_PRINTF(pReq, "<title>PDU Rack 8 Outlets Control</title>\n");
//    WEB_PRINTF(pReq, "<style>\n");
//    WEB_PRINTF(pReq, "* { box-sizing: border-box; margin: 0; padding: 0; }\n");
//    WEB_PRINTF(pReq, ":root { --bg1:#eef0f2; --bg2:#f5f6f8; --panel:#ffffff; --panel2:#ffffff; --border:#dde1e6; --green:#2f9e44; --green-dark:#1e7e34; --green-light:#d4edda; --red:#e03131; --red-light:#f8d7da; --grey:#868e96; --txt:#1c1f23; --txtdim:#6b7280; }\n");
//    WEB_PRINTF(pReq, "body { font-family: 'Segoe UI', Arial, sans-serif; background: var(--bg1); min-height: 100vh; padding: 14px; color: var(--txt); }\n");
//    WEB_PRINTF(pReq, ".container { max-width: 1180px; margin: 0 auto; }\n");
//    WEB_PRINTF(pReq, ".header { border-bottom: 2px solid var(--green); padding: 4px 4px 12px; margin-bottom: 20px; display: flex; align-items: center; justify-content: space-between; flex-wrap: wrap; gap: 10px; }\n");
//    WEB_PRINTF(pReq, ".page-title { font-size: 18px; font-weight: 700; letter-spacing: 0.5px; text-transform: uppercase; color: var(--txt); display: flex; align-items: center; gap: 8px; }\n");
//    WEB_PRINTF(pReq, ".page-title .ico { font-size: 18px; }\n");
//	
//    WEB_PRINTF(pReq, ".header-right { display: flex; align-items: center; gap: 28px; }\n");
//		WEB_PRINTF(pReq, ".header-item { display: flex; flex-direction: column; align-items: flex-end; gap: 2px; min-width: 140px; }\n");
//		WEB_PRINTF(pReq, ".header-item .label { font-size: 11px; font-weight: 600; color: var(--txt); text-transform: uppercase; letter-spacing: 0.6px; }\n");
//		WEB_PRINTF(pReq, ".header-item b { font-size: 18px; font-weight: 700; color: var(--txt); letter-spacing: 0.5px; line-height: 1.2; }\n");
//	
//    WEB_PRINTF(pReq, ".port-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 22px; }\n");
//    WEB_PRINTF(pReq, "@media (max-width: 900px) { .port-grid { grid-template-columns: repeat(2, 1fr); } }\n");

//    /* Base card (normal / clear look) */
//    WEB_PRINTF(pReq, ".port-item { position: relative; background: var(--panel); border: 1px solid var(--border); border-radius: 12px; padding: 20px; text-align: left; box-shadow: 0 1px 3px rgba(20,22,25,0.06); transition: border-color 0.2s, box-shadow 0.2s, background 0.3s, filter 0.3s; }\n");
//    WEB_PRINTF(pReq, ".port-item:hover { box-shadow: 0 3px 10px rgba(20,22,25,0.1); }\n");
//    WEB_PRINTF(pReq, ".port-item:active { box-shadow: 0 1px 2px rgba(20,22,25,0.08); }\n");

//    /* OFF ? COMPLETELY GRAY */
//    WEB_PRINTF(pReq, ".port-item.off-gray { background: #e8e8e8 !important; border-color: #c0c0c0 !important; box-shadow: none; }\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-label,\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-status,\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-current,\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-current .current-value,\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-controls label,\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-controls span { color: #9e9e9e !important; }\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .port-apply { background: #d0d0d0 !important; border-color: #b0b0b0 !important; color: #757575 !important; }\n");
//    WEB_PRINTF(pReq, ".port-item.off-gray .socket-icon { filter: grayscale(100%%); opacity: 0.65; }\n");

//    /* ON ? normal clear card (no special background) */
//    WEB_PRINTF(pReq, ".port-item.on-normal { background: var(--panel); border-color: var(--border); }\n");

//    WEB_PRINTF(pReq, ".port-item.pending { opacity: 0.65; pointer-events: none; }\n");

//    WEB_PRINTF(pReq, ".port-top { display: flex; align-items: center; justify-content: space-between; gap: 8px; margin-bottom: 16px; }\n");
//    WEB_PRINTF(pReq, ".port-info { display: flex; flex-direction: column; gap: 4px; }\n");
//    WEB_PRINTF(pReq, ".socket-icon { width: 100px; height: 100px; flex-shrink: 0; }\n");
//    WEB_PRINTF(pReq, ".socket-icon svg { width: 120%%; height: 120%%; }\n");
//    WEB_PRINTF(pReq, ".socket-icon .outline { stroke-width: 3; }\n");
//    WEB_PRINTF(pReq, ".socket-icon .pins { fill: none; stroke-width: 3; }\n");

//    /* Socket GREEN only when current > 0.1 */
//    WEB_PRINTF(pReq, ".socket-icon.on .outline { stroke:#1E8E3E; stroke-width:4; fill:#D8F0DB; }\n");
//    WEB_PRINTF(pReq, ".socket-icon.on .pins { stroke:#1E8E3E; stroke-width:4; }\n");
//    WEB_PRINTF(pReq, ".socket-icon.on { border:3px solid #1E8E3E; border-radius:12px; padding:4px; box-sizing:border-box; }\n");

//    /* Socket GRAY (default / no current) */
//    WEB_PRINTF(pReq, ".socket-icon.off .outline, .socket-icon.off .pins { stroke:#A0A0A0; opacity:1; }\n");
//    WEB_PRINTF(pReq, ".socket-icon.off .fill { fill:#E5E5E5; opacity:0.30; }\n");
//    WEB_PRINTF(pReq, ".socket-icon.off { border:3px solid #E5E5E5; border-radius:12px; padding:4px; box-sizing:border-box; }\n");

//    WEB_PRINTF(pReq, ".port-label { font-size: 16px; font-weight: 700; letter-spacing: 0.5px; text-transform: uppercase; color: var(--txtdim); }\n");
//    WEB_PRINTF(pReq, ".port-status { font-size: 19px; font-weight: 700; letter-spacing: 0.5px; }\n");
//    WEB_PRINTF(pReq, ".port-status.on { color: var(--green); }\n");
//    WEB_PRINTF(pReq, ".port-status.off { color: var(--red); }\n");
//    
//    WEB_PRINTF(pReq, ".port-current { font-size: 13px; color: var(--txtdim); margin-top: 2px; }\n");
//    WEB_PRINTF(pReq, ".port-current .current-value { font-weight: 600; color: var(--txt); }\n");
//    
//    WEB_PRINTF(pReq, ".port-controls { display: flex; align-items: center; gap: 16px; margin-top: 14px; padding: 12px 0; border-top: 1px solid var(--border); }\n");
//    WEB_PRINTF(pReq, ".port-controls label { display: flex; align-items: center; gap: 5px; font-size: 13px; font-weight: 600; cursor: pointer; }\n");
//    WEB_PRINTF(pReq, ".port-controls input[type=\"radio\"] { accent-color: var(--green); width: 16px; height: 16px; cursor: pointer; }\n");
//    WEB_PRINTF(pReq, ".port-controls input[type=\"radio\"]:checked + span { color: var(--green); }\n");
//    WEB_PRINTF(pReq, ".port-controls input[type=\"radio\"][value=\"0\"]:checked + span { color: var(--red); }\n");
//    WEB_PRINTF(pReq, ".port-controls input[type=\"radio\"][value=\"0\"] { accent-color: var(--red); }\n");

//    WEB_PRINTF(pReq, ".port-apply { width:100%%; padding:10px 0; margin-top:6px; border:2px solid #9E9E9E; border-radius:6px; font-size:13px; font-weight:700; letter-spacing:0.5px; text-transform:uppercase; cursor:pointer; transition:all 0.2s; background:#C0C0C0; color:#000000; }\n");
//    WEB_PRINTF(pReq, ".port-apply:hover { transform:translateY(-1px); box-shadow:0 2px 8px rgba(0,0,0,0.15); }\n");
//    WEB_PRINTF(pReq, ".port-apply:disabled { cursor:not-allowed; opacity:1; }\n");
//    WEB_PRINTF(pReq, ".port-apply.on-state { background: #A5D6A7; border-color: #81C784; color: #1b5e20; }\n");
//    WEB_PRINTF(pReq, ".port-apply.on-state:hover { background: #90C695; border-color: #81C784; color: #1b5e20; }\n");
//    WEB_PRINTF(pReq, ".port-apply.off-state { background: #E0E0E0; border-color: #BDBDBD; color: #424242; }\n");
//    WEB_PRINTF(pReq, ".port-apply.off-state:hover { background: #D5D5D5; border-color: #BDBDBD; color: #424242; }\n");
//    WEB_PRINTF(pReq, ".port-apply.applying { background: #E0E0E0; border-color: #BDBDBD; color: #424242; cursor:wait; animation:pulse 1s infinite; }\n");
//    WEB_PRINTF(pReq, "@keyframes pulse { 0%%,100%% { opacity:1; } 50%% { opacity:0.7; } }\n");
//    WEB_PRINTF(pReq, ".port-apply.applied { background:#E0E0E0; border-color:#BDBDBD; color:#424242; }\n");
//    WEB_PRINTF(pReq, "</style>\n");
//    WEB_PRINTF(pReq, "</head>\n");

//    /* ===== JavaScript ===== */
//    WEB_PRINTF(pReq, "<script>\n");
//    WEB_PRINTF(pReq, "let fetchFlag = true;\n");
//    WEB_PRINTF(pReq, "let fetchInterval;\n");
//    WEB_PRINTF(pReq, "async function getPduData() {\n");
//    WEB_PRINTF(pReq, "  try {\n");
//    WEB_PRINTF(pReq, "    const response = await fetch('/get_pdu_status', { cache: 'no-store' });\n");
//    WEB_PRINTF(pReq, "    if (!response.ok) throw new Error(`HTTP ${response.status}`);\n");
//    WEB_PRINTF(pReq, "    const res1 = await response.json();\n");
//    WEB_PRINTF(pReq, "    const voltage = res1.v || res1.V || 0;\n");
//    WEB_PRINTF(pReq, "    const voltageEl = document.getElementById('voltageDisplay');\n");
//    WEB_PRINTF(pReq, "    if (voltageEl) voltageEl.textContent = voltage.toFixed(1) + 'V';\n");
//    WEB_PRINTF(pReq, "    const ports = res1.p || res1.P || [0,0,0,0,0,0,0,0];\n");
//    WEB_PRINTF(pReq, "    let perPortCurrent = [0,0,0,0,0,0,0,0];\n");
//    WEB_PRINTF(pReq, "    if (res1.i && Array.isArray(res1.i)) {\n");
//    WEB_PRINTF(pReq, "      perPortCurrent = res1.i;\n");
//    WEB_PRINTF(pReq, "    } else if (res1.I && Array.isArray(res1.I)) {\n");
//    WEB_PRINTF(pReq, "      perPortCurrent = res1.I;\n");
//    WEB_PRINTF(pReq, "    } else {\n");
//    WEB_PRINTF(pReq, "      perPortCurrent = [\n");
//    WEB_PRINTF(pReq, "        res1.I1 || 0, res1.I2 || 0, res1.I3 || 0, res1.I4 || 0,\n");
//    WEB_PRINTF(pReq, "        res1.I5 || 0, res1.I6 || 0, res1.I7 || 0, res1.I8 || 0\n");
//    WEB_PRINTF(pReq, "      ];\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    let totalCurrent = 0;\n");
//    WEB_PRINTF(pReq, "    for (let i = 0; i < 8; i++) {\n");
//    WEB_PRINTF(pReq, "      const currentVal = perPortCurrent[i] ? perPortCurrent[i] / 100 : 0;\n");
//    WEB_PRINTF(pReq, "      totalCurrent += currentVal;\n");
//    WEB_PRINTF(pReq, "      applyPortState(i + 1, ports[i] || 0, currentVal);\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    const currentEl = document.getElementById('currentDisplay');\n");
//    WEB_PRINTF(pReq, "    if (currentEl) currentEl.textContent = totalCurrent.toFixed(1) + 'A';\n");
//    WEB_PRINTF(pReq, "  } catch (error) { console.error('Error fetching PDU data:', error); }\n");
//    WEB_PRINTF(pReq, "}\n");
//    
//    WEB_PRINTF(pReq, "function applyPortState(portNumber, portStatus, portCurrent) {\n");
//    WEB_PRINTF(pReq, "  const socket = document.getElementById(`socket-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const status = document.getElementById(`port-status-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const item = document.getElementById(`port-item-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const radioOn = document.getElementById(`radio-on-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const radioOff = document.getElementById(`radio-off-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const applyBtn = document.getElementById(`apply-btn-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const currentDisplay = document.getElementById(`current-display-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  if (!socket) return;\n");
//    WEB_PRINTF(pReq, "  if (currentDisplay) {\n");
//    WEB_PRINTF(pReq, "    currentDisplay.textContent = (portCurrent || 0).toFixed(2) + 'A';\n");
//    WEB_PRINTF(pReq, "  }\n");

//    WEB_PRINTF(pReq, "  if (portStatus === 1) {\n");
//    WEB_PRINTF(pReq, "    /* Port is ON ? card always normal/clear */\n");
//    WEB_PRINTF(pReq, "    if (status) { status.textContent = 'ON'; status.className = 'port-status on'; }\n");
//    WEB_PRINTF(pReq, "    if (item) item.className = 'port-item on-normal';\n");
//    WEB_PRINTF(pReq, "    if (radioOn) radioOn.checked = true;\n");
//    WEB_PRINTF(pReq, "    if (radioOff) radioOff.checked = false;\n");
//    WEB_PRINTF(pReq, "    if (applyBtn) { applyBtn.textContent = 'Apply'; applyBtn.className = 'port-apply on-state'; }\n");

//    WEB_PRINTF(pReq, "    if (portCurrent > 0.1) {\n");
//    WEB_PRINTF(pReq, "      /* current > 0.1 ? socket GREEN */\n");
//    WEB_PRINTF(pReq, "      socket.className = 'socket-icon on';\n");
//    WEB_PRINTF(pReq, "    } else {\n");
//    WEB_PRINTF(pReq, "      /* ON but current = 0.1 ? socket GRAY */\n");
//    WEB_PRINTF(pReq, "      socket.className = 'socket-icon off';\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    socket.dataset.state = 'on';\n");
//    WEB_PRINTF(pReq, "  } else {\n");
//    WEB_PRINTF(pReq, "    /* Port is OFF ? COMPLETELY GRAY card + gray socket */\n");
//    WEB_PRINTF(pReq, "    socket.className = 'socket-icon off';\n");
//    WEB_PRINTF(pReq, "    socket.dataset.state = 'off';\n");
//    WEB_PRINTF(pReq, "    if (status) { status.textContent = 'OFF'; status.className = 'port-status off'; }\n");
//    WEB_PRINTF(pReq, "    if (item) item.className = 'port-item off-gray';\n");
//    WEB_PRINTF(pReq, "    if (radioOn) radioOn.checked = false;\n");
//    WEB_PRINTF(pReq, "    if (radioOff) radioOff.checked = true;\n");
//    WEB_PRINTF(pReq, "    if (applyBtn) { applyBtn.textContent = 'Apply'; applyBtn.className = 'port-apply off-state'; }\n");
//    WEB_PRINTF(pReq, "  }\n");
//    WEB_PRINTF(pReq, "}\n");

//    WEB_PRINTF(pReq, "async function applyPort(portNumber) {\n");
//    WEB_PRINTF(pReq, "  const socket = document.getElementById(`socket-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const applyBtn = document.getElementById(`apply-btn-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const item = document.getElementById(`port-item-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  const radioOn = document.getElementById(`radio-on-${portNumber}`);\n");
//    WEB_PRINTF(pReq, "  if (!socket || !applyBtn || applyBtn.disabled) return;\n");
//    WEB_PRINTF(pReq, "  const newState = (radioOn && radioOn.checked) ? 1 : 0;\n");
//    WEB_PRINTF(pReq, "  const currentValue = (socket.dataset.state === 'on') ? 1 : 0;\n");
//    WEB_PRINTF(pReq, "  if (newState === currentValue) {\n");
//    WEB_PRINTF(pReq, "    applyBtn.textContent = 'Already Set';\n");
//    WEB_PRINTF(pReq, "    const stateClass = newState === 1 ? 'on-state' : 'off-state';\n");
//    WEB_PRINTF(pReq, "    setTimeout(() => { applyBtn.textContent = 'Apply'; applyBtn.className = 'port-apply ' + stateClass; }, 10000);\n");
//    WEB_PRINTF(pReq, "    return;\n");
//    WEB_PRINTF(pReq, "  }\n");
//    WEB_PRINTF(pReq, "  const applyStart = Date.now();\n");
//    WEB_PRINTF(pReq, "  let ok = true;\n");
//    WEB_PRINTF(pReq, "  fetchFlag = false;\n");
//    WEB_PRINTF(pReq, "  if (item) item.classList.add('pending');\n");
//    WEB_PRINTF(pReq, "  applyBtn.disabled = true;\n");
//    WEB_PRINTF(pReq, "  applyBtn.textContent = `Applying Outlet ${portNumber}...`;\n");
//    WEB_PRINTF(pReq, "  applyBtn.className = 'port-apply applying';\n");
//    WEB_PRINTF(pReq, "  const controller = new AbortController();\n");
//    WEB_PRINTF(pReq, "  const timeoutId = setTimeout(() => controller.abort(),5000);\n");
//    WEB_PRINTF(pReq, "  try {\n");
//    WEB_PRINTF(pReq, "    const url = `/set_port${portNumber}?state=${newState}`;\n");
//    WEB_PRINTF(pReq, "    const startTime = Date.now();\n");
//    WEB_PRINTF(pReq, "    const response = await fetch(url, { cache: 'no-store', signal: controller.signal });\n");
//    WEB_PRINTF(pReq, "    if (!response.ok) throw new Error(`HTTP ${response.status}`);\n");
//    WEB_PRINTF(pReq, "    const elapsed = Date.now() - startTime;\n");
//    WEB_PRINTF(pReq, "    if (elapsed < 1000) await new Promise(resolve => setTimeout(resolve, 1000 - elapsed));\n");
//    WEB_PRINTF(pReq, "    await response.text();\n");
//    WEB_PRINTF(pReq, "    applyPortState(portNumber, newState, 0);\n");
//    WEB_PRINTF(pReq, "  } catch (error) {\n");
//    WEB_PRINTF(pReq, "    clearTimeout(timeoutId);\n");
//    WEB_PRINTF(pReq, "    ok = false;\n");
//    WEB_PRINTF(pReq, "    if (error.name === 'AbortError') {\n");
//    WEB_PRINTF(pReq, "      applyBtn.textContent = 'Timeout - Retry';\n");
//    WEB_PRINTF(pReq, "    } else {\n");
//    WEB_PRINTF(pReq, "      applyBtn.textContent = 'Failed - Retry';\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    applyBtn.className = 'port-apply';\n");
//    WEB_PRINTF(pReq, "  } finally {\n");
//    WEB_PRINTF(pReq, "    clearTimeout(timeoutId);\n");
//    WEB_PRINTF(pReq, "    if (ok) {\n");
//    WEB_PRINTF(pReq, "      for (let i = 0; i < 5; i++) {\n");
//    WEB_PRINTF(pReq, "        if ((socket.dataset.state === 'on' ? 1 : 0) === newState) break;\n");
//    WEB_PRINTF(pReq, "        if (i < 4) await new Promise(r => setTimeout(r, 350));\n");
//    WEB_PRINTF(pReq, "      }\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    const totalElapsed = Date.now() - applyStart;\n");
//    WEB_PRINTF(pReq, "    if (totalElapsed < 3000) {\n");
//    WEB_PRINTF(pReq, "      await new Promise(resolve => setTimeout(resolve, 2000 - totalElapsed));\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    if (ok) {\n");
//    WEB_PRINTF(pReq, "      applyBtn.textContent = 'Apply';\n");
//    WEB_PRINTF(pReq, "      const stateClass = newState === 1 ? 'on-state' : 'off-state';\n");
//    WEB_PRINTF(pReq, "      applyBtn.className = 'port-apply ' + stateClass;\n");
//    WEB_PRINTF(pReq, "    }\n");
//    WEB_PRINTF(pReq, "    if (item) item.classList.remove('pending');\n");
//    WEB_PRINTF(pReq, "    applyBtn.disabled = false;\n");
//    WEB_PRINTF(pReq, "    fetchFlag = true;\n");
//    WEB_PRINTF(pReq, "  }\n");
//    WEB_PRINTF(pReq, "}\n");

//    WEB_PRINTF(pReq, "function startTimer() {\n");
//    WEB_PRINTF(pReq, "  fetchInterval = setInterval(() => { if (fetchFlag) getPduData(); }, 7000);\n");
//    WEB_PRINTF(pReq, "}\n");
//    WEB_PRINTF(pReq, "document.addEventListener('DOMContentLoaded', function() {\n");
//    WEB_PRINTF(pReq, "  setTimeout(function() { getPduData(); }, 300);\n");
//    WEB_PRINTF(pReq, "  startTimer();\n");
//    WEB_PRINTF(pReq, "});\n");
//    WEB_PRINTF(pReq, "</script>\n");

//    WEB_PRINTF(pReq, "<body>\n");
//    WEB_PRINTF(pReq, "<div class=\"container\">\n");
//    WEB_PRINTF(pReq, "  <div class=\"header\">\n");
//    WEB_PRINTF(pReq, "    <div class=\"page-title\"><span class=\"ico\">&#9889;</span>PDU Rack 8 &mdash; Control Panel</div>\n");
//		WEB_PRINTF(pReq, "    <div class=\"header-right\">\n");
//		WEB_PRINTF(pReq, "      <span class=\"header-item\">\n");
//		WEB_PRINTF(pReq, "        <span class=\"label\">INPUT VOLTAGE</span>\n");
//		WEB_PRINTF(pReq, "        <b id=\"voltageDisplay\">0.0V</b>\n");
//		WEB_PRINTF(pReq, "      </span>\n");
//		WEB_PRINTF(pReq, "      <span class=\"header-item\">\n");
//		WEB_PRINTF(pReq, "        <span class=\"label\">TOTAL CURRENT</span>\n");
//		WEB_PRINTF(pReq, "        <b id=\"currentDisplay\">0.0A</b>\n");
//		WEB_PRINTF(pReq, "      </span>\n");
//		WEB_PRINTF(pReq, "    </div>\n");
//    WEB_PRINTF(pReq, "  </div>\n");
//    WEB_PRINTF(pReq, "  <div class=\"port-grid\">\n");
//    for (port = 1; port <= 8; port++) {
//        WEB_PRINTF(pReq, "    <div id=\"port-item-%d\" class=\"port-item off-gray\">\n", port);
//        WEB_PRINTF(pReq, "      <div class=\"port-top\">\n");
//        WEB_PRINTF(pReq, "        <div class=\"port-info\">\n");
//        WEB_PRINTF(pReq, "          <div class=\"port-label\">Outlet %d</div>\n", port);
//        WEB_PRINTF(pReq, "          <div id=\"port-status-%d\" class=\"port-status off\">OFF</div>\n", port);
//        WEB_PRINTF(pReq, "          <div class=\"port-current\">Current: <span id=\"current-display-%d\" class=\"current-value\">0.00A</span></div>\n", port);
//        WEB_PRINTF(pReq, "        </div>\n");
//        WEB_PRINTF(pReq, "        <div id=\"socket-%d\" class=\"socket-icon off\" data-state=\"off\">\n", port);
//        WEB_PRINTF(pReq, "          <svg viewBox=\"0 0 120 120\">\n");
//        WEB_PRINTF(pReq, "            <rect class=\"outline fill\" x=\"5\" y=\"5\" width=\"90\" height=\"90\" rx=\"15\" ry=\"15\"/>\n");
//        WEB_PRINTF(pReq, "            <rect class=\"pins\" x=\"43\" y=\"10\" width=\"14\" height=\"30\" rx=\"3\"/>\n");
//        WEB_PRINTF(pReq, "            <rect class=\"pins\" x=\"20\" y=\"58\" width=\"16\" height=\"28\" rx=\"3\"/>\n");
//        WEB_PRINTF(pReq, "            <rect class=\"pins\" x=\"64\" y=\"58\" width=\"16\" height=\"28\" rx=\"3\"/>\n");
//        WEB_PRINTF(pReq, "          </svg>\n");
//        WEB_PRINTF(pReq, "        </div>\n");
//        WEB_PRINTF(pReq, "      </div>\n");
//        WEB_PRINTF(pReq, "      <div class=\"port-controls\">\n");
//        WEB_PRINTF(pReq, "        <label><input type=\"radio\" name=\"port-%d-state\" id=\"radio-on-%d\" value=\"1\"> <span>ON</span></label>\n", port, port);
//        WEB_PRINTF(pReq, "        <label><input type=\"radio\" name=\"port-%d-state\" id=\"radio-off-%d\" value=\"0\" checked> <span>OFF</span></label>\n", port, port);
//        WEB_PRINTF(pReq, "      </div>\n");
//        WEB_PRINTF(pReq, "      <button id=\"apply-btn-%d\" class=\"port-apply off-state\" onclick=\"applyPort(%d)\">Apply</button>\n", port, port);
//        WEB_PRINTF(pReq, "    </div>\n");
//    }
//    WEB_PRINTF(pReq, "  </div>\n");
//    WEB_PRINTF(pReq, "</div>\n");
//    WEB_PRINTF(pReq, "</body>\n");
//    WEB_PRINTF(pReq, "</html>\n");
//    WEB_END(pReq);
//    return SUCCESS;
//}



int32 get_pdu(struct http_request_s *pReq)
{
    int port = 0;
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_PUTS(pReq, "<html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"><title>PDU Rack 8 Outlets Control</title><style>\n");
    WEB_PUTS(pReq, "*{box-sizing:border-box;margin:0;padding:0}"
        ":root{--bg1:#eef0f2;--bg2:#f5f6f8;--panel:#fff;--panel2:#fff;--border:#dde1e6;--green:#2f9e44;--green-dark:#1e7e34;--green-light:#d4edda;--red:#e03131;--red-light:#f8d7da;--grey:#868e96;--txt:#1c1f23;--txtdim:#6b7280}"
        "body{font-family:'Segoe UI',Arial,sans-serif;background:var(--bg1);min-height:100vh;padding:14px;color:var(--txt)}"
        ".container{max-width:1180px;margin:0 auto}"
        ".header{border-bottom:2px solid var(--green);padding:4px 4px 12px;margin-bottom:20px;display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:10px}"
        ".page-title{font-size:18px;font-weight:700;letter-spacing:.5px;text-transform:uppercase;color:var(--txt);display:flex;align-items:center;gap:8px}"
        ".page-title .ico{font-size:18px}"
        ".header-right{display:flex;align-items:center;gap:28px}"
        ".header-item{display:flex;flex-direction:column;align-items:flex-end;gap:2px;min-width:140px}"
        ".header-item .label{font-size:11px;font-weight:600;color:var(--txt);text-transform:uppercase;letter-spacing:.6px}"
        ".header-item b{font-size:18px;font-weight:700;color:var(--txt);letter-spacing:.5px;line-height:1.2}"
        ".port-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:22px}"
        "@media(max-width:900px){.port-grid{grid-template-columns:repeat(2,1fr)}}"
        ".port-item{position:relative;background:var(--panel);border:1px solid var(--border);border-radius:12px;padding:20px;text-align:left;box-shadow:0 1px 3px rgba(20,22,25,.06);transition:border-color .2s,box-shadow .2s,background .3s,filter .3s}"
        ".port-item:hover{box-shadow:0 3px 10px rgba(20,22,25,.1)}"
        ".port-item:active{box-shadow:0 1px 2px rgba(20,22,25,.08)}"
        ".port-item.off-gray{background:#e8e8e8!important;border-color:#c0c0c0!important;box-shadow:none}"
        ".port-item.off-gray .port-label,.port-item.off-gray .port-status,.port-item.off-gray .port-current,.port-item.off-gray .port-current .current-value,.port-item.off-gray .port-controls label,.port-item.off-gray .port-controls span{color:#9e9e9e!important}"
        ".port-item.off-gray .port-apply{background:#d0d0d0!important;border-color:#b0b0b0!important;color:#757575!important}"
        ".port-item.off-gray .socket-icon{filter:grayscale(100%);opacity:.65}"
        ".port-item.on-normal{background:var(--panel);border-color:var(--border)}"
        ".port-item.pending{opacity:.65;pointer-events:none}"
        ".port-top{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:16px}"
        ".port-info{display:flex;flex-direction:column;gap:4px;flex:1 1 auto;min-width:0}"
        ".socket-icon{width:100px;height:100px;flex-shrink:0}"
        ".socket-icon svg{width:120%;height:120%}"
        ".socket-icon .outline{stroke-width:3}"
        ".socket-icon .pins{fill:none;stroke-width:3}"
        ".socket-icon.on .outline{stroke:#1E8E3E;stroke-width:4;fill:#D8F0DB}"
        ".socket-icon.on .pins{stroke:#1E8E3E;stroke-width:4}"
        ".socket-icon.on{border:3px solid #1E8E3E;border-radius:12px;padding:4px;box-sizing:border-box}"
        ".socket-icon.off .outline,.socket-icon.off .pins{stroke:#A0A0A0;opacity:1}"
        ".socket-icon.off .fill{fill:#E5E5E5;opacity:.30}"
        ".socket-icon.off{border:3px solid #E5E5E5;border-radius:12px;padding:4px;box-sizing:border-box}"
        ".port-label{font-size:16px;font-weight:700;letter-spacing:.5px;text-transform:uppercase;color:var(--txtdim)}"
        ".port-name{font-size:13px;font-weight:600;color:var(--txt);margin:2px 0;padding:4px 6px;border:1px solid transparent;border-radius:4px;background:transparent;width:100%;font-family:inherit;text-overflow:ellipsis}"
        ".port-name::placeholder{color:#b0b0b0;font-weight:400}"
        ".port-name:hover{border-color:var(--border);background:#fff}"
        ".port-name:focus{outline:none;border-color:var(--green);background:#fff;box-shadow:0 0 0 2px rgba(47,158,68,.15)}"
        ".port-item.off-gray .port-name{color:#9e9e9e!important}"
        ".port-item.off-gray .port-name:hover{background:#f0f0f0}"
        ".port-status{font-size:19px;font-weight:700;letter-spacing:.5px}"
        ".port-status.on{color:var(--green)}"
        ".port-status.off{color:var(--red)}"
        ".port-current{font-size:13px;color:var(--txtdim);margin-top:2px}"
        ".port-current .current-value{font-weight:600;color:var(--txt)}"
        ".port-controls{display:flex;align-items:center;gap:16px;margin-top:14px;padding:12px 0;border-top:1px solid var(--border)}"
        ".port-controls label{display:flex;align-items:center;gap:5px;font-size:13px;font-weight:600;cursor:pointer}"
        ".port-controls input[type=\"radio\"]{accent-color:var(--green);width:16px;height:16px;cursor:pointer}"
        ".port-controls input[type=\"radio\"]:checked+span{color:var(--green)}"
        ".port-controls input[type=\"radio\"][value=\"0\"]:checked+span{color:var(--red)}"
        ".port-controls input[type=\"radio\"][value=\"0\"]{accent-color:var(--red)}"
        ".port-apply{width:100%;padding:10px 0;margin-top:6px;border:2px solid #9E9E9E;border-radius:6px;font-size:13px;font-weight:700;letter-spacing:.5px;text-transform:uppercase;cursor:pointer;transition:all .2s;background:#C0C0C0;color:#000}"
        ".port-apply:hover{transform:translateY(-1px);box-shadow:0 2px 8px rgba(0,0,0,.15)}"
        ".port-apply:disabled{cursor:not-allowed;opacity:1}"
        ".port-apply.on-state{background:#A5D6A7;border-color:#81C784;color:#1b5e20}"
        ".port-apply.on-state:hover{background:#90C695;border-color:#81C784;color:#1b5e20}"
        ".port-apply.off-state{background:#E0E0E0;border-color:#BDBDBD;color:#424242}"
        ".port-apply.off-state:hover{background:#D5D5D5;border-color:#BDBDBD;color:#424242}"
        ".port-apply.applying{background:#E0E0E0;border-color:#BDBDBD;color:#424242;cursor:wait;animation:pulse 1s infinite}"
        "@keyframes pulse{0%,100%{opacity:1}50%{opacity:.7}}"
        ".port-apply.applied{background:#E0E0E0;border-color:#BDBDBD;color:#424242}"
        "</style></head>\n");

    /* ===== JavaScript ===== */
    WEB_PUTS(pReq, "<script>\n");
    WEB_PUTS(pReq, "let fetchFlag=true,fetchInterval;"
        "async function getPduData(){try{let r=await fetch('/get_pdu_status',{cache:'no-store'});"
        "if(!r.ok)throw new Error(r.status);let d=await r.json(),v=d.v||d.V||0,e=document.getElementById('voltageDisplay');"
        "if(e)e.textContent=v.toFixed(1)+'V';let p=d.p||d.P||[0,0,0,0,0,0,0,0],i=d.i||d.I||[d.I1||0,d.I2||0,d.I3||0,d.I4||0,d.I5||0,d.I6||0,d.I7||0,d.I8||0],t=0;"
        "for(let n=0;n<8;n++){let c=i[n]?i[n]/100:0;t+=c;applyPortState(n+1,p[n]||0,c)}let cl=document.getElementById('currentDisplay');"
        "if(cl)cl.textContent=t.toFixed(1)+'A'}catch(x){console.error('PDU:',x)}}"
        "async function loadPortNames(){try{let r=await fetch('/get_pdu_names',{cache:'no-store'});"
        "if(!r.ok)throw new Error(r.status);let d=await r.json(),n=d.n||d.N||d.names||[];"
        "for(let i=0;i<8;i++){let el=document.getElementById('port-name-'+(i+1));if(el&&n[i]!==undefined)el.value=String(n[i]).slice(0,32)}}catch(e){console.error('names:',e)}}"
        "let nameSaveTimers={};"
        "function setPortName(portNumber){let el=document.getElementById('port-name-'+portNumber);if(!el)return;"
        "let name=el.value.slice(0,32);el.value=name;if(nameSaveTimers[portNumber])clearTimeout(nameSaveTimers[portNumber]);"
        "nameSaveTimers[portNumber]=setTimeout(async()=>{try{let u=`/set_pdu_name?port=${portNumber}&name=${encodeURIComponent(name)}`,"
        "r=await fetch(u,{cache:'no-store'});if(!r.ok)throw new Error(r.status);el.style.borderColor='#2f9e44';"
        "setTimeout(()=>{el.style.borderColor=''},800)}catch(e){console.error('setName:',e);el.style.borderColor='#e03131';"
        "setTimeout(()=>{el.style.borderColor=''},2000)}},400)}"
        "function applyPortState(n,st,cur){let s=document.getElementById(`socket-${n}`),sS=document.getElementById(`port-status-${n}`),"
        "it=document.getElementById(`port-item-${n}`),rO=document.getElementById(`radio-on-${n}`),rF=document.getElementById(`radio-off-${n}`),"
        "b=document.getElementById(`apply-btn-${n}`),cd=document.getElementById(`current-display-${n}`);if(!s)return;"
        "if(cd)cd.textContent=(cur||0).toFixed(2)+'A';"
        "if(st===1){if(sS){sS.textContent='ON';sS.className='port-status on'}if(it)it.className='port-item on-normal';"
        "if(rO)rO.checked=true;if(rF)rF.checked=false;if(b){b.textContent='Apply';b.className='port-apply on-state'};"
        "s.className=cur>0.01?'socket-icon on':'socket-icon off';s.dataset.state='on'}"
        "else{s.className='socket-icon off';s.dataset.state='off';if(sS){sS.textContent='OFF';sS.className='port-status off'};"
        "if(it)it.className='port-item off-gray';if(rO)rO.checked=false;if(rF)rF.checked=true;"
        "if(b){b.textContent='Apply';b.className='port-apply off-state'}}}"
        "async function applyPort(n){let s=document.getElementById(`socket-${n}`),b=document.getElementById(`apply-btn-${n}`),"
        "it=document.getElementById(`port-item-${n}`),rO=document.getElementById(`radio-on-${n}`);"
        "if(!s||!b||b.disabled)return;let ns=(rO&&rO.checked)?1:0,cv=(s.dataset.state==='on')?1:0;"
        "if(ns===cv){b.textContent='Already Set';let sc=ns===1?'on-state':'off-state';"
        "setTimeout(()=>{b.textContent='Apply';b.className='port-apply '+sc},10000);return}"
        "let as=Date.now(),ok=true;fetchFlag=false;if(it)it.classList.add('pending');b.disabled=true;"
        "b.textContent=`Applying Outlet ${n}...`;b.className='port-apply applying';"
        "let c=new AbortController(),tid=setTimeout(()=>c.abort(),5000);"
        "try{let u=`/set_port${n}?state=${ns}`,t0=Date.now(),r=await fetch(u,{cache:'no-store',signal:c.signal});"
        "if(!r.ok)throw new Error(r.status);let el=Date.now()-t0;if(el<1000)await new Promise(x=>setTimeout(x,1000-el));"
        "await r.text();applyPortState(n,ns,0)}catch(e){clearTimeout(tid);ok=false;"
        "b.textContent=e.name==='AbortError'?'Timeout - Retry':'Failed - Retry';b.className='port-apply'}"
        "finally{clearTimeout(tid);if(ok)for(let i=0;i<5;i++){if((s.dataset.state==='on'?1:0)===ns)break;"
        "if(i<4)await new Promise(r=>setTimeout(r,350))}let te=Date.now()-as;if(te<3000)await new Promise(r=>setTimeout(r,2000-te));"
        "if(ok){b.textContent='Apply';let sc=ns===1?'on-state':'off-state';b.className='port-apply '+sc}"
        "if(it)it.classList.remove('pending');b.disabled=false;fetchFlag=true}}"
        "function startTimer(){fetchInterval=setInterval(()=>{if(fetchFlag)getPduData()},7000)}"
        "document.addEventListener('DOMContentLoaded',function(){setTimeout(function(){getPduData()},300);loadPortNames();startTimer()});\n");
    WEB_PUTS(pReq, "</script>\n");

    WEB_PUTS(pReq, "<body><div class=\"container\"><div class=\"header\">"
        "<div class=\"page-title\"><span class=\"ico\">&#9889;</span>PDU Rack 8 &mdash; Control Panel</div>"
        "<div class=\"header-right\">"
        "<span class=\"header-item\"><span class=\"label\">INPUT VOLTAGE</span><b id=\"voltageDisplay\">0.0V</b></span>"
        "<span class=\"header-item\"><span class=\"label\">TOTAL CURRENT</span><b id=\"currentDisplay\">0.0A</b></span>"
        "</div></div><div class=\"port-grid\">\n");
    for (port = 1; port <= 8; port++) {
        /* FIX: split into small calls (each < 256 bytes after expansion) and pass
         * the exact number of %d arguments (original had 14 %d but only 12 args). */
        WEB_PRINTF(pReq, "<div id=\"port-item-%d\" class=\"port-item off-gray\"><div class=\"port-top\"><div class=\"port-info\">", port);
        WEB_PRINTF(pReq, "<div class=\"port-label\">Outlet %d</div>", port);
        WEB_PRINTF(pReq, "<input id=\"port-name-%d\" class=\"port-name\" type=\"text\" maxlength=\"32\" placeholder=\"Name (32 chars)\" value=\"\" oninput=\"setPortName(%d)\" onblur=\"setPortName(%d)\">", port, port, port);
        WEB_PRINTF(pReq, "<div id=\"port-status-%d\" class=\"port-status off\">OFF</div>", port);
        WEB_PRINTF(pReq, "<div class=\"port-current\">Current: <span id=\"current-display-%d\" class=\"current-value\">0.00A</span></div>", port);
        WEB_PRINTF(pReq, "</div><div id=\"socket-%d\" class=\"socket-icon off\" data-state=\"off\">", port);
        WEB_PUTS(pReq, "<svg viewBox=\"0 0 120 120\">" "<rect class=\"outline fill\" x=\"5\" y=\"5\" width=\"90\" height=\"90\" rx=\"15\" ry=\"15\"/>" "<rect class=\"pins\" x=\"43\" y=\"10\" width=\"14\" height=\"30\" rx=\"3\"/>" "<rect class=\"pins\" x=\"20\" y=\"58\" width=\"16\" height=\"28\" rx=\"3\"/>" "<rect class=\"pins\" x=\"64\" y=\"58\" width=\"16\" height=\"28\" rx=\"3\"/>" "</svg></div></div><div class=\"port-controls\">");
        WEB_PRINTF(pReq, "<label><input type=\"radio\" name=\"port-%d-state\" id=\"radio-on-%d\" value=\"1\"> <span>ON</span></label>", port, port);
        WEB_PRINTF(pReq, "<label><input type=\"radio\" name=\"port-%d-state\" id=\"radio-off-%d\" value=\"0\" checked> <span>OFF</span></label>", port, port);
        WEB_PRINTF(pReq, "</div><button id=\"apply-btn-%d\" class=\"port-apply off-state\" onclick=\"applyPort(%d)\">Apply</button></div>\n", port, port);
    }
    WEB_PUTS(pReq, "</div></div></body></html>\n");
    WEB_END(pReq);
    return SUCCESS;
}
/* ==================================================================
 *  PDU port-name storage and helpers
 * ================================================================== */

///* Per-port names: 8 ports, each up to 32 chars + NUL terminator */
#define PDU_NUM_PORTS      8
#define PDU_NAME_MAX_LEN   32
 char far pdu_names[PDU_NUM_PORTS][PDU_NAME_MAX_LEN + 1] = {
    "", "", "", "", "", "", "", ""
};
 
char far name_enc[1] = {0};
char far name_dec[1] = {0};

///* URL-decode: handles %XX and '+'. Safe for the 32-char names we accept. */
//static  void pdu_url_decode(const char *src, char *dst, int dst_size)
//{
//    int di = 0;
//    if (!src || !dst || dst_size <= 0) return;

//    while (*src && di < dst_size - 1) {
//        if (*src == '%' && src[1] && src[2]) {
//            char hex[3];
//            hex[0] = src[1];
//            hex[1] = src[2];
//            hex[2] = '\0';
//            dst[di++] = (char)strtol(hex, NULL, 16);
//            src += 3;
//        } else if (*src == '+') {
//            dst[di++] = ' ';
//            src++;
//        } else {
//            dst[di++] = *src++;
//        }
//    }
//    dst[di] = '\0';
//}

///* Extract a URL-encoded string query parameter from pReq->pPath.
// * Returns 1 if found, 0 otherwise. Value is NOT URL-decoded here. */
//static int pdu_get_query_str(struct http_request_s *pReq,
//                             const char *name,
//                             char *out,
//                             int out_size)
//{
//    const char *p;
//    const char *qs_end;
//    size_t      nlen;

//    if (!pReq || !pReq->pPath || !name || !out || out_size <= 0)
//        return 0;

//    nlen = strlen(name);
//    if (nlen == 0) return 0;

//    p = (const char *)pReq->pPath;

//    /* advance to '?' */
//    while (*p && *p != '?' && *p != ' ' && *p != '\r' && *p != '\n')
//        p++;
//    if (*p != '?') return 0;
//    p++;

//    /* end of request line */
//    qs_end = p;
//    while (*qs_end && *qs_end != ' ' && *qs_end != '\r' && *qs_end != '\n')
//        qs_end++;

//    /* scan key=value&key=value */
//    while (p < qs_end) {
//        const char *kstart = p;
//        const char *kend;
//        size_t      klen;

//        while (p < qs_end && *p != '=' && *p != '&')
//            p++;
//        kend = p;
//        klen = (size_t)(kend - kstart);

//        if (p >= qs_end || *p != '=') {
//            while (p < qs_end && *p != '&') p++;
//            if (p < qs_end && *p == '&') p++;
//            continue;
//        }
//        p++;  /* skip '=' */

//        if (klen == nlen && memcmp(kstart, name, nlen) == 0) {
//            int i = 0;
//            while (p < qs_end && *p != '&' && i < out_size - 1)
//                out[i++] = *p++;
//            out[i] = '\0';
//            return 1;
//        }

//        while (p < qs_end && *p != '&') p++;
//        if (p < qs_end && *p == '&') p++;
//    }

//    return 0;
//}

///* ==================================================================
// *  GET /get_pdu_names
// *  Returns: {"n":["name1","name2",...,"name8"]}
// * ================================================================== */
//int32 get_pdu_names(struct http_request_s *pReq)
//{
//    int i;
//    WEB_BEGIN(pReq);
//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: application/json");
//    WEB_HEADER(pReq, "Cache-Control: no-cache");

//    WEB_PRINTF(pReq, "{\"n\":[");
//    for (i = 0; i < PDU_NUM_PORTS; i++) {
//        if (i) WEB_PRINTF(pReq, ",");
//        WEB_PRINTF(pReq, "\"%s\"", pdu_names[i]);
//    }
//    WEB_PRINTF(pReq, "]}");

//    WEB_END(pReq);
//    return SUCCESS;
//}

/* ==================================================================
 *  GET /get_pdu_names   (FIX: route + handler were missing; the page calls it on load)
 *  Returns: {"n":["","","","","","","",""]}
 *  Names are not persisted yet (set_pdu_name storage code was commented out).
 * ================================================================== */
int32 get_pdu_names(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_HEADER(pReq, "Cache-Control: no-cache");
    WEB_PUTS(pReq, "{\"n\":[\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"]}");
    WEB_END(pReq);
    return SUCCESS;
}

/* ==================================================================
 *  GET /set_pdu_name?port=N&name=xxx
 *  Stores up to 32 characters for outlet N (1..8).
 * ================================================================== */
int32 set_pdu_name(struct http_request_s *pReq)
{
    char port_buf[8]   = {0};

    int  port = 0;

//    if (pdu_get_query_str(pReq, "port", port_buf, sizeof(port_buf)))
//        port = atoi(port_buf);

//    if (pdu_get_query_str(pReq, "name", name_enc, sizeof(name_enc)))
//        pdu_url_decode(name_enc, name_dec, sizeof(name_dec));

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/plain");
    WEB_HEADER(pReq, "Cache-Control: no-cache");

    if (port >= 1 && port <= 8) {
        //strncpy(pdu_names[port - 1], name_dec, PDU_NAME_MAX_LEN);
        //pdu_names[port - 1][PDU_NAME_MAX_LEN] = '\0';
        /* Optionally persist to NVRAM/EEPROM here */
        WEB_PRINTF(pReq, "OK");
    } else {
        WEB_PRINTF(pReq, "ERR");
    }

    WEB_END(pReq);
    return SUCCESS;
}


uint8 far Get_Response_RTC[234];
extern uint8 start_pdu_status;
uint8 start_rtc_data = 0;
uint8 start_alarm_data = 0;

//uint8 far Get_Response_ALARM[64];



void Copy_RTC_DATA(uint8* tempData)
{
		strcpy((char*)Get_Response_RTC, tempData);
}

void Copy_Alarm_DATA(uint8* tempData)
{
//		strcpy((char*)Get_Response_ALARM, tempData);
}



/* ============================================================
 * GET /get_rtc_time
 * Returns complete RTC data: current time + date + all alarms.
 * NOTE: Static test values are used below — replace with real
 *       RTC / alarm reads when ready.
 * JSON keys are all uppercase.
 * ============================================================ */
int32 get_rtc_time(struct http_request_s *pReq)
{
    start_pdu_status = 0;
		start_rtc_data  = 1;
		
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_HEADER(pReq, "Cache-Control: no-cache");
		WEB_PRINTF(pReq, "%s" ,Get_Response_RTC);
    WEB_END(pReq);
		memset(Get_Response_RTC,'\0',234);
    return SUCCESS;
}



/* ============================================================
 * GET /get_alarms
 * Returns all 3 alarm times only (JSON, uppercase keys).
 * Use null for alarms that are not configured.
 *   {"A1H":8,"A1M":18,"A2H":2,"A2M":12,"A3H":4,"A3M":19}
 * ============================================================ */
int32 get_alarms(struct http_request_s *pReq)
{
		start_alarm_data = 1;
		//printf("GET_ALARMS\r\n");
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_HEADER(pReq, "Cache-Control: no-cache");
		//strcpy(Get_Response_ALARM, "{\"A1H\":2,\"A1M\":45,\"A2H\":4,\"A2M\":24,\"A3H\":6,\"A3M\":36}");
//    WEB_PRINTF(pReq,"%s", Get_Response_ALARM);

    WEB_END(pReq);
    return SUCCESS;
}

/* ================================================================== *
 *  parse_query_uint
 *
 *  Extract an integer query parameter from the raw request buffer.
 *
 *  The firmware stores the full HTTP request in pReq->pPath, e.g.:
 *
 *    "GET /set_rtc_time?H=12&M=12&S=12&D=12&MO=12&Y=2012 HTTP/1.1\r\n"
 *    "Host: 192.168.1.1\r\n"
 *    "Referer: http://192.168.1.1/vlan.cgi?page=set_rtc_data_alarm\r\n"
 *    ...
 *
 *  So we must:
 *    - start scanning right after the '?' in the request line
 *    - stop at the first ' ', '\r' or '\n' (end of request line)
 *    - never scan the headers (a '?' in Referer must not confuse us)
 *
 *  Exact-length key match, so "M" never matches "MO".
 *  Returns 0 on success, -1 if not found or malformed.
 * ================================================================== */
static int parse_query_uint(struct http_request_s *pReq,
                            const char *name,
                            int        *out)
{
    const char *p;
    const char *qs_end;
    size_t      nlen;

    if (!pReq || !pReq->pPath || !name || !out) return -1;

    nlen = strlen(name);
    if (nlen == 0) return -1;

    p = (const char *)pReq->pPath;

    /* advance to the first '?' — start of the query string */
    while (*p && *p != '?' && *p != ' ' && *p != '\r' && *p != '\n')
        p++;
    if (*p != '?') return -1;
    p++;                                   /* skip '?' */

    /* end of the request line — anything past this is headers */
    qs_end = p;
    while (*qs_end && *qs_end != ' ' && *qs_end != '\r' && *qs_end != '\n')
        qs_end++;

    /* scan "key=value&key=value..." inside [p, qs_end) */
    while (p < qs_end) {
        const char *kstart = p;
        const char *kend;
        size_t      klen;
        long        val    = 0;
        int         sign   = 1;
        int         digits = 0;

        /* key runs until '=' or '&' or end of query string */
        while (p < qs_end && *p != '=' && *p != '&')
            p++;
        kend = p;
        klen = (size_t)(kend - kstart);

        if (p >= qs_end || *p != '=') {
            /* key with no '=': skip to next '&' */
            while (p < qs_end && *p != '&') p++;
            if (p < qs_end && *p == '&') p++;
            continue;
        }
        p++;                               /* skip '=' */

        if (klen == nlen && memcmp(kstart, name, nlen) == 0) {
            /* matched — parse the value */
            if (p < qs_end && *p == '-')      { sign = -1; p++; }
            else if (p < qs_end && *p == '+') { p++; }

            while (p < qs_end && *p >= '0' && *p <= '9') {
                val = val * 10 + (*p - '0');
                p++;
                digits++;
            }

            /* value must end at '&' or end of query string */
            if (p != qs_end && *p != '&')
                return -1;
            if (digits == 0)
                return -1;

            *out = (int)(sign * val);
            return 0;
        }

        /* wrong key: skip to next '&' */
        while (p < qs_end && *p != '&') p++;
        if (p < qs_end && *p == '&') p++;
    }

    return -1;
}


/* ================================================================== *
 *  set_rtc_time_handler
 * ================================================================== */
int32 set_rtc_time_handler(struct http_request_s *pReq)
{
    int H = -1, M = -1, S = -1, D = -1, MO = -1, Y = -1;
    int ok = 0;

    if (parse_query_uint(pReq, "H",  &H)  != 0) H  = -1;
    if (parse_query_uint(pReq, "M",  &M)  != 0) M  = -1;
    if (parse_query_uint(pReq, "S",  &S)  != 0) S  = -1;
    if (parse_query_uint(pReq, "D",  &D)  != 0) D  = -1;
    if (parse_query_uint(pReq, "MO", &MO) != 0) MO = -1;
    if (parse_query_uint(pReq, "Y",  &Y)  != 0) Y  = -1;

    printf("RTC set: %02d:%02d:%02d  %02d-%02d-%04d\r\n", H, M, S, D, MO, Y);

    if (H  >= 0   && H  <= 23 &&
        M  >= 0   && M  <= 59 &&
        S  >= 0   && S  <= 59 &&
        D  >= 1   && D  <= 31 &&
        MO >= 1   && MO <= 12 &&
        Y  >= 2000 && Y <= 2099)
    {
        /* TODO: rtc_set_time(H, M, S); rtc_set_date(D, MO, Y); */
        ok = 1;
    }

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_HEADER(pReq, "Cache-Control: no-cache");

    if (ok)
        WEB_PRINTF(pReq, "{\"STATUS\":\"OK\"}");
    else
        WEB_PRINTF(pReq, "{\"STATUS\":\"ERR\"}");

    WEB_END(pReq);
    return SUCCESS;
}



int32 set_alarm_handler(struct http_request_s *pReq, int alarm_idx)
{
    int H = -1, M = -1, OUT = -1, ACT = -1;
    int ok = 0;

    /* Extract variables safely using our direct query string parser */
    if (parse_query_uint(pReq, "h",  &H)   != 0) H   = -1;
    if (parse_query_uint(pReq, "m",  &M)   != 0) M   = -1;
    if (parse_query_uint(pReq, "out", &OUT) != 0) OUT = -1;
    if (parse_query_uint(pReq, "act", &ACT) != 0) ACT = -1;

    /* Validate ranges */
    if (alarm_idx >= 1 && alarm_idx <= 3 &&
        H   >= 0 && H   <= 23 &&
        M   >= 0 && M   <= 59 &&
        OUT >= 1 && OUT <= 8  &&
        ACT >= 0 && ACT <= 1)
    {
        /* TODO: rtc_set_alarm(alarm_idx, H, M, OUT, ACT); */
        
//        /* Debug log to verify reception */
//        printf("ALARM %d SET: %02d:%02d -> Outlet %d %s\r\n", 
//               alarm_idx, H, M, OUT, ACT ? "ON" : "OFF");
			printf("A:%d H:%02d M:%02d O:%d S:%d\r\n",alarm_idx,H,M,OUT,ACT);
        ok = 1;
    }
    else
    {
        printf("ALARM %d FAILED: H=%d M=%d OUT=%d ACT=%d\r\n", 
               alarm_idx, H, M, OUT, ACT);
    }

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_HEADER(pReq, "Cache-Control: no-cache");

    if (ok)
        WEB_PRINTF(pReq,
            "{\"STATUS\":\"OK\",\"ALARM\":%d,\"H\":%d,\"M\":%d,\"OUT\":%d,\"ACT\":%d}",
            alarm_idx, H, M, OUT, ACT);
    else
        WEB_PRINTF(pReq,
            "{\"STATUS\":\"ERR\",\"ALARM\":%d}", alarm_idx);

    WEB_END(pReq);
    return SUCCESS;
}