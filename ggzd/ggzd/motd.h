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


#include <ggzd.h>


/* MOTD info */
typedef struct {
	char *motd_file;			/* cleanup() */
	char use_motd;
	unsigned long startup_time;
	int motd_lines;
	char *motd_text[MAX_MOTD_LINES];	/* cleanup() */
	char *hostname;				/* cleanup() */
	char *sysname;				/* cleanup() */
	char *cputype;				/* cleanup() */
	char *port;				/* cleanup() */
} MOTDInfo;


/* MOTD info */
extern MOTDInfo motd_info;

/* Read the MOTD file */
extern void motd_read_file(void);

/* Returns 'true' if motd is defined */
int motd_is_defined(void);

/* Return number of lines in the motd */
int motd_get_num_lines(void);

/* Parse a line of the motd, filling in parameters */
char *motd_get_line(int, char *, int);

