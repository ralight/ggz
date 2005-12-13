/*
 * File: util.h
 * Author: Brent Hendricks, GGZ Dev Team
 * Project: GGZ Server
 * Date: 05/04/2002 (code moved from control.c)
 * Desc: General utility functions for ggzd
 * $Id: util.c 7665 2005-12-13 00:35:51Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include "err_func.h"
#include "util.h"

/*
 * Given a path for a directory, we check to see if that directory
 * exists.  If it doesn't, we create it.
 *
 * This is functionally similar to "mkdir -p".
 */
void check_path(const char* full_path)
{
	/* This could be done with system("mkdir -p $(full_path)"),
	   but we'd have to check to see if -p was supported, etc.
	   And it doesn't appear any library functions will do this... */
	DIR* dir;
	
	if ( (dir = opendir(full_path)) == NULL) {
		dbg_msg(GGZ_DBG_CONFIGURATION,
			"Couldn't open %s -- trying to create", full_path);
		if (ggz_make_path(full_path) < 0)
			err_sys_exit("Couldn't create %s", full_path);
	} else /* Everything eas OK, so close it */
		closedir(dir);
}

/* Converts a timeval struct (with sec/usec) into
   a double-precision value (in seconds). */
ggztime_t timeval_to_ggztime(struct timeval tv)
{
	return (ggztime_t)tv.tv_sec + (ggztime_t)tv.tv_usec / 1000000.0;
}

/* Converts a double-precision value (in seconds) into
   a timeval struct (with sec/usec). */
struct timeval ggztime_to_timeval(ggztime_t ggztime)
{
	struct timeval tv;

	tv.tv_sec = ggztime;
	tv.tv_usec = (ggztime - tv.tv_sec) * 1000000;

	/* For negative times rounding will be done improperly. */
	if (tv.tv_usec < 0) {
		tv.tv_usec += 1000000;
		tv.tv_sec -= 1;
	}

	return tv;
}

/* Returns the current time, as a double-precision value in
   seconds. */
ggztime_t get_current_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return timeval_to_ggztime(tv);
}
