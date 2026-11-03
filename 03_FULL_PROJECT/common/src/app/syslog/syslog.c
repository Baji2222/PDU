#pragma STRING (far)
#include <string.h>
#include <stdio.h>

/**
 * @file
 * syslog 
 */

#include <lwps/lwps_api.h>
#include <salflash.h>
#include <salsys.h>
#include <salsyslog.h>
#include <syslog/syslogdefs.h>
#include <syslog/syslog.h>
#include <lib/lib_time.h>

/* UDP Protocol Control Block */
lwps_handler_t  syslogHandler;

static uint16 head_index;
static uint16 tail_index;
static sal_ipaddr_t sysloghostip;
static uint16 sysloghostudpport;
static struct syslog_msg syslogmsg;

extern volatile uint32 data current_time;

/*
 * Function: syslog_head_idx_set
 * -----------------------------
 * Sets the syslog buffer head index.
 * Updates head_index with the provided value.
 */
void syslog_head_idx_set(uint16 headIdx)
{
	head_index = headIdx;	
}

/*
 * Function: syslog_head_idx_get
 * -----------------------------
 * Retrieves the syslog buffer head index.
 * Copies head_index to the provided pointer.
 */
void syslog_head_idx_get(uint16* pHeadIdx)
{
	*pHeadIdx = head_index;
}

/*
 * Function: syslog_tail_idx_set
 * -----------------------------
 * Sets the syslog buffer tail index.
 * Updates tail_index with the provided value.
 */
void syslog_tail_idx_set(uint16 tailIdx)
{
	tail_index = tailIdx;
}

/*
 * Function: syslog_tail_idx_get
 * -----------------------------
 * Retrieves the syslog buffer tail index.
 * Copies tail_index to the provided pointer.
 */
void syslog_tail_idx_get(uint16* pTailIdx)
{
	*pTailIdx = tail_index;
}

/*
 * Function: syslog_add_new_buf_entry
 * ----------------------------------
 * Adds a new entry to the syslog buffer in flash.
 * Sets timestamp, severity, and content ID from message type.
 * Wraps around if buffer is full and saves to flash.
 * Sends syslog message via UDP if syslog is enabled.
 */
void  syslog_add_new_buf_entry(syslog_msg_content_id_t contentId, uint32 addInfo)
{
	sal_flash_syslog_buf_entry_t	sysLogBufEntry;
	uint16 headIdx, tailIdx;
	printf("\nLogging new entry, tail index:%d, head index: %d\n", tail_index, head_index);

	memset(&sysLogBufEntry, 0x00, sizeof(sal_flash_syslog_buf_entry_t));
	sysLogBufEntry.sysLogIdBuf = (uint8)contentId;
	sysLogBufEntry.timestamp = current_time;
	sysLogBufEntry.sysLogSevBuf = get_syslog_sev_frm_content_id(contentId);
	sysLogBufEntry.addInfo = addInfo;
//	memcpy(&sysLogBufEntry.addInfo, addInfo, sizeof(uint32));
//	memcpy(&sysLogBufEntry.sysLogMsgBuf, "Unauthorized access requested\0", 30);

	sal_syslog_buf_head_idx_get(&headIdx);
	sal_syslog_buf_tail_idx_get(&tailIdx);

	sal_flash_syslog_buf_set(&sysLogBufEntry, (uint16)tailIdx);
	INCREMENT_SYSLOG_MSG_INDEX(tailIdx);
	if(tailIdx == headIdx)
	{
		INCREMENT_SYSLOG_MSG_INDEX(headIdx);
//		sal_sys_syslog_headIdx_set(&headIdx, 1);
		sal_syslog_buf_head_idx_set(headIdx);	
	}
//	sal_sys_syslog_tailIdx_set(&tailIdx, 1);
	sal_syslog_buf_tail_idx_set(tailIdx);
	if(syslog_enable){
		send_syslog(&sysLogBufEntry);
	printf("\nsysLogIdBuf:%d, severity: %d\n", (uint16)sysLogBufEntry.sysLogIdBuf, (uint16)sysLogBufEntry.sysLogSevBuf);
	
	printf("\nAdded new entry, tail index:%d, head index: %d\n", tailIdx, headIdx);
	head_index = headIdx;
	tail_index = tailIdx;
	
	sal_flash_cfg_save();
	}
}


/**
 * Starts syslog Agent.
 * Allocates UDP pcb and binds it to IP_ADDR_ANY port 514.
 */
/*
 * Function: syslog_init
 * ---------------------
 * Initializes syslog client and creates UDP socket.
 * Binds socket to syslog server port.
 * Registers syslog handler for sending messages.
 */
void
syslog_init(void)
{
    int32 socketId;
	uint16 dstUdpPort;
	sal_ipaddr_t dstIpAddr;
    ip_addr_t dst_ipaddr;
//	uint16 i;
	
	sal_syslog_serverPort_get(&dstUdpPort);
	sal_syslog_serverIp_get(&dstIpAddr);

    dst_ipaddr.addr[0] = (dstIpAddr & 0xFF000000) >> 24;
    dst_ipaddr.addr[1] = (dstIpAddr & 0x00FF0000) >> 16;
    dst_ipaddr.addr[2] = (dstIpAddr & 0x0000FF00) >> 8;
    dst_ipaddr.addr[3] = (dstIpAddr & 0x000000FF) >> 0;
#if 0		
	for(i=0; i<4; i++)
		printf("\n dst_ip.addr[%d]: %d  \n", i,(uint16)dst_ipaddr.addr[i]);

	printf("\n Dst Port: %d\n", dstUdpPort);
#endif
    syslogHandler.event_connect    = NULL;
    syslogHandler.event_disconnect = NULL;
    syslogHandler.event_recv       = NULL;
    syslogHandler.event_recvfrom   = NULL;

    socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, syslogHandler);

    if (socketId < 0)
    {
        OSAL_PRINTF("syslog: Error - alloc socket failed!\n");
         lwps_close(socketId);  // ADDE by naveen
        return ;
    }
    
    if (LWPS_OK != lwps_bind(socketId, ip_addr_any, dstUdpPort))
    {
        OSAL_PRINTF("syslog: Error - bind failed!\n");
        lwps_close(socketId);
        return ;
    }
	syslogmsg.socketId = socketId;

//	syslog_add_new_buf_entry(SYSLOG_MSG_SYSTEM_STARTED, 0);
	lwps_close(socketId); // ADDED BY NAVEEN
}

/*
 * Function: calcDate
 * ------------------
 * Converts Unix timestamp to broken-down time structure.
 * Calculates seconds, minutes, hours, days, month, year, and day of week.
 * Handles leap years correctly for date calculations.
 */
void calcDate(struct tm *tm, uint32 timestamp)
{
  uint32 seconds, minutes, hours, days, year, month;
  uint32 dayOfWeek;
  static const uint8 daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  seconds = timestamp - JAN_1970;
//	seconds = 1549200462UL;
  /* calculate minutes */
  minutes  = seconds / 60;
  seconds -= minutes * 60;
  /* calculate hours */
  hours    = minutes / 60;
  minutes -= hours   * 60;
  /* calculate days */
  days     = hours   / 24;
  hours   -= days    * 24;

  /* Unix time starts in 1970 on a Thursday */
  year      = 1970;
  dayOfWeek = 4;

  while(1)
  {
    bool     leapYear   = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    uint16 daysInYear = leapYear ? 366 : 365;
    if (days >= daysInYear)
    {
      dayOfWeek += leapYear ? 2 : 1;
      days      -= daysInYear;
      if (dayOfWeek >= 7)
        dayOfWeek -= 7;
      ++year;
    }
    else
    {
      tm->tm_yday = days;
      dayOfWeek  += days;
      dayOfWeek  %= 7;

      /* calculate the month and day */
      for(month = 0; month < 12; ++month)
      {
        uint8 dim = daysInMonth[month];

        /* add a day to feburary if this is a leap year */
        if (month == 1 && leapYear)
          ++dim;

        if (days >= dim)
          days -= dim;
        else
          break;
      }
      break;
    }
  }

  tm->tm_sec  = seconds;
  tm->tm_min  = minutes;
  tm->tm_hour = hours;
  tm->tm_mday = days + 1;
  tm->tm_mon  = month;
  tm->tm_year = year;
  tm->tm_wday = dayOfWeek;

}

// priority = severity_level + 8*facility_level
//"<%d>%d %s %s     [%s] : ", priority, version,timestamp, hostname, applicationName);
/*
 * Function: send_syslog
 * ---------------------
 * Sends a syslog message to the configured syslog server via UDP.
 * Formats message with priority, timestamp, and message content.
 * Creates temporary UDP socket and sends to server IP/port.
 * Returns SUCCESS if sent, FAILED if error.
 */
int send_syslog(sal_flash_syslog_buf_entry_t* bufEntry)
{
//	uint16 i;
	uint8 buf[384];
//	uint16 pri;
//	char priBuf[3]={""};
    ip_addr_t dst_ipaddr;
//	sal_mac_t mac;
	int32 socketId; 
	uint16 dstUdpPort;
	sal_ipaddr_t dstIpAddr;
	char timeStr[30]={""};
	struct tm ts;
	uint16 bufSize=0;
	uint16 strSize;

	
	sal_syslog_serverPort_get(&dstUdpPort);
	sal_syslog_serverIp_get(&dstIpAddr);

    dst_ipaddr.addr[0] = (dstIpAddr & 0xFF000000) >> 24;
    dst_ipaddr.addr[1] = (dstIpAddr & 0x00FF0000) >> 16;
    dst_ipaddr.addr[2] = (dstIpAddr & 0x0000FF00) >> 8;
    dst_ipaddr.addr[3] = (dstIpAddr & 0x000000FF) >> 0;

	
	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	
	calcDate(&ts, bufEntry->timestamp);
	sprintf(timeStr, "%s %d %02d:%02d:%02d    \0", months[ts.tm_mon], (uint16)ts.tm_mday, (uint16)ts.tm_hour, (uint16)ts.tm_min, (uint16)ts.tm_sec);

	//	pri = SYSLOG_SEV_INFO + SYSLOG_FAC_LOCAL0;

	buf[bufSize] = '<';
	bufSize++;	
	buf[bufSize] = '1';
	bufSize++;
	buf[bufSize] = '3';
	bufSize++;

	if (bufEntry->sysLogSevBuf == SYSLOG_SEV_WARNING)
		buf[bufSize] = '2';
	else if (bufEntry->sysLogSevBuf == SYSLOG_SEV_CRIT)
		buf[bufSize] = '0';
	else
		buf[bufSize] = '4';
		
	bufSize++;
	buf[bufSize]= '>';
	bufSize++;

	memcpy(&buf[bufSize], timeStr, strlen(timeStr));
	bufSize += strlen(timeStr);

	if(bufEntry->sysLogIdBuf == SYSLOG_MSG_PORT_LINK_DOWN || bufEntry->sysLogIdBuf == SYSLOG_MSG_PORT_LINK_UP)
	{
		sprintf(&buf[bufSize], "%s %d", "Port", (uint16)bufEntry->addInfo);
		bufSize+=6;
	}
	
	strSize = strlen(syslogmsgcontent[bufEntry->sysLogIdBuf]);
	memcpy(&buf[bufSize], syslogmsgcontent[bufEntry->sysLogIdBuf], strSize);
	bufSize +=strSize;
	
	socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, syslogHandler);
	if (socketId < 0)
	{
	  OSAL_PRINTF("[%s][%d]SYSLOG Connect Error! alloc socket failed!\n", __FILE__, (uint16)__LINE__);
	  lwps_close(socketId);
	  return FAILED;
	}
	//lwps_close(socketId);   // added by naveen

	
	//printf("\n TIME STRING: %s", timeStr);
#if 0	
	for(i=0; i<4; i++)
		printf("\n dst_ip.addr[%d]: %d  \n", i,(uint16)dst_ipaddr.addr[i]);

	printf("\n Dst Port: %d\n", dstUdpPort);	
#endif
	if (LWPS_OK != lwps_sendto(socketId, (uint8 *)buf, bufSize, dst_ipaddr, dstUdpPort))
    {
        OSAL_PRINTF("%s():%d lwps_sendto failed!\n", __FILE__, (uint16)__LINE__);
			  lwps_close(socketId);
        return FAILED;
    }
    lwps_close(socketId); //added by naveen
    return SUCCESS;
}