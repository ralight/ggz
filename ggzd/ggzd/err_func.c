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
#include <time.h>

#include <ggzd.h>
#include <datatypes.h>
#include <easysock.h>
#include <err_func.h>

/* Logfile info */
LogInfo log_info = { 0, 0,
		     ( GGZ_LOGOPT_INC_PID
		       | GGZ_LOGOPT_USE_SYSLOG
		       | GGZ_DBGOPT_USE_SYSLOG ),
		     NULL, NULL, -1
#ifdef DEBUG
		   , NULL, NULL, -1
#endif
};


static void err_doit(int flag, int priority, const char *fmt, va_list ap)
{

	char buf[4096];
	int bsize;
	time_t now;
	struct tm localtm;
	int kill_me = 0;

	/* I subtract one from the buffer size since we */
	/* always want room for a '\n' at the end       */
	bsize = sizeof(buf) - 1;

	/* Include the PID if PIDInLogs is on */
	if((log_info.options & GGZ_LOGOPT_INC_PID) || priority == LOG_DEBUG)
		snprintf(buf, bsize-1, "[%d]: ", getpid());
	else
		buf[0] = '\0';

	/* Include the timestamp if TimeInLogs is on */
	if(log_info.options & GGZ_LOGOPT_INC_TIME) {
		time(&now);
		if(localtime_r(&now, &localtm) == NULL)
			/* Can't err_sys_exit, might cause a loop */
			kill_me = 1;
		else
			strftime(buf + strlen(buf), bsize - strlen(buf),
				"%b %d %T ", &localtm);
	}

	/* Put the actual message into the buffer */
	vsnprintf(buf + strlen(buf), bsize - strlen(buf), fmt, ap);
	if (flag)
		snprintf(buf + strlen(buf), bsize - strlen(buf),
			 ": %s", strerror(errno));
	strcat(buf, "\n");

	if(kill_me)
		snprintf(buf + strlen(buf), bsize - strlen(buf),
			"\nlocaltime_r failed in err_func.c, aborting\n");

	/* If logs not yet initialized, send to stderr */
	if(!log_info.log_initialized) {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(NULL);
	} else if(priority != LOG_DEBUG) {
		if(log_info.options & GGZ_LOGOPT_USE_SYSLOG) {
			syslog(priority, "%s", buf);
		} else {
			fputs(buf, log_info.logfile);
			fflush(NULL);
		}
	}
#ifdef DEBUG
	  else {
		if(log_info.options & GGZ_DBGOPT_USE_SYSLOG) {
			syslog(priority, "%s", buf);
		} else {
			fputs(buf, log_info.dbgfile);
			fflush(NULL);
		}
	}
#endif

	if(kill_me) {
		/* cleanup() */
		exit(-1);
	}
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


/* log_msg suggested log_levels:
 *   0 - WARNING situations, these are always logged
 *   1 - NOTICE situations, these are logged by default
 *  >1 - INFO situations, the higher the number the more trivial
 */
void log_msg(const int log_level, const char *fmt, ...)
{
	va_list ap;
	int priority;

	va_start(ap, fmt);
	if(log_level <= log_info.log_level) {
		switch(log_level) {
			case 0:
				priority = LOG_WARNING;
				break;
			case 1:
				priority = LOG_NOTICE;
				break;
			default:
				priority = LOG_INFO;
				break;
		}
		err_doit(0, priority, fmt, ap);
	}
	va_end(ap);
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
	if(log_info.log_level < 0)
		log_info.log_level = 1;
	if(log_info.log_fname) {
		if(strcmp("syslogd", log_info.log_fname)) {
			log_info.logfile = fopen(log_info.log_fname, "a");
			if(log_info.logfile == NULL)
				err_msg("Cannot open logfile for writing");
			else
				log_info.options &= ~GGZ_LOGOPT_USE_SYSLOG;
		}
	}

	if(log_info.options & GGZ_LOGOPT_USE_SYSLOG) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}

#ifdef DEBUG
	/* Setup the debug logfile */
	if(log_info.dbg_level < 0)
		log_info.dbg_level = 0;
	if(log_info.dbg_fname) {
		if(strcmp("syslogd", log_info.dbg_fname)) {
			log_info.dbgfile = fopen(log_info.dbg_fname, "a");
			if(log_info.dbgfile == NULL)
				err_msg("Cannot open dbgfile for writing");
			else
				log_info.options &= ~GGZ_DBGOPT_USE_SYSLOG;
		}
	}

	if(log_info.options & GGZ_DBGOPT_USE_SYSLOG) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}
#endif

	log_info.log_initialized = 1;
	dbg_msg("Logfiles initialized");
}
