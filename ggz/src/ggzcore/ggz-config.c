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
#include "protocol.h"


/* POPT Arguments Stuff */
#define QUERY_CONFIG	1
#define QUERY_GAMEDIR	2
#define QUERY_VERSION	3
#define QUERY_DATADIR	4
#define QUERY_PVERSION	5
static char *modname = NULL;
static char *modversion = NULL;
static char *modpengine = NULL;
static char *modpversion = NULL;
static char *modpsupport = NULL;
static char *modexec = NULL;
static char *modui = NULL;
static char *modauthor = NULL;
static char *modurl = NULL;
static char *modicon = NULL;
static char *modhelp = NULL;
static char *modfile = NULL;
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
	{"protocol",	'p',	POPT_ARG_NONE,	&did_query,	QUERY_PVERSION,
	 "Query GGZ_CS_PROTO_VERSION - version of core protocol"},

	{"install",	'\0',	POPT_ARG_NONE,	&install_mod,	0,
	 "Install a module"},
	{"remove",	'\0',	POPT_ARG_NONE,	&remove_mod,	0,
	 "Remove a module"},

	{"modfile",	'\0',	POPT_ARG_STRING,	&modfile,	0,
	 "Specifies module installation file", "FILENAME"},

	{"force",	'\0',	POPT_ARG_NONE,	&modforce,	0,
	 "Install over an existing module"},

	{"destdir",	'D',	POPT_ARG_NONE,	&moddest,	0,
	 "Use $DESTDIR as offset to ggz.modules file"},

	POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
};


int load_modfile(void)
{
	int from;
	int status = 1;

	from = ggzcore_confio_parse(modfile, GGZ_CONFIO_RDONLY);

	modname = ggzcore_confio_read_string(from, "ModuleInfo",
					     "Name", NULL);
	if(modname == NULL) {
		fprintf(stderr, "Critical: Module name not specified.\n");
		status = 0;
	}

	modversion = ggzcore_confio_read_string(from, "ModuleInfo",
						"Version", NULL);
	if(modversion == NULL) {
		fprintf(stderr, "Critical: Module version not specified.\n");
		status = 0;
	}

	modexec = ggzcore_confio_read_string(from, "ModuleInfo",
					     "CommandLine", NULL);
	if(modexec == NULL) {
		fprintf(stderr, "Critical: Executable not specified.\n");
		status = 0;
	}

	modui = ggzcore_confio_read_string(from, "ModuleInfo",
					   "Frontend", NULL);
	if(modui == NULL) {
		fprintf(stderr, "Critical: User interface not specified.\n");
		status = 0;
	}

	modpengine = ggzcore_confio_read_string(from, "ModuleInfo",
					        "ProtocolEngine", NULL);
	if(modpengine == NULL) {
		fprintf(stderr, "Critical: Protocol engine not specified.\n");
		status = 0;
	}

	modpversion = ggzcore_confio_read_string(from, "ModuleInfo",
						 "ProtocolVersion", NULL);
	if(modpversion == NULL) {
		fprintf(stderr, "Critical: Protocol version not specified.\n");
		status = 0;
	}

	modauthor = ggzcore_confio_read_string(from, "ModuleInfo",
					       "Author", NULL);
	if(modauthor == NULL) {
		fprintf(stderr, "Critical: Module author not specified.\n");
		status = 0;
	}

	modurl = ggzcore_confio_read_string(from, "ModuleInfo",
					    "Homepage", NULL);
	if(modurl == NULL)
		fprintf(stderr, "Warning: Module homepage not specified.\n");

	modpsupport = ggzcore_confio_read_string(from, "ModuleInfo",
						 "SupportedGames", NULL);
	modicon = ggzcore_confio_read_string(from, "ModuleInfo",
					     "IconPath", NULL);
	modhelp = ggzcore_confio_read_string(from, "ModuleInfo",
					     "HelpPath", NULL);

	return status;
}


int purge_engine_name(int global)
{
	int items;
	char **engine_list;
	int index;

	ggzcore_confio_read_list(global, "Games", "*Engines*",
				 &items, &engine_list);

	for(index=0; index<items; index++)
		if(!strcmp(engine_list[index], modpengine))
			break;

	if(index != items-1)
		engine_list[index] = engine_list[items-1];
	items--;

	if(items != 0)
		ggzcore_confio_write_list(global, "Games", "*Engines*",
					  items, engine_list);
	else
		ggzcore_confio_remove_key(global, "Games", "*Engines*");

	return 0;
}


char *new_engine_id(int global)
{
	char **engine_list, **engine_id_list;
	int engines, ids;
	int hi=0, t;
	int i, j;
	static char new_id[10];

	ggzcore_confio_read_list(global, "Games", "*Engines*",
				 &engines, &engine_list);

	for(i=0; i<engines; i++) {
		ggzcore_confio_read_list(global, "Games", engine_list[i],
						 &ids, &engine_id_list);
		for(j=0; j<ids; j++) {
			t = atoi(engine_id_list[j] + 1);
			if(t > hi)
				hi = t;
		}
	}

	snprintf(new_id, 10, "p%d", hi+1);

	return new_id;
}


char *get_engine_id(int global)
{
	int items;
	char **engine_list;
	char *engine_id=NULL;
	char *author, *ui, *version;
	int index;

	ggzcore_confio_read_list(global, "Games", modpengine,
				 &items, &engine_list);
	if(items == 0)
		return NULL;

	for(index=0; index<items; index++) {
		engine_id = engine_list[index];
		author = ggzcore_confio_read_string(global, engine_id,
						    "Author", NULL);
		ui = ggzcore_confio_read_string(global, engine_id,
						"Frontend", NULL);
		if(modversion) {
			version = ggzcore_confio_read_string(global, engine_id,
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
		return engine_id;
}


void purge_engine_id(int global, char *engine_id)
{
	int items;
	char **engine_list;
	int index;

	ggzcore_confio_read_list(global, "Games", modpengine,
				 &items, &engine_list);

	for(index=0; index<items; index++)
		if(!strcmp(engine_list[index], engine_id))
			break;

	if(index != items-1)
		engine_list[index] = engine_list[items-1];
	items--;

	if(items != 0)
		ggzcore_confio_write_list(global, "Games", modpengine,
					  items, engine_list);
	else {
		ggzcore_confio_remove_key(global, "Games", modpengine);
		purge_engine_name(global);
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
	char	*engine_id;
	int	global, rc;

	if((global = open_conffile()) < 0)
		return global;

	engine_id = get_engine_id(global);

	if(engine_id == NULL) {
		fprintf(stderr,"Warning: Tried to remove nonexistant module\n");
		_ggzcore_confio_cleanup();
		return -1;
	}

	rc = ggzcore_confio_remove_section(global, engine_id);
	if(rc == 0) {
		purge_engine_id(global, engine_id);
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
	char	*engine_id;
	int	global, rc;
	char	*engine_id_list, *engine_list;
	char	bigstr[1024];

	if((global = open_conffile()) < 0)
		return global;

	engine_id = get_engine_id(global);
	if(!modforce) {
		if(engine_id != NULL) {
			fprintf(stderr, "Cannot overwrite existing module\n");
			_ggzcore_confio_cleanup();
			return -1;
		}
	}

	if(engine_id == NULL) {
		engine_id = new_engine_id(global);
		modforce = 0;
	}

	rc = ggzcore_confio_write_string(global, engine_id, "Name", modname);
	if(rc == 0) {
		ggzcore_confio_write_string(global, engine_id,
					    "ProtocolEngine", modpengine);
		ggzcore_confio_write_string(global, engine_id,
					    "ProtocolVersion", modpversion);
		ggzcore_confio_write_string(global, engine_id,
					    "Frontend", modui);
		ggzcore_confio_write_string(global, engine_id,
					    "Version", modversion);
		ggzcore_confio_write_string(global, engine_id,
					    "Author", modauthor);
		ggzcore_confio_write_string(global, engine_id,
					    "CommandLine", modexec);
		ggzcore_confio_write_string(global, engine_id,
					    "Homepage", modurl);
		if(modpsupport)
			ggzcore_confio_write_string(global, engine_id,
						 "SupportedGames", modpsupport);
		if(modicon)
			ggzcore_confio_write_string(global, engine_id,
						    "IconPath", modicon);
		if(modhelp)
			ggzcore_confio_write_string(global, engine_id,
						    "HelpPath", modhelp);

		engine_id_list = ggzcore_confio_read_string(global, "Games",
						            modpengine, NULL);
		if(engine_id_list == NULL && !modforce) {
			ggzcore_confio_write_string(global, "Games",
						    modpengine, engine_id);
			engine_list = ggzcore_confio_read_string(global,"Games",
						    "*Engines*", NULL);
			if(engine_list == NULL)
				ggzcore_confio_write_string(global, "Games",
						    "*Engines*", modpengine);
			else {
				snprintf(bigstr, 1024, "%s %s",
					 engine_list, modpengine);
				ggzcore_confio_write_string(global, "Games",
						    "*Engines*", bigstr);
			}
		} else if(!modforce) {
			snprintf(bigstr, 1024, "%s %s",
				 engine_id_list, engine_id);
			ggzcore_confio_write_string(global, "Games",
					    	modpengine, bigstr);
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
				return 0;
			case QUERY_GAMEDIR:
				printf("%s\n", GAMEDIR);
				return 0;
			case QUERY_VERSION:
				printf("%s\n", VERSION);
				return 0;
			case QUERY_PVERSION:
				printf("%d\n", GGZ_CS_PROTO_VERSION);
				return 0;
			case QUERY_DATADIR:
				printf("%s\n", GGZDATADIR);
				return 0;
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

	if(modfile == NULL) {
		fprintf(stderr, "Must specify module installation file.\n");
		return 1;
	}

	if(!load_modfile()) {
		fprintf(stderr, "Required installation file entries missing\n");
		return 1;
	}

	if(moddest) {
		destdir = getenv("DESTDIR");
		if(destdir == NULL)
			moddest = 0;
	}

	if(install_mod)
		rc = install_module();
	else if(remove_mod)
		rc = remove_module();

	return rc;
}
