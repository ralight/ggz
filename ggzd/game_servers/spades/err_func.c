/*
 * File: err_func.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Error functions
 * $Id: err_func.c 2599 2001-10-24 00:36:12Z jdorje $
 *
 * Copyright (C) 1999 Brent Hendricks.
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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <err_func.h>
#include <easysock.h>
#include <engine.h>
#include <protocols.h>

extern gameInfo_t gameInfo;

static void err_doit(int flag, const char *fmt, va_list ap)
{
	char buf[4096];

	/*snprintf(buf, sizeof(buf), "[%d]: ", getpid());*/
        vsnprintf(buf, sizeof(buf), fmt, ap);
	if (flag)
		snprintf(buf + strlen(buf), sizeof(buf)-strlen(buf), ": %s", strerror(errno));
	/*strcat(buf, "\n");
	 fflush(stdout);*/
	fputs(buf, stderr);
	fflush(NULL);

}


void err_sys(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);

}


void err_sys_exit(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);

}


void err_msg(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);

}


void err_msg_exit(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);

}


void dbg_msg(const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
	es_write_int(gameInfo.ggz_sock, MSG_LOG);
	es_write_int(gameInfo.ggz_sock, GGZ_DBG_TABLE);
	es_write_string(gameInfo.ggz_sock, buf);
	va_end(ap);
}


void log_msg(const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
	es_write_int(gameInfo.ggz_sock, MSG_LOG);
	es_write_int(gameInfo.ggz_sock, GGZ_LOG_NOTICE);
	es_write_string(gameInfo.ggz_sock, buf);
	va_end(ap);
}
