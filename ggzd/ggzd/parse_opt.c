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

#include <datatypes.h>
#include <err_func.h>

extern Options opt;

/* Private file parsing functions */
static void parse_file(FILE *);
static void parse_line(char *);

/* Module local variables for parsing */
static char *varname;
static char *varvalue;

static const struct poptOption args[] = {
	
	{"file", 'f', POPT_ARG_STRING, &opt.local_conf, 0, 
	 "Configuration file", "FILE"},
	{"log", 'l', POPT_ARG_INT, &opt.log_level, 0,
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
	char *cfname;

	if (opt.local_conf) {
		if((configfile=fopen(opt.local_conf,"r"))) {
			dbg_msg("Reading local conf file : %s", opt.local_conf);
			parse_file(configfile);

			return;
		} else
			err_msg("WARNING:  Local conf file not found!");
	}

	if((cfname=malloc(strlen(SYSCONFDIR)+11)) == NULL)
		err_sys_exit("malloc error in parse_conf_file()");

	strcpy(cfname, SYSCONFDIR);	/* If this changes, be sure to change */
	strcat(cfname, "/ggzd.conf");	/* the malloc() above!!!!             */

	if((configfile=fopen(cfname,"r"))) {
		dbg_msg("Reading global conf file : %s", cfname);
		parse_file(configfile);
	} else
		err_msg("WARNING:  No configuration file loaded!");

	free(cfname);
}


/* Convience macro for parse_file() */
#define PARSE_ERR(s)  err_msg("Config file: %s, line %d", s, linenum)

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
		dbg_msg("parse_line file found '%s, %s'\n", varname, varvalue);

		/* Apply the configuration line, oh to be able to do */
		/* a case construct with strings :)                  */

		/*** PORT = X ***/
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
		/*** END PORT = X ***/

		/*** GAMEDIR = DIR ***/
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
		/*** END GAMEDIR = DIR ***/

		/*** INVALID VARIABLE ***/
		PARSE_ERR("Syntax error");
	}

	fclose(configfile);
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
