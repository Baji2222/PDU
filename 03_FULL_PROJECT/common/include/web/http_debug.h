/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file provides debug functions for web smart
*$Id: http_debug.h,v 1.0 2009/08/03 17:03:40 zhenyang_jin Exp $
*/

#ifndef __HTTP_DEBUG_H__
#define __HTTP_DEBUG_H__

#include <common_types.h>

#define DBG_LWPS_HTTP_ENABLE    (0)

#if DBG_LWPS_HTTP_ENABLE
  #define DBG_LWPS_HTTP_PRINTF(x)  do {  printf x ; } while(0)
#else /* else of DBG_LWPS_HTTP_ENABLE */
  #define DBG_LWPS_HTTP_PRINTF(x)  do { } while(0)   
#endif /* end of else of DBG_LWPS_HTTP_ENABLE */

#define DBG_LWPS_HTTP_STACK_ENABLE   (0)
#if (DBG_LWPS_HTTP_ENABLE) && (DBG_LWPS_HTTP_STACK_ENABLE)
#define DBG_LWPS_HTTP_STACK(x) do {  printf x ; } while(0)
#else
#define DBG_LWPS_HTTP_STACK(x) do { } while(0)
#endif

#define DBG_LWPS_HTTP_CONNECT_ENABLE   (0)
#if (DBG_LWPS_HTTP_ENABLE) && (DBG_LWPS_HTTP_CONNECT_ENABLE)
#define DBG_LWPS_HTTP_CONNECT(x) do {  printf x ; } while(0)
#else
#define DBG_LWPS_HTTP_CONNECT(x) do { } while(0)
#endif

#define DBG_LWPS_HTTP_SOCKET_ENABLE   (0)
#if (DBG_LWPS_HTTP_ENABLE) && (DBG_LWPS_HTTP_SOCKET_ENABLE)
#define DBG_LWPS_HTTP_SOCKET(x) do {  printf x ; } while(0)
#else
#define DBG_LWPS_HTTP_SOCKET(x) do { } while(0)
#endif

#define DBG_LWPS_HTTP_AUTH_ENABLE   (1)
#if (DBG_LWPS_HTTP_ENABLE) && (DBG_LWPS_HTTP_AUTH_ENABLE)
#define DBG_LWPS_HTTP_AUTH(x) do {  printf x ; } while(0)
#else
#define DBG_LWPS_HTTP_AUTH(x) do { } while(0)
#endif

#define DBG_LWPS_HTTP_FRAGMENT_ENABLE   (0)
#if (DBG_LWPS_HTTP_ENABLE) && (DBG_LWPS_HTTP_FRAGMENT_ENABLE)
#define DBG_LWPS_HTTP_FRAG(x) do {  printf x ; } while(0)
#else
#define DBG_LWPS_HTTP_FRAG(x) do { } while(0)
#endif

#if DBG_LWPS_HTTP_ENABLE
    #include <stdio.h>      \
    #define DBG_WEB_HTTP_PRINTF(msg, pkt, len) \
    do { \
        uint16 xdata loopi;     \
        printf(msg);   \
        for(loopi = 0;loopi < len;loopi++)  \
          {                     \
               if( (loopi&0xF) == 0)\
                printf("\n");   \
               if( (loopi&0x7) == 0)\
                printf(" ");   \
               printf("%02bx ",pkt[loopi]); \
           }            \
        printf("\n");   \
    } while(0)
#else /* else of DBG_LWPS_HTTP_ENABLE */
    #define DBG_WEB_HTTP_PRINTF(msg, pkt, len)    
#endif /* end of else of DBG_LWPS_HTTP_ENABLE */

#endif

