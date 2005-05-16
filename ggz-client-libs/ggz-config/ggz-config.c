/*
 * File: ggz-config.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 02/19/01
 * $Id: ggz-config.c 7192 2005-05-16 21:12:27Z josef $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/stat.h>

#include <ggz.h>

#include "protocol.h"

#include <locale.h>
#include <libintl.h>

#define _(x) gettext(x)
#define N_(x) (x)

/* Command line arguments and global variables */
static char *modname = NULL;
static char *modversion = NULL;
static char *modpengine = NULL;
static char *modpversion = NULL;
static char *modexec = NULL;
static char *modui = NULL;
static char *modauthor = NULL;
static char *modurl = NULL;
static char *modenvironment = NULL;
static char *modicon = NULL;
static char *modhelp = NULL;
static char *modfile = NULL;
static char *iconfile = NULL;
static char *managediconfile = NULL;
static int modforce = 0;
static int moddest = 0;
static char *destdir = NULL;
static int install_mod = 0;
static int remove_mod = 0;
static int check_file = 0;

/* Command line options */
/* If they are changed make sure to edit the manpage! */
static struct option options[] =
{
	{"configdir", no_argument, 0, 'c'},
	{"gamedir", no_argument, 0, 'g'},
	{"datadir", no_argument, 0, 'd'},
	{"version", no_argument, 0, 'v'},
	{"protocol", no_argument, 0, 'p'},
	{"install", no_argument, 0, 'i'},
	{"remove", no_argument, 0, 'r'},
	{"check", no_argument, 0, 'C'},
	{"modfile", required_argument, 0, 'm'},
	{"iconfile", required_argument, 0, 'I'},
	{"force", no_argument, 0, 'f'},
	{"destdir", no_argument, 0, 'D'},
	{"help", no_argument, 0, 'h'},
	{"usage", no_argument, 0, 'u'},
	{0, 0, 0, 0}
};

/* Help for the command line options */
static char *options_help[] = {
	 N_("Query GGZCONFDIR - location of configuration directory"),
	 N_("Query GGZGAMEDIR - location of game modules directory"),
	 N_("Query GGZDATADIR - location of game data directory"),
	 N_("Query VERSION - version identifier of ggzcore files"),
	 N_("Query GGZ_CS_PROTO_VERSION - version of core protocol"),
	 N_("Install a module"),
	 N_("Remove a module"),
	 N_("Check/repair module installation file"),
	 N_("Specifies module installation file (needs argument)"),
	 N_("Specifies icon file to use for the game (needs argument)"),
	 N_("Install over an existing module"),
	 N_("Use $DESTDIR as offset to ggz.modules file"),
	 N_("Display help"),
	 N_("Display usage"),
	 NULL
};


static int load_modfile(void)
{
	int from;
	int status = 1;

	from = ggz_conf_parse(modfile, GGZ_CONF_RDONLY);

	modname = ggz_conf_read_string(from, "ModuleInfo",
					     "Name", NULL);
	if(modname == NULL) {
		fprintf(stderr, _("Critical: Module name not specified.\n"));
		status = 0;
	}

	modversion = ggz_conf_read_string(from, "ModuleInfo",
						"Version", NULL);
	if(modversion == NULL) {
		fprintf(stderr, _("Critical: Module version not specified.\n"));
		status = 0;
	}

	modexec = ggz_conf_read_string(from, "ModuleInfo",
					     "CommandLine", NULL);
	if(modexec == NULL) {
		fprintf(stderr, _("Critical: Executable not specified.\n"));
		status = 0;
	}

	modui = ggz_conf_read_string(from, "ModuleInfo",
					   "Frontend", NULL);
	if(modui == NULL) {
		fprintf(stderr, _("Critical: User interface not specified.\n"));
		status = 0;
	}

	modpengine = ggz_conf_read_string(from, "ModuleInfo",
					        "ProtocolEngine", NULL);
	if(modpengine == NULL) {
		fprintf(stderr, _("Critical: Protocol engine not specified.\n"));
		status = 0;
	}

	modpversion = ggz_conf_read_string(from, "ModuleInfo",
						 "ProtocolVersion", NULL);
	if(modpversion == NULL) {
		fprintf(stderr, _("Critical: Protocol version not specified.\n"));
		status = 0;
	}

	modauthor = ggz_conf_read_string(from, "ModuleInfo",
					       "Author", NULL);
	if(modauthor == NULL) {
		fprintf(stderr, _("Critical: Module author not specified.\n"));
		status = 0;
	}

	modurl = ggz_conf_read_string(from, "ModuleInfo",
					    "Homepage", NULL);
	if(modurl == NULL)
		fprintf(stderr, _("Warning: Module homepage not specified.\n"));

	modenvironment = ggz_conf_read_string(from, "ModuleInfo",
					    "Environment", NULL);
	/*if(modenvironment == NULL)
		fprintf(stderr, _("Warning: Module environment not specified.\n"));*/

	modicon = ggz_conf_read_string(from, "ModuleInfo",
					     "IconPath", NULL);
	modhelp = ggz_conf_read_string(from, "ModuleInfo",
					     "HelpPath", NULL);

	return status;
}


static int purge_engine_name(int global)
{
	int items;
	char **engine_list;
	int index;

	ggz_conf_read_list(global, "Games", "*Engines*",
				 &items, &engine_list);

	for(index=0; index<items; index++)
		if(!strcmp(engine_list[index], modpengine))
			break;

	if(index != items-1)
		engine_list[index] = engine_list[items-1];
	items--;

	if(items != 0)
		ggz_conf_write_list(global, "Games", "*Engines*",
					  items, engine_list);
	else
		ggz_conf_remove_key(global, "Games", "*Engines*");

	return 0;
}


static char *new_engine_id(int global)
{
	char **engine_list, **engine_id_list;
	int engines, ids;
	int hi=0, t;
	int i, j;
	static char new_id[10];

	ggz_conf_read_list(global, "Games", "*Engines*",
				 &engines, &engine_list);

	for(i=0; i<engines; i++) {
		ggz_conf_read_list(global, "Games", engine_list[i],
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


static char *get_engine_id(int global)
{
	int items;
	char **engine_list;
	char *engine_id=NULL;
	char *author, *ui, *version;
	int index;

	ggz_conf_read_list(global, "Games", modpengine,
				 &items, &engine_list);
	if(items == 0)
		return NULL;

	for(index=0; index<items; index++) {
		engine_id = engine_list[index];
		author = ggz_conf_read_string(global, engine_id,
						    "Author", NULL);
		ui = ggz_conf_read_string(global, engine_id,
						"Frontend", NULL);
		managediconfile = ggz_conf_read_string(global, engine_id,
						"IconPath", NULL);

		if((!author) || (!ui))
		{
			return "error";
		}

		if(modversion) {
			version = ggz_conf_read_string(global, engine_id,
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


static void purge_engine_id(int global, char *engine_id)
{
	int items;
	char **engine_list;
	int index;

	ggz_conf_read_list(global, "Games", modpengine,
				 &items, &engine_list);

	for(index=0; index<items; index++)
		if(!strcmp(engine_list[index], engine_id))
			break;

	if(index != items-1)
		engine_list[index] = engine_list[items-1];
	items--;

	if(items != 0)
		ggz_conf_write_list(global, "Games", modpengine,
					  items, engine_list);
	else {
		ggz_conf_remove_key(global, "Games", modpengine);
		purge_engine_name(global);
	}
}


static int open_conffile(void)
{
	char	*global_pathname;
	char	*global_filename = "ggz.modules";
	int	global;

	if(moddest)
		global_pathname = ggz_malloc(strlen(destdir) +
					     strlen(GGZCONFDIR) +
					     strlen(global_filename) + 3);
	else
		global_pathname = ggz_malloc(strlen(GGZCONFDIR) +
					     strlen(global_filename) + 2);

	if(moddest)
		sprintf(global_pathname, "%s/%s/%s", destdir,
					  GGZCONFDIR, global_filename);
	else
		sprintf(global_pathname, "%s/%s", GGZCONFDIR, global_filename);

	global = ggz_conf_parse(global_pathname, GGZ_CONF_RDONLY);
	if(global < 0) {
		printf(_("Setting up GGZ game modules configuration in %s\n"), global_pathname);
	}
	global = ggz_conf_parse(global_pathname, GGZ_CONF_CREATE | GGZ_CONF_RDWR);
	if(global < 0) {
		fprintf(stderr, _("Insufficient permission to install modules\n"));
		ggz_conf_cleanup();
		return -1;
	} else {
#ifndef HAVE_WINSOCK2_H
		/* HACK: chmod flags aren't available on windows. */
		chmod(global_pathname, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
	}

	return global;
}


static void handle_icon(void)
{
	char *path;
	FILE *fin, *fout;
	int ch;

	if(install_mod) {
		if(!iconfile) return;
		/*modicon = modname;*/
		modicon = iconfile;
		while(strchr(modicon, '/')) {
			modicon = strchr(modicon, '/') + 1;
		}
		path = ggz_malloc(strlen(GGZDATADIR) + strlen(modicon) + 30);
		strcpy(path, GGZDATADIR);
		strcat(path, "/ggz-config/");
		mkdir(path, 0700);
		strcat(path, modicon);
		/* ggz_filecopy(iconfile, path) */
		fin = fopen(iconfile, "r");
		if(!fin) {
			fprintf(stderr, _("Icon file cannot be read (%s)\n"), iconfile);
			modicon = NULL;
			return;
		}
		fout = fopen(path, "w");
		if(!fout) {
			fprintf(stderr, _("Icon file cannot be written to (%s)\n"), path);
			fclose(fin);
			modicon = NULL;
			return;
		}
		while((ch = fgetc(fin)) != EOF) {
			fputc(ch, fout);
		}
		fclose(fout);
		fclose(fin);
		ggz_free(path);
	} else if(remove_mod) {
		modicon = managediconfile;
		if(!modicon) return;
		if(modicon[0] == '/') return;
		path = ggz_malloc(strlen(GGZDATADIR) + strlen(modicon) + 30);
		strcpy(path, GGZDATADIR);
		strcat(path, "/ggz-config/");
		strcat(path, modicon);
		unlink(path);
		ggz_free(path);
	}
}


static int remove_module(void)
{
	char	*engine_id;
	int	global, rc;

	if((global = open_conffile()) < 0)
		return global;

	engine_id = get_engine_id(global);

	if(engine_id == NULL) {
		fprintf(stderr, _("Warning: Tried to remove nonexistant module\n"));
		ggz_conf_cleanup();
		return -1;
	}

	handle_icon();

	rc = ggz_conf_remove_section(global, engine_id);
	if(rc == 0) {
		purge_engine_id(global, engine_id);
		rc = ggz_conf_commit(global);
	}

	if(rc != 0) {
		fprintf(stderr, _("ggz.modules configuration may be corrupt\n"));
		fprintf(stderr, _("Module removal failed, see documentation\n"));
	}

	ggz_conf_cleanup();

	return rc;
}


static int install_module(void)
{
	char	*engine_id;
	int	global, rc;
	char	*engine_id_list, *engine_list;
	char	bigstr[1024];

	if((global = open_conffile()) < 0)
		return global;

	engine_id = get_engine_id(global);

	if((engine_id) && (!strcmp(engine_id, "error")))
	{
		fprintf(stderr, _("Your configuration is broken - aborting\n"));
		ggz_conf_cleanup();
		return -1;
	}

	if(!modforce) {
		if(engine_id != NULL) {
			fprintf(stderr, _("Cannot overwrite existing module\n"));
			ggz_conf_cleanup();
			return -1;
		}
	}

	if(engine_id == NULL) {
		engine_id = new_engine_id(global);
		modforce = 0;
	}

	handle_icon();

	rc = ggz_conf_write_string(global, engine_id, "Name", modname);
	if(rc == 0) {
		ggz_conf_write_string(global, engine_id,
					    "ProtocolEngine", modpengine);
		ggz_conf_write_string(global, engine_id,
					    "ProtocolVersion", modpversion);
		ggz_conf_write_string(global, engine_id,
					    "Frontend", modui);
		ggz_conf_write_string(global, engine_id,
					    "Version", modversion);
		ggz_conf_write_string(global, engine_id,
					    "Author", modauthor);
		ggz_conf_write_string(global, engine_id,
					    "CommandLine", modexec);
		ggz_conf_write_string(global, engine_id,
					    "Homepage", modurl);
		if(modenvironment)
			ggz_conf_write_string(global, engine_id,
						    "Environment", modenvironment);
		if(modicon)
			ggz_conf_write_string(global, engine_id,
						    "IconPath", modicon);
		if(modhelp)
			ggz_conf_write_string(global, engine_id,
						    "HelpPath", modhelp);

		engine_id_list = ggz_conf_read_string(global, "Games",
						            modpengine, NULL);
		if(engine_id_list == NULL && !modforce) {
			ggz_conf_write_string(global, "Games",
						    modpengine, engine_id);
			engine_list = ggz_conf_read_string(global,"Games",
						    "*Engines*", NULL);
			if(engine_list == NULL)
				ggz_conf_write_string(global, "Games",
						    "*Engines*", modpengine);
			else {
				snprintf(bigstr, 1024, "%s %s",
					 engine_list, modpengine);
				ggz_conf_write_string(global, "Games",
						    "*Engines*", bigstr);
			}
		} else if(!modforce) {
			snprintf(bigstr, 1024, "%s %s",
				 engine_id_list, engine_id);
			ggz_conf_write_string(global, "Games",
					    	modpengine, bigstr);
		}

		rc = ggz_conf_commit(global);
	}

	if(rc != 0) {
		fprintf(stderr, _("ggz.modules configuration may be corrupt\n"));
		fprintf(stderr, _("Module installation failed, see documentation\n"));
	}

	ggz_conf_cleanup();

	return rc;
}


#if 0 /* unused */
static int query(char *name, char *text, int def)
{
	char buf[3];

	fflush(stdin);
	printf("\n%s: %s [%c] ", name, text, def == 1?'Y':'N');
	fgets(buf, 3, stdin);
	if(def == 0) {
		if(*buf == 'Y' || *buf == 'y')
			return 1;
	} else {
		if(*buf == 'N' || *buf == 'n')
			return 0;
	}

	return def;
}
#endif


static int check_module_file(void)
{
	int	global;
	int	rc;
	int	e_count, s_count, k_count, g_count;
	char	**e_list, **s_list, **k_list, **g_list;
	char	*str, *str2;
	int	kill, ok, alt;
	int	i, j, k;
	int	*section_refd;
	int	errs=0;

	const char *reqd_keys[] = {
		"Author",
		"Frontend",
		"Name",
		"ProtocolEngine",
		"ProtocolVersion",
		"Version",
		NULL
	};

	if((global = open_conffile()) < 0)
		return global;

	/* Phase One */
	/* Check that every game engine section has req'd entries */
	if((rc = ggz_conf_get_sections(global, &s_count, &s_list)) <0
	   || s_count == 0) {
		printf(_("Error getting config file sections list\n"));
		printf(_("May be an empty config file?\n"));
		return rc;
	}
	for(i=0; i<s_count; i++) {
		if(!strcmp(s_list[i], "Games")) {
			ggz_free(s_list[i]);
			continue;
		}
		printf(_("*** Checking game config section [%s]\n"), s_list[i]);
		kill=0;
		for(j=0; reqd_keys[j]; j++) {
			str = ggz_conf_read_string(global, s_list[i],
						     reqd_keys[j], NULL);
			if(str == NULL) {
				errs++;
				printf(_("ERR: missing required key '%s'\n"),
					reqd_keys[j]);
				kill=1;
			} else
				ggz_free(str);
		}
		if(!kill) {
			str = ggz_conf_read_string(global, s_list[i],
						     "CommandLine", NULL);
			if(str != NULL) {
				/* Without command line args */
				str2 = str;
				while(*str2 && *str2 != ' ') str2++;
				*str2 = '\0';
				if(access(str, X_OK))
					kill=1;
				ggz_free(str);
			}
			if(kill) {
				errs++;
				printf(_("ERR: missing or invalid executable\n"));
			}
		}
		if(kill) {
			printf(_("Removing section for '%s'\n"), s_list[i]);
			modpengine = ggz_conf_read_string(global, s_list[i], "ProtocolEngine", NULL);
			rc = ggz_conf_remove_section(global, s_list[i]);
			if(rc == 0) {
				if(modpengine) {
					purge_engine_id(global, s_list[i]);
				}
				rc = ggz_conf_commit(global);
			}
		}

		ggz_free(s_list[i]);
	}
	ggz_free(s_list);


	/* Phase Two */
	/* Check for cross references (multiple engines -> one game section) */
	if((rc = ggz_conf_get_sections(global, &s_count, &s_list)) <0) {
		printf(_("Error getting config file sections list\n"));
		return rc;
	}
phase_two:
	if((rc = ggz_conf_get_keys(global, "Games", &k_count, &k_list)) <0
	   || k_count == 0) {
		printf(_("Error getting config file [Games]:keys list\n"));
		printf(_("May be an empty config file?\n"));
		return rc;
	}
	printf(_("*** Computing section cross references\n"));
	section_refd = ggz_malloc(s_count * sizeof(int));
	for(i=0; i<k_count; i++) {
		if(!strcmp(k_list[i], "*Engines*"))
			continue;
		/*str = ggz_conf_read_string(global, "Games", k_list[i], NULL);*/
		ggz_conf_read_list(global, "Games", k_list[i], &g_count, &g_list);
		for(k = 0; k < g_count; k++)
		{
			str = g_list[k];

			for(j=0; j<s_count; j++) {
				if(!strcmp(str, s_list[j])) {
					if(!section_refd[j]) {
						section_refd[j] = i+1;
						continue;
					}
					/* Check name in [p#] to see if it matches */
					/* either of the xrefs to decide smartly */
					str2 = ggz_conf_read_string(global, s_list[j],
								    "Name", NULL);
					if(str2 && !strcmp(k_list[i], str2))
						kill = section_refd[j]-1;
					else
						kill = i;
					if(str2)
						ggz_free(str2);
					errs++;
					printf(_("ERR %s and %s references [%s], deleting %s reference\n"),
						k_list[i], k_list[section_refd[j]-1],
						s_list[j], k_list[kill]);
					ggz_conf_remove_key(global, "Games", k_list[kill]);
					for(i=0; i<k_count; i++)
						ggz_free(k_list[i]);
					ggz_free(k_list);
					for(i=k; i<g_count; i++)
						ggz_free(g_list[i]);
					ggz_free(g_list);
					ggz_free(section_refd);

					/* Restart the phase */
					goto phase_two;
				}
			}
			ggz_free(g_list[k]);
		}
		ggz_free(g_list);
	}
	ggz_free(section_refd);
	for(i=0; i<s_count; i++)
		ggz_free(s_list[i]);
	ggz_free(s_list);
	/* Since we made it through phase two, k_list hasn't */
	/* changed, so we carry it over to phase three */


	/* Phase Three */
	/* Check each game key for correct engine name */
	for(i=0; i<k_count; i++) {
		if(!strcmp(k_list[i], "*Engines*")) {
			ggz_free(k_list[i]);
			continue;
		}
		printf(_("*** Checking ProtocolEngine key for engine '%s'\n"), k_list[i]);
		/*str = ggz_conf_read_string(global, "Games", k_list[i], NULL);*/
		ggz_conf_read_list(global, "Games", k_list[i], &g_count, &g_list);
		for(k = 0; k < g_count; k++) {
			str = g_list[k];

			str2 = ggz_conf_read_string(global, str, "ProtocolEngine", NULL);
			if(str2 && strcmp(k_list[i], str2)) {
				errs++;
				printf(_("ERR Setting ProtocolEngine key [%s] to '%s'\n"),
					str, k_list[i]);
				ggz_conf_write_string(global, str, "ProtocolEngine", k_list[i]);
			}
			/*ggz_free(str);*/
			if(str2) ggz_free(str2);
			ggz_free(g_list[k]);
		}
		ggz_free(g_list);
		ggz_free(k_list[i]);
	}
	ggz_free(k_list);


	/* Phase Four */
	/* Check that each section references back to a [Games]:key */
	printf(_("*** Checking back references\n"));
	if((rc = ggz_conf_get_sections(global, &s_count, &s_list)) <0) {
		printf(_("Error getting config file sections list\n"));
		return rc;
	}
	for(i=0; i<s_count; i++) {
		if(!strcmp(s_list[i], "Games")) {
			ggz_free(s_list[i]);
			continue;
		}
		str = ggz_conf_read_string(global, s_list[i], "ProtocolEngine", NULL);
		ggz_conf_read_list(global, "Games", str, &g_count, &g_list);

		ok = 0;
		for(j = 0; j < g_count; j++) {
			if(!strcmp(s_list[i], g_list[j])) {
				ok = 1;
			}
		}

		if(!ok) {
			errs++;
			printf(_("ERR Adding [Games]:%s key pointing to [%s]\n"),
				 str, s_list[i]);
			g_count++;
			g_list = ggz_realloc(g_list, g_count * sizeof(char*));
			g_list[g_count - 1] = ggz_strdup(s_list[i]);
			ggz_conf_write_list(global, "Games", str, g_count, g_list);
		}

		for(j = 0; j < g_count; j++) {
			ggz_free(g_list[j]);
		}
		ggz_free(g_list);

		ggz_free(str);
		ggz_free(s_list[i]);
	}
	ggz_free(s_list);

	/* Phase Four/b */
	/* Check that each section key actually has a section*/
	printf(_("*** Checking forward references\n"));
	if((rc = ggz_conf_get_keys(global, "Games", &k_count, &k_list)) <0) {
		printf(_("Error getting config file [Games]:keys list\n"));
		return rc;
	}

	for(i = 0; i < k_count; i++) {
		if(!strcmp(k_list[i], "*Engines*")) {
			ggz_free(k_list[i]);
			continue;
		}
		ggz_conf_read_list(global, "Games", k_list[i], &g_count, &g_list);

		for(k = 0; k < g_count; k++) {
			str = ggz_conf_read_string(global, g_list[k], "ProtocolEngine", NULL);
			if(!str)
			{
				errs++;
				printf(_("ERR Section %s doesn't exist in %s, removed reference\n"),
					g_list[k], k_list[i]);
				ggz_free(g_list[k]);
				g_count -= 1;
				if(g_count) {
					if(k <= g_count)
						g_list[k] = g_list[g_count];
					ggz_conf_write_list(global, "Games", k_list[i], g_count, g_list);
				} else {
					ggz_conf_remove_key(global, "Games", k_list[i]);
				}
			}
			else ggz_free(str);
		}
		for(k = 0; k < g_count; k++)
			ggz_free(g_list[k]);
		ggz_free(g_list);
	}
	ggz_free(k_list);

	/* Phase Five */
	/* Check that each entry in *Engines* points to something */
	printf(_("*** Checking for spurious game engine entries\n"));
phase_five:
	ggz_conf_read_list(global, "Games", "*Engines*", &e_count, &e_list);
	for(i=0; i<e_count; i++) {
		str = ggz_conf_read_string(global, "Games", e_list[i], NULL);

			if(!str) {
				errs++;
				printf(_("ERR Game engine '%s' invalid, removing\n"),
				       e_list[i]);
				ggz_free(e_list[i]);
				e_count--;
				if(e_count) {
					if(i < e_count)
						e_list[i] = e_list[e_count];
					ggz_conf_write_list(global, "Games", "*Engines*",
						e_count, e_list);
				} else {
					ggz_conf_remove_key(global, "Games", "*Engines*");
				}
				for(i=0; i<e_count; i++)
					ggz_free(e_list[i]);
				ggz_free(e_list);

				/*** Restart the phase ***/
				goto phase_five;
			}
			ggz_free(str);
	}
	/* e_list is still valid and used in next phase */

	/* Final Phase */
	/* Make sure that every [Games]:key exists in [Games]:*Engines* */
	printf(_("*** Checking for missing game engine pointers\n"));
	if((rc = ggz_conf_get_keys(global, "Games", &k_count, &k_list)) <0) {
		printf(_("Error getting config file [Games]:keys list\n"));
		return rc;
	}
	alt = 0;
	for(i=0; i<k_count; i++) {
		if(!strcmp(k_list[i], "*Engines*")) {
			ggz_free(k_list[i]);
			continue;
		}
		ok = 0;
		for(j=0; j<e_count; j++)
			if(!strcmp(k_list[i], e_list[j]))
				ok=1;
		if(!ok) {
			errs++;
			printf(_("ERR Adding '%s' to game engine list\n"),
				k_list[i]);
			e_count++;
			e_list = ggz_realloc(e_list, e_count*sizeof(char *));
			e_list[e_count-1] = ggz_strdup(k_list[i]);
			alt=1;
		}
		ggz_free(k_list[i]);
	}
	if(k_list) ggz_free(k_list);
	if(e_count)
		ggz_conf_write_list(global, "Games", "*Engines*", e_count, e_list);
	for(i=0; i<e_count; i++)
		if(alt || i<e_count-1)
			ggz_free(e_list[i]);
	if(e_list) ggz_free(e_list);

	if(errs)
		printf(_("Finished - writing %d repairs\n"), errs);
	else
		printf(_("Finished - no configuration errors detected\n"));
	ggz_conf_commit(global);

	ggz_conf_cleanup();

	return 0;
}

int main(int argc, char *argv[])
{
	int optindex;
	int opt;
	int i;
	int rc;

	/* Set up translation */
	bindtextdomain("ggz-config", PREFIX "/share/locale");
	textdomain("ggz-config");
	setlocale(LC_ALL, "");

	/* Parse the command line options */
	while(1)
	{
		opt = getopt_long(argc, argv, "cgdvpirCm:I:fDhu", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf(_("GGZ-Config - the GGZ Gaming Zone Configuration Utility\n"));
				printf(_("Copyright (C) 2001 Rich Gade, rgade@users.sourceforge.net\n"));
				printf(_("Copyright (C) 2002 - 2005 The GGZ Gaming Zone developers\n"));
				printf(_("Published under GNU GPL conditions\n"));
				printf("\n");
				printf(_("Recognized options:\n"));

				for(i = 0; options[i].name; i++)
				{
					printf("[-%c | --%-10s]: %s\n",
						options[i].val, options[i].name, _(options_help[i]));
				}

				return 0;
				break;
			case 'u':
				printf(_("Usage:\n"));
				printf("\tggz-config --install --modfile=<module.dsc> [--force]\n");
				printf("\tggz-config --remove --modfile=<module.dsc> [--force]\n");
				printf("\tggz-config --check\n");
				return 0;
				break;
			case 'm':
				iconfile = optarg;
				break;
			case 'I':
				modfile = optarg;
				break;
			case 'f':
				modforce = 1;
				break;
			case 'D':
				moddest = 1;
				break;
			case 'i':
				install_mod = 1;
				break;
			case 'r':
				remove_mod = 1;
				break;
			case 'C':
				check_file = 1;
				break;
			case 'c':
				printf("%s\n", GGZCONFDIR);
				return 0;
				break;
			case 'g':
				printf("%s\n", GAMEDIR);
				return 0;
				break;
			case 'd':
				printf("%s\n", GGZDATADIR);
				return 0;
				break;
			case 'v':
				printf("%s\n", VERSION);
				return 0;
				break;
			case 'p':
				printf("%i\n", GGZ_CS_PROTO_VERSION);
				return 0;
				break;
			default:
				/*fprintf(stderr, _("Unknown command line option, try --help.\n"));*/
				return 1;
				break;
		}
	}

	/* Execute at least one operation (install or remove or check) */
	if(install_mod + remove_mod + check_file != 1) {
		fprintf(stderr, _("No operation specified, try --help.\n"));
		return 1;
	}

	if(check_file)
		return check_module_file();

	if(modfile == NULL) {
		fprintf(stderr, _("Must specify module installation file.\n"));
		return 1;
	}

	if(!load_modfile()) {
		fprintf(stderr, _("Required installation file entries missing\n"));
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
	else
		rc = -1;

	return rc;
}
