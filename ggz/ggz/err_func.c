/*
 * File: err_func.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 10/11/99
 * Desc: Error functions
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
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <easysock.h>
#include <err_func.h>
#include <dlg_error.h>


static void err_doit(gint flag, const gchar *fmt, va_list ap)
{
	gchar buf[4096];

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


void err_sys(const gchar *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
}


void err_sys_exit(const gchar *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);
}


void err_msg(const gchar *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}


void err_msg_exit(const gchar *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);
}


void dbg_msg(const gchar *fmt, ...)
{
#ifdef DEBUG
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
#endif
}


void err_sock(const gchar* err, const EsOpType op, const EsDataType type)
{
	switch (op) {
#ifdef DEBUG
	case ES_CREATE:
		err_dlg("Error while creating socket: %s\n", err);
		break;
	case ES_READ:
		switch (type) {
		case ES_CHAR:
			err_dlg("Error reading byte from socket: %s\n", err);
			break;
		case ES_INT:
			err_dlg("Error reading int from socket: %s\n", err);
			break;
		case ES_STRING:
			err_dlg("Error reading string from socket: %s\n", err);
			break;
		default:
			err_dlg("Error reading from socket: %s\n", err);
			break;
		}
		break;
	case ES_WRITE:
		switch (type) {
		case ES_CHAR:
			err_dlg("Error writing byte from socket: %s\n", err);
			break;
		case ES_INT:
			err_dlg("Error writing int from socket: %s\n", err);
			break;
		case ES_STRING:
			err_dlg("Error writing string from socket: %s\n", err);
			break;
		default:
			err_dlg("Error writing to socket: %s\n", err);
			break;
		}
		break;
#endif
	case ES_ALLOCATE:
		err_dlg("Error while allocating memory: %s\n", err);
		break;
	}
}


void eprintf(const gchar *fmt, ...)
{
#ifdef DEBUG
	va_list ap;
	gchar buf[4096];

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	printf("%s\n",buf);
	va_end(ap);
#endif
}
