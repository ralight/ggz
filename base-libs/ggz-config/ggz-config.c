/*
 * ggz-config: Configuration query and module install program.
 * This tool is part of the ggz-base-libs package.
 *
 * Copyright (C) 2001 Rich Gade
 * Copyright (C) 2002-2008 GGZ Gaming Zone Development Team
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
#include <dirent.h>

#include <ggz.h>
#include <ggzcore.h>

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
static char *copydir = NULL;
static char *managediconfile = NULL;
static int modforce = 0;
static int moddest = 0;
static char *destdir = NULL;
static int install_mod = 0;
static int remove_mod = 0;
static int list_mods = 0;
static int check_file = 0;
static char *registry = NULL;

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
	{"noregistry", required_argument, 0, 'n'},
	{"registry", required_argument, 0, 'R'},
	{"force", no_argument, 0, 'f'},
	{"destdir", no_argument, 0, 'D'},
	{"help", no_argument, 0, 'h'},
	{"usage", no_argument, 0, 'u'},
	{"list", no_argument, 0, 'l'},
	{0, 0, 0, 0}
};

/* Help for the command line options */
static const char *options_help[] = {
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
	 N_("Use auxiliary directory for module registration instead of ggz.modules (needs argument)"),
	 N_("Use a custom modules registry instead of ggz.modules (needs argument)"),
	 N_("Install over an existing module"),
	 N_("Use $DESTDIR as offset to ggz.modules file"),
	 N_("Display help"),
	 N_("Display usage"),
	 N_("List all currently registered game modules"),
	 NULL
};


/* Helper function for cross-platform mkdir() */
/* FIXME: move into libggz? */
static int ggz_mkdir(const char *dirname, mode_t mode)
{
#ifdef MKDIR_TAKES_ONE_ARG
	return mkdir(dirname);
#else
	return mkdir(dirname, mode);
#endif
}


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
	int i;

	ggz_conf_read_list(global, "Games", "*Engines*",
				 &items, &engine_list);

	for (i = 0; i < items; i++)
		if(!strcmp(engine_list[i], modpengine))
			break;

	if (i != items - 1)
		engine_list[i] = engine_list[items - 1];
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

	snprintf(new_id, sizeof(new_id), "p%d", hi+1);

	return new_id;
}


static char *get_engine_id(int global)
{
	int items;
	char **engine_list;
	char *engine_id=NULL;
	char *author, *ui, *version;
	int i;

	ggz_conf_read_list(global, "Games", modpengine,
				 &items, &engine_list);
	if(items == 0)
		return NULL;

	for (i = 0; i < items; i++) {
		engine_id = engine_list[i];
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

	if (i >= items)
		return NULL;
	else
		return engine_id;
}


static void purge_engine_id(int global, char *engine_id)
{
	int items;
	char **engine_list;
	int i;

	ggz_conf_read_list(global, "Games", modpengine,
				 &items, &engine_list);

	for (i = 0; i < items; i++)
		if(!strcmp(engine_list[i], engine_id))
			break;

	if (i != items - 1)
		engine_list[i] = engine_list[items-1];
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
	const char *global_filename = "ggz.modules";
	char *global_pathname, *global_dirname;
	int global = -1;
	GGZConfType conftype;

	if(registry)
		if(moddest)
			global_pathname = ggz_strbuild("%s/%s/%s.d/%s",
				destdir, GGZCONFDIR, global_filename, registry);
		else
			global_pathname = ggz_strbuild("%s/%s.d/%s",
				GGZCONFDIR, global_filename, registry);
	else
		if(moddest)
			global_pathname = ggz_strbuild("%s/%s/%s",
				destdir, GGZCONFDIR, global_filename);
		else
			global_pathname = ggz_strbuild("%s/%s",
				GGZCONFDIR, global_filename);

	if(install_mod + remove_mod + check_file == 0) {
		conftype = GGZ_CONF_RDONLY;
	} else {
		conftype = GGZ_CONF_RDWR;
	}

	global = ggz_conf_parse(global_pathname, conftype);
	if(global < 0) {
		if(install_mod + remove_mod + check_file == 1) {
			printf(_("Setting up GGZ game modules configuration in %s\n"), global_pathname);
			if(registry) {
				if(moddest)
					global_dirname = ggz_strbuild("%s/%s/%s.d",
						destdir, GGZCONFDIR, global_filename);
				else
					global_dirname = ggz_strbuild("%s/%s.d",
						GGZCONFDIR, global_filename);
				ggz_mkdir(global_dirname, 0700);
				ggz_free(global_dirname);
			}
			conftype |= GGZ_CONF_CREATE;

			global = ggz_conf_parse(global_pathname, conftype);
			if(global < 0) {
				fprintf(stderr, _("Insufficient permission to alter modules registry\n"));
				ggz_free(global_pathname);
				ggz_conf_cleanup();
				return -1;
			} else {
#ifndef HAVE_WINSOCK2_H
				/* HACK: chmod flags aren't available on windows. */
				chmod(global_pathname, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
			}
		} else {
			fprintf(stderr, _("Insufficient permission read modules registry\n"));
			ggz_free(global_pathname);
			ggz_conf_cleanup();
		}
	}
	ggz_free(global_pathname);

	return global;
}


static int filecopy(const char *src, const char *dst)
{
	FILE *fin, *fout;
	int ch;

	fin = fopen(src, "r");
	if(!fin) {
		fprintf(stderr, _("File cannot be read (%s)\n"), src);
		return -1;
	}
	fout = fopen(dst, "w");
	if(!fout) {
		fprintf(stderr, _("File cannot be written to (%s)\n"), dst);
		fclose(fin);
		return -1;
	}
	while((ch = fgetc(fin)) != EOF) {
		fputc(ch, fout);
	}
	fclose(fout);
	fclose(fin);

	return 0;
}


static void handle_icon(void)
{
	int ret;

	if(install_mod) {
		if(!iconfile) return;
		/*modicon = modname;*/
		modicon = iconfile;
		while(strchr(modicon, '/')) {
			modicon = strchr(modicon, '/') + 1;
		}

		{
			char path[strlen(GGZDATADIR) + strlen(modicon) + 30];

			strcpy(path, GGZDATADIR);
			strcat(path, "/ggz-config/");
			ggz_mkdir(path, 0700);
			strcat(path, modicon);
			ret = filecopy(iconfile, path);
			if(ret != 0) modicon = NULL;
		}
	} else if(remove_mod) {
		modicon = managediconfile;
		if (modicon && modicon[0] != '/') {
			char path[strlen(GGZDATADIR) + strlen(modicon) + 30];

			strcpy(path, GGZDATADIR);
			strcat(path, "/ggz-config/");
			strcat(path, modicon);
			unlink(path);
		}
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
		fprintf(stderr,
			_("Warning: Tried to remove nonexistent module.\n"));
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


static int noregister_module(void)
{
	const char *suffix = ".module.dsc";
	char *global_pathname;
	char fixedmodname[strlen(modname) + 1];
	unsigned int i;

	if(moddest)
		global_pathname = ggz_strbuild("%s/%s",
			destdir, copydir);
	else
		global_pathname = ggz_strbuild("%s",
			copydir);

	if (ggz_make_path(global_pathname) != 0) {
		fprintf(stderr, _("Directory cannot be created (%s)\n"),
			global_pathname);
		ggz_free(global_pathname);
		return -1;
	}

	strcpy(fixedmodname, modname);
	for(i = 0; i < strlen(fixedmodname); i++) {
		if(fixedmodname[i] == '/') fixedmodname[i] = '_';
	}
	char *global_pathname_modname = ggz_strbuild("%s/%s%s",
		global_pathname, fixedmodname, suffix);
	ggz_free(global_pathname);

	printf(_("Preserving %s as %s...\n"), modfile, global_pathname_modname);
	int ret = filecopy(modfile, global_pathname_modname);
	ggz_free(global_pathname_modname);
	return ret;
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
				snprintf(bigstr, sizeof(bigstr), "%s %s",
					 engine_list, modpengine);
				ggz_conf_write_string(global, "Games",
						    "*Engines*", bigstr);
			}
		} else if(!modforce) {
			snprintf(bigstr, sizeof(bigstr), "%s %s",
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


static int noregister_all(void)
{
	DIR *d;
	struct dirent *e;
	int ret;

	d = opendir(copydir);
	if(!d) {
		fprintf(stderr, _("Could not open auxiliary directory\n"));
		return -1;
	}
	while((e = readdir(d)) != NULL) {
#ifdef _DIRENT_HAVE_D_TYPE
		/* _DIRENT_HAVE_D_TYPE is defined in dirent.h...but this
		 * should be replaced by a configure check. */
		if(e->d_type != DT_REG) continue;
#endif
		modfile = ggz_strbuild("%s/%s", copydir, e->d_name);
		if(load_modfile()) {
			if(install_mod) {
				printf(_("- register %s\n"), e->d_name);
				ret = install_module();
			} else if(remove_mod) {
				printf(_("- unregister %s\n"), e->d_name);
				ret = remove_module();
			} else ret = -1;
		} else {
			ret = -1;
		}
		ggz_free(modfile);
		modfile = NULL;
		if(ret != 0) {
			fprintf(stderr, _("An error occurred, continuing anyway.\n"));
		}
	}
	closedir(d);

	return 0;
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


/* This function works like the which(2) program. An application
 * is searched in the search path ($PATH). Absolute paths
 * are returned as-are.
 * However, the return codes are more fine grained:
 * 0 - executable not found in $PATH
 * 1 - found but not executable
 * 2 - found and executable
 * 3 - invalid arguments
 */
static int which(const char *executable)
{
	char execpath[2048];
	int ret = 0;
	struct stat st;

	if(!executable) return 3;

	if(executable[0] == '/')
	{
		if(!stat(executable, &st))
		{
			ret = (ret < 1 ? 1 : ret);
			if(!access(executable, X_OK))
			{
				ret = (ret < 2 ? 2 : ret);
			}
		}
		return ret;
	}

	const char *path = getenv("PATH");
	if(!path)
	{
		path = "/bin:/usr/bin:.";
	}

	char *copy = strdup(path);
	char *token = strtok(copy, ":");
	while(token)
	{
		snprintf(execpath, sizeof(execpath), "%s/%s", token, executable);

		if(!stat(execpath, &st))
		{
			ret = (ret < 1 ? 1 : ret);
			if(!access(execpath, X_OK))
			{
				ret = (ret < 2 ? 2 : ret);
			}
		}

		token = strtok(NULL, ":");
	}

	free(copy);

	return ret;
}


/* This is so far the only function which works using standard ggzcore
 * interface to mimic the view of core clients. */
static int list_modules(void)
{
	int ret = 0;
	const GGZOptions opt = {.flags = GGZ_OPT_MODULES};
	int num, i;

	ret = ggzcore_init(opt);
	if(ret == -1)
		return -1;

	num = ggzcore_module_get_num();
	printf(_("%i modules found in the GGZ registry(ies).\n"), num);
	for(i = 0; i < num; i++) {
		GGZModule *module = ggzcore_module_get_nth(i);

		printf(_("Module: %s %s (%s)\n"),
			ggzcore_module_get_name(module),
			ggzcore_module_get_version(module),
			ggzcore_module_get_frontend(module));
	}

	return ret;
}


static int check_module_file(void)
{
	int	global;
	int	rc;
	int	e_count, s_count, k_count, g_count, n_count;
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
		printf(_("Is this an empty config file?\n"));
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
				/* Be more tolerant - as we support $PATH now */
				rc = which(str);
				if(rc != 2) {
					printf(_("WARN: missing or invalid executable '%s'\n"), str);
				}
				ggz_free(str);
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
		printf(_("Is this an empty config file?\n"));
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
		n_count = g_count;

		for(k = 0; k < g_count; k++) {
			str = ggz_conf_read_string(global, g_list[k], "ProtocolEngine", NULL);
			if(!str)
			{
				errs++;
				printf(_("ERR Section %s doesn't exist in %s, reference removed\n"),
					g_list[k], k_list[i]);
				ggz_free(g_list[k]);
				n_count -= 1;
				if(n_count) {
					if(k <= n_count)
						g_list[k] = g_list[n_count];
					ggz_conf_write_list(global, "Games", k_list[i], n_count, g_list);
				} else {
					ggz_conf_remove_key(global, "Games", k_list[i]);
				}
			}
			else ggz_free(str);
		}
		for(k = 0; k < n_count; k++)
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
		opt = getopt_long(argc, argv, "cgdvpirCm:I:n:fDhulR:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf(_("GGZ-Config - the GGZ Gaming Zone Configuration Utility\n"));
				printf(_("Copyright (C) 2001 Rich Gade, rgade@users.sourceforge.net\n"));
				printf(_("Copyright (C) 2002 - 2008 The GGZ Gaming Zone developers\n"));
				printf(_("Published under the terms of the GNU GPL\n"));
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
				printf("\tggz-config --remove --modfile=<module.dsc>\n");
				printf("\tggz-config --check\n");
				return 0;
				break;
			case 'm':
				modfile = optarg;
				break;
			case 'I':
				iconfile = optarg;
				break;
			case 'n':
				copydir = optarg;
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
			case 'R':
				registry = optarg;
				break;
			case 'C':
				check_file = 1;
				break;
			case 'l':
				list_mods = 1;
				break;
			case 'c':
				printf("%s\n", GGZCONFDIR);
				return 0;
				break;
			case 'g':
				printf("%s\n", GGZEXECMODDIR);
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
	if(install_mod + remove_mod + check_file + list_mods != 1) {
		fprintf(stderr, _("No operation specified, try --help.\n"));
		return 1;
	}

	if(check_file)
		return check_module_file();

	if(list_mods)
		return list_modules();

	if(modfile == NULL) {
		if(copydir) {
			printf(_("Using auxiliary directory to proceed...\n"));
		} else {
			fprintf(stderr, _("Must specify module installation file.\n"));
			return 1;
		}
	} else if(!load_modfile()) {
		fprintf(stderr, _("Required installation file entries missing\n"));
		return 1;
	}

	if(moddest) {
		destdir = getenv("DESTDIR");
		if(destdir == NULL)
			moddest = 0;
	}

	if(modfile) {
		if(install_mod) {
			if(copydir) {
				rc = noregister_module();
			} else {
				rc = install_module();
			}
		} else if(remove_mod)
			rc = remove_module();
		else
			rc = -1;
	} else {
		rc = noregister_all();
	}

	return rc;
}
