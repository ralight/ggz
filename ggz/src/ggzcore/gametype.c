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

struct _GGZGameType* _ggzcore_gametype_new(void)
{
	struct _GGZGameType *gametype;

	gametype = ggzcore_malloc(sizeof(struct _GGZGameType));
	
	/* FIXME: any fields we should fill in defaults? */
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
	gametype->allow_players = allow_players;
	gametype->allow_bots = allow_bots;
	
	if (name)
		gametype->name = strdup(name);
	if (version)
		gametype->version = strdup(version);
	if (desc)
		gametype->desc = strdup(desc);
	if (author)
		gametype->author = strdup(author);
	if (url)
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

	ggzcore_free(type);
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


/* Return 0 if equal, -1 greaterthan, 1 lessthan */
int _ggzcore_gametype_compare(void* p, void* q)
{
        if(((struct _GGZGameType*)p)->id == ((struct _GGZGameType*)q)->id)
                return 0;
        if(((struct _GGZGameType*)p)->id > ((struct _GGZGameType*)q)->id)
                return 1;
        if(((struct _GGZGameType*)p)->id < ((struct _GGZGameType*)q)->id)
                return -1;
         
        return 0;
}


void* _ggzcore_gametype_create(void* p)
{
	struct _GGZGameType *new, *src = p;

	new = _ggzcore_gametype_new();

	_ggzcore_gametype_init(new, src->id, src->name, src->version,
			       src->allow_players, src->allow_bots, src->desc,
			       src->author, src->url);
	
	return (void*)new;
}


void  _ggzcore_gametype_destroy(void* p)
{
	_ggzcore_gametype_free(p);
}


/* Private functions internal to this file */





