/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 * $Id: parse_opt.c 10635 2008-12-23 19:24:59Z josef $
 *
 * Copyright (C) 1999-2002 Brent Hendricks.
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
#include <dirent.h>
#include <getopt.h>
#include <ggz_threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "client.h"
#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "motd.h"
#include "parse_conf.h"
#include "parse_opt.h"
#include "perms.h"
#include "players.h"
#include "room.h"
#include "util.h"
#include "unicode.h"


/* Private file parsing functions */
static void parse_game(char *, char *);
static void parse_room(char *, char *, int);
static int parse_gselect(const struct dirent *);
static int parse_rselect(const struct dirent *);

/* Convience macro for parse_file(), parse_game() */
#define PARSE_ERR(s)  ggz_error_msg("Config file: %s, line %d", s, linenum)

static struct option options[] = {
	{"foreground", no_argument, 0, 'F'},
	{"file", required_argument, 0, 'f'},
	{"log", required_argument, 0, 'l'},
	{"port", required_argument, 0, 'p'},
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'v'},
	{"specs", no_argument, 0, 's'},
	{0, 0, 0, 0}
};

/* Convert a string into a list */
/* FIXME: should be made available from ggz_read_list() */
static char **_ggz_string_to_list(const char *s, const char *sep)
{
	char *sc = ggz_strdup(s);
	char **list = NULL;
	int count = 0;
	char *tok;

	tok = strtok(sc, sep);
	while(tok)
	{
		count++;
		list = (char**)ggz_realloc(list, (count + 1) * sizeof(char**));
		list[count - 1] = ggz_strdup(tok);
		list[count] = NULL;
		tok = strtok(NULL, sep);
	}
	ggz_free(sc);

	return list;
}

#ifdef DEBUG
#define SPEC_DEBUG 1
#else
#define SPEC_DEBUG 0
#endif
#ifdef WITH_AVAHI
#define SPEC_AVAHI 1
#else
#define SPEC_AVAHI 0
#endif
#ifdef HAVE_INOTIFY
#define SPEC_INOTIFY 1
#else
#define SPEC_INOTIFY 0
#endif
#ifdef WITH_FAM
#define SPEC_FAM 1
#else
#define SPEC_FAM 0
#endif
#ifdef WITH_ICU
#define SPEC_ICU 1
#else
#define SPEC_ICU 0
#endif
#ifdef WITH_CRACKLIB
#define SPEC_CRACKLIB 1
#else
#define SPEC_CRACKLIB 0
#endif
#ifdef WITH_OMNICRACKLIB
#define SPEC_OMNICRACKLIB 1
#else
#define SPEC_OMNICRACKLIB 0
#endif

static void dump_specs(void)
{
	int tls = ggz_tls_support_query();
	const char *tlsname = ggz_tls_support_name();
	char tlsstring[32], dbstring[32];

	parse_conf_file();

	if(tls)
	{
		snprintf(tlsstring, sizeof(tlsstring), "yes (%s)", tlsname);
	}
	if(opt.db.type)	{
		snprintf(dbstring, sizeof(dbstring), " [using %s::%s]", opt.db.type, opt.db.option);
	} else {
		snprintf(dbstring, sizeof(dbstring), " [autoselect %s]", ggzdb_get_default_backend());
	}

	printf("GGZ Gaming Zone server (ggzd) specifications\n");
	printf("Version: %s\n", VERSION);
	printf("Protocol version: %i\n", GGZ_CS_PROTO_VERSION);
	printf("Configuration: %s/ggzd.conf [%s]\n",
		GGZDCONFDIR,
		(opt.conf_valid ? "ok" : "not ok"));
	printf("Debugging: %s\n", (SPEC_DEBUG ? "yes" : "no"));
	printf("Database backend(s): %s\n", dbstring);
	printf("Zeroconf support: %s [%s]\n",
		(SPEC_AVAHI ? "yes (avahi)" : "no"),
		(opt.conf_valid ? (opt.announce_lan ? "used" : "not used") : "unknown"));
	printf("TLS support: %s [%s]\n",
		(tls ? tlsstring : "no"),
		(opt.conf_valid ? (opt.tls_use ? "used" : "not used") : "unknown"));
	printf("Reconfiguration support: %s [%s]\n",
		(SPEC_INOTIFY ? "yes (inotify)" : (SPEC_FAM ? "yes (fam)" : "no")),
		(opt.conf_valid ? (opt.reconfigure_rooms ? "used" : "not used") : "unknown" ));
	printf("Unicode support: %s\n", (SPEC_ICU ? "yes" : "no"));
	printf("Password checking support: %s\n",
		(SPEC_CRACKLIB ? "yes (cracklib)" : (SPEC_OMNICRACKLIB ? "yes (omnicracklib)" : "no")));
}

/* Parse command-line options */
void parse_args(int argc, char *argv[])
{
	int sopt, optindex;
	int t_count;
	char **t_list;

	while(1)
	{
		sopt = getopt_long(argc, argv, "vhsFf:l:p:", options, &optindex);
		if(sopt == -1) break;
		switch(sopt)
		{
			case 'F':
				opt.foreground = 1;
				break;
			case 'f':
				opt.local_conf = optarg;
				break;
			case 'l':
				t_list = _ggz_string_to_list(optarg, ",");
				t_count = 0;
				while((t_list) && (t_list[t_count]))
					t_count++;
				log_info.log_types_console = parse_log_types(t_count, t_list);
				break;
			case 'p':
				opt.main_port = atoi(optarg);
				break;
			case 'h':
	 			printf("GGZD - The main server of the GGZ Gaming Zone\n"),
	 			printf("Copyright (C) 1999 - 2002 Brent Hendricks\n"),
	 			printf("Copyright (C) 2003 - 2007 The GGZ Gaming Zone developers\n"),
	 			printf("\n"),
	 			printf("[-F | --foreground ] Tells ggzd to run in the foreground\n"),
				printf("[-f | --file <file>] Configuration file\n"),
				printf("[-l | --log <types>] Types of logging to the console to perform\n"),
				printf("[-p | --port <port>] GGZ port number\n"),
				printf("[-s | --specs      ] Display compilation specifications\n"),
				printf("[-v | --version    ] Display version number\n"),
				printf("[-h | --help       ] Display this help screen\n"),
				exit(0);
				break;
			case 'v':
				printf("%s\n", VERSION);
				exit(0);
				break;
			case 's':
				dump_specs();
				exit(0);
				break;
			default:
				exit(0);
				break;
		}
	}
}


/* Main entry point for parsing the game files */
void parse_game_files(void)
{
	struct dirent **namelist;
	const char *suffix = "/games/";
	char dir[strlen(opt.conf_dir) + strlen(suffix) + 1];
	int num_games, i, j;
	int addit;

	/* Setup our directory to "conf_dir/games/" */
	snprintf(dir, sizeof(dir), "%s%s", opt.conf_dir, suffix);

	if(g_count > 0) {
		/* Go through all games explicitly included in the add list */
		ggz_debug(GGZ_DBG_CONFIGURATION, "Adding games in add list");
		for(i=0; i<g_count; i++)
			parse_game(g_list[i], dir);
	} else {
		/* Scan for all .dsc files in the dir */
		ggz_debug(GGZ_DBG_CONFIGURATION, "Adding all games in %s", dir);
		num_games = scandir(dir, &namelist, parse_gselect, 0);
		for(i=0; i<num_games; i++) {
			/* Make a temporary copy of the name w/o .dsc */
			char name[strlen(namelist[i]->d_name) + 1];

			strcpy(name, namelist[i]->d_name);
			name[strlen(name)-4] = '\0';

			/* Check to see if this game is on the ignore list */
			addit=1;
			for(j=0; j>g_count; j--) {
				if(!strcmp(name, g_list[0-j]))
					addit = 0;
					break;
			}

			/* Add it if it's not on the ignore list */
			if(addit)
				parse_game(name, dir);
			else
				ggz_debug(GGZ_DBG_CONFIGURATION,
					"Ignoring game %s", name);

			free(namelist[i]);
		}
		if (num_games >= 0) {
			free(namelist);
		}
	}

	/* Cleanup the g_list */
	if(g_count < 0)
		g_count = -g_count;
	if(g_count > 0) {
		for(i=0; i<g_count; i++)
			ggz_free(g_list[i]);
		ggz_free(g_list);
		g_list = NULL;
		g_count = 0;
	}

	ggz_conf_cleanup();
}


/* Parse a single game file, adding it's values to the game table */
static void parse_game(char *name, char *dir)
{
	char fname[strlen(name) + strlen(dir) + 6];
	int ch;
	GameInfo game_info;
	int len, num_args;
	char *tmp;
	ggz_intlstring *intltmp;
	int argcp;
	char **argvp;
	int ret, i;
	ggzdbPlayerEntry db_pe;
	GGZDBResult db_status;

	/* Check to see if we are allocating too many games */
	if(state.types == MAX_GAME_TYPES) {
		ggz_error_msg("Ignoring game %s, already at MAX_GAME_TYPES", name);
		return;
	}

	/* Setup a full pathname to description file */
	snprintf(fname, sizeof(fname), "%s/%s.dsc", dir, name);

	if((ch = ggz_conf_parse(fname, GGZ_CONF_RDONLY)) < 0) {
		ggz_error_msg("Ignoring %s, could not open %s", name, fname);
		return;
	}

	ggz_debug(GGZ_DBG_CONFIGURATION, "Adding game %s from %s", name, fname);

	/* Initialize the basic data for the game info struct. */
	memset(&game_info, 0, sizeof(GameInfo));
	pthread_rwlock_init(&game_info.lock, NULL);	

	game_info.game = ggz_strdup(name);

	/* [GameInfo] */
	intltmp = ggz_conf_read_intlstring(ch, "GameInfo", "Name");
	tmp = ggz_intlstring_translated(intltmp, NULL);
	if (!tmp) {
		ggz_error_msg("Game name missing for '%s'.", name);
		ggz_intlstring_free(intltmp);
		return;
	}
	if (strlen(tmp) > MAX_GAME_NAME_LEN) {
		ggz_error_msg("Game name '%s' too long: max length %d.",
			tmp, MAX_GAME_NAME_LEN);
		ggz_intlstring_free(intltmp);
		return;
	}
	game_info.name = intltmp;
	game_info.version = ggz_conf_read_string(ch, "GameInfo",
						 "Version", "");
	game_info.desc = ggz_conf_read_intlstring(ch, "GameInfo",
					      "Description");
	game_info.author = ggz_conf_read_string(ch, "GameInfo",
						"Author", "");
	game_info.homepage = ggz_conf_read_string(ch, "GameInfo",
						  "Homepage", "");

	/* [LaunchInfo] */
	ggz_conf_read_list(ch, "LaunchInfo", "ExecutablePath",
			   &num_args, &game_info.exec_args);
	if (!game_info.exec_args || !game_info.exec_args[0]) {
		ggz_error_msg("Missing ExecutablePath for game %s.",
			ggz_intlstring_translated(game_info.name, NULL));
		/* This leaves some memory leak, but it's acceptable under
		   the circumstances. */
		return;
	}
	/* If there's no absolute path given, we prepend the game_exec_dir. */
	if (game_info.exec_args[0][0] != '/') {
		char *new_exec;
		len = strlen(game_info.exec_args[0])
		  + strlen(opt.game_exec_dir) + 2;
		new_exec = ggz_malloc(len);
		snprintf(new_exec, len, "%s/%s",
			 opt.game_exec_dir, game_info.exec_args[0]);
		ggz_free(game_info.exec_args[0]);
		game_info.exec_args[0] = new_exec;
	}

	/* [Protocol] */
	game_info.p_engine = ggz_conf_read_string(ch, "Protocol",
						  "Engine", "");
	game_info.p_version = ggz_conf_read_string(ch, "Protocol",
						   "Version", "");

	/* [TableOptions] */
	game_info.allow_leave = ggz_conf_read_int(ch, "TableOptions", 
						  "AllowLeave", 0);
	game_info.kill_when_empty = ggz_conf_read_int(ch, "TableOptions",
						      "KillWhenEmpty", 1);

	game_info.stats_records = ggz_conf_read_int(ch, "Statistics",
						    "Records", 0);
	game_info.stats_ratings = ggz_conf_read_int(ch, "Statistics",
						    "Ratings", 0);
	game_info.stats_highscores = ggz_conf_read_int(ch, "Statistics",
						       "Highscores", 0);

	tmp = ggz_conf_read_string(ch, "TableOptions", "BotsAllowed", "");
	game_info.bot_allow_list = ggz_numberlist_read(tmp);
	ggz_free(tmp);

	tmp = ggz_conf_read_string(ch, "TableOptions", "PlayersAllowed", "1");
	game_info.player_allow_list = ggz_numberlist_read(tmp);
	ggz_free(tmp);

	game_info.allow_spectators = ggz_conf_read_int(ch, "TableOptions",
						       "AllowSpectators", 0);

	game_info.allow_peers = ggz_conf_read_int(ch, "TableOptions", 
						  "AllowPeers", 0);

	game_info.restore_allow = ggz_conf_read_int(ch, "TableOptions",
						    "AllowRestore", 0);

	/* Named bots */
	game_info.named_bots = NULL;

	ret = ggz_conf_get_keys(ch, "NamedBots", &argcp, &argvp);
	if(ret == 0) {
		game_info.named_bots = (char***)ggz_malloc((argcp + 1) * sizeof(char**));
		for (i = 0; i < argcp; i++) {
			ggz_debug(GGZ_DBG_CONFIGURATION,
				"Found named bot <%s>.",
				argvp[i]);
			tmp = ggz_conf_read_string(ch, "NamedBots", argvp[i], NULL);
			game_info.named_bots[i] = (char**)ggz_malloc(2 * sizeof(char*));
			game_info.named_bots[i][0] = ggz_strdup(argvp[i]);
			game_info.named_bots[i][1] = ggz_strdup(tmp);
			ggz_free(tmp);
			ggz_free(argvp[i]);
		}
		game_info.named_bots[argcp] = NULL;
		ggz_free(argvp);
	}

	if (game_info.named_bots) {
		for (i = 0; game_info.named_bots[i]; i++) {
			snprintf(db_pe.handle, sizeof(db_pe.handle), "%s",
				game_info.named_bots[i][0]);
			db_status = ggzdb_player_get(&db_pe);
			if ((db_status != GGZDB_ERR_NOTFOUND)
			    && (db_pe.perms & (1 << GGZ_PERM_CHAT_BOT))) {
				ggz_debug(GGZ_DBG_CONFIGURATION,
					"named_bot <%s> registered already.",
					game_info.named_bots[i][0]);
				continue;
			}

			db_pe.user_id = ggzdb_player_next_uid();
			ggz_perms_init_from_list(&db_pe.perms,
					     perms_default, num_perms_default);
			db_pe.perms |= (1 << GGZ_PERM_CHAT_BOT);
			db_pe.last_login = time(NULL);
			db_pe.confirmed = 0;
			snprintf(db_pe.handle, sizeof(db_pe.handle), "%s",
				game_info.named_bots[i][0]);
			ggz_strncpy(db_pe.password, "", sizeof(db_pe.password));
			ggz_strncpy(db_pe.email, "", sizeof(db_pe.email));
			ggz_strncpy(db_pe.name, "", sizeof(db_pe.name));

			if (ggzdb_player_add(&db_pe) == GGZDB_ERR_DUPKEY) {
				ggz_error_msg("Could not register named bot <%s>.",
					game_info.named_bots[i][0]);
			} else {
				log_msg(GGZ_LOG_SECURITY, "NEWACCT (%u) (named bot) for %s",
					db_pe.user_id, game_info.named_bots[i][0]);
			}
		}
	}

	/* Set up data_dir. */
	len = strlen(opt.data_dir) + strlen("/gamedata/")
              + strlen(ggz_intlstring_translated(game_info.name, NULL)) + 1;
	game_info.data_dir = ggz_malloc(len);
	snprintf(game_info.data_dir, len, "%s/gamedata/%s",
	         opt.data_dir, ggz_intlstring_translated(game_info.name, NULL));
	check_path(game_info.data_dir);

	game_types[state.types] = game_info;
	state.types++;
}


/* Main entry point for parsing the room files */
void parse_room_files(void)
{
	struct dirent **namelist;
	const char *suffix = "/rooms/";
	char dir[strlen(opt.conf_dir) + strlen(suffix) + 1];
	int num_rooms, i, j;
	int addit;

	/* Setup our directory to "conf_dir/rooms/" */
	snprintf(dir, sizeof(dir), "%s%s", opt.conf_dir, suffix);

	parse_room("entry", dir, 0);

	if(r_count > 0) {
		/* Go through all rooms explicitly included in the add list */
		ggz_debug(GGZ_DBG_CONFIGURATION, "Adding rooms in add list");
		for(i=0; i<r_count; i++)
			/* Add everything, but don't readd entry room */
			if(strcmp(r_list[i], "entry"))
				parse_room(r_list[i], dir, 0);
	} else {
		/* Scan for all .room files in dir */
		ggz_debug(GGZ_DBG_CONFIGURATION, "Adding all rooms in %s", dir);
		num_rooms = scandir(dir, &namelist, parse_rselect, 0);
		for(i=0; i<num_rooms; i++) {
			/* Make a temporary copy of the name w/o .room */
			char name[strlen(namelist[i]->d_name) + 1];
			strcpy(name, namelist[i]->d_name);
			name[strlen(name)-5] = '\0';

			/* Don't readd the entry room */
			if(!strcmp(name, "entry")){
				free(namelist[i]);
				continue;
			}
			/* Check to see if this game is on the ignore list */
			addit=1;
			for(j=0; j>r_count; j--) {
				if(!strcmp(name, r_list[0-j]))
					addit = 0;
					break;
			}

			/* Add it if it's not on the ignore list */
			if(addit)
				parse_room(name, dir, 0);
			else
				ggz_debug(GGZ_DBG_CONFIGURATION,
					"Ignoring room %s", name);

			free(namelist[i]);
		}
		if (num_rooms >= 0) {
			free(namelist);
		}
	}

	/* At this point, we should have at least one working room */
	if(room_info.num_rooms == 0)
		ggz_error_msg_exit("No rooms defined, ggzd unusable");

	/* Enter rooms into the room template table in the database */
	ggz_debug(GGZ_DBG_CONFIGURATION, "Adding rooms to database.");
	ggzdb_rooms(rooms, room_info.num_rooms);

	/* Cleanup the r_list */
	if(r_count < 0)
		r_count = -r_count;
	if(r_count > 0) {
		for(i=0; i<r_count; i++)
			ggz_free(r_list[i]);
		ggz_free(r_list);
		r_list = NULL;
		r_count = 0;
	}
}


/* FIXME: combine some aspects with parse_room(), i.e. let it use this function */
static void parse_room_struct(RoomStruct room)
{
	int num;

	/* Allocate a room struct for this room */
	if(room_info.num_rooms == 0)
		room_initialize();
	else
		room_create_additional();
	num = room_info.num_rooms - 1;

	rooms[num].room = room.room;
	rooms[num].name = room.name;
	rooms[num].description = room.description;
	rooms[num].max_players = room.max_players;
	rooms[num].max_tables = room.max_tables;
	rooms[num].game_type = room.game_type;
	rooms[num].perms = room.perms;

	rooms[num].players = ggz_malloc(rooms[num].max_players * sizeof(GGZPlayer*));
	if (rooms[num].max_tables > 0)
		rooms[num].tables = ggz_malloc(rooms[num].max_tables * sizeof(GGZTable*));
	else
		rooms[num].tables = NULL;
	rooms[num].exec_args = NULL;
}


/* Parse a single room file, adding it's values to the room table */
static void parse_room(char *name, char *dir, int announce)
{
	char fname[strlen(name) + strlen(dir) + 7];
	char *strval;
	int ch, num;

	/* Allocate space and setup a full pathname to description file */
	snprintf(fname, sizeof(fname), "%s/%s.room", dir, name);

	if((ch = ggz_conf_parse(fname, GGZ_CONF_RDONLY)) < 0) {
		ggz_error_msg("Ignoring %s, could not open %s", name, fname);
		return;
	}

	ggz_debug(GGZ_DBG_CONFIGURATION, "Adding room %s from %s", name, fname);

	/* Allocate a room struct for this room */
	if(room_info.num_rooms == 0)
		room_initialize();
	else
		room_create_additional();
	num = room_info.num_rooms - 1;
	rooms[num].game_type = -2;
	rooms[num].perms = 0;
	rooms[num].room = ggz_strdup(name);

	/* [RoomInfo] */
	rooms[num].name = ggz_conf_read_intlstring(ch, "RoomInfo", "Name");
	rooms[num].description = ggz_conf_read_intlstring(ch, "RoomInfo",
						      "Description");
	rooms[num].max_players = ggz_conf_read_int(ch, "RoomInfo",
						   "MaxPlayers", 0);
	rooms[num].max_tables = ggz_conf_read_int(ch, "RoomInfo",
						  "MaxTables", -1);
	strval = ggz_conf_read_string(ch, "RoomInfo", "GameType", NULL);
	if(strval) {
		unsigned int i;
		for(i=0; i<state.types; i++)
			if(!strcmp(strval, ggz_intlstring_translated(game_types[i].name, NULL)))
				break;
		if(i != state.types)
			rooms[num].game_type = i;
		else if(!strcasecmp(strval, "none"))
			rooms[num].game_type = -1;
		else
			ggz_error_msg("Invalid GameType specified in room %s", name);
		ggz_free(strval);
	}
	strval = ggz_conf_read_string(ch, "RoomInfo",
				      "EntryRestriction", NULL);
	if(strval) {
		if(!strcasecmp(strval, "Admin"))
			rooms[num].perms = 1 << GGZ_PERM_ROOMS_ADMIN;
		else if(!strcasecmp(strval, "Registered"))
			rooms[num].perms = 1 << GGZ_PERM_ROOMS_LOGIN;
		else if(!strcasecmp(strval, "None"))
			rooms[num].perms = 0;
		else
			ggz_error_msg("Invalid EntryRestriction in room %s", name);
		ggz_free(strval);
	}

	if(rooms[num].name == NULL) {
		ggz_error_msg("No Name given for room %s", name);
		rooms[num].name = ggz_intlstring_fromstring("none");
	}
	if(rooms[num].description == NULL) {
		ggz_error_msg("No Description given for room %s", name);
		rooms[num].description = ggz_intlstring_fromstring("none");
	}
	if(rooms[num].max_players <= 0) {
		if(rooms[num].max_players < 0)
			ggz_error_msg("Invalid MaxPlayers given for room %s", name);
		rooms[num].max_players = DEFAULT_MAX_ROOM_USERS;
	}
	if(rooms[num].max_tables < 0) {
		if(rooms[num].max_tables < -1)
			ggz_error_msg("Invalid MaxTables given for room %s", name);
		rooms[num].max_tables = DEFAULT_MAX_ROOM_TABLES;
	}
	if(rooms[num].game_type == -2) {
		ggz_error_msg("No GameType given for room %s", name);
		rooms[num].game_type = -1;
	}

	if (rooms[num].game_type != -1) {
		char **exec_args;
		int num_args, i, j;
		GameInfo *gtype = &game_types[rooms[num].game_type];

		ggz_conf_read_list(ch, "RoomInfo", "ExecutableArgs",
				   &num_args, &exec_args);
		for (i = 0; gtype->exec_args[i]; i++) {

		}
		rooms[num].exec_args
		  = ggz_malloc((num_args + i + 1)
			       * sizeof(*rooms[num].exec_args));
		for (i = 0; gtype->exec_args[i]; i++) {
			rooms[num].exec_args[i]
			  = ggz_strdup(gtype->exec_args[i]);
		}
		for (j = 0; j < num_args; j++) {
		  rooms[num].exec_args[i + j] = exec_args ? exec_args[j] : NULL;
		}
		rooms[num].exec_args[i + j] = NULL;
#if 0
		printf("%s : %d args\n", rooms[num].name, i + j);
		for (i = 0; rooms[num].exec_args[i]; i++) {
		  printf("  %s\n", rooms[num].exec_args[i]);
		}
#endif
		if (exec_args) ggz_free(exec_args);
	} else {
		rooms[num].exec_args = NULL;
	}

	rooms[num].players = ggz_malloc(rooms[num].max_players
					* sizeof(GGZPlayer*));
	if (rooms[num].max_tables > 0)
		rooms[num].tables = ggz_malloc(rooms[num].max_tables
					       * sizeof(GGZTable*));
	else
		rooms[num].tables = NULL;

	if (announce) {
		room_add(num);
	}
}


void parse_room_change(const char *room, int addition)
{
	const char *suffix = "/rooms/";
	char dir[strlen(opt.conf_dir) + strlen(suffix) + 1];
	char roomname[strlen(room) + 1];
	int i;

	/* Setup our directory to "conf_dir/rooms/" */
	snprintf(dir, sizeof(dir), "%s%s", opt.conf_dir, suffix);

	snprintf(roomname, sizeof(roomname), "%s", room);
	roomname[strlen(roomname) - 5] = '\0';

	for(i = 1; i < room_info.num_rooms; i++)
	{
		if(!strcmp(rooms[i].room, roomname))
		{
			if(rooms[i].removal_done)
			{
				/* 'k, we can add it then? */
				/* this code should never be reached, since removal_done rooms */
				/* have lost their .room association */
			}
			else if(rooms[i].removal_pending)
			{
				/* FIXME: FAM does indeed send removal requests twice */
				/* so we must discard additions during this time, too */

				ggz_debug(GGZ_DBG_CONFIGURATION, "Room %s subject of removal", roomname);
				return;
			}
			else
			{
				ggz_debug(GGZ_DBG_CONFIGURATION, "Removing room %s", roomname);
				room_remove(i);
				if(!addition)
					return;
			}
		}
	}

	parse_room(roomname, dir, 1);
}


void parse_room_change_db(RoomStruct *dbrooms)
{
	int i, j;

	for(j = 0; dbrooms[j].name; j++)
	{
		RoomStruct room = dbrooms[j];

		for(i = 1; i < room_info.num_rooms; i++)
		{
			if(!strcmp(rooms[i].room, room.room))
			{
				if((rooms[i].removal_done) || (rooms[i].removal_pending))
				{
					/* shouldn't be reached - see parse_room_change() */
					ggz_debug(GGZ_DBG_CONFIGURATION, "Ignoring room %s double removal", room.room);
					return;
				}
				else
				{
					ggz_debug(GGZ_DBG_CONFIGURATION, "Removing room %s", room.room);
					room_remove(i);
					return;
				}
			}
		}

		parse_room_struct(room);
	}

	ggz_free(dbrooms);
}


/* Return 1 if filename matches our pattern (ends in '.dsc') */
static int parse_gselect(const struct dirent *dent)
{
	/* Note: be careful to avoid underflow when the name is too short. */
	int len = strlen(dent->d_name);
	return len > 4 && !strcmp(".dsc", dent->d_name + len - 4);
}


/* Return 1 if filename matches our pattern (ends in '.room') */
static int parse_rselect(const struct dirent *dent)
{
	/* Note: be careful to avoid underflow when the name is too short. */
	int len = strlen(dent->d_name);
	return len > 5 && !strcmp(".room", dent->d_name + len - 5);
}
