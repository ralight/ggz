/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 * $Id: parse_opt.c 2266 2001-08-26 21:51:03Z jdorje $
 *
 * Copyright (C) 1999,2000,2001 Brent Hendricks.
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
#include <datatypes.h>
#include <err_func.h>
#include <motd.h>
#include <room.h>
#include <conf.h>

/* Stuff from control.c we need access to */
extern Options opt;
extern struct GGZState state;
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Private file parsing functions */
static void get_config_options(int);
static void parse_line(char *);
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
	{ "connections",	GGZ_LOG_CONNECTION_INFO }
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
	{ "game_msg",           GGZ_DBG_GAME_MSG}
};
static int num_dbg_types = sizeof(dbg_types) / sizeof(dbg_types[0]);

/* Module local variables for parsing */
static char *varname;
static char *varvalue;

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
		if((c_handle = conf_parse(opt.local_conf, CONF_RDONLY)) >= 0) {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"Reading local conf file : %s", opt.local_conf);
		} else
			err_msg("WARNING:  Local conf file not found!");
	} else {
		if((tempstr=malloc(strlen(GGZDCONFDIR)+11)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");

		strcpy(tempstr, GGZDCONFDIR);  /* If this changes be sure to */
		strcat(tempstr, "/ggzd.conf"); /* change the malloc() above! */

		if((c_handle = conf_parse(tempstr, CONF_RDONLY)) >= 0) {
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
		opt.main_port = conf_read_int(ch, "General", "Port", 5688);
	opt.admin_name = conf_read_string(ch, "General", "AdminName", NULL);
	opt.admin_email = conf_read_string(ch, "General", "AdminEmail", NULL);

	/* [Directories] */
	opt.game_dir = conf_read_string(ch, "Directories", "GameDir", NULL);
	opt.conf_dir = conf_read_string(ch, "Directories", "ConfDir", NULL);
	opt.data_dir = conf_read_string(ch, "Directories", "DataDir", NULL);

	/* [Games] */
	conf_read_list(ch, "Games", "GameList", &g_count, &g_list);
	if(g_count == 0) {
		conf_read_list(ch, "Games", "IgnoredGames", &g_count, &g_list);
		g_count *= -1;
	}
	conf_read_list(ch, "Games", "RoomList", &r_count, &r_list);
	if(r_count == 0) {
		conf_read_list(ch, "Games", "IgnoredRooms", &r_count, &r_list);
		r_count *= -1;
	}

	/* [Files] */
	motd_info.motd_file = conf_read_string(ch, "Files", "MOTD", NULL);
	if(motd_info.motd_file != NULL)
		motd_info.use_motd = 1;

	/* [Logs] */
	log_info.log_fname = conf_read_string(ch, "Logs", "LogFile", NULL);
	conf_read_list(ch, "Logs", "LogTypes", &t_count, &t_list);
	if(t_count > 0)
		log_info.log_types |= parse_log_types(t_count, t_list);
#ifdef DEBUG
	log_info.dbg_fname = conf_read_string(ch, "Logs", "DebugFile", NULL);
	t_count = 0;
	conf_read_list(ch, "Logs", "DebugTypes", &t_count, &t_list);
	if(t_count > 0)
		log_info.dbg_types |= parse_dbg_types(t_count, t_list);
#endif /*DEBUG*/
	strval = conf_read_string(ch, "Logs", "Facility", NULL);
	if(strval)
		if(logfile_set_facility(strval) < 0)
			err_msg("Configuration: Invalid syslogd facility");
	intval = conf_read_int(ch, "Logs", "PIDInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_PID;
	else
		log_info.options |= GGZ_LOGOPT_INC_PID;
	intval = conf_read_int(ch, "Logs", "ThreadLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_THREAD_LOGS;
	else
		log_info.options |= GGZ_LOGOPT_THREAD_LOGS;
	intval = conf_read_int(ch, "Logs", "TimeInLogs", 0);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_TIME;
	else
		log_info.options |= GGZ_LOGOPT_INC_TIME;
	intval = conf_read_int(ch, "Logs", "GameTypeInLogs", 1);
	if(intval == 0)
		log_info.options &= ~GGZ_LOGOPT_INC_GAMETYPE;
	else
		log_info.options |= GGZ_LOGOPT_INC_GAMETYPE;

	conf_cleanup();
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
		dbg_msg(GGZ_DBG_CONFIGURATION, "Addding all games in %s", dir);
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

	/* FIXME - Cleanup g_list */
}


/* Parse a single game file, adding it's values to the game table */
static void parse_game(char *name, char *dir)
{
	char *fname;
	FILE *gamefile;
	GameInfo *game_info;
	char line[256];
	int linenum = 0;
	int intval;
	int len;
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

	if((gamefile = fopen(fname, "r")) == NULL) {
		err_msg("Ignoring %s, could not open %s", name, fname);
		free(fname);
		return;
	}

	dbg_msg(GGZ_DBG_CONFIGURATION, "Adding game %s from %s", name, fname);

	/* Allocate a game_info struct for this game and default to enabled */
	if((game_info = malloc(sizeof(GameInfo))) == NULL)
		err_sys_exit("malloc error in parse_game()");
	memset(game_info, 0, sizeof(GameInfo));
	game_info->enabled = 1;
	pthread_rwlock_init(&game_info->lock, NULL);	

	while(fgets(line, 256, gamefile)) {
		linenum++;
		parse_line(line);
		if(varname == NULL)
			continue; /* Blank line or comment */
		dbg_msg(GGZ_DBG_CONFIGURATION, "  found '%s, %s'",
			varname, varvalue);

		/*** Name = String ***/
		if(!strcmp(varname, "name")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->name, varvalue, MAX_GAME_NAME_LEN);
		}

		/*** Version = X.Y.Z ***/
		if(!strcmp(varname, "version")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->version, varvalue, MAX_GAME_VER_LEN);
		}

		/*** Description = String ***/
		if(!strcmp(varname, "description")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->desc, varvalue, MAX_GAME_DESC_LEN);
		}

		/*** ProtocolEngine = String ***/
		if(!strcmp(varname, "protocolengine")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->p_engine, varvalue,
				 MAX_GAME_PROTOCOL_LEN);
		}

		/*** ProtocolVersion = String ***/
		if(!strcmp(varname, "protocolversion")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->p_version, varvalue,
				MAX_GAME_VER_LEN);
		}

		/*** Author = String ***/
		if(!strcmp(varname, "author")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->author, varvalue, MAX_GAME_AUTH_LEN);
		}

		/*** Homepage = String ***/
		if(!strcmp(varname, "homepage")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			strncpy(game_info->homepage, varvalue,MAX_GAME_WEB_LEN);
		}

		/*** PlayersAllowed = # ***/
		if(!strcmp(varname, "playersallowed")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 1 || intval > 8) {
				PARSE_ERR("PlayersAllowed value invalid");
				continue;
			}
			game_info->player_allow_mask |= allow_bits[intval];
		}

		/*** BotsAllowed = # ***/
		if(!strcmp(varname, "botsallowed")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0 || intval > 8) {
				PARSE_ERR("BotAllowed value invalid");
				continue;
			}
			game_info->bot_allow_mask |= allow_bits[intval];
		}

		/*** OptionsSize = # ***/
		if(!strcmp(varname, "optionssize")) {
			PARSE_ERR("Obsolete reference to OptionsSize ignored");
		}

		/*** ExecutablePath = String ***/
		if(!strcmp(varname, "executablepath")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			/* Copy just the string if we have an absolute path */
			if(*varvalue == '/')
				strncpy(game_info->path, varvalue,MAX_PATH_LEN);
			else
				snprintf(game_info->path, MAX_PATH_LEN,
					  "%s/%s", opt.game_dir, varvalue);
		}

		/*** AllowLeave = 0,1 ***/
		if(!strcmp(varname, "allowleave")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval != 0 && intval != 1) {
				PARSE_ERR("AllowLeave value invalid");
				continue;
			}
			game_info->allow_leave = intval;
		}

		/*** GameDisabled ***/
		if(!strcmp(varname, "gamedisabled"))
			game_info->enabled = 0;
	}

	game_types[state.types] = *game_info;
	state.types++;

	fclose(gamefile);
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
		dbg_msg(GGZ_DBG_CONFIGURATION, "Addding all rooms in %s", dir);
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

	/* FIXME - Cleanup r_list */
}


/* Parse a single room file, adding it's values to the room table */
static void parse_room(char *name, char *dir)
{
	char *fname;
	FILE *roomfile;
	char line[256];
	int linenum = 0;
	int intval;
	char *strval;
	int num;
	int i;
	int len;

	/* Allocate space and setup a full pathname to description file */
	len = strlen(name)+strlen(dir)+7;
	if((fname = malloc(len)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	snprintf(fname, len, "%s/%s.room", dir, name);

	if((roomfile = fopen(fname, "r")) == NULL) {
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
	rooms[num].max_tables = -1;

	while(fgets(line, 256, roomfile)) {
		linenum++;
		parse_line(line);
		if(varname == NULL)
			continue; /* Blank line or comment */
		dbg_msg(GGZ_DBG_CONFIGURATION, "  found '%s, %s'",
			varname, varvalue);

		/*** Name = String ***/
		if(!strcmp(varname, "name")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("malloc failed in parse_room()");
			strcpy(strval, varvalue);
			rooms[num].name = strval;
		}

		/*** Description = String ***/
		if(!strcmp(varname, "description")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("malloc failed in parse_room()");
			strcpy(strval, varvalue);
			rooms[num].description = strval;
		}

		/*** MaxPlayers = # ***/
		if(!strcmp(varname, "maxplayers")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 1) {
				PARSE_ERR("MaxPlayers value invalid");
				continue;
			}
			rooms[num].max_players = intval;
		}

		/*** MaxTables = # ***/
		if(!strcmp(varname, "maxtables")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0) {
				PARSE_ERR("MaxTables value invalid");
				continue;
			}
			rooms[num].max_tables = intval;
		}

		/*** GameType = String ***/
		if(!strcmp(varname, "gametype")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			for(i=0; i<state.types; i++)
				if(!strcmp(varvalue, game_types[i].name))
					break;
			if(i != state.types)
				rooms[num].game_type = i;
			else if(!strcasecmp(varvalue, "none"))
				rooms[num].game_type = -1;
			else
				PARSE_ERR("Invalid game type specified");
		}
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
	if(rooms[num].max_players == 0) {
		err_msg("No MaxPlayers given for room %s", name);
		rooms[num].max_players = DEFAULT_MAX_ROOM_USERS;
	}
	if(rooms[num].max_tables < 0) {
		err_msg("No MaxTables given for room %s", name);
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

	fclose(roomfile);
	free(fname);
}


/* Parse a single line of input into a left half and a right half */
/* separated by an (optional) equals sign                         */
static void parse_line(char *p)
{
	char csave;

	varname = NULL;
	/* Skip over whitespace */
	while((*p == ' ' || *p == '\t' || *p == '\n') && *p != '\0')
		p++;
	if(*p == '\0' || *p == '#')
		return;	/* The line is a comment */

	varname = p;

	varvalue = NULL;
	/* Skip until we find the end of the variable name, converting */
	/* everything (for convenience) to lowercase as we go */
	while(*p != ' ' && *p != '\t' && *p != '\n' && *p != '=' && *p != '\0'){
		*p = tolower(*p);
		p++;
	}
	csave = *p;
	*p = '\0';
	p++;

	if(csave == '\n' || csave == '\0')
		return;	/* There is no argument */

	/* There appears to be an argument, skip to the start of it */
	while((*p == ' ' || *p == '\t' || *p == '\n' || *p == '=')&& *p != '\0')
		p++;
	if(*p == '\0')
		return; /* Argument is missing */

	/* There is an argument ... */
	varvalue = p;

	/* Terminate it ... */
	while(*p != '\n' && *p != '\0')
		p++;
	/* Found EOL, now backspace over whitespace to remove trailing space */
	p--;
	while(*p == ' ' || *p == '\t' || *p == '\n')
		p--;
	p++;
	/* Finally terminate it with a NUL */
	*p = '\0'; /* Might have already been the NUL, but who cares? */
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
	}

	return types;
	/* FIXME - Cleanup the ENTRY array */
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
	}

	return types;
	/* FIXME - Cleanup the ENTRY array */
}
