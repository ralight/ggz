/*
 * File: gametype.c
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This file contains functions for hadiling games
 *
 * Copyright (C) 1998 Justin Zaun.
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
#include "gametype.h"
#include "lists.h"
#include "msg.h"

#include <stdlib.h>
#include <string.h>


/* Utility functions used by _ggzcore_list */
#if 0
static int   _ggzcore_gametype_compare(void* p, void* q);
static void* _ggzcore_gametype_create(void* p);
static void  _ggzcore_gametype_destroy(void* p);
#endif


/* Publicly exported functions */

char* ggzcore_gametype_get_name(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_name(type);
}


char* ggzcore_gametype_get_author(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_author(type);
}


char* ggzcore_gametype_get_url(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_url(type);
}


char* ggzcore_gametype_get_desc(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_desc(type);
}


/* Internal library functions (prototypes in gametype.h) */

struct _GGZGameType* _ggzcore_gametype_new(const unsigned int id, 
					   const char* name, 
					   const char* version,
					   const GGZAllowed allow_players, 
					   const GGZAllowed allow_bots,  
					   const char* desc,
					   const char* author, 
					   const char *url)
{
	struct _GGZGameType *gametype;

	/* Allocate and zero space for GGZGameType object */
	if (!(gametype = calloc(1, sizeof(struct _GGZGameType))))
		ggzcore_error_sys_exit("malloc() failed in ggzcore_gametype_new");
	
	_ggzcore_gametype_init(gametype, id, name, version, allow_players,
			       allow_bots, desc, author, url);

	return gametype;
}


void _ggzcore_gametype_init(struct _GGZGameType *gametype,
			    const unsigned int id,
			    const char* name, 
			    const char* version,
			    const GGZAllowed allow_players, 
			    const GGZAllowed allow_bots,  
			    const char* desc,
			    const char* author, 
			    const char *url)
{
	gametype->id = id;
	gametype->name = strdup(name);
	gametype->version = strdup(version);
	gametype->allow_players = allow_players;
	gametype->allow_bots = allow_bots;
	gametype->desc = strdup(desc);
	gametype->author = strdup(author);
	gametype->url = strdup(url);
}


void _ggzcore_gametype_free(struct _GGZGameType *type)
{
	if (type->name)
		free(type->name);
	if (type->version)
		free(type->version);
	if (type->desc)
		free(type->desc);
	if (type->author)
		free(type->author);
	if (type->url)
		free(type->url);

	free(type);
}


unsigned int _ggzcore_gametype_get_id(struct _GGZGameType *type)
{
	return type->id;
}


char*  _ggzcore_gametype_get_name(struct _GGZGameType *type)
{
	return type->name;
}


char*  _ggzcore_gametype_get_version(struct _GGZGameType *type)
{
	return type->version;
}


char*  _ggzcore_gametype_get_author(struct _GGZGameType *type)
{
	return type->author;
}


char*  _ggzcore_gametype_get_url(struct _GGZGameType *type)
{
	return type->url;
}


char*  _ggzcore_gametype_get_desc(struct _GGZGameType *type)
{
	return type->desc;
}


#if 0
void _ggzcore_gametype_list_clear(void)
{
	if (gametype_list)
		_ggzcore_list_destroy(gametype_list);
	
	gametype_list = _ggzcore_list_create(_ggzcore_gametype_compare,
					 _ggzcore_gametype_create,
					 _ggzcore_gametype_destroy,
					 0);
	num_gametypes = 0;
}


int _ggzcore_gametype_list_add(const unsigned int id, const char* name, const char* game,
			   const unsigned int players, const int bots,  const char* desc,
			   const char* author, const char *url)
{
	int status;
	struct _GGZGameType gametype;

	ggzcore_debug(GGZ_DBG_GAMETYPE, "Adding gametype %d to gametype list", id);
	
	gametype.id = id;
	gametype.name = (char*)name;
	gametype.game = (char*)game;
	gametype.players = players;
	gametype.bots = bots;
	gametype.desc = (char*)desc;
	gametype.author = (char*)author;
	gametype.url = (char*)url;

	if ( (status = _ggzcore_list_insert(gametype_list, (void*)&gametype)) == 0)
		num_gametypes++;
	_ggzcore_gametype_list_print();

	return status;
}


int _ggzcore_gametype_list_remove(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZGameType gametype;

	ggzcore_debug(GGZ_DBG_GAMETYPE, "Removing game type %d from gametype list", id);
	
	gametype.id = id;
	if (!(entry = _ggzcore_list_search(gametype_list, &gametype)))
		return -1;

	_ggzcore_list_delete_entry(gametype_list, entry);
	num_gametypes--;

	_ggzcore_gametype_list_print();

	return 0;
}
#endif

/* Private functions internal to this file */

#if 0
/* Return 0 if equal, -1 greaterthan, 1 lessthan */
static int _ggzcore_gametype_compare(void* p, void* q)
{
        if(((struct _GGZGameType*)p)->id == ((struct _GGZGameType*)q)->id)
                return 0;
        if(((struct _GGZGameType*)p)->id > ((struct _GGZGameType*)q)->id)
                return 1;
        if(((struct _GGZGameType*)p)->id < ((struct _GGZGameType*)q)->id)
                return -1;
         
        return 0;
}


static void* _ggzcore_gametype_create(void* p)
{
	struct _GGZGameType *new, *src = p;

	if (!(new = malloc(sizeof(struct _GGZGameType))))
		ggzcore_error_sys_exit("malloc failed in gametype_create");

	new->id = src->id;
	new->name = strdup(src->name);
	new->version = strdup(src->version);
	new->allow_players = src->allow_players;
	new->allow_bots = src->allow_bots;
	if (src->desc)
		new->desc = strdup(src->desc);
	else
		new->desc = NULL;
	if (src->author)
		new->author = strdup(src->author);
	else
		new->author = NULL;
	if (src->url)
		new->url = strdup(src->url);
	else
		new->url = NULL;

	return (void*)new;
}


static void  _ggzcore_gametype_destroy(void* p)
{
	_ggzcore_gametype_free(p);
}
#endif




