/*
 * File: ggzrc.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 03/02/2000
 *
 * This fils contains functions for connecting with the server
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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "ggzrc.h"
#include "err_func.h"


/* Local use only functions */
static FILE *ggzrc_open_rc(void);


/* Initialize and read in the configuration file */
int ggzrc_initialize(void)
{
	FILE *rc_file;

	if((rc_file = ggzrc_open_rc()) == NULL) {
		dbg_msg("ggzrc: Unable to located ggz.rc or .ggzrc");
		return -1;
	}

	return 0;
}


/* Locate the rc file to use */
static FILE *ggzrc_open_rc(void)
{
	FILE *rc_file;
	char *home;
	char *tempstr=NULL;

	/* Look for a user level rc file first */
	if((home = getenv("HOME")) != NULL)
		tempstr = g_strconcat(home, "/.ggzrc", NULL);
	if(tempstr && (rc_file=fopen(tempstr, "r")))
		dbg_msg("ggzrc: Using %s/.ggzrc", home);
	else {
		/* Look for an admin level rc file */
		if(tempstr)
			g_free(tempstr);
		tempstr = g_strconcat(SYSCONFDIR, "/ggz.rc", NULL);
		if((rc_file=fopen(tempstr, "r")))
			dbg_msg("ggzrc: Using %s", tempstr);
	}

	g_free(tempstr);
	return rc_file;
}


#if 0
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

#endif /*0*/
