/*
 * File: module.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 11/23/00
 *
 * This fils contains functions for handling client-side game modules
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

#include "config.h"
#include "module.h"
#include "game.h"

#include <ggz.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define GGZ_MOD_RC "ggz.modules"

       
/* List of modules */
static GGZList *module_list;
static unsigned int num_modules;
static int mod_handle = -1;

/* static internal functions */
static struct _GGZModule* _ggzcore_module_new(void);
#if 0
static void _ggzcore_module_init(struct _GGZModule *module,
				 const char *name,
				 const char *version,
				 const char *prot_engine,
				 const char *prot_version,
				 const char *author,
				 const char *frontend,
				 const char *url,
				 const char *exec_path,
				 const char *icon_path,
				 const char *help_path);
#endif /* #if 0 */
static void _ggzcore_module_free(struct _GGZModule *module);
static void _ggzcore_module_read(struct _GGZModule *mod, char *id);
static int _ggzcore_module_add(struct _GGZModule *module);

static char* _ggzcore_module_conf_filename(void);
static void _ggzcore_module_print(struct _GGZModule*);
static void _ggzcore_module_list_print(void);
/* Utility functions used by ggz_list */
static void _ggz_free_chars(char **argv);
static int   _ggzcore_module_compare(void* p, void* q);
static int   _ggzcore_module_match_version(void *p, void *q);
#if 0
static void* _ggzcore_module_create(void* p);
#endif /* #if 0 */
static void  _ggzcore_module_destroy(void* p);


/* Publicly exported functions */


/* This returns the number of registered modules */
unsigned int ggzcore_module_get_num(void)
{
	return _ggzcore_module_get_num();
}


/* Returns how many modules support this game and protocol */
int ggzcore_module_get_num_by_type(const char *game, 
				   const char *engine,
				   const char *version)
{
	if (!game || !engine || !version)
		return -1;

	return _ggzcore_module_get_num_by_type(game, engine, version);
}


/* Returns n-th module that supports this game and protocol */
GGZModule* ggzcore_module_get_nth_by_type(const char *game, 
					  const char *engine,
					  const char *version,
					  const unsigned int num)
{
	/* FIXME: should check bounds on num */
	if (!game || !engine || !version)
		return NULL;

	return _ggzcore_module_get_nth_by_type(game, engine, version, num);
}


/* This adds a local module to the list.  It returns 0 if successful or
   -1 on failure. */
int ggzcore_module_add(const char *name,
	               const char *version,
	               const char *prot_engine,
	               const char *prot_version,
                       const char *author,
		       const char *frontend,
		       const char *url,
		       const char *exe_path,
		       const char *icon_path,
		       const char *help_path)
{
	return -1;
}


/* This attempts to launch the specified module and returns 0 is
   successful or -1 on error. */
int ggzcore_module_launch(GGZModule *module)
{
	if (!module)
		return -1;
	
	return _ggzcore_module_launch(module);	
}


/* These functions lookup a particular property of a module.  I've added
   icon to the list we discussed at the meeting.  This is an optional xpm
   file that the module can provide to use for representing the game
   graphically.*/
char* ggzcore_module_get_name(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_name(module);
}


char* ggzcore_module_get_version(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_version(module);
}


char* ggzcore_module_get_prot_engine(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_prot_engine(module);
}


char* ggzcore_module_get_prot_version(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_prot_version(module);
}


char* ggzcore_module_get_author(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_author(module);
}


char* ggzcore_module_get_frontend(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_frontend(module);
}


char* ggzcore_module_get_url(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_url(module);
}


char* ggzcore_module_get_icon_path(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_icon_path(module);
}


char* ggzcore_module_get_help_path(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_help_path(module);
}


char** ggzcore_module_get_argv(GGZModule *module)
{	
	if (!module)
		return NULL;

	return _ggzcore_module_get_argv(module);
}


/* Internal library functions (prototypes in module.h) */


/* _ggzcore_module_setup()
 *	Opens the global game module file
 *
 *	Returns:
 *	  -1 on error
 *	  0 on success
 */
int _ggzcore_module_setup(void)
{
	char *file;
	char **ids;
	char **games;
	int i, j, count_types, count_modules, status;
	struct _GGZModule *module;

	if (mod_handle != -1) {
		ggz_debug("GGZCORE:MODULE", "module_setup() called twice");
		return -1;
	}

	
	module_list = ggz_list_create(_ggzcore_module_match_version, NULL,
				      _ggzcore_module_destroy, 0);
	num_modules = 0;

	file = _ggzcore_module_conf_filename();
	ggz_debug("GGZCORE:MODULE", "Reading %s", file);
	mod_handle = ggz_conf_parse(file, GGZ_CONF_RDONLY);
	/* Free up space taken by name */
	ggz_free(file);
	
	if (mod_handle == -1) {
		ggz_debug("GGZCORE:MODULE", "Unable to load module conffile");
		return -1;
	}
	
	/* Read in list of supported gametypes */
	status = ggz_conf_read_list(mod_handle, "Games", "*Engines*", 
				    &count_types, &games);
	if (status < 0) {
		ggz_debug("GGZCORE:MODULE", "Couldn't read engine list");
		return -1;
	}	
	ggz_debug("GGZCORE:MODULE", "%d game engines supported", count_types);
	
	for (i = 0; i < count_types; i++) {
		status = ggz_conf_read_list(mod_handle, "Games", games[i], 
					    &count_modules, &ids);
					    

		ggz_debug("GGZCORE:MODULE", "%d modules for %s", count_modules,
			      games[i]);

		for (j = 0; j < count_modules; j++) {
			module = _ggzcore_module_new();
			_ggzcore_module_read(module, ids[j]);
			_ggzcore_module_add(module);
			ggz_debug("GGZCORE:MODULE", "Module %d: %s", j, 
				      ids[j]);

		}
		
		_ggz_free_chars(ids);
	}

	_ggz_free_chars(games);

	_ggzcore_module_list_print();
	
	signal(SIGCHLD, _ggzcore_game_dead);

	return 0;
}


unsigned int _ggzcore_module_get_num(void)
{
	return num_modules;
}


/* FIXME: do this right.  We should parse through module_list not
   re-read the config file */
int _ggzcore_module_get_num_by_type(const char *game, 
				    const char *engine,
				    const char *version)
{
	int count, status, i;
	char **ids;
	struct _GGZModule module;

	/* Get total count for this engine (regardless of version) */
	status = ggz_conf_read_list(mod_handle, "Games", engine, &count, &ids);
				    
	if (status < 0)
		return 0;
	
	for (i = 0; i < count; i++) {
		_ggzcore_module_read(&module, ids[i]);
		/* Subtract out modules that aren't the same protocol */
		if (strcmp(engine, module.prot_engine) != 0
		    || strcmp(version, module.prot_version) != 0
		    /* || game not included in game list */)
			count--;
	}

	_ggz_free_chars(ids);


	return count;
}


/* FIXME: do this right.  We should parse through module_list not
   re-read the config file */
struct _GGZModule* _ggzcore_module_get_nth_by_type(const char *game, 
						   const char *engine,
						   const char *version,
						   const unsigned int num)
{
	int i, total, status, count;
	char **ids;
	struct _GGZModule *module, *found = NULL;
	GGZListEntry *entry;

	status = ggz_conf_read_list(mod_handle, "Games", engine, &total, &ids);
	
	ggz_debug("GGZCORE:MODULE", "Found %d modules matching %s", total,
		      engine);
	
	if (status < 0)
		return NULL;

	if (num >= total) {
		return NULL;
		_ggz_free_chars(ids);
	}

	count = 0;
	for (i = 0; i < total; i++) {
		module = _ggzcore_module_new();
		_ggzcore_module_read(module, ids[i]);
		if (strcmp(version, module->prot_version) == 0) {
			/* FIXME:  also check to see if 'game' is in supported list */
			if (count++ == num) {
				/* Now find same module in list */
				entry = ggz_list_search(module_list, module);
				found = ggz_list_get_data(entry);
				_ggzcore_module_free(module);
				break;
			}
		}
		_ggzcore_module_free(module);
	}

	/* Free the rest of the ggz_conf memory */
	_ggz_free_chars(ids);
	
	
	/* Return found module (if any) */
	return found;
}


char* _ggzcore_module_get_name(struct _GGZModule *module)
{
	return module->name;
}


char* _ggzcore_module_get_version(struct _GGZModule *module)
{
	return module->version;
}


char* _ggzcore_module_get_prot_engine(struct _GGZModule *module)
{
	return module->prot_engine;
}


char* _ggzcore_module_get_prot_version(struct _GGZModule *module)
{
	return module->prot_version;
}


char* _ggzcore_module_get_author(struct _GGZModule *module)
{
	return module->author;
}


char* _ggzcore_module_get_frontend(struct _GGZModule *module)
{
	return module->frontend;
}


char* _ggzcore_module_get_url(struct _GGZModule *module)
{
	return module->url;
}


char* _ggzcore_module_get_icon_path(struct _GGZModule *module)
{
	return module->icon;
}


char* _ggzcore_module_get_help_path(struct _GGZModule *module)
{
	return module->help;
}


char** _ggzcore_module_get_argv(struct _GGZModule *module)
{
	return module->argv;
}


int _ggzcore_module_launch(struct _GGZModule *module)
{
	ggz_debug("GGZCORE:MODULE", "Launching module: ");
	_ggzcore_module_print(module);

	return -1;
}


void _ggzcore_module_cleanup(void)
{
	if (module_list)
		ggz_list_free(module_list);
	num_modules = 0;
	/* FIXME: do we need to close the file? */
}


/* Static functions internal to this file */

static struct _GGZModule* _ggzcore_module_new(void)
{
	struct _GGZModule *module;

	module = ggz_malloc(sizeof(struct _GGZModule));

	return module;
}

#if 0
static void _ggzcore_module_init(struct _GGZModule *module,
				 const char *name,
				 const char *version,
				 const char *prot_engine,
				 const char *prot_version,
				 const char *author,
				 const char *frontend,
				 const char *url,
				 const char *exec_path,
				 const char *icon_path,
				 const char *help_path)
{
	module->name = ggz_strdup(name);
	module->version = ggz_strdup(version);
	module->prot_engine = ggz_strdup(prot_engine);
	module->prot_version = ggz_strdup(prot_version);
	module->author = ggz_strdup(author);
	module->frontend = ggz_strdup(frontend);
	module->url = ggz_strdup(url);
	/* module->path = ggz_strdup(exec_path);*/
	module->icon = ggz_strdup(icon_path);
	module->help = ggz_strdup(help_path);
}
#endif /* #if 0 */


static void _ggzcore_module_free(struct _GGZModule *module)
{
	
	if (module->name)
		ggz_free(module->name);
	if (module->version)
		ggz_free(module->version);
	if (module->prot_engine)
		ggz_free(module->prot_engine);
	if (module->prot_version)
		ggz_free(module->prot_version);
	if (module->author)
		ggz_free(module->author);
	if (module->frontend)
		ggz_free(module->frontend);
	if (module->url)
		ggz_free(module->url);
	if (module->icon)
		ggz_free(module->icon);
	if (module->help)
		ggz_free(module->help);
	if (module->games)
		_ggz_free_chars(module->games);
	if (module->argv)
		_ggz_free_chars(module->argv);
	
	ggz_free(module);
}


static int _ggzcore_module_add(struct _GGZModule *module)
{
	int status;
	
	if ( (status = ggz_list_insert(module_list, module)) == 0)
		num_modules++;

	return status;
}


static char* _ggzcore_module_conf_filename(void)
{
	char *filename;
	int new_len;

	/* Allow for extra slash and newline when concatenating */
	new_len = strlen(GGZCONFDIR) + strlen(GGZ_MOD_RC) + 2;
	filename = ggz_malloc(new_len);

	strcpy(filename, GGZCONFDIR);
	strcat(filename, "/");
	strcat(filename, GGZ_MOD_RC);

	return filename;
}


static void _ggzcore_module_read(struct _GGZModule *mod, char *id)
{
	int argc;
	/* FIXME: check for errors on all of these */

	/* Note: the memory allocated here is freed in _ggzcore_module_free */
	mod->name = ggz_conf_read_string(mod_handle, id, "Name", NULL);
	mod->version = ggz_conf_read_string(mod_handle, id, "Version", NULL);
	mod->prot_engine = ggz_conf_read_string(mod_handle, id, 
						"ProtocolEngine", NULL);
	mod->prot_version = ggz_conf_read_string(mod_handle, id, 
						 "ProtocolVersion", NULL);
	ggz_conf_read_list(mod_handle, id, "SupportedGames", &argc, 
			   &mod->games);
	mod->author = ggz_conf_read_string(mod_handle, id, "Author", NULL);
	mod->frontend = ggz_conf_read_string(mod_handle, id, "Frontend", NULL);
	mod->url = ggz_conf_read_string(mod_handle, id, "Homepage", NULL);
     	ggz_conf_read_list(mod_handle, id, "CommandLine", &argc, &mod->argv);
	mod->icon = ggz_conf_read_string(mod_handle, id, "IconPath", NULL);
	mod->help = ggz_conf_read_string(mod_handle, id, "HelpPath",  NULL);
}


static void _ggzcore_module_print(struct _GGZModule *module)
{
	int i=0;
	
	ggz_debug("GGZCORE:MODULE", "Name: %s", module->name);
	ggz_debug("GGZCORE:MODULE", "Version: %s", module->version);
	ggz_debug("GGZCORE:MODULE", "ProtocolEngine: %s", module->prot_engine);	
	ggz_debug("GGZCORE:MODULE", "ProtocolVersion: %s", module->prot_version);
	if (module->games)
		while (module->games[i]) {
			ggz_debug("GGZCORE:MODULE", "Game[%d]: %s", i, 
				  module->games[i]);
			++i;
		}

	ggz_debug("GGZCORE:MODULE", "Author: %s", module->author);
	ggz_debug("GGZCORE:MODULE", "Frontend: %s", module->frontend);
	ggz_debug("GGZCORE:MODULE", "URL: %s", module->url);
	ggz_debug("GGZCORE:MODULE", "Icon: %s", module->icon);
	ggz_debug("GGZCORE:MODULE", "Help: %s", module->help);
	while (module->argv[i]) {
		ggz_debug("GGZCORE:MODULE", "Argv[%d]: %s", i, 
			  module->argv[i]);
		++i;
	}
}


static void _ggzcore_module_list_print(void)
{
	GGZListEntry *cur;
	
	for (cur = ggz_list_head(module_list); cur; cur = ggz_list_next(cur))
		_ggzcore_module_print(ggz_list_get_data(cur));
}


/* Utility functions used by ggz_list */

static void _ggz_free_chars(char **argv)
{
	int i;
	
	for (i = 0; argv[i]; i++)
		ggz_free(argv[i]);
	
	ggz_free(argv);
}


static int _ggzcore_module_compare(void* p, void* q)
{
	return 1;
}


/* Match game module by 'name', 'prot_engine', 'prot_version' */
static int _ggzcore_module_match_version(void *p, void *q)
{
	int compare;

	struct _GGZModule *pmod = (struct _GGZModule*)p;
	struct _GGZModule *qmod = (struct _GGZModule*)q;

	compare = strcmp(pmod->name, qmod->name);

	if (compare == 0) {
		compare = strcmp(pmod->prot_engine, qmod->prot_engine);
		if (compare == 0) {
			compare = strcmp(pmod->prot_version, qmod->prot_version);
		}
	}
	
	return compare; 
}

#if 0
static void* _ggzcore_module_create(void* p)
{
	struct _GGZModule *new, *src = p;

	new = _ggzcore_module_new();
	
	_ggzcore_module_init(new, src->name, src->version, 
			     src->prot_engine, src->prot_version,
			     src->author, src->frontend, src->url, 
			     src->argv[0], src->icon, src->help);
			     

	return (void*)new;
}
#endif /* #if 0 */

static void _ggzcore_module_destroy(void* p)
{
	/* Quick sanity check */
	if (!p)
		return;

	_ggzcore_module_free(p);
}
