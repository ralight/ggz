/*
 * File: err_func.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Error functions
 * $Id: err_func.c 7191 2005-05-16 21:11:37Z josef $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

/* We need access to the facility names from syslog.h */
#define SYSLOG_NAMES

#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"

/* Logfile info */
LogInfo log_info = {
	log_initialized: 0,
	syslog_facility: 0,
	options: ( GGZ_LOGOPT_INC_PID
		       | GGZ_LOGOPT_USE_SYSLOG
		       | GGZ_LOGOPT_INC_GAMETYPE
		       | GGZ_DBGOPT_USE_SYSLOG ),
	log_fname: NULL,
	logfile: NULL,
	log_types: 0
#ifdef DEBUG
	,
	dbg_fname: NULL,
	dbgfile: NULL,
	verbose_updates: 0
#endif
};

/* Update log values */
static struct {
	pthread_mutex_t mut;
	int update_interval;
	time_t start_time;
	time_t next_update;
	int anon_users;
	int regd_users;
	int num_logins;
	int num_logouts;
	int kb_in;
	int kb_out;
	int db_rd;
	int db_wr;
	int tables;
	int tables_created;
	int tables_closed;
} update_info;


/* Internal use functions */
static FILE *log_open_logfile(char *);


static void send_debug_output(int priority,
                              const char *header,
                              const char *msg)
{
	FILE *f;
	
	/* If logs not yet initialized, send to stderr */
	if(!log_info.log_initialized) {
		fflush(stdout);
		fputs(header, stderr);
		fputs(msg, stderr);
		fputs("\n", stderr);
		fflush(NULL);
	} else if(priority != LOG_DEBUG) {
		if(log_info.options & GGZ_LOGOPT_USE_SYSLOG) {
			syslog(priority, "%s%s\n", header, msg);
		} else {
			if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
				f = log_open_logfile(log_info.log_fname);
			else
				f = log_info.logfile;
			fputs(header, f);
			fputs(msg, f);
			fputs("\n", f);
			fflush(NULL);
			if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
				fclose(f);
		}
	}
#ifdef DEBUG
	  else {
		if(log_info.options & GGZ_DBGOPT_USE_SYSLOG) {
			syslog(priority, "%s%s\n", header, msg);
		} else {
			if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
				f = log_open_logfile(log_info.dbg_fname);
			else
				f = log_info.dbgfile;
			fputs(header, f);
			fputs(msg, f);
			fputs("\n", f);
			fflush(NULL);
			if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
				fclose(f);
		}
	}
#endif
}

static void debug_handler(int priority, const char *msg)
{
	char hdr[128];
	time_t now;
	struct tm localtm;
	int kill_me = 0;

	if((log_info.options & GGZ_LOGOPT_INC_PID) || priority == LOG_DEBUG)
		snprintf(hdr, sizeof(hdr), "[%d]: ", getpid());
	else
		hdr[0] = '\0';

	/* Include the timestamp if TimeInLogs is on */
	if(log_info.options & GGZ_LOGOPT_INC_TIME) {
		time(&now);
		if(localtime_r(&now, &localtm) == NULL)
			/* Can't err_sys_exit, might cause a loop */
			kill_me = 1;
		else
			strftime(hdr + strlen(hdr), sizeof(hdr) - strlen(hdr),
				"%b %d %Y %T ", &localtm);
	}

	send_debug_output(priority, hdr, msg);
	
	if (kill_me) {
		send_debug_output(LOG_CRIT, "", "localtime_r failed in err_func.c, aborting");
		/* cleanup() */
		exit(-1);
	}
}


/* log_msg sends to the log if it passes the log_types mask */
void log_msg(const unsigned log_type, const char *fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	
	if((log_type == 0) || (log_type & log_info.log_types)) {
		int priority;
		char msg[4096];

		vsnprintf(msg, sizeof(msg), fmt, ap);
	
		switch(log_type) {
			case GGZ_LOG_ALWAYS:
				priority = LOG_WARNING;
				break;
			case GGZ_LOG_NOTICE:
				priority = LOG_NOTICE;
				break;
			default:
				priority = LOG_INFO;
				break;
		}
		debug_handler(priority, msg);
	}
	
	va_end(ap);
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
		dbg_msg(GGZ_DBG_CONFIGURATION,
			"syslogd facility set to %s", facstr);
	}

	return fac;
}


/* Initialize the log files */
void logfile_initialize(void)
{
	const char *debug_types[] = {NULL}; /* Initialized in the config file */
	ggz_debug_init(debug_types, NULL);
	ggz_debug_set_func(debug_handler);

	if(log_info.log_fname) {
		if(strcmp("syslogd", log_info.log_fname)) {
			log_info.logfile = log_open_logfile(log_info.log_fname);
			if(log_info.logfile == NULL)
				err_msg("Cannot open logfile for writing");
			else {
				log_info.options &= ~GGZ_LOGOPT_USE_SYSLOG;
				if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
					fclose(log_info.logfile);
			}
		}
	}

	if(log_info.options & GGZ_LOGOPT_USE_SYSLOG) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}

	pthread_mutex_init(&update_info.mut, NULL);
	update_info.update_interval = LOG_UPDATE_INTERVAL;
	update_info.next_update = time(NULL) + update_info.update_interval;
	update_info.start_time = time(NULL);
	update_info.anon_users = 0;
	update_info.regd_users = 0;
	update_info.num_logins = 0;
	update_info.num_logouts = 0;
	update_info.kb_in = 0;
	update_info.kb_out = 0;
	update_info.db_rd = 0;
	update_info.db_wr = 0;
	update_info.tables = 0;
	update_info.tables_created = 0;
	update_info.tables_closed = 0;

#ifdef DEBUG
	/* Setup the debug logfile */
	if(log_info.dbg_fname) {
		if(strcmp("syslogd", log_info.dbg_fname)) {
			log_info.dbgfile = log_open_logfile(log_info.dbg_fname);
			if(log_info.dbgfile == NULL)
				err_msg("Cannot open dbgfile for writing");
			else {
				log_info.options &= ~GGZ_DBGOPT_USE_SYSLOG;
				if(log_info.options & GGZ_LOGOPT_THREAD_LOGS)
					fclose(log_info.dbgfile);
			}
		}
	}

	if(log_info.options & GGZ_DBGOPT_USE_SYSLOG) {
		if(log_info.syslog_facility == 0)
			log_info.syslog_facility = LOG_LOCAL0;
		openlog("ggzd", 0, log_info.syslog_facility);
	}
#endif

	log_info.log_initialized = 1;
	dbg_msg(GGZ_DBG_CONFIGURATION, "Logfiles initialized");
}


/* Open a logfile and return a FILE pointer */
static FILE *log_open_logfile(char *fname)
{
	char *f=fname;
	char pidname[strlen(fname) + 9];

	if(log_info.options & GGZ_LOGOPT_THREAD_LOGS) {
		snprintf(pidname, sizeof(pidname), "%s_%u", fname, getpid());
		f = pidname;
	}

	return( fopen(f, "a") );
}


/* Return number of seconds until next update log entry */
int log_next_update_sec(void)
{
	int max_select_wait;

	if((max_select_wait = update_info.next_update - time(NULL)) < 1)
		max_select_wait = 1;

	return max_select_wait;
}


/* Generate an update log entry */
void log_generate_update(void)
{
	int uptime;
	int anon, regd, login, logout;

	/* Not in critical section (only our thread uses these) */
	uptime = time(NULL) - update_info.start_time;
	update_info.next_update = time(NULL) + update_info.update_interval;

	pthread_mutex_lock(&update_info.mut);

	/* Get all our info */
	anon = update_info.anon_users;
	regd = update_info.regd_users;
	login = update_info.num_logins;
	logout = update_info.num_logouts;

	/* Clear periodic counters */
	update_info.num_logins = 0;
	update_info.num_logouts = 0;
	update_info.kb_in = 0;
	update_info.kb_out = 0;
	update_info.db_rd = 0;
	update_info.db_wr = 0;
	update_info.tables_created = 0;
	update_info.tables_closed = 0;

	pthread_mutex_unlock(&update_info.mut);

#ifdef DEBUG
	if(log_info.verbose_updates) {
		log_msg(GGZ_LOG_UPDATE, "UPDATE Uptime=%d sec", uptime);
		log_msg(GGZ_LOG_UPDATE, "UPDATE There are %d anonymous users and %d registered users online", anon, regd);
		log_msg(GGZ_LOG_UPDATE, "UPDATE Since the last update, %d users have logged in, %d logged out", login, logout);
		
	} else
#endif
		log_msg(GGZ_LOG_UPDATE, "UPDATE %d %d %d %d %d",
			uptime, anon, regd, login, logout);
}


void log_login_anon(void)
{
	pthread_mutex_lock(&update_info.mut);
	update_info.anon_users++;
	update_info.num_logins++;
	pthread_mutex_unlock(&update_info.mut);
}


void log_logout_anon(void)
{
	pthread_mutex_lock(&update_info.mut);
	update_info.anon_users--;
	update_info.num_logouts++;
	pthread_mutex_unlock(&update_info.mut);
}


void log_login_regd(void)
{
	pthread_mutex_lock(&update_info.mut);
	update_info.regd_users++;
	update_info.num_logins++;
	pthread_mutex_unlock(&update_info.mut);
}


void log_logout_regd(void)
{
	pthread_mutex_lock(&update_info.mut);
	update_info.regd_users--;
	update_info.num_logouts++;
	pthread_mutex_unlock(&update_info.mut);
}


void log_update_set_interval(int sec)
{
	/* This must be called before threading starts.  We could potentially
	 * re-read config files later in which case some form of locking
	 * would be needed.  There used to be a mutex lock here but it
	 * caused crashes because it was called before the mutex was even
	 * initialized.  The easiest solution was just to remove it. */
	update_info.update_interval = sec;
}
