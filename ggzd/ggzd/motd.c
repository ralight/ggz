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

#include <datatypes.h>
#include <err_func.h>
#include <easysock.h>
#include <protocols.h>

/* MOTD info */
MOTDInfo motd_info;

/* We need to get the game_dir */
extern Options opt;

#define MOTD_MAX 1024		/* Warning: changing this may break clients! */

/* Local functions */
static char *motd_parse_motd_line(char *);
static char *motd_get_uptime(void);
static char *motd_get_date(void);
static char *motd_get_time(void);

/* Read the motd file */
void motd_read_file(void)
{
	char *fullpath;
	FILE *motd_file;
	char line[128];
	int lines;

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
	motd_info.use_motd = 1;
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
	dbg_msg("Read MOTD file, %d lines", lines);
}


/* Send out the message of the day, return TRUE on success */
int motd_send_motd(int sock)
{
	int i;
	char *pline;

	if(FAIL(es_write_int(sock, MSG_MOTD)) ||
	   FAIL(es_write_int(sock, motd_info.motd_lines)))
		return -1;
	
	for(i=0; i<motd_info.motd_lines; i++) {
		pline = motd_parse_motd_line(motd_info.motd_text[i]);
		if(FAIL(es_write_string(sock, pline)))
			return -1;
	}

	return 0;
}


/* Parse a motd line and return a pointer to a string */
static char *motd_parse_motd_line(char *line)
{
	static char outline[1024];
	static char hostname[128];
	static char *sysname;
	static int firsttime = 1;
	static struct utsname unames;

	char *in = line;
	char *p;
	int outindex = 0;

	/* Initialize statics that should remain as long as the server is up */
	if(firsttime) {
		if(gethostname(hostname, 127) != 0)
			strcpy(hostname, "hostname.toolong.fixme");
		if(uname(&unames))
			err_sys_exit("uname error in motd_parse_motd()");
		sysname = unames.sysname;
		firsttime = 0;
	}

	/* Loop through the string, watching out for % specifiers */
	while(*in && (outindex < 1023)) {
		if(*in == '%') {
			in++;
			/* Set a pointer to a string to replace the %? with */
			switch(*in) {
				case 'h':
					p = hostname;
					break;
				case 'u':
					p = motd_get_uptime();
					break;
				case 'd':
					p = motd_get_date();
					break;
				case 't':
					p = motd_get_time();
					break;
				case 'o':
					p = sysname;
					break;
				default:
					p = NULL;
					outline[outindex] = '%';
					outindex++;
					if(outindex < 1023) {
						outline[outindex] = *in;
						outindex++;
					}
					break;
			}

			/* Copy a string if we have one pointed to */
			if(p) {
				while(*p && outindex < 1023) {
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
char *motd_get_uptime(void)
{
	static char uptime_str[24];
	unsigned long uptime;
	int days;
	int hours;

	uptime = time(NULL) - motd_info.startup_time;

	days = uptime / 86400;
	hours = (uptime - days*86400) / 3600;

	if(days == 0 && hours == 0)
		return "less than 1 hour";
	else if(days == 0)
		snprintf(uptime_str, 24, "%d hour", hours);
	else {
		snprintf(uptime_str, 24, "%d day%sand %d hour",
			days,
			(days != 1) ? "s " : " ",
			hours );
	}
	if(hours != 1 && strlen(uptime_str) < 24)
		strcat(uptime_str, "s");

	return uptime_str;
}


/* Setup a string to send the date */
static char *motd_get_date(void)
{
	static char date_str[20];
 	time_t now;
	struct tm *localtm;

	time(&now);
	localtm = localtime(&now);
	strftime(date_str, 20, "%B %e, %Y", localtm);

	return date_str;
}


/* Setup a string to send the time */
static char *motd_get_time(void)
{
	static char time_str[16];
 	time_t now;
	struct tm *localtm;

	time(&now);
	localtm = localtime(&now);
	strftime(time_str, 16, "%H:%M %Z", localtm);

	return time_str;
}
