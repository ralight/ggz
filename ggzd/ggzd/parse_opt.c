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

/* Stuff from control.c we need access to */
extern Options opt;
extern struct GameTypes game_types;

/* Structure for Add/Ignore Games list */
typedef struct AddIgnore {
  char *name;
  struct AddIgnore *next;
} AddIgnoreStruct;

/* Private file parsing functions */
static void parse_file(FILE *);
static void parse_line(char *);
static void parse_put_add_ignore_list(char *);
static void parse_cleanup_add_ignore_list(void);
static void parse_game(char *);
static int parse_dselect(struct dirent *);

/* Module local variables for parsing */
static char *varname;
static char *varvalue;
static AddIgnoreStruct *add_ignore_list = NULL;
static char add_all_games = 't';

/* Convience macro for parse_file(), parse_game() */
#define PARSE_ERR(s)  err_msg("Config file: %s, line %d", s, linenum)

static const struct poptOption args[] = {
	
	{"file", 'f', POPT_ARG_STRING, &opt.local_conf, 0, 
	 "Configuration file", "FILE"},
	{"log", 'l', POPT_ARG_INT, &log_info.log_level, 0,
	 "Verbosity of logging", "LEVEL"},
	{"port", 'p', POPT_ARG_INT, &opt.main_port, 0,
	 "GGZ port number", "PORT"},
	{"version", 'V', POPT_ARG_NONE, NULL, 1},
	POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}	/* end the list */
};

/* Parse command-line options */
void parse_args(int argc, char *argv[])
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
			dbg_msg("Reading local conf file : %s", opt.local_conf);
			parse_file(configfile);
		} else
			err_msg("WARNING:  Local conf file not found!");
	} else {
		if((tempstr=malloc(strlen(SYSCONFDIR)+11)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");

		strcpy(tempstr, SYSCONFDIR);   /* If this changes be sure to */
		strcat(tempstr, "/ggzd.conf"); /* change the malloc() above! */

		if((configfile=fopen(tempstr,"r"))) {
			dbg_msg("Reading global conf file : %s", tempstr);
			parse_file(configfile);
		} else
			err_msg("WARNING:  No configuration file loaded!");

		free(tempstr);
	}

	/* Add any defaults which were not config'ed */

	/* If no game_dir, default it to SYSCONFDIR */
	if(!opt.game_dir) {
		if((tempstr=malloc(strlen(SYSCONFDIR)+1)) == NULL)
			err_sys_exit("malloc error in parse_conf_file()");
		strcpy(tempstr, SYSCONFDIR);
		opt.game_dir = tempstr;
	}

	/* If no main_port, default it to 5688 */
	if(!opt.main_port)
		opt.main_port = 5688;

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
		dbg_msg("  found '%s, %s'", varname, varvalue);

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
				parse_put_add_ignore_list(varvalue);
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
				parse_put_add_ignore_list(varvalue);
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

		/*** LOGLEVEL = X ***/
		if(!strcmp(varname, "loglevel")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0) {
				PARSE_ERR("Invalid LogLevel specified");
				continue;
			}
			if(log_info.log_level == -1)
				log_info.log_level = intval;
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
		
		/*** DEBUGLEVEL = X ***/
		if(!strcmp(varname, "debuglevel")) {
#ifndef DEBUG
			PARSE_ERR("Debugging not enabled");
			continue;
#else
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0) {
				PARSE_ERR("Invalid DebugLevel specified");
				continue;
			}
			if(log_info.dbg_level == -1)
				log_info.dbg_level = intval;
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
			if(intval < 0 || intval > 1) {
				PARSE_ERR("Invalid value for PIDInLogs");
				continue;
			}
			log_info.include_pid = intval;
			continue;
		}

		/*** HOSTNAMELOOKUP = 0,1 ***/
		if(!strcmp(varname, "hostnamelookup")) {
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
		}

		/*** TIMEINLOGS = 0,1 ***/
		if(!strcmp(varname, "timeinlogs")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0 || intval > 1) {
				PARSE_ERR("Invalid value for TimeInLogs");
				continue;
			}
			log_info.include_timestamp = intval;
			continue;
		}

		/*** INVALID VARIABLE ***/
		PARSE_ERR("Syntax error");
	}


	fclose(configfile);

#ifdef DEBUG
	{
	  AddIgnoreStruct *AI = add_ignore_list;
	  if(AI) {
	    if(add_all_games == 'T')
	      dbg_msg("Game ignore list:");
	    else
	      dbg_msg("Game add list:");
	    while(AI)
	    {
	      dbg_msg("  %s", AI->name);
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
	int num_games, i;
	int addit;

	if(add_all_games == 'F') {
		/* Go through all games explicitly included in the add list */
		dbg_msg("Adding games in add list");
		game = add_ignore_list;
		while(game) {
			parse_game(game->name);
			game = game->next;
		}
	} else {
		/* Scan for all .dsc files in the game_dir */
		dbg_msg("Addding all games in %s", opt.game_dir);
		num_games = scandir(opt.game_dir, &namelist, parse_dselect, 0);
		for(i=0; i<num_games; i++) {
			/* Make a temporary copy of the name w/o .dsc */
			if((name=malloc(strlen(namelist[i]->d_name)+1)) == NULL)
			       err_sys_exit("malloc error in parse_game_files");
			strcpy(name, namelist[i]->d_name);
			name[strlen(name)-4] = '\0';
			/* Check to see if this game is on the ignore list */
			game = add_ignore_list;
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
				parse_game(name);
			else
				dbg_msg("Ignoring game %s", name);

			free(name);
		}
	}

	parse_cleanup_add_ignore_list();
}


/* Parse a single game file, adding it's values to the game table */
static void parse_game(char *name)
{
	char *fname;
	FILE *gamefile;
	GameInfo *game_info;
	char line[256];
	int linenum = 0;
	int intval;
	char players_bits[] = { PLAY_ALLOW_ZERO, PLAY_ALLOW_ONE, PLAY_ALLOW_TWO,
		PLAY_ALLOW_THREE, PLAY_ALLOW_FOUR, PLAY_ALLOW_FIVE,
		PLAY_ALLOW_SIX, PLAY_ALLOW_SEVEN, PLAY_ALLOW_EIGHT };

	/* Allocate space and setup a full pathname to description file */
	if((fname = malloc(strlen(name)+strlen(opt.game_dir)+6)) == NULL)
		err_sys_exit("malloc error in parse_game()");
	sprintf(fname, "%s/%s.dsc", opt.game_dir, name);

	if((gamefile = fopen(fname, "r")) == NULL) {
		err_msg("Ignoring %s, could not open %s", name, fname);
		free(fname);
		return;
	}

	dbg_msg("Adding game %s from %s", name, fname);

	/* Allocate a game_info struct for this game and default to enabled */
	if((game_info = malloc(sizeof(GameInfo))) == NULL)
		err_sys_exit("malloc error in parse_game()");
	game_info->enabled = 1;
	game_info->launch = NULL;

	while(fgets(line, 256, gamefile)) {
		linenum++;
		parse_line(line);
		if(varname == NULL)
			continue; /* Blank line or comment */
		dbg_msg("  found '%s, %s'", varname, varvalue);

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
			game_info->num_play_allow |= players_bits[intval];
		}

		/*** BotsAllowed = # ***/
		if(!strcmp(varname, "botsallowed")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 1 || intval > 8) {
				PARSE_ERR("BotAllowed value invalid");
				continue;
			}
			game_info->comp_allow |= players_bits[intval];
		}

		/*** OptionsSize = # ***/
		if(!strcmp(varname, "optionssize")) {
			if(varvalue == NULL) {
				PARSE_ERR("Syntax error");
				continue;
			}
			intval = atoi(varvalue);
			if(intval < 0) {
				PARSE_ERR("OptionsSize value invalid");
				continue;
			}
			game_info->options_size = intval;
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

		/*** GameDisabled ***/
		if(!strcmp(varname, "gamedisabled"))
			game_info->enabled = 0;
	}

	game_types.info[game_types.count] = *game_info;
	game_types.count++;

	free(game_info);
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
static void parse_put_add_ignore_list(char *s)
{
	AddIgnoreStruct *new_listitem;
	char *newstring;

	if((new_listitem=malloc(sizeof(AddIgnoreStruct))) == NULL)
		err_sys_exit("malloc error in parse_put_add_ignore_list()");
	if((newstring=malloc(strlen(s)+1)) == NULL)
		err_sys_exit("malloc error in parse_put_add_ignore_list()");

	strcpy(newstring, s);
	new_listitem->name = newstring;
	new_listitem->next = add_ignore_list;
	add_ignore_list = new_listitem;
}


/* Cleanup the Add/Ignore list after we are finished */
static void parse_cleanup_add_ignore_list(void)
{
	AddIgnoreStruct *temp;

	while((temp = add_ignore_list)) {
		free(add_ignore_list->name);
		add_ignore_list = add_ignore_list->next;
		free(temp);
	}
}


/* Return 1 if filename matches our pattern (ends in '.dsc') */
static int parse_dselect(struct dirent *dent)
{
	return(!strcmp(".dsc", dent->d_name+strlen(dent->d_name)-4));
}
