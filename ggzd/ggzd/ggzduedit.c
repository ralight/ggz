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


void add_player(void)
{
	int i, rc;
	char time_asc[128];

	printf("Adding new user\n");
	printf("User handle:   ");
	getline();
	for(i=0; i<MAX_USER_NAME_LEN; i++) {
		if(lb[i] == '\0')
			break;
		lb[i] = tolower(lb[i]);
	}
	lb[i] = '\0';
	strcpy(pe.handle, lb);

	printf("Real name:     ");
	getline();
	strncpy(pe.name, lb, 32);
	pe.name[32] = '\0';

	printf("Email address: ");
	getline();
	strncpy(pe.email, lb, 32);
	pe.email[32] = '\0';

	printf("Password:      ");
	getline();
	strncpy(pe.password, lb, 16);
	pe.password[16] = '\0';

	printf("Permissions:   0x");
	getline();
	sscanf(lb, "%x", &pe.perms);

	pe.last_login = 0;
	strftime(time_asc, 128, "%a, %d %b %Y %H:%M:%S %Z",
		 gmtime(&pe.last_login));
	printf("\nAdding entry for %s\n", pe.handle);
	printf("Real name:     [%s]\n", pe.name);
	printf("Email address: [%s]\n", pe.email);
	printf("Password:      [%s]\n", pe.password);
	printf("Permissions:   0x%08X\n", pe.perms);
	printf("Last login:    %s\n", time_asc);

	lb[0] = '\0';
	while(lb[0] != 'A' && lb[0] != 'a') {
		printf("A)ccept or C)ancel: ");
		getline();
		if(lb[0] == 'C' || lb[0] == 'c') {
			printf("\nNew user entry canceled\n");
			return;
		}
	}

	if((rc = _ggzdb_player_add(&pe)) != 0)
		if(rc == GGZDB_ERR_DUPKEY)
			printf("\nError: user handle already exists\n\n");
		else
			fprintf(stderr, "_ggzdb_player_add() rc=%d\n", rc);
	else
		printf("\nNew user entry added\n\n");
}


void edit_player(int edit)
{
	int i, rc;
	char time_asc[128];
	int accept=0;

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
	if(!edit) {
		printf("User entry for %s\n", pe.handle);
		printf("Real name:     [%s]\n", pe.name);
		printf("Email address: [%s]\n", pe.email);
		printf("Password:      [%s]\n", pe.password);
		printf("Permissions:   0x%08X\n", pe.perms);
		printf("Last login:    %s\n\n", time_asc);
		return;
	}

	printf("Editing user entry for %s\n", pe.handle);
	printf("1) Real name:     [%s]\n", pe.name);
	printf("2) Email address: [%s]\n", pe.email);
	printf("3) Password:      [%s]\n", pe.password);
	printf("4) Permissions:   0x%08X\n", pe.perms);
	printf("   Last login:    %s\n", time_asc);
	printf("   A) Accepts     C) Cancels\n");
	while(!accept) {
		printf("===> ");
		getline();
		switch((int)lb[0]) {
			case '1':
				printf("[%s] >", pe.name);
				getline();
				if(!lb[0])
					break;
				strncpy(pe.name, lb, 32);
				pe.name[32] = '\0';
				printf("Real name:     [%s]\n", pe.name);
				break;
			case '2':
				printf("[%s] >", pe.email);
				getline();
				if(!lb[0])
					break;
				strncpy(pe.email, lb, 32);
				pe.email[32] = '\0';
				printf("Email address: [%s]\n", pe.email);
				break;
			case '3':
				printf("[%s] >", pe.password);
				getline();
				if(!lb[0])
					break;
				strncpy(pe.password, lb, 16);
				pe.password[16] = '\0';
				printf("Password:      [%s]\n", pe.password);
				break;
			case '4':
				printf("[0x%08X] >0x", pe.perms);
				getline();
				if(!lb[0])
					break;
				sscanf(lb, "%x", &pe.perms);
				printf("Permissions:   0x%08X\n", pe.perms);
				break;
			case 'A':
			case 'a':
				accept = 1;
				break;
			case 'C':
			case 'c':
				printf("\nUser entry unchanged\n\n");
				return;
				break;
			default:
				printf("Editing user entry for %s\n",pe.handle);
				printf("1) Real name:     [%s]\n", pe.name);
				printf("2) Email address: [%s]\n", pe.email);
				printf("3) Password:      [%s]\n", pe.password);
				printf("4) Permissions:   0x%08X\n", pe.perms);
				printf("   Last login:    %s\n", time_asc);
				printf("   A) Accepts     C) Cancels\n");
				printf("Huh?\007 ");
				break;
		}
	}

	if((rc = _ggzdb_player_update(&pe)) != 0)
		fprintf(stderr, "_ggzdb_player_update() rc=%d\n", rc);
	else
		printf("\nUser entry updated\n\n");
}


int main_menu(void)
{
	printf("*** MAIN MENU ***\n");
	printf("1) List all known player handles\n");
	printf("2) Add a new user\n");
	printf("3) Show a single user's information\n");
	printf("4) Edit a user's information\n");
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
			case '2':	add_player();
					return 1;
					break;
			case '3':	edit_player(0);
					return 1;
					break;
			case '4':	edit_player(1);
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
