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
#include <stdlib.h>

#include "config.h"
#include "ggzcore.h"
#include "confio.h"


/* POPT Arguments Stuff */
#define QUERY_CONFIG	1
#define QUERY_GAMEDIR	2
#define QUERY_VERSION	3
#define QUERY_DATADIR	4
static char *modname = NULL;
static char *modversion = NULL;
static char *modexec = NULL;
static char *modui = NULL;
static char *modproto = NULL;
static char *modauthor = NULL;
static char *modurl = NULL;
static char *modicon = NULL;
static char *modhelp = NULL;
static char *fromfile = NULL;
static int modforce = 0;
static int moddest = 0;
static char *destdir = NULL;
static int install_mod = 0;
static int remove_mod = 0;
static int did_query = 0;
static const struct poptOption args[] = {
	{"configdir",	'c',	POPT_ARG_NONE,	&did_query,	QUERY_CONFIG,
	 "Query GGZCONFDIR - location of configuration directory"},
	{"gamedir",	'g',	POPT_ARG_NONE,	&did_query,	QUERY_GAMEDIR,
	 "Query GGZGAMEDIR - location of game modules directory"},
	{"datadir",	'd',	POPT_ARG_NONE,	&did_query,	QUERY_DATADIR,
	 "Query GGZDATADIR - location of game data directory"},
	{"version",	'v',	POPT_ARG_NONE,	&did_query,	QUERY_VERSION,
	 "Query VERSION - version identifier of ggzcore files"},
	{"install",	'\0',	POPT_ARG_NONE,	&install_mod,	0,
	 "Install a module"},
	{"remove",	'\0',	POPT_ARG_NONE,	&remove_mod,	0,
	 "Remove a module"},

	{"modname",	'\0',	POPT_ARG_STRING,	&modname,	0,
	 "[INSTALL | REMOVE] (RQD) - set the game name", "NAME"},
	{"modauthor",	'\0',	POPT_ARG_STRING,	&modauthor,	0,
	 "[INSTALL | REMOVE] (RQD) - author of the module", "AUTHOR"},
	{"modui",	'\0',	POPT_ARG_STRING,	&modui,		0,
	 "[INSTALL | REMOVE] (RQD) - the modules UI setting", "<gtk | kde>"},
	{"modexec",	'\0',	POPT_ARG_STRING,	&modexec,	0,
	 "[INSTALL] (RQD) - set the executable filename", "FILENAME"},
	{"modproto",	'\0',	POPT_ARG_STRING,	&modproto,	0,
	 "[INSTALL] (RQD) - protocol version of module", "VERSION"},
	{"modurl",	'\0',	POPT_ARG_STRING,	&modurl,	0,
	 "[INSTALL] (RQD) - homepage to find the module", "URL"},
	{"modversion",	'\0',	POPT_ARG_STRING,	&modversion,	0,
	 "[INSTALL] (RQD) - set the module version", "VERSION"},
	{"modicon",	'\0',	POPT_ARG_STRING,	&modicon,	0,
	 "[INSTALL] (OPT) - set path to module icon", "FILENAME"},
	{"modhelp",	'\0',	POPT_ARG_STRING,	&modhelp,	0,
	 "[INSTALL] (OPT) - set path to main help file", "FILENAME"},

	{"fromfile",	'\0',	POPT_ARG_STRING,	&fromfile,	0,
	 "[INSTALL] - load RQD/OPT parameters from a file", "FILENAME"},

	{"force",	'\0',	POPT_ARG_NONE,	&modforce,	0,
	 "[INSTALL] (OPT) - overwrite an existing module"},

	{"destdir",	'D',	POPT_ARG_NONE,	&moddest,	0,
	 "[INSTALL] (OPT) - use $DESTDIR as offset to config file"},

	POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
};


void load_fromfile(void)
{
	int from;

	from = ggzcore_confio_parse(fromfile, GGZ_CONFIO_RDONLY);

	if(modname == NULL) 
		modname = ggzcore_confio_read_string(from, "ModuleInfo",
						     "Name", NULL);
	if(modversion == NULL)
		modversion = ggzcore_confio_read_string(from, "ModuleInfo",
							"Version", NULL);
	if(modexec == NULL)
		modexec = ggzcore_confio_read_string(from, "ModuleInfo",
						     "CommandLine", NULL);
	if(modui == NULL)
		modui = ggzcore_confio_read_string(from, "ModuleInfo",
						   "Frontend", NULL);
	if(modproto == NULL)
		modproto = ggzcore_confio_read_string(from, "ModuleInfo",
						      "Protocol", NULL);
	if(modauthor == NULL)
		modauthor = ggzcore_confio_read_string(from, "ModuleInfo",
						       "Author", NULL);
	if(modurl == NULL)
		modurl = ggzcore_confio_read_string(from, "ModuleInfo",
						    "Homepage", NULL);
	if(modicon == NULL)
		modicon = ggzcore_confio_read_string(from, "ModuleInfo",
						     "IconPath", NULL);
	if(modhelp == NULL)
		modhelp = ggzcore_confio_read_string(from, "ModuleInfo",
						     "HelpPath", NULL);
}


int purge_module_name(int global)
{
	int items;
	char **name_list;
	int index;

	ggzcore_confio_read_list(global, "Games", "*GameList*",&items,&name_list);

	for(index=0; index<items; index++)
		if(!strcmp(name_list[index], modname))
			break;

	if(index != items-1)
		name_list[index] = name_list[items-1];
	items--;

	if(items != 0)
		ggzcore_confio_write_list(global, "Games", "*GameList*",
					  items, name_list);
	else
		ggzcore_confio_remove_key(global, "Games", "*GameList*");

	return 0;
}


char *new_module_id(int global)
{
	char **name_list, **mod_list;
	int names, mods;
	int hi=0, t;
	int i, j;
	static char new_id[10];

	ggzcore_confio_read_list(global, "Games", "*GameList*",&names,&name_list);

	for(i=0; i<names; i++) {
		ggzcore_confio_read_list(global, "Games", name_list[i],
						 &mods, &mod_list);
		for(j=0; j<mods; j++) {
			t = atoi(mod_list[j] + 1);
			if(t > hi)
				hi = t;
		}
	}

	snprintf(new_id, 10, "g%d", hi+1);

	return new_id;
}


char *get_module_id(int global)
{
	int items;
	char **mod_list;
	char *module_id=NULL;
	char *author, *ui, *version;
	int index;

	ggzcore_confio_read_list(global, "Games", modname, &items, &mod_list);
	if(items == 0)
		return NULL;

	for(index=0; index<items; index++) {
		module_id = mod_list[index];
		author = ggzcore_confio_read_string(global, module_id,
						    "Author", NULL);
		ui = ggzcore_confio_read_string(global, module_id,
						"Frontend", NULL);
		if(modversion) {
			version = ggzcore_confio_read_string(global, module_id,
							     "Version", NULL);
			if(!strcmp(author, modauthor) && !strcmp(ui, modui) &&
			   !strcmp(version, modversion))
				break;
		} else if(!strcmp(author, modauthor) && !strcmp(ui, modui))
			break;
	}

	if(index >= items)
		return NULL;
	else
		return module_id;
}


void purge_module_id(int global, char *module_id)
{
	int items;
	char **mod_list;
	int index;

	ggzcore_confio_read_list(global, "Games", modname, &items, &mod_list);

	for(index=0; index<items; index++)
		if(!strcmp(mod_list[index], module_id))
			break;

	if(index != items-1)
		mod_list[index] = mod_list[items-1];
	items--;

	if(items != 0)
		ggzcore_confio_write_list(global, "Games", modname,
					  items, mod_list);
	else {
		ggzcore_confio_remove_key(global, "Games", modname);
		purge_module_name(global);
	}
}


int open_conffile(void)
{
	char	*global_pathname;
	char	*global_filename = "ggz.modules";
	int	global;

	if(moddest)
		global_pathname = malloc(strlen(destdir) +
					 strlen(GGZCONFDIR) +
					 strlen(global_filename) + 3);
	else
		global_pathname = malloc(strlen(GGZCONFDIR) +
					 strlen(global_filename) + 2);
	if(global_pathname == NULL) {
		fprintf(stderr, "malloc failure\n");
		return -1;
	}

	if(moddest)
		sprintf(global_pathname, "%s/%s/%s", destdir,
					  GGZCONFDIR, global_filename);
	else
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


int remove_module(void)
{
	char	*module_id;
	int	global, rc;

	if((global = open_conffile()) < 0)
		return global;

	module_id = get_module_id(global);

	if(module_id == NULL) {
		fprintf(stderr,"Warning: Tried to remove nonexistant module\n");
		_ggzcore_confio_cleanup();
		return -1;
	}

	rc = ggzcore_confio_remove_section(global, module_id);
	if(rc == 0) {
		purge_module_id(global, module_id);
		rc = ggzcore_confio_commit(global);
	}

	if(rc != 0) {
		fprintf(stderr, "ggz.modules configuration may be corrupt\n");
		fprintf(stderr, "Module removal failed, see documentation\n");
	}

	_ggzcore_confio_cleanup();

	return rc;
}


int install_module(void)
{
	char	*module_id;
	int	global, rc;
	char	*mod_list, *game_list;
	char	bigstr[1024];

	if((global = open_conffile()) < 0)
		return global;

	module_id = get_module_id(global);
	if(!modforce) {
		if(module_id != NULL) {
			fprintf(stderr, "Cannot overwrite existing module\n");
			_ggzcore_confio_cleanup();
			return -1;
		}
	}

	if(module_id == NULL) {
		module_id = new_module_id(global);
		modforce = 0;
	}

	rc = ggzcore_confio_write_string(global, module_id, "Name", modname);
	if(rc == 0) {
		ggzcore_confio_write_string(global, module_id,
					    "Protocol", modproto);
		ggzcore_confio_write_string(global, module_id,
					    "Frontend", modui);
		ggzcore_confio_write_string(global, module_id,
					    "Version", modversion);
		ggzcore_confio_write_string(global, module_id,
					    "Author", modauthor);
		ggzcore_confio_write_string(global, module_id,
					    "CommandLine", modexec);
		ggzcore_confio_write_string(global, module_id,
					    "Homepage", modurl);
		if(modicon)
			ggzcore_confio_write_string(global, module_id,
						    "IconPath", modicon);
		if(modhelp)
			ggzcore_confio_write_string(global, module_id,
						    "HelpPath", modhelp);

		mod_list = ggzcore_confio_read_string(global, "Games",
						      modname, NULL);
		if(mod_list == NULL && !modforce) {
			ggzcore_confio_write_string(global, "Games",
						    modname, module_id);
			game_list = ggzcore_confio_read_string(global, "Games",
						    "*GameList*", NULL);
			if(game_list == NULL)
				ggzcore_confio_write_string(global, "Games",
						    "*GameList*", modname);
			else {
				snprintf(bigstr, 1024, "%s %s",
					 game_list, modname);
				ggzcore_confio_write_string(global, "Games",
						    "*GameList*", bigstr);
			}
		} else if(!modforce) {
			snprintf(bigstr, 1024, "%s %s", mod_list, module_id);
			ggzcore_confio_write_string(global, "Games",
					    	modname, bigstr);
		}

		rc = ggzcore_confio_commit(global);
	}

	if(rc != 0) {
		fprintf(stderr, "ggz.modules configuration may be corrupt\n");
		fprintf(stderr, "Module installation failed, see documentation");
	}

	_ggzcore_confio_cleanup();

	return rc;
}


int main(const int argc, const char **argv)
{
	poptContext	context;
	int		rc;

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
			case QUERY_DATADIR:
				printf("%s\n", GGZDATADIR);
				break;
			default:
				fprintf(stderr, "%s: %s\n",
					poptBadOption(context, 0),
					poptStrerror(rc));
				poptFreeContext(context);
				return 1;
				break;
		}
	}

	if(install_mod + remove_mod != 1) {
		if(!did_query) {
			fprintf(stderr, "Try '%s --help' for help\n", argv[0]);
			return 1;
		}
		return 0;
	}

	if(fromfile != NULL)
		load_fromfile();

	if(modname == NULL || modauthor == NULL || modui == NULL) {
		fprintf(stderr, "Required arguments missing\n");
		return 1;
	}

	if(moddest) {
		destdir = getenv("DESTDIR");
		if(destdir == NULL)
			moddest = 0;
	}

	if(install_mod) {
		if(modversion == NULL || modexec == NULL ||
		   modproto == NULL || modurl == NULL) {
			fprintf(stderr, "Required arguments missing\n");
			return 1;
		}
		rc = install_module();
	} else if(remove_mod)
		rc = remove_module();

	return rc;
}
