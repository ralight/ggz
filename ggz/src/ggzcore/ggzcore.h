/*
 * File: ggzcore.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Interface file to be included by client frontends
 *
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef __GGZCORE_H__
#define __GGZCORE_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif 


/* GGZCore library features */
typedef enum
{
	GGZ_ENABLE_PARSER      = 1,   /* %0000 0000 */
	GGZ_ENABLE_MODULES     = 2,   /* %0000 0010 */
	GGZ_ENABLE_THREADED_IO = 4,   /* %0000 0100 */
} GGZOptionFlags;

/* ggzcore_init() - Initializtion function for ggzcore lib.
 *
 * Receives:
 * GGZOptionsFlags options : bitmask of enabled features
 * const char* global_conf : Path to system configuration file
 * const char* local_conf  : Path to user configuration file
 *
 * Returns:
 * int : 0 if successful, -1 on failure
 */
int ggzcore_init(GGZOptionFlags options, 
		 const char* global_conf, 
		 const char* local_conf);
	

int ggzcore_event_connect();

int ggzcore_event_pending();

int ggzcore_event_process();

int ggzcore_event_trigger();

void ggzcore_debug(const char *fmt, ...);

void ggzcore_destroy();


#ifdef __cplusplus
}
#endif 

#endif  /* __GGZCORE_H__ */
