#ifndef __LIB_TIME_H__
#define __LIB_TIME_H__
#include <local/config.h>
#include <lib/lib_fixedpoint.h>
#include <common_types.h>

#define TIME_TIMERTICK2FRACTION_COUNT    (FIXEDP_FRAC_MAX/CONFIG_TIMER_DIVISOR)
#define JAN_1970                         (2208988800UL)        /* 1970 - 1900 in seconds */
#define SECS_PER_HOUR                    (3600UL)
#define SECS_PER_DAY                     (86400UL)


#define	__isleap(year)	\
     ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))


typedef fixed_point_t fixedp_time_t;

struct tm {
    u8_t  tm_sec;         /* seconds */
    u8_t  tm_min;         /* minutes */
    u8_t  tm_hour;        /* hours */
    u8_t  tm_mday;        /* day of the month */
    u8_t  tm_mon;         /* month */
    u16_t tm_year;        /* year */
    u16_t tm_wday;        /* day of the week */
    u16_t tm_yday;        /* day in the year */
    u16_t tm_isdst;       /* daylight saving time */
};


extern u32_t data system_clock;
extern u8_t  data timeCount;
extern s8_t  utc_plus;

extern s8_t get_time(fixedp_time_t *t);
extern s8_t set_time(fixedp_time_t *t);
extern struct tm *localtime(u32_t secs);

#endif /* end of __LIB_TIME_H__ */

