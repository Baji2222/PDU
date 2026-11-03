/*
* Copyright c                  Realtek Semiconductor Corporation, 2009  
* All rights reserved.
* 
* Abstract : This file provides errno number definition and error 
*	handlering functions for http module 
*$Id: http_errno.h,v 1.0 2009/07/06 08:03:40 zhenyang_jin Exp $
*/

#ifndef __HTTP_ERRNO_H__
#define __HTTP_ERRNO_H__

#include <common_types.h>

/*  General */
#define HTTP_NOT_ENOUGH_MEMORY          -1

/* ASP Pages */
#define HTTP_ASP_PAGE_NONEXIST	        -10
#define HTTP_FRAGMENT_REQUEST           -11


  
extern void http_error(uint8 err);

#endif