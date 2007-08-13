/*
 * File: ggzduedit.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 09/24/01
 * Desc: User database editor for ggzd server
 * $Id: ggzduedit.c 9246 2007-08-13 07:01:56Z josef $
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

/* FIXME: get this from international-branch once merged */
#define ggz_strncpy(x, y, s) strncpy(x, y, s); x[s] = '\0'

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
#include "datatypes.h" /* For Options type */
#include "perms.h"


/* Server options */
/* This is required by the database functions
 * but isn't actually used here */
Options opt;

static char lb[1024];
static ggzdbPlayerEntry pe;
static int needs_id;


static void getnextline(void);
static void list_players(void);
static void show_perms(unsigned int perms, int add_spaces);
static void add_player(void);
static void edit_player(int edit);
static int main_menu(void);


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


static void show_roles(void)
{
	printf("Available roles:\n");
	printf(" [admin][host][registered][guest][chatbot]\n");
}


static void show_all_perms(void)
{
	GGZPerm p;

	printf("Available permissions:\n");
	for (p = 0; p < GGZ_PERM_COUNT; p++) {
		printf(" [%32s] (bit %i)\n", ggz_perm_to_string(p), p);
	}
}


static void show_perms(GGZPermset perms, int add_spaces)
{
	int first=1;
	int col=1;
	GGZPerm p;

	for (p = 0; p < GGZ_PERM_COUNT; p++) {
		if (ggz_perms_is_set(perms, p)) {
			const char *name = ggz_perm_to_string(p);

			if(first) {
				printf("%24s", name);
				first = 0;
			} else if(col) {
				printf("        %24s\n", name);
				col = 0;
			} else {
				if(add_spaces)
					printf("%*c", add_spaces, ' ');
				printf("               %24s", name);
				col = 1;
			}
		}
	}

	if(first)
		printf("no permissions set\n");
	else if(col)
		printf("\n");

	printf("               -> roles: ");
	if (ggz_perms_is_admin(perms))
		printf("[admin]");
	if (ggz_perms_is_host(perms))
		printf("[host]");
	if (ggz_perms_is_set(perms, GGZ_PERM_ROOMS_LOGIN))
		printf("[registered]");
	if (ggz_perms_is_set(perms, GGZ_PERM_NO_STATS))
		printf("[guest]");
	if (ggz_perms_is_set(perms, GGZ_PERM_CHAT_BOT))
		printf("[chatbot]");
	printf("\n");
}


static void add_player(void)
{
	int i, rc;
	char time_asc[128];

	printf("Adding new user\n");
	if(needs_id) {
		printf("UserID Number: ");
		getnextline();
		pe.user_id = strtoul(lb, NULL, 10);
	} else {
		pe.user_id = -1;
	}

	printf("User handle:   ");
	getnextline();
	/* FIXME: tolower() is evil. Must use unicode canonicalization. */
	for(i=0; i<MAX_USER_NAME_LEN; i++) {
		if(lb[i] == '\0')
			break;
		lb[i] = tolower(lb[i]);
	}
	lb[i] = '\0';
	strcpy(pe.handle, lb);

	printf("Real name:     ");
	getnextline();
	ggz_strncpy(pe.name, lb, 32);

	printf("Email address: ");
	getnextline();
	ggz_strncpy(pe.email, lb, 32);

	printf("Password:      ");
	getnextline();
	ggz_strncpy(pe.password, lb, 16);

	show_roles();
	printf("Roles:         ");
	getnextline();

	pe.perms = 0;
	pe.perms |= 1 << GGZ_PERM_JOIN_TABLE;
	pe.perms |= 1 << GGZ_PERM_LAUNCH_TABLE;
	if(strstr(lb, "registered")) {
		pe.perms |= 1 << GGZ_PERM_ROOMS_LOGIN;
	}
	if(strstr(lb, "chatbot")) {
		pe.perms |= 1 << GGZ_PERM_CHAT_BOT;
	}
	if(strstr(lb, "guest")) {
		pe.perms |= 1 << GGZ_PERM_NO_STATS;
	}
	if(strstr(lb, "host")) {
		pe.perms |= 1 << GGZ_PERM_ROOMS_LOGIN;
		pe.perms |= 1 << GGZ_PERM_EDIT_TABLES;
		pe.perms |= 1 << GGZ_PERM_TABLE_PRIVMSG;
	}
	if(strstr(lb, "admin")) {
		pe.perms |= 1 << GGZ_PERM_ROOMS_LOGIN;
		pe.perms |= 1 << GGZ_PERM_ROOMS_ADMIN;
		pe.perms |= 1 << GGZ_PERM_CHAT_ANNOUNCE;
		pe.perms |= 1 << GGZ_PERM_EDIT_TABLES;
		pe.perms |= 1 << GGZ_PERM_TABLE_PRIVMSG;
	}

	printf("Preselected permissions: [0x%08X]\n", pe.perms);

	show_all_perms();
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
	/* FIXME: tolower() is evil. Must use unicode canonicalization. */
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
				ggz_strncpy(pe.name, lb, 32);
				printf("Real name:     [%s]\n", pe.name);
				break;
			case '3':
				printf("[%s] >", pe.email);
				getnextline();
				if(!lb[0])
					break;
				ggz_strncpy(pe.email, lb, 32);
				printf("Email address: [%s]\n", pe.email);
				break;
			case '4':
				printf("[%s] >", pe.password);
				getnextline();
				if(!lb[0])
					break;
				ggz_strncpy(pe.password, lb, 16);
				printf("Password:      [%s]\n", pe.password);
				break;
			case '5':
				show_all_perms();
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
		{"type", optional_argument, 0, 't'},
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	conn.type = NULL;
	conn.datadir = NULL;
	conn.database = NULL;
	conn.host = NULL;
	conn.username = NULL;
	conn.password = NULL;
	conn.hashing = ggz_strdup("plain");

	while(1)
	{
		opt = getopt_long(argc, argv, "vhd:H:D:u:t:p::", options, &optindex);
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
			case 't':
				conn.type = optarg;
				break;
			case 'p':
				if(!optarg)
				{
					printf("Password: ");
					fflush(NULL);
					echomode(0);
					fgets(password, sizeof(password), stdin);
					echomode(1);
					printf("\n");
					password[strlen(password) - 1] = '\0';
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
				printf("[-p | --password=[password]] Database password\n");
				printf("[-t | --type     <dbtype>  ] Type of database (for DBI only)\n");
				printf("[-h | --help               ] Display this help\n");
				printf("[-v | --version            ] Display version number only\n");
				exit(0);
				break;
			default:
				fprintf(stderr, "Usage: %s [options]\n", argv[0]);
				break;
		}
	}

	if((!conn.database) || (!conn.host) || (!conn.username) || (!conn.password)) {
		rc = ggz_conf_parse(GGZDCONFDIR "/ggzd.conf", GGZ_CONF_RDONLY);
		if(!conn.database)
			conn.database = ggz_conf_read_string(rc, "General", "DatabaseName", NULL);
		if(!conn.host)
			conn.host = ggz_conf_read_string(rc, "General", "DatabaseHost", NULL);
		if(!conn.username)
			conn.username = ggz_conf_read_string(rc, "General", "DatabaseUsername", NULL);
		if(!conn.password)
			conn.password = ggz_conf_read_string(rc, "General", "DatabasePassword", NULL);
		if(!conn.type)
			conn.type = ggz_conf_read_string(rc, "General", "DatabaseType", NULL);
		ggz_conf_close(rc);
	}

	if(!conn.datadir) {
		if(!conn.database) {
			conn.datadir = GGZDSTATEDIR;
		}
	}

	needs_id = 0;

	if((!strcmp(DATABASE_TYPE, "db2"))
	|| (!strcmp(DATABASE_TYPE, "db3"))
	|| (!strcmp(DATABASE_TYPE, "db4"))) {
		needs_id = 1;

		if(!conn.datadir) {
			fprintf(stderr, "Database type '%s' needs data directory\n",
				DATABASE_TYPE);
			exit(-1);
		}
	}

	if(!strcmp(DATABASE_TYPE, "dbi")) {
		if(!conn.type) {
			fprintf(stderr, "Database type '%s' needs plugin type name\n",
				DATABASE_TYPE);
			exit(-1);
		}
	}

	if((!strcmp(DATABASE_TYPE, "mysql"))
	|| (!strcmp(DATABASE_TYPE, "pgsql"))
	|| (!strcmp(DATABASE_TYPE, "sqlite"))
	|| (!strcmp(DATABASE_TYPE, "dbi"))) {
		if((!conn.database) || (!conn.host) || (!conn.username) || (!conn.password)) {
			fprintf(stderr, "Database type '%s' needs database access parameters\n",
				DATABASE_TYPE);
			exit(-1);
		} else {
			conn.datadir = NULL;
		}
	}

	if(conn.datadir) {
		printf("Using data directory '%s'\n", conn.datadir);
	} else {
		printf("Using database '%s' as '%s' on '%s'\n",
			conn.database, conn.username, conn.host);
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

