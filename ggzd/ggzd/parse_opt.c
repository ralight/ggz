/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 * $Id: parse_opt.c 3072 2002-01-12 02:18:47Z jdorje $
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

#include <config.h>

#include <pthread.h>
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include <ggzd.h>
#include <ggz.h>
#include <datatypes.h>
#include <err_func.h>
#include <motd.h>
#include <room.h>
#include <perms.h>

/* Stuff from control.c we need access to */
extern Options opt;
extern struct GGZState state;
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Private file parsing functions */
static void get_config_options(int);
static void parse_game(char *, char *);
static void parse_room(char *, char *);
static int parse_gselect(const struct dirent *);
static int parse_rselect(const struct dirent *);
static unsigned parse_log_types(int, char **);
static unsigned parse_dbg_types(int, char **);

/* Log types name lookup tables*/
struct LogTypes {
	char *name;
	unsigned type;
};
static const struct LogTypes log_types[] = {
	{ "all",		GGZ_LOG_ALL },
	{ "notices",		GGZ_LOG_NOTICE },
	{ "connections",	GGZ_LOG_CONNECTION_INFO },
	{ "security",		GGZ_LOG_SECURITY },
	{ "tables",		GGZ_LOG_TABLES }
};
static int num_log_types = sizeof(log_types) / sizeof(log_types[0]);
static struct LogTypes dbg_types[] = {
	{ "all",		GGZ_DBG_ALL },
	{ "configuration",	GGZ_DBG_CONFIGURATION },
	{ "process",		GGZ_DBG_PROCESS },
	{ "connection",		GGZ_DBG_CONNECTION },
	{ "chat",		GGZ_DBG_CHAT },
	{ "table",		GGZ_DBG_TABLE },
	{ "protocol",		GGZ_DBG_PROTOCOL },
	{ "update",		GGZ_DBG_UPDATE },
	{ "misc",		GGZ_DBG_MISC },
	{ "room",		GGZ_DBG_ROOM },
	{ "lists",		GGZ_DBG_LISTS },
	{ "game_msg",           GGZ_DBG_GAME_MSG},
	{ "xml",                GGZ_DBG_XML}
};
static int num_dbg_types = sizeof(dbg_types) / sizeof(dbg_types[0]);

/* Game and room lists */
static int g_count = 0;
static char **g_list = NULL;
static int r_count = 0;
static char **r_list = NULL;

/* Convience macro for parse_file(), parse_game() */
#define PARSE_ERR(s)  err_msg("Config file: %s, line %d", s, linenum)

/* Defaults for Admin items */
#define ADMIN_ERR     "<You must set this parameter in ggzd.conf>"

static const struct poptOption args[] = {
	
	{"foreground", 'F', POPT_ARG_NONE, &opt.foreground, 0,
	 "Tells ggzd to run in the foreground"},
	{"file", 'f', POPT_ARG_STRING, &opt.local_conf, 0, 
	 "Configuration file", "FILE"},
	{"log", 'l', POPT_ARG_INT, &log_info.log_types, 0,
	 "Types of logging to perform", "LEVEL"},
#ifdef DEBUG
	{"debug", 'd', POPT_ARG_INT, &log_info.dbg_types, 2,
	 "Types of debug logging to perform", "DBGLEVEL"},
#endif
	{"port", 'p', POPT_ARG_INT, &opt.main_port, 0,
	 "GGZ port number", "PORT"},
	{"version", 'V', POPT_ARG_NONE, NULL, 1},
	POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}	/* end the list */
};

/* Parse command-line options */
void parse_args(int argc, const char *argv[])
{

	poptContext context = poptGetContext(NULL, argc, argv, args, 0);
	int rc;

	while ((rc = poptGetNextOpt(context)) != -1) {
		switch (rc) {
		case 1:	/* Version command */
			printf("GGZ Gaming Zone server: version %s\n", 
			       VERSION);
			poptFreeContext(context);
			exit(0);
#ifdef DEBUG
		case 2: /* They specified --debug */
			log_info.popt_dbg++;
			break;
#endif
		case POPT_ERROR_NOARG:
		case POPT_ERROR_BADOPT:
		case POPT_ERROR_BADNUMBER:
		case POPT_ERROR_OVERFLOW:
			err_msg("%s: %s", poptBadOption(context, 0),
				poptStrerror(rc));
			poptFreeContext(context);
			exit(-1);
			break;
		}
	}

	poptFreeContext(context);

}


/* Parse options from conf file, but don't overwrite existing options*/
void parse_conf_file(void)
{
	char *tempstr;
	int c_handle = -1;

	/* Use conf_parse on an approrpriate configuration file */
	if (opt.local_conf) {
		if((c_handle = ggz_conf_parse(opt.local_conf, GGZ_CONF_RDONLY)) >= 0) {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"Reading local conf file : %s", opt.local_conf);
		} else
			err_msg("WARNING:  Local conf file not found!");
	} else {
		if((tempstr=malloc(strlen(GGZDCONFDIR)+11)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");

		strcpy(tempstr, GGZDCONFDIR);  /* If this changes be sure to */
		strcat(tempstr, "/ggzd.conf"); /* change the malloc() above! */

		if((c_handle = ggz_conf_parse(tempstr, GGZ_CONF_RDONLY)) >= 0) {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"Reading global conf file : %s", tempstr);
		} else
			err_msg("WARNING:  No configuration file loaded!");

		free(tempstr);
	}

	/* Get options from the file */
	if(c_handle >= 0)
		get_config_options(c_handle);

	/* Add any defaults which were not config'ed */

	/* If no game_dir, default it to GAMEDIR */
	if(!opt.game_dir) {
		if((tempstr=malloc(strlen(GAMEDIR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, GAMEDIR);
		opt.game_dir = tempstr;
	}

	/* If no conf_dir, default it to GGZDCONFDIR */
	if(!opt.conf_dir) {
		if((tempstr=malloc(strlen(GGZDCONFDIR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, GGZDCONFDIR);
		opt.conf_dir = tempstr;
	}

	/* If no data_dir, default it to DATADIR */
	if(!opt.data_dir) {
		if((tempstr=malloc(strlen(DATADIR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, DATADIR);
		opt.data_dir = tempstr;
	}

	/* If no tmp_dir, default it to TMPDIR */
	if(!opt.tmp_dir) {
		if((tempstr=malloc(strlen(TMPDIR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, TMPDIR);
		opt.tmp_dir = tempstr;
	}


	/* If no main_port, default it to 5688 */
	if(!opt.main_port)
		opt.main_port = 5688;

	/* Set admin stuff to ADMIN_ERR if not specified */
	if(!opt.admin_name) {
		if((tempstr=malloc(strlen(ADMIN_ERR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, ADMIN_ERR);
		opt.admin_name = tempstr;
	}
	if(!opt.admin_email) {
		if((tempstr=malloc(strlen(ADMIN_ERR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, ADMIN_ERR);
		opt.admin_email = tempstr;
	}
}


/* Parse the pre-openend configuration file, close the file when done */
static void get_config_options(int ch)
{
	int intval;
	char *strval;
	char **t_list;
	int t_count = 0;

	/* [General] */
	if(opt.main_port == 0)
		opt.main_port = ggz_conf_read_int(ch, "General", "Port", 5688);
	opt.admin_name = ggz_conf_read_string(ch, "General", "AdminName",
					  "<unconfigured>");
	opt.admin_email = ggz_conf_read_string(ch, "General", "AdminEmail",
					  "<unconfigured>");
	opt.server_name = ggz_conf_read_string(ch, "General", "ServerName",
					   "An Unconfigured GGZ Server");

	/* [Directories] */
	opt.game_dir = ggz_conf_read_string(ch, "Directories", "GameDir", NULL);
	opt.conf_dir = ggz_conf_read_string(ch, "Directories", "ConfDir", NULL);
	opt.data_dir = ggz_conf_read_string(ch, "Directories", "DataDir", NULL);

	/* [Games] */
	ggz_conf_read_list(ch, "Games", "GameList", &g_count, &g_list);
	if(g_count == 0) {
		ggz_conf_read_list(ch, "Games", "IgnoredGames", &g_count, &g_list);
		g_count *= -1;
	}
	ggz_conf_read_list(ch, "Games", "RoomList", &r_count, &r_list);
	if(r_count == 0) {
		ggz_conf_read_list(ch, "Games", "IgnoredRooms", &r_count, &r_list);
		r_count *= -1;
	}

	/* [Files] */
	motd_info.motd_file = ggz_conf_read_string(ch, "Files", "MOTD", NULL);
	if(motd_info.motd_file != NULL)
		motd_info.use_motd = 1;

	/* [Logs] */
	log_info.log_fname = ggz_conf_read_string(ch, "Logs", "LogFile", NULL);
	ggz_conf_read_list(ch, "Logs", "LogTypes", &t_count, &t_list);
	if(t_count > 0)
		log_info.log_types |= parse_log_types(t_count, t_list);
#ifdef DEBUG
	log_info.dbg_fname = ggz_conf_read_string(ch, "Logs", "DebugFile", NULL);
	t_count = 0;
	ggz_conf_read_list(ch, "Logs", "DebugTypes", &t_count, &t_list);
	if(t_count > 0)
		log_info.dbg_types |= parse_dbg_types(t_count, t_list);
#endif /*DEBUG*/
	strval = ggz_conf_read_string(ch, "Logs", "Facility", NULL);
	if(strval) {
		if(logfile_set_facility(strval) < 0)
			err_msg("Configuration: Invalid syslogd facility");
		free(strval);
	}
	intval = ggz_conf_read_int(ch, "Logs", "PIDInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_PID;
	else
		log_info.options |= GGZ_LOGOPT_INC_PID;
	intval = ggz_conf_read_int(ch, "Logs", "ThreadLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_THREAD_LOGS;
	else
		log_info.options |= GGZ_LOGOPT_THREAD_LOGS;
	intval = ggz_conf_read_int(ch, "Logs", "TimeInLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_TIME;
	else
		log_info.options |= GGZ_LOGOPT_INC_TIME;
	intval = ggz_conf_read_int(ch, "Logs", "GameTypeInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_GAMETYPE;
	else
		log_info.options |= GGZ_LOGOPT_INC_GAMETYPE;

	/* Miscellaneous */
	opt.ping_freq = ggz_conf_read_int(ch, "Miscellaneous", "LagFrequency", 10);
	opt.lag_class[0] = ggz_conf_read_int(ch, "Miscellaneous","LagClass1", 500);
	opt.lag_class[1] = ggz_conf_read_int(ch, "Miscellaneous","LagClass2", 1000);
	opt.lag_class[2] = ggz_conf_read_int(ch, "Miscellaneous","LagClass3", 2000);
	opt.lag_class[3] = ggz_conf_read_int(ch, "Miscellaneous","LagClass4", 5000);

	ggz_conf_cleanup();
}


/* Main entry point for parsing the game files */
void parse_game_files(void)
{
	struct dirent **namelist;
	char *name;
	char *dir;
	int num_games, i, j;
	int addit;

	/* Setup our directory to "conf_dir/games/" */
	if((dir = malloc(strlen(opt.conf_dir)+8)) == NULL)
		err_sys_exit("malloc error in parse_game_files()");
	strcpy(dir, opt.conf_dir);
	strcat(dir, "/games/");

	if(g_count > 0) {
		/* Go through all games explicitly included in the add list */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding games in add list");
		for(i=0; i<g_count; i++)
			parse_game(g_list[i], dir);
	} else {
		/* Scan for all .dsc files in the dir */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding all games in %s", dir);
		num_games = scandir(dir, &namelist, parse_gselect, 0);
		for(i=0; i<num_games; i++) {
			/* Make a temporary copy of the name w/o .dsc */
			if((name=malloc(strlen(namelist[i]->d_name)+1)) == NULL)
			       err_sys_exit("malloc error in parse_game_files");
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
				dbg_msg(GGZ_DBG_CONFIGURATION,
					"Ignoring game %s", name);

			free(name);
		}
	}

	free(dir);

	/* Cleanup the g_list */
	if(g_count < 0)
		g_count = -g_count;
	if(g_count > 0) {
		for(i=0; i<g_count; i++)
			free(g_list[i]);
		free(g_list);
		g_list = NULL;
		g_count = 0;
	}

	ggz_conf_cleanup();
}


/* Parse a single game file, adding it's values to the game table */
static void parse_game(char *name, char *dir)
{
	char *fname;
	int ch;
	GameInfo *game_info;
	char *strval;
	int intval, len, i;
	char **b_list;
	int b_count = 0;
	char allow_bits[] = { GGZ_ALLOW_ZERO, GGZ_ALLOW_ONE, GGZ_ALLOW_TWO,
		GGZ_ALLOW_THREE, GGZ_ALLOW_FOUR, GGZ_ALLOW_FIVE,
		GGZ_ALLOW_SIX, GGZ_ALLOW_SEVEN, GGZ_ALLOW_EIGHT };

	/* Check to see if we are allocating too many games */
	if(state.types == MAX_GAME_TYPES) {
		err_msg("Ignoring game %s, already at MAX_GAME_TYPES", name);
		return;
	}

	/* Allocate space and setup a full pathname to description file */
	len = strlen(name)+strlen(dir)+6;
	if((fname = malloc(len)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	snprintf(fname, len, "%s/%s.dsc", dir, name);

	if((ch = ggz_conf_parse(fname, GGZ_CONF_RDONLY)) < 0) {
		err_msg("Ignoring %s, could not open %s", name, fname);
		free(fname);
		return;
	}

	dbg_msg(GGZ_DBG_CONFIGURATION, "Adding game %s from %s", name, fname);

	/* Allocate a game_info struct for this game and default to enabled */
	if((game_info = malloc(sizeof(GameInfo))) == NULL)
		err_sys_exit("malloc error in parse_game()");
	memset(game_info, 0, sizeof(GameInfo));
	pthread_rwlock_init(&game_info->lock, NULL);	

	/* [GameInfo] */
	strval = ggz_conf_read_string(ch, "GameInfo", "Name", NULL);
	if(strval) {
		strncpy(game_info->name, strval, MAX_GAME_NAME_LEN);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "GameInfo", "Version", NULL);
	if(strval) {
		strncpy(game_info->version, strval, MAX_GAME_VER_LEN);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "GameInfo", "Description", NULL);
	if(strval) {
		strncpy(game_info->desc, strval, MAX_GAME_DESC_LEN);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "GameInfo", "Author", NULL);
	if(strval) {
		strncpy(game_info->author, strval, MAX_GAME_AUTH_LEN);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "GameInfo", "Homepage", NULL);
	if(strval) {
		strncpy(game_info->homepage, strval, MAX_GAME_WEB_LEN);
		free(strval);
	}

	/* [LaunchInfo] */
	strval = ggz_conf_read_string(ch, "LaunchInfo", "ExecutablePath", NULL);
	if(strval) {
		/* Copy just the string if we have an absolute path */
		if(*strval == '/')
			strncpy(game_info->path, strval, MAX_PATH_LEN);
		else
			snprintf(game_info->path, MAX_PATH_LEN,
				  "%s/%s", opt.game_dir, strval);
		free(strval);
	}
	ggz_conf_read_list(ch, "LaunchInfo", "ArgList",
		       &game_info->n_args, &game_info->args);

	/* [Protocol] */
	strval = ggz_conf_read_string(ch, "Protocol", "Engine", NULL);
	if(strval) {
		strncpy(game_info->p_engine, strval, MAX_GAME_PROTOCOL_LEN);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "Protocol", "Version", NULL);
	if(strval) {
		strncpy(game_info->p_version, strval, MAX_GAME_VER_LEN);
		free(strval);
	}

	/* [TableOptions] */
	game_info->allow_leave = ggz_conf_read_int(ch,"TableOptions","AllowLeave",0);
	game_info->kill_when_empty =
		ggz_conf_read_int(ch, "TableOptions", "KillWhenEmpty", 1);
	ggz_conf_read_list(ch, "TableOptions", "BotsAllowed", &b_count, &b_list);
	if(b_count != 0) {
		for(i=0; i<b_count; i++) {
			intval = atoi(b_list[i]);
			if(intval < 0 || intval > MAX_TABLE_SIZE) {
				err_msg("BotsAllowed has invalid value [%s]",
					name);
				continue;
			}
			game_info->bot_allow_mask |= allow_bits[intval];
			free(b_list[i]);
		}
		free(b_list);
		b_list = NULL;
		b_count = 0;
	}
	ggz_conf_read_list(ch, "TableOptions", "PlayersAllowed", &b_count, &b_list);
	if(b_count != 0) {
		for(i=0; i<b_count; i++) {
			intval = atoi(b_list[i]);
			if(intval < 1 || intval > MAX_TABLE_SIZE) {
				err_msg("PlayersAllowed has invalid value");
				continue;
			}
			game_info->player_allow_mask |= allow_bits[intval];
			free(b_list[i]);
		}
		free(b_list);
	}

	game_types[state.types] = *game_info;
	state.types++;

	free(game_info);
	free(fname);
}


/* Main entry point for parsing the room files */
void parse_room_files(void)
{
	struct dirent **namelist;
	char *dir;
	char *name;
	int num_rooms, i, j;
	int addit;

	/* Setup our directory to "conf_dir/rooms/" */
	if((dir = malloc(strlen(opt.conf_dir)+8)) == NULL)
		err_sys_exit("malloc error in parse_game_files()");
	strcpy(dir, opt.conf_dir);
	strcat(dir, "/rooms/");

	parse_room("entry", dir);

	if(r_count > 0) {
		/* Go through all rooms explicitly included in the add list */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding rooms in add list");
		for(i=0; i<r_count; i++)
			/* Add everything, but don't readd entry room */
			if(strcmp(r_list[i], "entry"))
				parse_room(r_list[i], dir);
	} else {
		/* Scan for all .room files in dir */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding all rooms in %s", dir);
		num_rooms = scandir(dir, &namelist, parse_rselect, 0);
		for(i=0; i<num_rooms; i++) {
			/* Make a temporary copy of the name w/o .room */
			if((name=malloc(strlen(namelist[i]->d_name)+1)) == NULL)
			       err_sys_exit("malloc error in parse_game_files");
			strcpy(name, namelist[i]->d_name);
			name[strlen(name)-5] = '\0';
			/* Don't readd the entry room */
			if(!strcmp(name, "entry"))
				continue;
			/* Check to see if this game is on the ignore list */
			addit=1;
			for(j=0; j>r_count; j--) {
				if(!strcmp(name, r_list[0-j]))
					addit = 0;
					break;
			}

			/* Add it if it's not on the ignore list */
			if(addit)
				parse_room(name, dir);
			else
				dbg_msg(GGZ_DBG_CONFIGURATION,
					"Ignoring room %s", name);

			free(name);
		}
	}

	free(dir);

	/* At this point, we should have at least one working room */
	if(room_info.num_rooms == 0)
		err_msg_exit("No rooms defined, ggzd unusable");

	/* Cleanup the r_list */
	if(r_count < 0)
		r_count = -r_count;
	if(r_count > 0) {
		for(i=0; i<r_count; i++)
			free(r_list[i]);
		free(r_list);
		r_list = NULL;
		r_count = 0;
	}
}


/* Parse a single room file, adding it's values to the room table */
static void parse_room(char *name, char *dir)
{
	char *fname;
	int ch;
	char *strval;
	int num;
	int i;
	int len;

	/* Allocate space and setup a full pathname to description file */
	len = strlen(name)+strlen(dir)+7;
	if((fname = malloc(len)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	snprintf(fname, len, "%s/%s.room", dir, name);

	if((ch = ggz_conf_parse(fname, GGZ_CONF_RDONLY)) < 0) {
		err_msg("Ignoring %s, could not open %s", name, fname);
		free(fname);
		return;
	}

	dbg_msg(GGZ_DBG_CONFIGURATION, "Adding room %s from %s", name, fname);

	/* Allocate a room struct for this room */
	if(room_info.num_rooms == 0)
		room_initialize();
	else
		room_create_additional();
	num = room_info.num_rooms - 1;
	rooms[num].game_type = -2;
	rooms[num].perms = 0;

	/* [RoomInfo] */
	rooms[num].name = ggz_conf_read_string(ch, "RoomInfo", "Name", NULL);
	rooms[num].description = ggz_conf_read_string(ch, "RoomInfo", "Description",
						  NULL);
	rooms[num].max_players = ggz_conf_read_int(ch, "RoomInfo", "MaxPlayers", 0);
	rooms[num].max_tables = ggz_conf_read_int(ch, "RoomInfo", "MaxTables", -1);
	strval = ggz_conf_read_string(ch, "RoomInfo", "GameType", NULL);
	if(strval) {
		for(i=0; i<state.types; i++)
			if(!strcmp(strval, game_types[i].name))
				break;
		if(i != state.types)
			rooms[num].game_type = i;
		else if(!strcasecmp(strval, "none"))
			rooms[num].game_type = -1;
		else
			err_msg("Invalid GameType specified in room %s", name);
		free(strval);
	}
	strval = ggz_conf_read_string(ch, "RoomInfo", "EntryRestriction", NULL);
	if(strval) {
		if(!strcasecmp(strval, "Admin"))
			rooms[num].perms = PERMS_ROOMS_ADMIN;
		else if(!strcasecmp(strval, "Registered"))
			rooms[num].perms = PERMS_ROOMS_LOGIN;
		else if(!strcasecmp(strval, "None"))
			rooms[num].perms = 0;
		else
			err_msg("Invalid EntryRestriction in room %s", name);
		free(strval);
	}

	if(rooms[num].name == NULL) {
		err_msg("No Name given for room %s", name);
		if((strval = malloc(5)) == NULL)
			err_sys_exit("malloc failed in parse_room()");
		strcpy(strval, "none");
		rooms[num].name = strval;
	}
	if(rooms[num].description == NULL) {
		err_msg("No Description given for room %s", name);
		if((strval = malloc(5)) == NULL)
			err_sys_exit("malloc failed in parse_room()");
		strcpy(strval, "none");
		rooms[num].description = strval;
	}
	if(rooms[num].max_players <= 0) {
		if(rooms[num].max_players < 0)
			err_msg("Invalid MaxPlayers given for room %s", name);
		rooms[num].max_players = DEFAULT_MAX_ROOM_USERS;
	}
	if(rooms[num].max_tables < 0) {
		if(rooms[num].max_tables < -1)
			err_msg("Invalid MaxTables given for room %s", name);
		rooms[num].max_tables = DEFAULT_MAX_ROOM_TABLES;
	}
	if(rooms[num].game_type == -2) {
		err_msg("No GameType given for room %s", name);
		rooms[num].game_type = -1;
	}

	rooms[num].players = calloc(rooms[num].max_players, sizeof(GGZPlayer*));
	if(rooms[num].players == NULL)
		err_sys_exit("calloc failed in parse_room()");
	rooms[num].tables = calloc(rooms[num].max_tables, sizeof(GGZTable*));
					
	if(rooms[num].tables == NULL)
		err_sys_exit("calloc failed in parse_room()");

	free(fname);
}


/* Return 1 if filename matches our pattern (ends in '.dsc') */
static int parse_gselect(const struct dirent *dent)
{
	return(!strcmp(".dsc", dent->d_name+strlen(dent->d_name)-4));
}


/* Return 1 if filename matches our pattern (ends in '.room') */
static int parse_rselect(const struct dirent *dent)
{
	return(!strcmp(".room", dent->d_name+strlen(dent->d_name)-5));
}


/* Parse the logging types into an unsigned int bitfield */
static unsigned parse_log_types(int num, char **entry)
{
	unsigned types=0;
	int i, j;

	for(i=0; i<num; i++) {
		for(j=0; j<num_log_types; j++)
			if(!strcasecmp(entry[i], log_types[j].name))
				break;
		if(j == num_log_types)
			err_msg("Config: Invalid log type '%s' specified",
				entry[i]);
		else {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"%s added to log types", log_types[j].name);
			types |= log_types[j].type;
		}

		free(entry[i]);
	}

	free(entry);
	return types;
}


/* Parse the debugging types into an unsigned int bitfield */
static unsigned parse_dbg_types(int num, char **entry)
{
	unsigned types=0;
	int i, j;

	for(i=0; i<num; i++) {
		for(j=0; j<num_dbg_types; j++)
			if(!strcasecmp(entry[i], dbg_types[j].name))
				break;
		if(j == num_dbg_types)
			err_msg("Config: Invalid debug type '%s' specified",
				entry[i]);
		else {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"%s added to debug types", dbg_types[j].name);
			types |= dbg_types[j].type;
		}

		free(entry[i]);
	}

	free(entry);
	return types;
}
