/**
 * @file
 * SYSLOG Agent message handling structures.
 */

#ifndef __SYSLOGDEFS_H__
#define __SYSLOGDEFS_H__

#include "local/config.h"
#include "lwps/lwps_api.h"
#include "salflash.h"

extern void syslog_head_idx_set(uint16 headIdx);
extern void syslog_head_idx_get(uint16* pHeadIdx);
extern void syslog_tail_idx_set(uint16 tailIdx);
extern void syslog_tail_idx_get(uint16* pTailIdx);
extern int send_syslog(sal_flash_syslog_buf_entry_t* bufEntry);

extern void syslog_init(void);
extern void calcDate(struct tm *tm, uint32 timestamp);


/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */

#if 0
#define SYSLOG_FAC_EMERG 0 /* system is unusable */
#define SYSLOG_FAC_ALERT 1 /* action must be taken immediately */
#define SYSLOG_FAC_ERR   3 /* error conditions */
#define SYSLOG_FAC_CRIT  2 /* critical conditions */
#define SYSLOG_FAC_WARNING 4 /* warning conditions */
#define SYSLOG_FAC_NOTICE  5 /* normal but significant condition */
#define SYSLOG_FAC_INFO  6 /* informational */
#define SYSLOG_FAC_DEBUG 7 /* debug-level messages */
#endif
#define SYSLOG_PRIMASK 0x07  /* mask to extract priority part (internal) */
        /* extract priority */
#define SYSLOG_PRI(p)  ((p) & SYSLOG_PRIMASK)
#define SYSLOG_MAKEPRI(fac, pri) (((fac) << 3) | (pri))

/* facility codes */
#define SYSLOG_FAC_KERN  (0<<3)  /* kernel messages */
#define SYSLOG_FAC_USER  (1<<3)  /* random user-level messages */
#define SYSLOG_FAC_MAIL  (2<<3)  /* mail system */
#define SYSLOG_FAC_DAEMON  (3<<3)  /* system daemons */
#define SYSLOG_FAC_AUTH  (4<<3)  /* security/authorization messages */
#define SYSLOG_FAC_SYSLOG  (5<<3)  /* messages generated internally by syslogd */
#define SYSLOG_FAC_LPR   (6<<3)  /* line printer subsystem */
#define SYSLOG_FAC_NEWS  (7<<3)  /* network news subsystem */
#define SYSLOG_FAC_UUCP  (8<<3)  /* UUCP subsystem */
#define SYSLOG_FAC_CRON  (9<<3)  /* clock daemon */
#define SYSLOG_FAC_AUTHPRIV  (10<<3) /* security/authorization messages (private) */
#define SYSLOG_FAC_FTP   (11<<3) /* ftp daemon */

/* other codes through 15 reserved for system use */
#define SYSLOG_FAC_LOCAL0  (16<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL1  (17<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL2  (18<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL3  (19<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL4  (20<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL5  (21<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL6  (22<<3) /* reserved for local use */
#define SYSLOG_FAC_LOCAL7  (23<<3) /* reserved for local use */

#define SYSLOG_NFACILITIES 24  /* current number of facilities */
#define SYSLOG_FACMASK 0x03f8  /* mask to extract facility part */
                            /* facility of pri */
#define SYSLOG_FAC(p)  (((p) & LOG_FACMASK) >> 3)

#define SYSLOG_MASK(pri)  (1 << (pri))	/* mask for one priority */
#define SYSLOG_UPTO(pri)  ((1 << ((pri)+1)) - 1)	/* all priorities through pri */

typedef enum syslog_severity_e
{
	SYSLOG_SEV_EMERG = 0, //0 /* system is unusable */
	SYSLOG_SEV_ALERT, //1 /* action must be taken immediately */
	SYSLOG_SEV_ERR,   //3 /* error conditions */ 
	SYSLOG_SEV_CRIT,  //2 /* critical conditions ::Check CHARAN */
	SYSLOG_SEV_WARNING, //4 /* warning conditions */
	SYSLOG_SEV_NOTICE,  //5 /* normal but significant condition */
	SYSLOG_SEV_INFO,  //6 /* informational */
	SYSLOG_SEV_DEBUG, //7 /* debug-level messages */
}syslog_severity_t;

//Add all log content types here
//Warning: DO NOT CHANGE ORDER. syslog_msg_content_id_t & syslogmsgcontent have one to one mapping
typedef enum syslog_msg_content_id_e
{
	SYSLOG_MSG_SYSTEM_STARTED = 0,
	SYSLOG_MSG_SYSTEM_REBOOTED,
	SYSLOG_MSG_MANAGEMENT_IP_CHANGED , 
	SYSLOG_MSG_PASSWORD_CHANGED , 
	SYSLOG_MSG_SUCCESSFUL_LOGIN , 
	SYSLOG_MSG_LOGOUT , 
	SYSLOG_MSG_WEB_SESSION_TIMEOUT , 
	SYSLOG_MSG_LOGIN_FAILED , 
	SYSLOG_MSG_PORT_LINK_UP_100M_HALF, 
	SYSLOG_MSG_PORT_LINK_UP_100M_FULL,
	SYSLOG_MSG_PORT_LINK_UP_1G_HALF,
	SYSLOG_MSG_PORT_LINK_UP_1G_FULL,
	SYSLOG_MSG_PORT_LINK_DOWN , 	
	SYSLOG_MSG_PORT_LINK_UP , 
	SYSLOG_MSG_PORT_STATE_CHANGE , 
	SYSLOG_MSG_FACTORY_RESET , 
}syslog_msg_content_id_t;

extern void syslog_add_new_buf_entry(syslog_msg_content_id_t contentId, uint32 addInfo);

#endif /* __SYSLOGDEFS_H__ */
