/*
 * File: ggzduedit.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 09/24/01
 * Desc: User database editor for ggzd server
 * $Id: ggzduedit.c 5383 2003-02-04 17:09:50Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <termios.h>

#include "ggzdb.h"
#include "ggzdb_proto.h"
#define PERMS_DEFINE_STR_TABLE
#include "perms.h"


char lb[1024];
ggzdbPlayerEntry pe;


static void getnextline(void);
static void list_players(void);
static void show_perms(unsigned int perms, int add_spaces);
static void add_player(void);
static void edit_player(int edit);
static int main_menu(void);

#if 0 /* Not used */
static void err_sys(const char *fmt, ...);
static void err_sys_exit(const char *fmt, ...);
#endif


static void getnextline(void)
{
	char *p;

	fgets(lb, 1024, stdin);
	p = lb + strlen(lb) - 1;
	*p = '\0';
}


static void list_players(void)
{
	int rc;
	int count=0;

	printf("\nList of all known player handles:\n");
	if((rc = _ggzdb_player_get_first(&pe)) != 0) {
		fprintf(stderr, "_ggzdb_player_get_first() rc=%d\n", rc);
		return;
	}
	printf("%-8u %s\n", pe.user_id, pe.handle);
	while((rc = _ggzdb_player_get_next(&pe)) == 0) {
		if(++count % 23 == 0) {
			printf("[pause]\007 ");
			getnextline();
			if(lb[0] == 'Q' || lb[0] == 'q')
				break;
		}
		printf("%-8u %s\n", pe.user_id, pe.handle);
	}
	if(lb[0] != 'Q' && lb[0] != 'q' && (count % 23) > 12) {
		printf("[pause]\007 ");
		getnextline();
	}

	printf("\n");

	if(rc != GGZDB_ERR_NOTFOUND)
		fprintf(stderr, "_ggzdb_player_get_next() rc=%d\n", rc);

	_ggzdb_player_drop_cursor();
}


static void show_perms(unsigned int perms, int add_spaces)
{
	int i;
	unsigned int testbit=1;
	int first=1;
	int col=1;

	for(i=0; i<32; i++) {
		if((perms & testbit) == testbit) {
			if(first) {
				printf("%s", perms_str_table[i]);
				first = 0;
			} else if(col) {
				printf("      %s\n", perms_str_table[i]);
				col = 0;
			} else {
				if(add_spaces)
					printf("%*c", add_spaces, ' ');
				printf("               %s", perms_str_table[i]);
				col = 1;
			}
		}
		testbit <<= 1;
	}

	if(first)
		printf("no permissions set\n");
	else if(col)
		printf("\n");
}


static void add_player(void)
{
	int i, rc;
	char time_asc[128];

	printf("Adding new user\n");
	printf("UserID Number: ");
	getnextline();
	pe.user_id = strtoul(lb, NULL, 10);

	printf("User handle:   ");
	getnextline();
	for(i=0; i<MAX_USER_NAME_LEN; i++) {
		if(lb[i] == '\0')
			break;
		lb[i] = tolower(lb[i]);
	}
	lb[i] = '\0';
	strcpy(pe.handle, lb);

	printf("Real name:     ");
	getnextline();
	strncpy(pe.name, lb, 32);
	pe.name[32] = '\0';

	printf("Email address: ");
	getnextline();
	strncpy(pe.email, lb, 32);
	pe.email[32] = '\0';

	printf("Password:      ");
	getnextline();
	strncpy(pe.password, lb, 16);
	pe.password[16] = '\0';

	printf("Permissions:   0x");
	getnextline();
	sscanf(lb, "%x", &pe.perms);

	pe.last_login = 0;
	strftime(time_asc, 128, "%a, %d %b %Y %H:%M:%S %Z",
		 gmtime(&pe.last_login));
	printf("\nAdding entry for %s\n", pe.handle);
	printf("Real name:     [%s]\n", pe.name);
	printf("Email address: [%s]\n", pe.email);
	printf("Password:      [%s]\n", pe.password);
	printf("Permissions:   ");
	show_perms(pe.perms, 0);
	printf("Last login:    %s\n", time_asc);

	lb[0] = '\0';
	while(lb[0] != 'A' && lb[0] != 'a') {
		printf("A)ccept or C)ancel: ");
		getnextline();
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


static void edit_player(int edit)
{
	int i, rc;
	char time_asc[128];
	int accept=0;

	printf("Player handle: ");
	getnextline();
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
		printf("UserID number: %u\n", pe.user_id);
		printf("Real name:     [%s]\n", pe.name);
		printf("Email address: [%s]\n", pe.email);
		printf("Password:      [%s]\n", pe.password);
		printf("Permissions:   ");
		show_perms(pe.perms, 0);
		printf("Last login:    %s\n\n", time_asc);
		return;
	}

	printf("Editing user entry for %s\n", pe.handle);
	printf("1) UserID number: %u\n", pe.user_id);
	printf("2) Real name:     [%s]\n", pe.name);
	printf("3) Email address: [%s]\n", pe.email);
	printf("4) Password:      [%s]\n", pe.password);
	printf("5) Permissions:   ");
	show_perms(pe.perms, 3);
	printf("   Last login:    %s\n", time_asc);
	printf("   A) Accepts     C) Cancels\n");
	while(!accept) {
		printf("===> ");
		getnextline();
		switch((int)lb[0]) {
			case '1':
				printf("%u >", pe.user_id);
				getnextline();
				if(!lb[0])
					break;
				pe.user_id = strtoul(lb, NULL, 10);
				printf("UserID number: %u\n", pe.user_id);
				break;
			case '2':
				printf("[%s] >", pe.name);
				getnextline();
				if(!lb[0])
					break;
				strncpy(pe.name, lb, 32);
				pe.name[32] = '\0';
				printf("Real name:     [%s]\n", pe.name);
				break;
			case '3':
				printf("[%s] >", pe.email);
				getnextline();
				if(!lb[0])
					break;
				strncpy(pe.email, lb, 32);
				pe.email[32] = '\0';
				printf("Email address: [%s]\n", pe.email);
				break;
			case '4':
				printf("[%s] >", pe.password);
				getnextline();
				if(!lb[0])
					break;
				strncpy(pe.password, lb, 16);
				pe.password[16] = '\0';
				printf("Password:      [%s]\n", pe.password);
				break;
			case '5':
				printf("[0x%08X] >0x", pe.perms);
				getnextline();
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
				printf("1) UserID number: %u\n", pe.user_id);
				printf("2) Real name:     [%s]\n", pe.name);
				printf("3) Email address: [%s]\n", pe.email);
				printf("4) Password:      [%s]\n", pe.password);
				printf("5) Permissions:   ");
				show_perms(pe.perms, 3);
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


static int main_menu(void)
{
	printf("*** MAIN MENU ***\n");
	printf("1) List all known player handles\n");
	printf("2) Add a new user\n");
	printf("3) Show a single user's information\n");
	printf("4) Edit a user's information\n");
	printf("Q) Quit\n");
	while(1) {
		printf("===> ");
		getnextline();
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

/* Password input mode, inspired by psql */
static void echomode(int echo)
{
	static struct termios t_orig;
	struct termios t;
	FILE *termin;

	termin = fopen("/dev/tty", "r");
	if(!termin) termin = stdin;

	if(echo)
	{
		tcsetattr(fileno(termin), TCSAFLUSH, &t_orig);
	}
	else
	{
		tcgetattr(fileno(termin), &t);
		t_orig = t;
		t.c_lflag &= ~ECHO;
		tcsetattr(fileno(termin), TCSAFLUSH, &t);
	}
}

int main(int argc, char **argv)
{
	int	rc;
	ggzdbConnection conn;
	int opt, optindex;
	char password[32];
	struct option options[] = {
		{"datadir", required_argument, 0, 'd'},
		{"host", required_argument, 0, 'H'},
		{"dbname", required_argument, 0, 'D'},
		{"username", required_argument, 0, 'u'},
		{"password", optional_argument, 0, 'p'},
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	conn.datadir = NULL;
	conn.database = NULL;

	while(1)
	{
		opt = getopt_long(argc, argv, "vhd:H:D:u:p::", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'd':
				conn.datadir = optarg;
				break;
			case 'D':
				conn.database = optarg;
				break;
			case 'H':
				conn.host = optarg;
				break;
			case 'u':
				conn.username = optarg;
				break;
			case 'p':
				if(!optarg)
				{
					printf("Password: ");
					fflush(NULL);
					echomode(0);
					fgets(password, sizeof(password), stdin);
					echomode(1);
					optarg = password;
				}
				conn.password = optarg;
				break;
			case 'v':
				printf("%s\n", VERSION);
				exit(0);
				break;
			case 'h':
				printf("GGZD user editor, version %s\n\n", VERSION);
				printf("[-d | --datadir  <datadir> ] Directory for local database\n");
				printf("[-D | --dbname   <dbname>  ] Name of the database\n");
				printf("[-H | --host     <hostname>] Host of the database\n");
				printf("[-u | --username <username>] Database user\n");
				printf("[-p | --password [password]] Database password\n");
				printf("[-h | --help               ] Display this help\n");
				printf("[-v | --version            ] Display version number only\n");
				exit(0);
				break;
			default:
				fprintf(stderr, "Usage: %s [options]\n", argv[0]);
				break;
		}
	}

	if(!conn.datadir) {
		if(!conn.database) {
			conn.datadir = DATADIR;
		}
	}

	if(conn.datadir) {
		printf("Using '%s' for data directory\n", conn.datadir);
	} else {
		printf("Using '%s' for database access\n", conn.database);
	}

	if((rc = _ggzdb_init(conn, 1)) != 0) {
		fprintf(stderr, "_ggzdb_init() rc=%d\n", rc);
		return 1;
	}
	if((rc = _ggzdb_init_player(conn.datadir)) != 0) {
		fprintf(stderr, "_ggzdb_init_player() rc=%d\n", rc);
		return 1;
	}

	while(main_menu())
		;

	_ggzdb_close();
	return 0;
}


#if 0
/* err_sys functionalities */
static void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	putc('\n', stderr);
	va_end(ap);
}

static void err_sys_exit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	putc('\n', stderr);
	va_end(ap);

	exit(1);
}
#endif
