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
#include <msg.h>

#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* Workhorse function for actually outputting messages */
static void err_doit(int flag, const char *fmt, va_list ap);

/* Debug file pointer */
static FILE * debug_file;


void ggzcore_debug(const char *fmt, ...)
{
#ifdef DEBUG
	va_list ap;
	
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
#endif
}


void ggzcore_error_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
}


void ggzcore_error_sys_exit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);
}


void ggzcore_error_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}


void ggzcore_error_msg_exit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);
}


void ggzcore_debug_file_init(const char* file)
{
	if (strcmp(file, "stderr") != 0)
		if ( (debug_file = fopen(file, "a")) == NULL)
			ggzcore_error_sys_exit("fopen() failed in ggzcore_debug_file_init");
}


void ggzcore_debug_file_cleanup(void)
{
	if (debug_file)
		fclose(debug_file);
}


static void err_doit(int flag, const char *fmt, va_list ap)
{
	char buf[4096];
	unsigned int size;

	/* Subtract one to leave room for newline */
	size = sizeof(buf) -1;

#ifdef DEBUG
	sprintf(buf, "[%d]: ", getpid());
#endif

	vsnprintf(buf + strlen(buf), (size - strlen(buf)), fmt, ap);
	if (flag)
		snprintf(buf + strlen(buf), (size - strlen(buf)), ": %s", 
			 strerror(errno));
	strcat(buf, "\n");

	if (debug_file)
		fputs(buf, debug_file);
	else {
		fflush(stdout);
		fputs(buf, stderr);
	}
	
	fflush(NULL);
}

