/*
 * File: err_func.c
 * Author: Brent Hendricks
 * Project: GGZ Server
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

#define SYSLOG_NAMES	/* We need access to the facility names from syslog.h */
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <ggzd.h>
#include <datatypes.h>
#include <easysock.h>
#include <err_func.h>

/* Logfile info */
LogInfo log_info;


static void err_doit(int flag, int priority, const char *fmt, va_list ap)
{

	char buf[4096];

	sprintf(buf, "[%d]: ", getpid());
	vsprintf(buf + strlen(buf), fmt, ap);
	if (flag)
		sprintf(buf + strlen(buf), ": %s", strerror(errno));
	strcat(buf, "\n");

	/* If logs not yet initialized, send to stderr */
	if(!log_info.log_initialized) {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(NULL);
	} else if(priority != LOG_DEBUG) {
		if(log_info.log_use_syslog) {
			syslog(priority, "%s", buf);
		} else {
			fputs(buf, log_info.logfile);
			fflush(NULL);
		}
	}
#ifdef DEBUG
	  else {
		if(log_info.dbg_use_syslog) {
			syslog(priority, "%s", buf);
		} else {
			fputs(buf, log_info.dbgfile);
			fflush(NULL);
		}
	}
#endif
}


void err_sys(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);

}


void err_sys_exit(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_CRIT, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);

}


void err_msg(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);

}


void err_msg_exit(const char *fmt, ...)
{

	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_CRIT, fmt, ap);
	va_end(ap);
	/*cleanup(); */
	exit(-1);

}


void dbg_msg(const char *fmt, ...)
{
#ifdef DEBUG
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_DEBUG, fmt, ap);
	va_end(ap);
#endif
}


void err_sock(const char *err, const EsOpType op, const EsDataType type)
{

	switch (op) {
	case ES_CREATE:
		err_msg("Error while creating socket: %s\n", err);
		break;
	case ES_READ:
		err_msg("Error while reading from socket: %s\n", err);
		break;
	case ES_WRITE:
		err_msg("Error while writing to socket: %s\n", err);
		break;
	case ES_ALLOCATE:
		err_msg("Error while allocating memory: %s\n", err);
		break;
	}
}


/* Set the syslogd facility */
int logfile_set_facility(char *facstr)
{
	int i, fac = -1;

	for(i=0; facilitynames[i].c_name != NULL; i++)
		if(!strcmp(facstr, facilitynames[i].c_name))
			fac = facilitynames[i].c_val;

	if(fac >= 0) {
		log_info.syslog_facility = fac;
		dbg_msg("syslogd facility set to %s", facstr);
	}

	return fac;
}


/* Initialize the log files */
void logfile_initialize(void)
{
	/* Setup the primary logfile */
	log_info.log_use_syslog = 1;
	if(log_info.log_fname) {
		if(strcmp("syslogd", log_info.log_fname)) {
			log_info.logfile = fopen(log_info.log_fname, "w");
			if(log_info.logfile == NULL)
				err_msg("Cannot open logfile for writing");
			else
				log_info.log_use_syslog = 0;
		}
	}

	if(log_info.log_use_syslog) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}

#ifdef DEBUG
	/* Setup the debug logfile */
	log_info.dbg_use_syslog = 1;
	if(log_info.dbg_fname) {
		if(strcmp("syslogd", log_info.dbg_fname)) {
			log_info.dbgfile = fopen(log_info.dbg_fname, "w");
			if(log_info.dbgfile == NULL)
				err_msg("Cannot open dbgfile for writing");
			else
				log_info.dbg_use_syslog = 0;
		}
	}

	if(log_info.dbg_use_syslog) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}
#endif

	log_info.log_initialized = 1;
	dbg_msg("Logfiles initialized");
}
