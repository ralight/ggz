/*
 * File: motd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 02/05/2000
 * Desc: Handle message of the day functions
 * $Id: motd.c 10214 2008-07-08 16:44:13Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

#include <ggz.h>

#include "ggzd.h"
#include "datatypes.h"
#include "err_func.h"
#include "protocols.h"
#include "table.h"
#include "motd.h"

/* Check for system-defined maximum host name limit. */
#ifndef HOST_NAME_MAX
#  define HOST_NAME_MAX 256
#endif

/* MOTD info */
typedef struct {
	unsigned long startup_time;
	int motd_lines;
	char **motd_text;			/* cleanup() */
	char *hostname;				/* cleanup() */
	char *sysname;				/* cleanup() */
	char *cputype;				/* cleanup() */
	char *port;				/* cleanup() */
} MOTDInfo;

/* MOTD info - the fields are initialized to their starting values. */
MOTDInfo motd_info = { .startup_time = 0,
		       .motd_lines = 0,
		       .motd_text = NULL,
		       .hostname = NULL,
		       .sysname = NULL,
		       .cputype = NULL,
		       .port = NULL};

/* Local functions */
static char *motd_get_uptime(char *uptime_str, size_t sz_uptime_str);
static char *motd_get_date(char *date_str, size_t sz_date_str);
static char *motd_get_time(char *time_str, size_t sz_time_str);
static char *motd_get_users(char *users_str, size_t sz_users_str);
static char *motd_get_tables(int option,
			     char *tables_str, size_t sz_tables_str);

static void motd_cleanup(void)
{
	int i;

	/* Cleanup... */
	for (i = 0; i < motd_info.motd_lines; i++)
	  ggz_free(motd_info.motd_text[i]);
	if (motd_info.motd_text)
	  ggz_free(motd_info.motd_text);
	motd_info.motd_text = NULL;
	motd_info.motd_lines = 0;

	if (motd_info.hostname)
	  ggz_free(motd_info.hostname);
	motd_info.hostname = NULL;

	if (motd_info.sysname)
	  ggz_free(motd_info.sysname);
	motd_info.sysname = NULL;

	if (motd_info.cputype)
	  ggz_free(motd_info.cputype);
	motd_info.cputype = NULL;

	if (motd_info.port)
	  ggz_free(motd_info.port);
	motd_info.port = NULL;
}

/* Read the motd file */
void motd_read_file(const char *file)
{
	char *fullpath;
	int motd_fdes;
	GGZFile *motd_file;
	char *line;
	int alloc_lines;
	int lines;
	int len;
	struct utsname unames;
	char hostname[HOST_NAME_MAX + 1];

	motd_cleanup();

	/* Save the server startup time so we can calculate uptime later.
	   But only do this once (in case we re-load the MOTD later) */
	if (!motd_info.startup_time)
		motd_info.startup_time = (unsigned long) time(NULL);

	/* Check if we're even supposed to have a MOTD. */
	if (!file) return;

	/* If it's an absolute path already, we don't need to add conf_dir */
	if (file[0] == '/')
		fullpath = ggz_strdup(file);
	else if (!opt.conf_dir) {
		ggz_error_msg("motd_read_file: config directory unspecified");
		return;
	} else {
		len = strlen(file) + strlen(opt.conf_dir) + 2;
		fullpath = ggz_malloc(len);
		snprintf(fullpath, len, "%s/%s", opt.conf_dir, file);
	}

	/* Try to open the file */
	if((motd_fdes = open(fullpath, O_RDONLY)) < 0) {
		ggz_error_msg("MOTD file not found");
		ggz_free(fullpath);
		motd_read_file(NULL);
		return;
	}
	ggz_free(fullpath);

	motd_file = ggz_get_file_struct(motd_fdes);

	/* Read the file one line at a time (up to MAX_MOTD_LINES) */
	alloc_lines = 10;
	motd_info.motd_text = ggz_malloc(alloc_lines * sizeof(char *));
	lines = 0;
	while((line = ggz_read_line(motd_file))) {
		if(lines == alloc_lines) {
			alloc_lines *= 2;
			motd_info.motd_text = ggz_realloc(motd_info.motd_text,
						alloc_lines * sizeof(char *));
		}
		motd_info.motd_text[lines++] = line;
	}
	motd_info.motd_lines = lines;

	ggz_free_file_struct(motd_file);
	close(motd_fdes);
	ggz_debug(GGZ_DBG_CONFIGURATION, "Read MOTD file, %d lines", lines);

	/* Initialize stuff that is constant as long as the server is up.
	   This stuff is currently re-read every time we reread the MOTD.
	   But that's probably OK... */

	/* Get the hostname */
	if(gethostname(hostname, HOST_NAME_MAX) != 0)
		ggz_strncpy(hostname, "hostname.toolong.fixme", HOST_NAME_MAX);
	motd_info.hostname = ggz_strdup(hostname);

	/* Get the OS and CPU Type */
	if(uname(&unames) < 0)
		ggz_error_sys_exit("uname error in motd_parse_motd()");
	motd_info.sysname = ggz_strdup(unames.sysname);
	motd_info.cputype = ggz_strdup(unames.machine);

	/* Get our port number */
	motd_info.port = ggz_malloc(6);
	snprintf(motd_info.port, 6, "%d", opt.main_port);
}


/* Returns 'true' if motd is defined */
bool motd_is_defined(void)
{
	return (motd_info.motd_text != NULL);
}


/* Return number of lines in the motd */
int motd_get_num_lines(void)
{
	return motd_info.motd_lines;
}


/* Return cooked version of a motd line */
char *motd_get_line(int lineno)
{
	char *in = motd_info.motd_text[lineno];
	char *outline;
	int sz_outline;
	const char *p;
	int outindex = 0;
	char str[32];

	sz_outline = 1024;
	outline = ggz_malloc(sz_outline);

	/* Loop through the string, watching out for % specifiers */
	while(*in) {
		if(outindex >= sz_outline-1) {
			sz_outline += 512;
			outline = ggz_realloc(outline, sz_outline);
		}
		if(*in == '%') {
			in++;
			/* Set a pointer to a string to replace the %? with */
			switch(*in) {
				case 'a':
					p = opt.admin_name;
					break;
				case 'C':
					p = motd_info.cputype;
					break;
				case 'd':
					p = motd_get_date(str, sizeof(str));
					break;
				case 'e':
					p = opt.admin_email;
					break;
				case 'g':
					p = motd_get_tables(0, str,sizeof(str));
					break;
				case 'G':
					p = motd_get_tables(1, str,sizeof(str));
					break;
				case 'h':
					p = motd_info.hostname;
					break;
				case 'o':
					p = motd_info.sysname;
					break;
				case 'p':
					p = motd_info.port;
					break;
				case 't':
					p = motd_get_time(str, sizeof(str));
					break;
				case 'u':
					p = motd_get_uptime(str, sizeof(str));
					break;
				case 'U':
					p = motd_get_users(str, sizeof(str));
					break;
				case 'v':
					p = VERSION;
					break;
				default:
					p = NULL;
					outline[outindex] = '%';
					outindex++;
					if(outindex >= sz_outline-1) {
						sz_outline += 512;
						outline = ggz_realloc(outline, sz_outline);
					}
					outline[outindex] = *in;
					outindex++;
					break;
			}

			/* Copy a string if we have one pointed to */
			if(p) {
				while(*p && outindex < (sz_outline-1)) {
					if(outindex >= sz_outline-1) {
						sz_outline += 512;
						outline = ggz_realloc(outline, sz_outline);
					}
					outline[outindex] = *p;
					p++;
					outindex++;
				}
			}

			in++;
			continue;
		}
		/* Copy the string, byte by byte */
		outline[outindex] = *in;
		outindex++;
		in++;
	}
	if(outindex >= sz_outline-1) {
		sz_outline += 1;
		outline = ggz_realloc(outline, sz_outline);
	}
	outline[outindex] = '\0';

	return outline;
}


/* Convert the curent uptime into a nice string format and return a pointer */
static char *motd_get_uptime(char *uptime_str, size_t sz_uptime_str)
{
	unsigned long uptime;
	int days;
	int hours;

	uptime = time(NULL) - motd_info.startup_time;

	days = uptime / 86400;
	hours = (uptime - days*86400) / 3600;

	if(days == 0 && hours == 0) {
		strcpy(uptime_str, "less than 1 hour");
		return uptime_str;
	} else if(days == 0) {
		snprintf(uptime_str, sz_uptime_str, "%d hour", hours);
	} else {
		snprintf(uptime_str, sz_uptime_str, "%d day%sand %d hour",
			days,
			(days != 1) ? "s " : " ",
			hours );
	}
	if(hours != 1 && strlen(uptime_str) < sz_uptime_str)
		strcat(uptime_str, "s");

	return uptime_str;
}


/* Setup a string to send the date */
static char *motd_get_date(char *date_str, size_t sz_date_str)
{
 	time_t now;
	struct tm localtm;

	time(&now);
	if(localtime_r(&now, &localtm) == NULL)
		ggz_error_sys_exit("localtime_r returned error in motd_get_date()");
	strftime(date_str, sz_date_str, "%B %e, %Y", &localtm);

	return date_str;
}


/* Setup a string to send the time */
static char *motd_get_time(char *time_str, size_t sz_time_str)
{
 	time_t now;
	struct tm localtm;

	time(&now);
	if(localtime_r(&now, &localtm) == NULL)
		ggz_error_sys_exit("localtime_r returned error in motd_get_date()");
	strftime(time_str, sz_time_str, "%H:%M %Z", &localtm);

	return time_str;
}


/* Setup a string showing the number of users online */
static char *motd_get_users(char *users_str, size_t sz_users_str)
{
	pthread_rwlock_rdlock(&state.lock);
	snprintf(users_str, sz_users_str, "%d", state.players);
	pthread_rwlock_unlock(&state.lock);

	return users_str;
}


/* Get the number of tables based on option */
static char *motd_get_tables(int option,
			     char *tables_str,
			     size_t sz_tables_str)
{
	int num_tables=0;
	/* int i; */

	if(option == 0) {
		/* Get total number of tables */
		pthread_rwlock_rdlock(&state.lock);
		num_tables = state.tables;
		pthread_rwlock_unlock(&state.lock);		
	}
	else
		num_tables = 0;
#if 0
		/* Determine number of tables open */
		for(i = 0; i <  opt.max_tables; i++) {
			pthread_rwlock_rdlock(&tables[i].lock);
			if(tables[i].type != -1 &&
			   tables[i].state != GGZ_TABLE_PLAYING)
				num_tables++;
			pthread_rwlock_unlock(&tables[i].lock);
		}
#endif

	snprintf(tables_str, sz_tables_str, "%d", num_tables);
	return tables_str;
}
