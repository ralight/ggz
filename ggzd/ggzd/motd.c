/*
 * File: motd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 02/05/2000
 * Desc: Handle message of the day functions
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/utsname.h>
#include <pthread.h>

#include <ggzd.h>
#include <datatypes.h>
#include <err_func.h>
#include <easysock.h>
#include <protocols.h>

/* MOTD info */
MOTDInfo motd_info;

/* Server wide data structures */
extern Options opt;
extern struct Users players;
extern struct GameTables tables;

/* Local functions */
static char *motd_parse_motd_line(char *, char *, int);
static char *motd_get_uptime(char *, int);
static char *motd_get_date(char *, int);
static char *motd_get_time(char *, int);
static char *motd_get_users(char *, int);
static char *motd_get_tables(int, char *, int);

/* Read the motd file */
void motd_read_file(void)
{
	char *fullpath;
	FILE *motd_file;
	char line[128];
	int lines;
	struct utsname unames;

	/* Save the server startup time so we can calculate uptime later */
	motd_info.startup_time = (unsigned int) time(NULL);

	/* If it's an absolute path already, we don't need to add game_dir */
	if(motd_info.motd_file[0] == '/')
		fullpath = motd_info.motd_file;
	else {
		if((fullpath = malloc(strlen(motd_info.motd_file) +
				      strlen(opt.game_dir) + 2)) == NULL)
			err_sys_exit("malloc error in motd_read_file()");
		sprintf(fullpath, "%s/%s", opt.game_dir, motd_info.motd_file);
	}

	/* Try to open the file */
	if((motd_file = fopen(fullpath, "r")) == NULL) {
		err_msg("MOTD file not found");
		motd_info.use_motd = 0;
		return;
	}

	/* Read the file one line at a time (up to MAX_MOTD_LINES) */
	lines = 0;
	while(fgets(line, 128, motd_file) && (lines < MAX_MOTD_LINES)) {
		if((motd_info.motd_text[lines] = malloc(strlen(line)+1)) ==NULL)
			err_sys_exit("malloc error in motd_read_file()");
		strcpy(motd_info.motd_text[lines], line);
		lines++;
	}
	motd_info.motd_lines = lines;

	fclose(motd_file);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Read MOTD file, %d lines", lines);

	/* Initialize stuff that is constant as long as the server is up */

	/* Get the hostname */
	if((motd_info.hostname = malloc(128)) == NULL)
		err_sys_exit("malloc error in motd_read_file()");
	if(gethostname(motd_info.hostname, 127) != 0)
		strcpy(motd_info.hostname, "hostname.toolong.fixme");

	/* Get the OS and CPU Type */
	if(uname(&unames))
		err_sys_exit("uname error in motd_parse_motd()");
	if((motd_info.sysname = malloc(strlen(unames.sysname)+1)) == NULL)
		err_sys_exit("malloc error in motd_read_file()");
	strcpy(motd_info.sysname, unames.sysname);
	if((motd_info.cputype = malloc(strlen(unames.machine)+1)) == NULL)
		err_sys_exit("malloc error in motd_read_file()");
	strcpy(motd_info.cputype, unames.machine);

	/* Get our port number */
	if((motd_info.port = malloc(6)) == NULL)
		err_sys_exit("malloc error in motd_read_file()");
	snprintf(motd_info.port, 6, "%d", opt.main_port);
}


/* Send out the message of the day, return TRUE on success */
int motd_send_motd(int sock)
{
	int i;
	char pline[1024];

	if(es_write_int(sock, MSG_MOTD) < 0 ||
	   es_write_int(sock, motd_info.motd_lines) < 0)
		return -1;
	
	for(i=0; i<motd_info.motd_lines; i++) {
		motd_parse_motd_line(motd_info.motd_text[i],
					 pline, sizeof(pline));
		if(es_write_string(sock, pline) < 0)
			return -1;
	}

	return 0;
}


/* Parse a motd line and return a pointer to a string */
static char *motd_parse_motd_line(char *line, char *outline, int sz_outline)
{
	char *in = line;
	char *p;
	int outindex = 0;
	char str[32];

	/* Loop through the string, watching out for % specifiers */
	while(*in && (outindex < (sz_outline-1))) {
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
					if(outindex < (sz_outline-1)) {
						outline[outindex] = *in;
						outindex++;
					}
					break;
			}

			/* Copy a string if we have one pointed to */
			if(p) {
				while(*p && outindex < (sz_outline-1)) {
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
	outline[outindex] = '\0';

	return outline;
}


/* Convert the curent uptime into a nice string format and return a pointer */
char *motd_get_uptime(char *uptime_str, int sz_uptime_str)
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
static char *motd_get_date(char *date_str, int sz_date_str)
{
 	time_t now;
	struct tm localtm;

	time(&now);
	if(localtime_r(&now, &localtm) == NULL)
		err_sys_exit("localtime_r returned error in motd_get_date()");
	strftime(date_str, sz_date_str, "%B %e, %Y", &localtm);

	return date_str;
}


/* Setup a string to send the time */
static char *motd_get_time(char *time_str, int sz_time_str)
{
 	time_t now;
	struct tm localtm;

	time(&now);
	if(localtime_r(&now, &localtm) == NULL)
		err_sys_exit("localtime_r returned error in motd_get_date()");
	strftime(time_str, sz_time_str, "%H:%M %Z", &localtm);

	return time_str;
}


/* Setup a string showing the number of users online */
static char *motd_get_users(char *users_str, int sz_users_str)
{
	pthread_rwlock_rdlock(&players.lock);
	snprintf(users_str, sz_users_str, "%d", players.count);
	pthread_rwlock_unlock(&players.lock);

	return users_str;
}


/* Get the number of tables based on option */
static char *motd_get_tables(int option,
			     char *tables_str,
			     int sz_tables_str)
{
	int i, num_tables=0;

	pthread_rwlock_rdlock(&tables.lock);
	if(option == 0)
		/* Get total number of tables */
		num_tables = tables.count;
	else
		/* Determine number of tables open */
		for(i=0; i<MAX_TABLES; i++)
			if(tables.info[i].type_index != -1 &&
			   tables.info[i].state != GGZ_TABLE_PLAYING)
				num_tables++;
	pthread_rwlock_unlock(&tables.lock);

	snprintf(tables_str, sz_tables_str, "%d", num_tables);
	return tables_str;
}
