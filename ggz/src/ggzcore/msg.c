/*
 * File: msg.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Debug and error messages
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


#include <config.h>
#include <ggzcore.h>

#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void err_doit(int flag, const char *fmt, va_list ap);

void ggzcore_debug(const char *fmt, ...)
{
#ifdef DEBUG
	va_list ap;
	
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
#endif
}


static void err_doit(int flag, const char *fmt, va_list ap)
{
	char buf[4096];

#ifdef DEBUG
	sprintf(buf, "[%d]: ", getpid());
#endif

	vsprintf(buf + strlen(buf), fmt, ap);
	if (flag)
		sprintf(buf + strlen(buf), ": %s", strerror(errno));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}
