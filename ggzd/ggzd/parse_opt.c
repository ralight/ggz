/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 *
 * Copyright (C) 1999 Brent Hendricks.
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

/* Stuff from control.c we need access to */
extern Options opt;
extern struct GGZState state;
extern struct GameInfo game_types[MAX_GAME_TYPES];

/* Structure for Add/Ignore Games list */
typedef struct AddIgnore {
  char *name;
  struct AddIgnore *next;
} AddIgnoreStruct;

/* Private file parsing functions */
static void parse_file(FILE *);
static void parse_line(char *);
static AddIgnoreStruct *parse_put_add_ignore_list(char *, AddIgnoreStruct *);
static AddIgnoreStruct *parse_cleanup_add_ignore_list(AddIgnoreStruct *);
static void parse_game(char *, char *);
static void parse_room(char *, char *);
static int parse_gselect(struct dirent *);
static int parse_rselect(struct dirent *);
static unsigned parse_log_types(char *, int);
static unsigned parse_dbg_types(char *, int);

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
static AddIgnoreStruct *add_ignore_games = NULL;
static char add_all_games = 't';
static AddIgnoreStruct *add_ignore_rooms = NULL;
static char add_all_rooms = 't';

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
			printf("Gnu Gaming Zone server: version %s\n", 
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
	FILE *configfile;
	char *tempstr;

	if (opt.local_conf) {
		if((configfile=fopen(opt.local_conf,"r"))) {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"Reading local conf file : %s", opt.local_conf);
			parse_file(configfile);
		} else
			err_msg("WARNING:  Local conf file not found!");
	} else {
		if((tempstr=malloc(strlen(GGZDCONFDIR)+11)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");

		strcpy(tempstr, GGZDCONFDIR);  /* If this changes be sure to */
		strcat(tempstr, "/ggzd.conf"); /* change the malloc() above! */

		if((configfile=fopen(tempstr,"r"))) {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"Reading global conf file : %s", tempstr);
			parse_file(configfile);
		} else
			err_msg("WARNING:  No configuration file loaded!");

		free(tempstr);
	}

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
static void parse_file(FILE *configfile)
{
	char line[256];		/* Lines longer than 256 are trunced */
	int intval;
	char *strval;
	int linenum = 0;

	while(fgets(line, 256, configfile)) {
		linenum++;
		parse_line(line);
		if(varname == NULL)
			continue; /* Blank line or comment */
		dbg_msg(GGZ_DBG_CONFIGURATION, "  found '%s, %s'",
			varname, varvalue);

		/* Apply the configuration line, oh to be able to do */
		/* a case construct with strings :)                  */

		/*** Port = X ***/
		if(!strcmp(varname, "port")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 1024 || intval > 32767) {
				PARSE_ERR("Invalid port number");
				continue;
			}
			if(opt.main_port == 0)
				opt.main_port = intval;
			continue;
		}

		/*** GameDir = DIR ***/
		if(!strcmp(varname, "gamedir")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.game_dir = strval;
			continue;
		 }

		/*** ConfDir = DIR ***/
		if(!strcmp(varname, "confdir")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.conf_dir = strval;
			continue;
		 }

		/*** DataDir = DIR ***/
		if(!strcmp(varname, "datadir")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.data_dir = strval;
			continue;
		 }

#if 0
		/*** TmpDir = DIR ***/
		if(!strcmp(varname, "tmpdir")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.tmp_dir = strval;
			continue;
		 }
#endif

		/*** AddAllGames ***/
		if(!strcmp(varname, "addallgames")) {
			if(add_all_games == 'F')
				PARSE_ERR("AddAllGames after AddGame ignored");
			else
				add_all_games = 'T';
			continue;
		}

		/*** AddGame = GAME ***/
		if(!strcmp(varname, "addgame")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if(add_all_games == 'T')
				PARSE_ERR("AddGame after AddAllGames ignored");
			else {
				add_all_games = 'F';
				add_ignore_games =
					parse_put_add_ignore_list(varvalue,
							 add_ignore_games);
			}
			continue;
		}

		/*** IgnoreGame = GAME ***/
		if(!strcmp(varname, "ignoregame")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if(add_all_games == 'F')
				PARSE_ERR("IgnoreGame after AddGame ignored");
			else {
				add_all_games = 'T';
				add_ignore_games =
					parse_put_add_ignore_list(varvalue,
							add_ignore_games);
			}
			continue;
		}

		/*** AddAllRooms ***/
		if(!strcmp(varname, "addallrooms")) {
			if(add_all_rooms == 'F')
				PARSE_ERR("AddAllRooms after AddRoom ignored");
			else
				add_all_rooms = 'T';
			continue;
		}

		/*** AddRoom = ROOM ***/
		if(!strcmp(varname, "addroom")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if(add_all_rooms == 'T')
				PARSE_ERR("AddRoom after AddAllRooms ignored");
			else {
				add_all_rooms = 'F';
				add_ignore_rooms =
					parse_put_add_ignore_list(varvalue,
							 add_ignore_rooms);
			}
			continue;
		}

		/*** IgnoreRoom = ROOM ***/
		if(!strcmp(varname, "ignoreroom")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if(add_all_rooms == 'F')
				PARSE_ERR("IgnoreRoom after AddRoom ignored");
			else {
				add_all_rooms = 'T';
				add_ignore_rooms =
					parse_put_add_ignore_list(varvalue,
							add_ignore_rooms);
			}
			continue;
		}

		/*** MOTD = filename ***/
		if(!strcmp(varname, "motd")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			motd_info.motd_file = strval;
			motd_info.use_motd = 1;
			continue;
		 }
		
		/*** AdminName = string ***/
		if(!strcmp(varname, "adminname")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.admin_name = strval;
			continue;
		 }
		
		/*** AdminEmail = string ***/
		if(!strcmp(varname, "adminemail")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			opt.admin_email = strval;
			continue;
		 }

		/*** LOGFILE = filename ***/
		if(!strcmp(varname, "logfile")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			log_info.log_fname = strval;
			continue;
		 }

		/*** LOGTYPES = List ***/
		if(!strcmp(varname, "logtypes")) {
			if(varvalue == NULL) {
				PARSE_ERR("Warning: no LogTypes specified");
				continue;
			}
			log_info.log_types |= parse_log_types(varvalue,linenum);
			continue;
		}

		/*** DEBUGFILE = filename ***/
		if(!strcmp(varname, "debugfile")) {
#ifndef DEBUG
			PARSE_ERR("Debugging not enabled");
			continue;
#else
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if((strval = malloc(strlen(varvalue)+1)) == NULL)
				err_sys_exit("parse_file: malloc error");
			strcpy(strval, varvalue);
			log_info.dbg_fname = strval;
			continue;
#endif
		 }
		
		/*** DEBUGTYPES = List ***/
		if(!strcmp(varname, "debugtypes")) {
#ifndef DEBUG
			PARSE_ERR("Debugging not enabled");
			continue;
#else
			if(log_info.popt_dbg)	/* Don't overwrite popt's */
				continue;	/* --debug=X stuff        */
			if(varvalue == NULL) {
				PARSE_ERR("Warning: no DebugTypes specified");
				continue;
			}
			log_info.dbg_types |= parse_dbg_types(varvalue,linenum);
			continue;
#endif
		}

		/*** FACILITY = string ***/
		if(!strcmp(varname, "facility")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			if(logfile_set_facility(varvalue) < 0)
				PARSE_ERR("Invalid syslogd facility name");
			continue;
		 }

		/*** PIDINLOGS = 0,1 ***/
		if(!strcmp(varname, "pidinlogs")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval == 0)
				log_info.options &= ~GGZ_LOGOPT_INC_PID;
			else if(intval == 1)
				log_info.options |= GGZ_LOGOPT_INC_PID;
			else
				PARSE_ERR("Invalid value for PIDInLogs");
			continue;
		}

		/*** THREADLOGS = 0,1 ***/
		if(!strcmp(varname, "threadlogs")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval == 0)
				log_info.options &= ~GGZ_LOGOPT_THREAD_LOGS;
			else if(intval == 1)
				log_info.options |= GGZ_LOGOPT_THREAD_LOGS;
			else
				PARSE_ERR("Invalid value for ThreadLogs");
			continue;
		}

		/*** HOSTNAMELOOKUP = 0,1 ***/
		if(!strcmp(varname, "hostnamelookup")) {
#if 0
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0 || intval > 1) {
				PARSE_ERR("Invalid value for HostnameLookup");
				continue;
			}
			opt.perform_lookups = intval;
			continue;
#else /*1*/
			PARSE_ERR("HostnameLookup is disabled");
			continue;
#endif
		}

		/*** TIMEINLOGS = 0,1 ***/
		if(!strcmp(varname, "timeinlogs")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval == 0)
				log_info.options &= ~GGZ_LOGOPT_INC_TIME;
			else if(intval == 1)
				log_info.options |= GGZ_LOGOPT_INC_TIME;
			else
				PARSE_ERR("Invalid value for TimeInLogs");
			continue;
		}

		/*** GAMETYPEINLOGS = 0,1 ***/
		if(!strcmp(varname, "gametypeinlogs")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval == 0)
				log_info.options &= ~GGZ_LOGOPT_INC_GAMETYPE;
			else if(intval == 1)
				log_info.options |= GGZ_LOGOPT_INC_GAMETYPE;
			else
				PARSE_ERR("Invalid value for GameTypeInLogs");
			continue;
		}

		/*** INVALID VARIABLE ***/
		PARSE_ERR("Syntax error");
	}


	fclose(configfile);

#ifdef DEBUG
	{
	  AddIgnoreStruct *AI = add_ignore_games;
	  if(AI) {
	    if(add_all_games == 'T')
	      dbg_msg(GGZ_DBG_CONFIGURATION, "Game ignore list:");
	    else
	      dbg_msg(GGZ_DBG_CONFIGURATION, "Game add list:");
	    while(AI)
	    {
	      dbg_msg(GGZ_DBG_CONFIGURATION, "  %s", AI->name);
	      AI = AI->next;
	    }
	  }
	  AI = add_ignore_rooms;
	  if(AI) {
	    if(add_all_rooms == 'T')
	      dbg_msg(GGZ_DBG_CONFIGURATION, "Room ignore list:");
	    else
	      dbg_msg(GGZ_DBG_CONFIGURATION, "Room add list:");
	    while(AI)
	    {
	      dbg_msg(GGZ_DBG_CONFIGURATION, "  %s", AI->name);
	      AI = AI->next;
	    }
	  }
	}
#endif /*DEBUG*/
}


/* Main entry point for parsing the game files */
void parse_game_files(void)
{
	AddIgnoreStruct *game;
	struct dirent **namelist;
	char *name;
	char *dir;
	int num_games, i;
	int addit;

	/* Setup our directory to "conf_dir/games/" */
	if((dir = malloc(strlen(opt.conf_dir)+8)) == NULL)
		err_sys_exit("malloc error in parse_game_files()");
	strcpy(dir, opt.conf_dir);
	strcat(dir, "/games/");

	if(add_all_games == 'F') {
		/* Go through all games explicitly included in the add list */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding games in add list");
		game = add_ignore_games;
		while(game) {
			parse_game(game->name, dir);
			game = game->next;
		}
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
			game = add_ignore_games;
			addit = 1;
			while(game) {
				if(strncmp(name,game->name,strlen(game->name)))
					game = game->next;
				else {
					addit=0;
					break;
				}
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
	add_ignore_games = parse_cleanup_add_ignore_list(add_ignore_games);
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
	char allow_bits[] = { GGZ_ALLOW_ZERO, GGZ_ALLOW_ONE, GGZ_ALLOW_TWO,
		GGZ_ALLOW_THREE, GGZ_ALLOW_FOUR, GGZ_ALLOW_FIVE,
		GGZ_ALLOW_SIX, GGZ_ALLOW_SEVEN, GGZ_ALLOW_EIGHT };

	/* Allocate space and setup a full pathname to description file */
	if((fname = malloc(strlen(name)+strlen(dir)+6)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	sprintf(fname, "%s/%s.dsc", dir, name);

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
	AddIgnoreStruct *room;
	struct dirent **namelist;
	char *dir;
	char *name;
	int num_rooms, i;
	int addit;

	/* Setup our directory to "conf_dir/rooms/" */
	if((dir = malloc(strlen(opt.conf_dir)+8)) == NULL)
		err_sys_exit("malloc error in parse_game_files()");
	strcpy(dir, opt.conf_dir);
	strcat(dir, "/rooms/");

	if(add_all_rooms == 'F') {
		/* Go through all rooms explicitly included in the add list */
		dbg_msg(GGZ_DBG_CONFIGURATION, "Adding rooms in add list");
		room = add_ignore_rooms;
		while(room) {
			parse_room(room->name, dir);
			room = room->next;
		}
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
			/* Check to see if this game is on the ignore list */
			room = add_ignore_rooms;
			addit = 1;
			while(room) {
				if(strncmp(name,room->name,strlen(room->name)))
					room = room->next;
				else {
					addit=0;
					break;
				}
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
	add_ignore_rooms = parse_cleanup_add_ignore_list(add_ignore_rooms);

	/* At this point, we should have at least one working room */
	if(room_info.num_rooms == 0)
		err_msg_exit("No rooms defined, ggzd unusable");
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

	/* Allocate space and setup a full pathname to description file */
	if((fname = malloc(strlen(name)+strlen(dir)+7)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	sprintf(fname, "%s/%s.room", dir, name);

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
	rooms[num].game_type = -1;

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
			if(intval < 1) {
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
	if(rooms[num].max_tables == 0) {
		err_msg("No MaxTables given for room %s", name);
		rooms[num].max_tables = DEFAULT_MAX_ROOM_TABLES;
	}
	if(rooms[num].game_type == -1) {
		err_msg("No GameType given for room %s", name);
		rooms[num].game_type = 0;
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


/* Put a string onto the Add/Ignore list */
static AddIgnoreStruct *parse_put_add_ignore_list(char *s, AddIgnoreStruct *old)
{
	AddIgnoreStruct *new_listitem;
	char *newstring;

	if((new_listitem=malloc(sizeof(AddIgnoreStruct))) == NULL)
		err_sys_exit("malloc error in parse_put_add_ignore_list()");
	if((newstring=malloc(strlen(s)+1)) == NULL)
		err_sys_exit("malloc error in parse_put_add_ignore_list()");

	strcpy(newstring, s);
	new_listitem->name = newstring;
	new_listitem->next = old;
	return new_listitem;
}


/* Cleanup the Add/Ignore list after we are finished */
static AddIgnoreStruct *parse_cleanup_add_ignore_list(AddIgnoreStruct *start)
{
	AddIgnoreStruct *temp;

	while((temp = start)) {
		free(start->name);
		start = start->next;
		free(temp);
	}

	return start;
}


/* Return 1 if filename matches our pattern (ends in '.dsc') */
static int parse_gselect(struct dirent *dent)
{
	return(!strcmp(".dsc", dent->d_name+strlen(dent->d_name)-4));
}


/* Return 1 if filename matches our pattern (ends in '.room') */
static int parse_rselect(struct dirent *dent)
{
	return(!strcmp(".room", dent->d_name+strlen(dent->d_name)-5));
}


/* Parse the logging types into an unsigned int bitfield */
static unsigned parse_log_types(char *var, int linenum)
{
	char *s, *e;
	unsigned types=0;
	int lasttime=0;
	int i;

	s = var;
	while(*s != '\0' && !lasttime) {
		/* Skip over whitespace and commas */
		while((*s == ' ' || *s == '\t' || *s == ',') && *s != '\0')
			s++;
		if(*s == '\0')
			break;
		/* Find the end of the type specifier */
		e = s;
		while(*e != ' ' && *e != '\t' && *e != ',' && *e != '\0') {
			*e = tolower(*e);
			e++;
		}
		if(*e == '\0')
			lasttime++;
		/* Plop a NIL on the end */
		*e = '\0';

		/* Now 's' points to a log type */
		for(i = 0; i<num_log_types; i++)
			if(!strcmp(s, log_types[i].name))
				break;

		if(i == num_log_types)
			PARSE_ERR("Invalid log type specified");
		else {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"%s added to log types", log_types[i].name);
			types |= log_types[i].type;
		}

		s = e+1;
	}

	return types;
}


/* Parse the debugging types into an unsigned int bitfield */
static unsigned parse_dbg_types(char *var, int linenum)
{
	char *s, *e;
	unsigned types=0;
	int lasttime=0;
	int i;

	s = var;
	while(*s != '\0' && !lasttime) {
		/* Skip over whitespace and commas */
		while((*s == ' ' || *s == '\t' || *s == ',') && *s != '\0')
			s++;
		if(*s == '\0')
			break;
		/* Find the end of the type specifier */
		e = s;
		while(*e != ' ' && *e != '\t' && *e != ',' && *e != '\0') {
			*e = tolower(*e);
			e++;
		}
		if(*e == '\0')
			lasttime++;
		/* Plop a NIL on the end */
		*e = '\0';

		/* Now 's' points to a log type */
		for(i = 0; i<num_dbg_types; i++)
			if(!strcmp(s, dbg_types[i].name))
				break;

		if(i == num_dbg_types)
			PARSE_ERR("Invalid debug type specified");
		else {
			dbg_msg(GGZ_DBG_CONFIGURATION,
				"%s added to debug types", dbg_types[i].name);
			types |= dbg_types[i].type;
		}

		s = e+1;
	}

	return types;
}
