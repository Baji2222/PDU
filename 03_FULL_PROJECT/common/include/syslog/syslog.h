/**
 * @file
 * SYSLOG Agent message handling structures.
 */

#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#include "local/config.h"
#include "lwps/lwps_api.h"
#include "syslog/syslogdefs.h"

//extern uint16 head_index;
//extern uint16 tail_index;


extern lwps_handler_t xdata syslogHandler;


#define INCREMENT_SYSLOG_MSG_INDEX(idx)                 \
	idx++;												\
	if(idx == SAL_SYS_LOG_BUF_MAX_ENTRY)				\
	{													\
		idx = 0;										\
	}


static uint8 get_syslog_sev_frm_content_id(syslog_msg_content_id_t contId)
{
	switch(contId)
	{
		case SYSLOG_MSG_SYSTEM_STARTED:
		case SYSLOG_MSG_SYSTEM_REBOOTED:
				return SYSLOG_SEV_CRIT;

		case SYSLOG_MSG_MANAGEMENT_IP_CHANGED: 
		case SYSLOG_MSG_PASSWORD_CHANGED:
		case SYSLOG_MSG_SUCCESSFUL_LOGIN: 
		case SYSLOG_MSG_LOGOUT:
		case SYSLOG_MSG_WEB_SESSION_TIMEOUT: 
		case SYSLOG_MSG_PORT_LINK_UP_100M_HALF: 
		case SYSLOG_MSG_PORT_LINK_UP_100M_FULL:
		case SYSLOG_MSG_PORT_LINK_UP_1G_HALF:
		case SYSLOG_MSG_PORT_LINK_UP_1G_FULL:
		case SYSLOG_MSG_PORT_LINK_DOWN:
		case SYSLOG_MSG_PORT_STATE_CHANGE:
		case SYSLOG_MSG_FACTORY_RESET:	
			return SYSLOG_SEV_INFO;

		case SYSLOG_MSG_LOGIN_FAILED:
			return SYSLOG_SEV_WARNING;
		default:
			return SYSLOG_SEV_INFO;
	}

}


struct syslog_msg
{
    int32 socketId;
	uint8 protocol;
	ip_addr_t ip;
	const char* server;
	uint16 port;
	const char* deviceHostname;
	const char* appName;
	uint16 priDefault;
	uint8 priMask;
};


static const char syslogmsgcontent[16][36] =
{
	"System started up..",
	"System rebooted..",	
    "Management IP was changed",
    "Password was changed",
    "Successful login through Web", //IP part of addInfo
    "Logout through Web", //IP part of addInfo
    "Web session timed out", //IP part of addInfo
    "Login failed through Web", //IP part of addInfo
    " Link up, 100Mbps HALF DUPLEX", //Append "Port x" at log handler, part of addInfo
    " Link up, 100Mbps FULL DUPLEX", //Append "Port x" at log handler, part of addInfo
    " Link up, 1Gbps HALF DUPLEX", //Append "Port x" at log handler, part of addInfo
    " Link up, 1Gbps FULL DUPLEX", //Append "Port x" at log handler, part of addInfo
    " Link Down", //Append "Port x" at log handler, part of addInfo
    " Link Up", //Append "Port x" at log handler, part of addInfo
    "Port State Changed", //Append "Port x" at log handler, part of addInfo    
	"Reset to default factory settings"
};

//Months
static const char months[12][4] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};


#endif /* __LWIP_SYSLOG_MSG_H__ */
