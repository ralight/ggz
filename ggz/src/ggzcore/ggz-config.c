/*
 * File: ggz-config.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 02/19/01
 *
 * Configuration query and module install program.
 *
 * Copyright (C) 2001 Rich Gade.
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

#include <stdio.h>
#include <string.h>
#include <popt.h>

#include "config.h"
#include "ggzcore.h"
#include "confio.h"


int open_conffile(char *global_filename)
{
	char	*global_pathname;
	int	global;

	if(global_filename == NULL)
		global_filename = "ggz.rc";

	global_pathname = malloc(strlen(GGZCONFDIR) + strlen(global_filename) + 2);
	if(global_pathname == NULL) {
		fprintf(stderr, "malloc failure\n");
		return -1;
	}
	sprintf(global_pathname, "%s/%s", GGZCONFDIR, global_filename);

	global = ggzcore_confio_parse(global_pathname, GGZ_CONFIO_RDONLY);
	if(global < 0) {
		fprintf(stderr, "Invalid GGZ installation, please reinstall\n");
		return -1;
	}
	global = ggzcore_confio_parse(global_pathname, GGZ_CONFIO_RDWR);
	if(global < 0) {
		fprintf(stderr, "Insufficient permission to install modules\n");
		_ggzcore_confio_cleanup();
		return -1;
	}

	return global;
}


int remove_module(const char *name, char *global_filename)
{
	char	*temp;
	int	global, rc;

	if((global = open_conffile(global_filename)) < 0)
		return global;
	temp = ggzcore_confio_read_string(global, name, "Version", NULL);
	if(temp == NULL) {
		fprintf(stderr, "Warning: Tried to remove nonexistant module\n");
		_ggzcore_confio_cleanup();
		return -1;
	}

	rc = ggzcore_confio_remove_section(global, name);
	rc |= ggzcore_confio_commit(global);
	if(rc != 0)
		fprintf(stderr, "Module removal failed, see documentation");

	_ggzcore_confio_cleanup();

	return rc;
}


int install_module(const char *name, const char *version,
		   const char *executable, char *global_filename,
		   const int force)
{
	char	*temp;
	int	global, rc;

	if((global = open_conffile(global_filename)) < 0)
		return global;

	if(!force) {
		temp = ggzcore_confio_read_string(global, name, "Version", NULL);
		if(temp != NULL) {
			fprintf(stderr, "Cannot overwrite existing module\n");
			_ggzcore_confio_cleanup();
			return -1;
		}
	}

	rc = ggzcore_confio_write_string(global, name, "Version", version);
	rc |= ggzcore_confio_write_string(global, name, "Path", executable);
	rc |= ggzcore_confio_commit(global);
	if(rc != 0)
		fprintf(stderr, "Module installation failed, see documentation");

	_ggzcore_confio_cleanup();

	return rc;
}


/* POPT Arguments Stuff */
#define QUERY_CONFIG	1
#define QUERY_GAMEDIR	2
#define QUERY_VERSION	3
#define INSTALL_MODULE	4
#define REMOVE_MODULE	5
#define MODNAME		6
#define MODVERS		7
#define MODEXEC		8
#define MODGLOB		9
static char *modname = NULL;
static char *modvers = NULL;
static char *modexec = NULL;
static char *modglob = NULL;
static int modforce = 0;
static int install_mod = 0;
static int remove_mod = 0;
static int did_query = 0;
static const struct poptOption args[] = {
	{"configdir",	'c',	POPT_ARG_NONE,	&did_query,	QUERY_CONFIG,
	 "Query GGZCONFDIR - location of configuration directory"},
	{"gamedir",	'g',	POPT_ARG_NONE,	&did_query,	QUERY_GAMEDIR,
	 "Query GGZGAMEDIR - location of game modules directory"},
	{"version",	'v',	POPT_ARG_NONE,	&did_query,	QUERY_VERSION,
	 "Query VERSION - version identifier of ggzcore files"},
	{"install",	'\0',	POPT_ARG_NONE,	&install_mod,	0,
	 "Install a module"},
	{"remove",	'\0',	POPT_ARG_NONE,	&remove_mod,	0,
	 "Remove a module"},

	{"modname",	'\0',	POPT_ARG_STRING,	&modname,	MODNAME,
	 "[INSTALL | REMOVE] (RQD) - set the game name", "NAME"},
	{"conffile",	'\0',	POPT_ARG_STRING,	&modglob,	MODGLOB,
	 "[INSTALL | REMOVE] (OPT) - set the configuration filename - note that it must reside in GGZCONFDIR", "FILENAME"},
	{"modversion",	'\0',	POPT_ARG_STRING,	&modvers,	MODVERS,
	 "[INSTALL] (RQD) - set the game version", "VERSION"},
	{"modexec",	'\0',	POPT_ARG_STRING,	&modexec,	MODEXEC,
	 "[INSTALL] (RQD) - set the executable filename", "FILENAME"},

	{"force",	'\0',	POPT_ARG_NONE,	&modforce,	0,
	 "[INSTALL] (OPT) - overwrite an existing module"},

	POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
};


int main(const int argc, const char **argv)
{
	poptContext	context;
	int		rc;
	int		install_name=0, install_vers=0,
			install_exec=0, install_glob=0;

	context = poptGetContext(NULL, argc, argv, args, 0);
	while((rc = poptGetNextOpt(context)) != -1) {
		switch(rc) {
			case QUERY_CONFIG:
				printf("%s\n", GGZCONFDIR);
				break;
			case QUERY_GAMEDIR:
				printf("%s\n", GAMEDIR);
				break;
			case QUERY_VERSION:
				printf("%s\n", VERSION);
				break;
			case MODNAME:
				install_name++;
				break;
			case MODVERS:
				install_vers++;
				break;
			case MODEXEC:
				install_exec++;
				break;
			case MODGLOB:
				install_glob++;
				break;

			default:
				fprintf(stderr, "%s: %s\n",
					poptBadOption(context, 0),
					poptStrerror(rc));
				poptFreeContext(context);
				return -1;
				break;
		}
	}

	if(install_mod == 0 && remove_mod == 0) {
		if(!did_query)
			fprintf(stderr, "Try '%s --help' for help\n", argv[0]);
		return 0;
	}

	if(install_mod > 0 &&
	   (install_glob > 1 || remove_mod > 0 ||
	   (install_name == 0 || install_name > 1) ||
	   (install_vers == 0 || install_vers > 1) ||
	   (install_exec == 0 || install_exec > 1))) {
		fprintf(stderr, "Error: redundant or missing arguments for module installation\n");
		return -1;
	}

	if(remove_mod > 0 &&
	   (install_glob > 1 || install_mod > 0 || install_vers > 0 ||
	    install_exec > 0 ||
	   (install_name == 0 || install_name > 1))) {
		fprintf(stderr, "Error: redundant or missing arguments for module removal\n");
		return -1;
	}

	if(install_mod)
		rc = install_module(modname, modvers, modexec, modglob, modforce);
	else if(remove_mod)
		rc = remove_module(modname, modglob);

	return rc;
}
