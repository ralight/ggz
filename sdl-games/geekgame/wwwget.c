/*
 * Geekgame - a game which only real geeks understand
 * Copyright (C) 2002, 2003 Josef Spillner, josef@ggzgamingzone.org
 * $Id: wwwget.c 6759 2005-01-20 05:17:31Z jdorje $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include "wwwget.h"

/* Copy a file via FTP or HTTP to a local tempfile */
static void wwwget_internal(const char *source, const char *dest)
{
	const char *program = "wget";
	char *const args[] =
	{
		strdup(program),
		strdup(source),
		"-q",
		"-O",
		strdup(dest),
		NULL
	};

	execvp(program, args);
}

/* Copy a local file to a local tempfile */
static void wwwcopy_internal(const char *source, const char *dest)
{
	const char *program = "cp";
	char *const args[] =
	{
		strdup(program),
		strdup(source),
		strdup(dest),
		NULL
	};

	execvp(program, args);
}

/* Copy any file, no matter its location, to a local tempfile */
void wwwget(const char *source, const char *dest)
{
	pid_t pid;
	int status;

	unlink(dest);

	pid = fork();
	switch(pid)
	{
		case -1:
			return;
		case 0:
			if(source[0] != '/')
			{
				wwwget_internal(source, dest);
			}
			else
			{
				wwwcopy_internal(source, dest);
			}
			_exit(0);
	}

	waitpid(pid, &status, 0);
}

