/*
 * File: util.h
 * Author: Brent Hendricks, GGZ Dev Team
 * Project: GGZ Server
 * Date: 05/04/2002 (code moved from control.c)
 * Desc: General utility functions for ggzd
 * $Id: util.c 4150 2002-05-04 23:46:47Z jdorje $
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
#include <sys/types.h>

#include "err_func.h"
#include "util.h"

/*
 * Given a path and a mode, we create the given directory.  This
 * is called only if we've determined the directory doesn't
 * already exist.
 */
static int make_path(const char* full, mode_t mode)
{
	const char* slash = "/";
	char* copy, *dir, *path;
	struct stat stats;

	copy = strdup(full);

	/* FIXME: check validity */
	if ( (path = calloc(sizeof(char), strlen(full)+1)) == NULL)
		err_sys_exit("malloc failed in make_path");
	
	/* Skip preceding / */
	if (copy[0] == '/')
		copy++;

	while ((dir = strsep(&copy, slash))) {
		strcat(strcat(path, "/"), dir);
		if (mkdir(path, mode) < 0
		    && (stat(path, &stats) < 0 || !S_ISDIR(stats.st_mode))) {
			free(path);
			free(copy);
			return -1;
		}
	}

	return 0;
}


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
		if (make_path(full_path, S_IRWXU) < 0)
			err_sys_exit("Couldn't create %s", full_path);
	} else /* Everything eas OK, so close it */
		closedir(dir);
}
