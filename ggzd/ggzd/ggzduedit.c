/*
 * File: ggzduedit.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 09/24/01
 * Desc: User database editor for ggzd server
 *
 * Copyright (C) 2001 Brent Hendricks.
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define DB_STANDALONE 1
#include <ggzdb.h>


char *datadir = DATADIR;
char lb[1024];
ggzdbPlayerEntry pe;

void getline(void)
{
	char *p;

	fgets(lb, 1024, stdin);
	p = lb + strlen(lb) - 1;
	*p = '\0';
}


void list_players(void)
{
	int rc;
	int count=0;

	printf("\nList of all known player handles:\n");
	if((rc = _ggzdb_player_get_first(&pe)) != 0) {
		fprintf(stderr, "_ggzdb_player_get_first() rc=%d\n", rc);
		return;
	}
	printf("%s\n", pe.handle);
	while((rc = _ggzdb_player_get_next(&pe)) == 0) {
		if(++count % 23 == 0) {
			printf("[pause]\007 ");
			getline();
			if(lb[0] == 'Q' || lb[0] == 'q')
				break;
		}
		printf("%s\n", pe.handle);
	}
	if(lb[0] != 'Q' && lb[0] != 'q' && (count % 23) > 12) {
		printf("[pause]\007 ");
		getline();
	}

	printf("\n");

	if(rc != GGZDB_ERR_NOTFOUND)
		fprintf(stderr, "_ggzdb_player_get_next() rc=%d\n", rc);

	_ggzdb_player_drop_cursor();
}


void show_player(void)
{
	int i, rc;
	char time_asc[128];

	printf("Player handle: ");
	getline();
	for(i=0; i<MAX_USER_NAME_LEN; i++) {
		if(lb[i] == '\0')
			break;
		lb[i] = tolower(lb[i]);
	}
	lb[i] = '\0';
	strcpy(pe.handle, lb);

	if((rc = _ggzdb_player_get(&pe)) != 0) {
		if(rc == GGZDB_ERR_NOTFOUND)
			printf("Handle not found in database\n");
		else
			fprintf(stderr, "_ggzdb_player_get() rc=%d\n", rc);
		return;
	}

	strftime(time_asc, 128, "%a, %d %b %Y %H:%M:%S %Z",
		 gmtime(&pe.last_login));
	printf("\n");
	printf("User entry for %s\n", pe.handle);
	printf("Real name:     [%s]\n", pe.name);
	printf("Email address: [%s]\n", pe.email);
	printf("Password:      [%s]\n", pe.password);
	printf("Permissions:   0x%08X\n", pe.perms);
	printf("Last login:    %s\n\n", time_asc);
}


int main_menu(void)
{
	printf("*** MAIN MENU ***\n");
	printf("1) List all known player handles\n");
	printf("2) Show a single user's information\n");
	printf("Q) Quit\n");
	while(1) {
		printf("===> ");
		getline();
		switch((int)lb[0]) {
			case 'Q':
			case 'q':	return 0;
					break;
			case '1':	list_players();
					return 1;
					break;
			case '2':	show_player();
					return 1;
					break;
			default:	printf("Huh?\007 ");
					break;
		}
	}
	/* Not reached */
	return 1;
}


int main(int argc, char **argv)
{
	int	rc;

	if(argc > 2) {
		fprintf(stderr, "usage: ggzduedit [datadir]\n");
		return 1;
	}
	if(argc == 2)
		datadir = argv[1];
	printf("GGZD user editor, version %s\n", VERSION);
	printf("Using '%s' for data directory\n", datadir);
	if((rc = _ggzdb_init(datadir, 1)) != 0) {
		fprintf(stderr, "_ggzdb_init() rc=%d\n", rc);
		return 1;
	}
	if((rc = _ggzdb_init_player(datadir)) != 0) {
		fprintf(stderr, "_ggzdb_init_player() rc=%d\n", rc);
		return 1;
	}

	while(main_menu())
		;

	_ggzdb_close();
	return 0;
}



/* err_sys functionalities */
void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	putc('\n', stderr);
	va_end(ap);
}

void err_sys_exit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	putc('\n', stderr);
	va_end(ap);

	exit(1);
}
