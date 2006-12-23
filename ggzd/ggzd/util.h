/*
 * File: util.h
 * Author: Brent Hendricks, GGZ Dev Team
 * Project: GGZ Server
 * Date: 05/04/2002 (code moved from control.c)
 * Desc: General utility functions for ggzd
 * $Id: util.h 8744 2006-12-23 06:27:16Z jdorje $
 *
 * Copyright (C) 1999-2002 Brent Hendricks.
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

#ifndef __UTIL_H__
#define __UTIL_H__

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Functionally similar to "mkdir -p". */
void check_path(const char* full_path);

/* Functions for handling time in an "easy" way. */
typedef double ggztime_t;

ggztime_t timeval_to_ggztime(struct timeval tv);
struct timeval ggztime_to_timeval(ggztime_t ggztime);
ggztime_t get_current_time(void);

#endif /* __UTIL_H__ */
