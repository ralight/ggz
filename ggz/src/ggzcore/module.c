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
#include "confio.h"
#include "module.h"
#include "msg.h"
#include "game.h"

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define GGZ_MOD_RC "ggz.modules"

       
/* List of modules */
static struct _ggzcore_list *module_list;
static unsigned int num_modules;
static int mod_handle = -1;

/* static internal functions */
static struct _GGZModule* _ggzcore_module_new(void);
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
static void _ggzcore_module_free(struct _GGZModule *module);
static void _ggzcore_module_read(struct _GGZModule *mod, char *id);
static int _ggzcore_module_add(struct _GGZModule *module);

static char* _ggzcore_module_conf_filename(void);
static void _ggzcore_module_print(struct _GGZModule*);
static void _ggzcore_module_list_print(void);
/* Utility functions used by _ggzcore_list */
static int   _ggzcore_module_compare(void* p, void* q);
#if 0
static int   _ggzcore_module_match_version(void *p, void *q);
#endif
static void* _ggzcore_module_create(void* p);
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
		ggzcore_debug(GGZ_DBG_MODULE, "module_setup() called twice");
		return -1;
	}

	
	module_list = _ggzcore_list_create(_ggzcore_module_compare, NULL,
					   _ggzcore_module_destroy, 0);
	num_modules = 0;

	file = _ggzcore_module_conf_filename();
	ggzcore_debug(GGZ_DBG_MODULE, "Reading %s", file);
	mod_handle = ggzcore_confio_parse(file, GGZ_CONFIO_RDONLY);
	/* Free up space taken by name */
	ggzcore_free(file);
	
	if (mod_handle == -1) {
		ggzcore_debug(GGZ_DBG_MODULE, "Unable to load module conffile");
		return -1;
	}
	
	/* Read in list of supported gametypes */
	status = ggzcore_confio_read_list(mod_handle, "Games", "*Engines*", 
					  &count_types, &games);
	if (status < 0) {
		ggzcore_debug(GGZ_DBG_MODULE, "Couldn't read engine list");
		return -1;
	}	
	ggzcore_debug(GGZ_DBG_MODULE, "%d game engines supported", count_types);
	
	for (i = 0; i < count_types; i++) {
		status = ggzcore_confio_read_list(mod_handle, "Games", 
						  games[i], &count_modules, 
						  &ids);

		ggzcore_debug(GGZ_DBG_MODULE, "%d modules for %s", count_modules,
			      games[i]);

		for (j = 0; j < count_modules; j++) {
			module = _ggzcore_module_new();
			_ggzcore_module_read(module, ids[j]);
			_ggzcore_module_add(module);
			ggzcore_debug(GGZ_DBG_MODULE, "Module %d: %s", j, 
				      ids[j]);
		}
	}

	_ggzcore_module_list_print();
	
	signal(SIGCHLD, _ggzcore_game_dead);

	return 0;
}


unsigned int _ggzcore_module_get_num(void)
{
	return num_modules;
}


int _ggzcore_module_get_num_by_type(const char *game, 
				    const char *engine,
				    const char *version)
{
	int count, status, i;
	char **ids;
	struct _GGZModule module;

	/* Get total count for this game (regardless of version) */
	status = ggzcore_confio_read_list(mod_handle, "Games", game,
					  &count, &ids);
	if (status < 0)
		return 0;
	
	for (i = 0; i < count; i++) {
		_ggzcore_module_read(&module, ids[i]);
		/* Subtract out modules that aren't the same protocol */
		if (strcmp(engine, module.prot_engine) != 0
		    || strcmp(version, module.prot_version) != 0)
			count--;
	}


	return count;
}


/* FIXME: do this right */
struct _GGZModule* _ggzcore_module_get_nth_by_type(const char *game, 
						   const char *engine,
						   const char *version,
						   const unsigned int num)
{
	int i, total, status, count;
	char **ids;
	struct _GGZModule *module;

	status = ggzcore_confio_read_list(mod_handle, "Games", engine,
					  &total, &ids);

	ggzcore_debug(GGZ_DBG_MODULE, "Found %d modules matching %s", total,
		      engine);
	
	if (status < 0)
		return NULL;

	if (num >= total)
		return NULL;
	
	count = 0;
	for (i = 0; i < total; i++) {
		module = _ggzcore_module_new();
		_ggzcore_module_read(module, ids[i]);
		if (strcmp(version, module->prot_version) == 0) {
			/* FIXME:  also check to see if 'game' is in supported list */
			if (count++ == num)
				return module;
		}
	}
	
	/* If we get here it wasn't found */

	return NULL;
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
	ggzcore_debug(GGZ_DBG_MODULE, "Launching module: ");
	_ggzcore_module_print(module);

	return -1;
}


void _ggzcore_module_cleanup(void)
{
	if (module_list)
		_ggzcore_list_destroy(module_list);
	num_modules = 0;
	/* FIXME: do we need to close the file? */
}


/* Static functions internal to this file */

static struct _GGZModule* _ggzcore_module_new(void)
{
	struct _GGZModule *module;

	module = ggzcore_malloc(sizeof(struct _GGZModule));

	return module;
}

		
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
	if (name)
		module->name = strdup(name);
	if (version)
		module->version = strdup(version);
	if (prot_engine)
		module->prot_engine = strdup(prot_engine);
	if (prot_version)
		module->prot_version = strdup(prot_version);
	if (author)
		module->author = strdup(author);
	if (frontend)
		module->frontend = strdup(frontend);
	if (url)
		module->url = strdup(url);
/*	if (exec_path)
	module->path = strdup(exec_path);*/
	if (icon_path)
		module->icon = strdup(icon_path);
	if (help_path)
		module->help = strdup(help_path);
}


static void _ggzcore_module_free(struct _GGZModule *module)
{
	
	if (module->name)
		free(module->name);
	if (module->version)
		free(module->version);
	if (module->prot_engine)
		free(module->prot_engine);
	if (module->prot_version)
		free(module->prot_version);
	if (module->author)
		free(module->author);
	if (module->frontend)
		free(module->frontend);
	if (module->url)
		free(module->url);
	/*if (module->path)
	  free(module->path);*/
	if (module->icon)
		free(module->icon);
	if (module->help)
		free(module->help);
	
	ggzcore_free(module);
}


static int _ggzcore_module_add(struct _GGZModule *module)
{
	int status;
	
	if ( (status = _ggzcore_list_insert(module_list, module)) == 0)
		num_modules++;

	return status;
}


static char* _ggzcore_module_conf_filename(void)
{
	char *filename;
	int new_len;

	/* Allow for extra slash and newline when concatenating */
	new_len = strlen(GGZCONFDIR) + strlen(GGZ_MOD_RC) + 2;
	filename = ggzcore_malloc(new_len);

	strcpy(filename, GGZCONFDIR);
	strcat(filename, "/");
	strcat(filename, GGZ_MOD_RC);

	return filename;
}


static void _ggzcore_module_read(struct _GGZModule *mod, char *id)
{
	int argc;
	/* FIXME: check for errors on all of these */

	mod->name = ggzcore_confio_read_string(mod_handle, id, "Name", NULL);
	mod->version = ggzcore_confio_read_string(mod_handle, id, "Version",
						     NULL);
	mod->prot_engine = ggzcore_confio_read_string(mod_handle, id, 
						      "ProtocolEngine", NULL);

	mod->prot_engine = ggzcore_confio_read_string(mod_handle, id, 
						      "ProtocolEngine", NULL);

	mod->prot_version = ggzcore_confio_read_string(mod_handle, id, 
						       "ProtocolVersion", NULL);
	ggzcore_confio_read_list(mod_handle, id, "SupportedGames", &argc, 
				 &mod->games);
      
	mod->author = ggzcore_confio_read_string(mod_handle, id, "Author", 
						  NULL);
	mod->frontend = ggzcore_confio_read_string(mod_handle, id, "Frontend",
						    NULL);
	mod->url = ggzcore_confio_read_string(mod_handle, id, "Homepage", NULL);
     	ggzcore_confio_read_list(mod_handle, id, "CommandLine", &argc, 
				 &mod->argv);
	
	mod->icon = ggzcore_confio_read_string(mod_handle, id, "IconPath", 
						NULL);
	mod->help = ggzcore_confio_read_string(mod_handle, id, "HelpPath", 
						NULL);
}


static void _ggzcore_module_print(struct _GGZModule *module)
{
	int i=0;
	
	ggzcore_debug(GGZ_DBG_MODULE, "Name: %s", module->name);
	ggzcore_debug(GGZ_DBG_MODULE, "Version: %s", module->version);
	ggzcore_debug(GGZ_DBG_MODULE, "ProtocolEngine: %s", module->prot_engine);	
	ggzcore_debug(GGZ_DBG_MODULE, "ProtocolVersion: %s", module->prot_version);
	if (module->games)
		while (module->games[i]) {
			ggzcore_debug(GGZ_DBG_MODULE, "Game[%d]: %s", i, 
				      module->games[i]);
			++i;
		}

	ggzcore_debug(GGZ_DBG_MODULE, "Author: %s", module->author);
	ggzcore_debug(GGZ_DBG_MODULE, "Frontend: %s", module->frontend);
	ggzcore_debug(GGZ_DBG_MODULE, "URL: %s", module->url);
	ggzcore_debug(GGZ_DBG_MODULE, "Icon: %s", module->icon);
	ggzcore_debug(GGZ_DBG_MODULE, "Help: %s", module->help);
	while (module->argv[i]) {
		ggzcore_debug(GGZ_DBG_MODULE, "Argv[%d]: %s", i, 
			      module->argv[i]);
		++i;
	}
}


static void _ggzcore_module_list_print(void)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(module_list); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_module_print(_ggzcore_list_get_data(cur));
}


/* Utility functions used by _ggzcore_list */
static int _ggzcore_module_compare(void* p, void* q)
{
	return 1;
}


#if 0
static int _ggzcore_module_match_version(void *p, void *q)
{
	int compare;

	struct _GGZModule *pmod = (struct _GGZModule*)p;
	struct _GGZModule *qmod = (struct _GGZModule*)q;

	compare = strcmp(pmod->game, qmod->game);

	if (compare != 0)
		return compare;
	else
		return strcmp(pmod->protocol, qmod->protocol);
}
#endif

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


static void _ggzcore_module_destroy(void* p)
{
	/* Quick sanity check */
	if (!p)
		return;

	_ggzcore_module_free(p);
}
