/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the functions related to port function
 *
 * Feature : Response the related request of port function
 *
 */

#ifndef __WEB_PANEL_H__
#define __WEB_PANEL_H__

/*
 * Include Files
 */
#include <web/http_types.h>

#define WEB_DOUBLE_LINE_PORT_NUM    12 // when the port number displayed is beyond this value, two lines will be drawed.
/*
 * Function Declaration
 */
extern int32 get_panel(struct http_request_s *pReq);


#endif  /* __WEB_PANEL_H__ */


