/*
 *  Http Daemon for LwPS
 */

#ifndef __LWPS_HTTPD_H__
#define __LWPS_HTTPD_H__

/*
 * Include Files
 */
#include <common_types.h>
#include <local/web_config.h> // local/web_config.h should be defined in user directory

/*
 * Symbol Definition: user can define his own symbolic constant
 */
#ifndef HTTPD_LISTEN_PORT
#define HTTPD_LISTEN_PORT       (80)
#endif

#ifndef HTTPD_CLIENT_MAX
#define HTTPD_CLIENT_MAX        (10)
#endif

#ifndef HTTPD_REQ_BUF_SIZE
#define HTTPD_REQ_BUF_SIZE     (1560)  // Internal request buffer size used to assemble fragment packets 
#endif
/*
 * Function Declaration
 */
extern int32 httpd_init(void) large;

#endif  /* __LWPS_HTTPD_H__ */

