/*
 * File: gametype.c
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: gametype.c 5174 2002-11-03 19:37:36Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "gametype.h"
#include "ggzcore.h"

/* Publicly exported functions */

const char * ggzcore_gametype_get_name(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_name(type);
}


const char * ggzcore_gametype_get_author(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_author(type);
}

const char * ggzcore_gametype_get_prot_engine(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_prot_engine(type);
}


const char * ggzcore_gametype_get_prot_version(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_prot_version(type);
}


const char * ggzcore_gametype_get_version(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_version(type);
}


const char * ggzcore_gametype_get_url(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_url(type);
}


const char * ggzcore_gametype_get_desc(GGZGameType *type)
{
	if (!type)
		return NULL;

	return _ggzcore_gametype_get_desc(type);
}

/* Return the maximum number of allowed players/bots */
int ggzcore_gametype_get_max_players(GGZGameType *type)
{
	if (!type)
		return -1;

	return _ggzcore_gametype_get_max_players(type);
}


int ggzcore_gametype_get_max_bots(GGZGameType *type)
{
	if (!type)
		return -1;

	return _ggzcore_gametype_get_max_bots(type);
}


int ggzcore_gametype_get_spectators_allowed(GGZGameType *type)
{
	if (!type)
		return 0;

	return _ggzcore_gametype_get_spectators_allowed(type);
}


/* Verify that a paticular number of players/bots is valid */
int ggzcore_gametype_num_players_is_valid(GGZGameType *type, unsigned int num)
{
	if (!type)
		return 0;

	return _ggzcore_gametype_num_players_is_valid(type, num);
}


int ggzcore_gametype_num_bots_is_valid(GGZGameType *type, unsigned int num)
{
	if (!type)
		return 0;

	return _ggzcore_gametype_num_bots_is_valid(type, num);
}


/* Internal library functions (prototypes in gametype.h) */

struct _GGZGameType* _ggzcore_gametype_new(void)
{
	struct _GGZGameType *gametype;

	gametype = ggz_malloc(sizeof(struct _GGZGameType));
	
	/* FIXME: any fields we should fill in defaults? */
	return gametype;
}


void _ggzcore_gametype_init(struct _GGZGameType *gametype,
			    const unsigned int id,
			    const char* name, 
			    const char* version,
			    const char* prot_engine,
			    const char* prot_version,
			    const GGZNumberList player_allow_list,
			    const GGZNumberList bot_allow_list,
			    const unsigned int spectators_allowed,
			    const char* desc,
			    const char* author,
			    const char *url)
{
	gametype->id = id;
	gametype->player_allow_list = player_allow_list;
	gametype->bot_allow_list = bot_allow_list;
	gametype->spectators_allowed = spectators_allowed;
	
	gametype->name = ggz_strdup(name);
	gametype->version = ggz_strdup(version);
	gametype->prot_engine = ggz_strdup(prot_engine);
	gametype->prot_version = ggz_strdup(prot_version);
	gametype->desc = ggz_strdup(desc);
	gametype->author = ggz_strdup(author);
	gametype->url = ggz_strdup(url);
}


void _ggzcore_gametype_free(struct _GGZGameType *type)
{
	if (type->name)
		ggz_free(type->name);
	if (type->prot_engine)
		ggz_free(type->prot_engine);
	if (type->prot_version)
		ggz_free(type->prot_version);
	if (type->version)
		ggz_free(type->version);
	if (type->desc)
		ggz_free(type->desc);
	if (type->author)
		ggz_free(type->author);
	if (type->url)
		ggz_free(type->url);

	ggz_free(type);
}


unsigned int _ggzcore_gametype_get_id(struct _GGZGameType *type)
{
	return type->id;
}


const char *  _ggzcore_gametype_get_name(struct _GGZGameType *type)
{
	return type->name;
}


const char * _ggzcore_gametype_get_prot_engine(struct _GGZGameType *type)
{
	return type->prot_engine;
}


const char * _ggzcore_gametype_get_prot_version(struct _GGZGameType *type)
{
	return type->prot_version;
}


const char * _ggzcore_gametype_get_version(struct _GGZGameType *type)
{
	return type->version;
}


const char * _ggzcore_gametype_get_author(struct _GGZGameType *type)
{
	return type->author;
}


const char * _ggzcore_gametype_get_url(struct _GGZGameType *type)
{
	return type->url;
}


const char * _ggzcore_gametype_get_desc(struct _GGZGameType *type)
{
	return type->desc;
}


/* Return the maximum number of allowed players/bots */
unsigned int _ggzcore_gametype_get_max_players(struct _GGZGameType *type)
{
	return ggz_numberlist_get_max(&type->player_allow_list);
}


unsigned int _ggzcore_gametype_get_max_bots(struct _GGZGameType *type)
{
	return ggz_numberlist_get_max(&type->bot_allow_list);
}


unsigned int _ggzcore_gametype_get_spectators_allowed(struct _GGZGameType *type)
{
	return type->spectators_allowed;
}

/* Verify that a paticular number of players/bots is valid */
unsigned int _ggzcore_gametype_num_players_is_valid(struct _GGZGameType *type, unsigned int num)
{
	return ggz_numberlist_isset(&type->player_allow_list, num);
}


unsigned int _ggzcore_gametype_num_bots_is_valid(struct _GGZGameType *type, unsigned int num)
{
	return num == 0
	  || ggz_numberlist_isset(&type->bot_allow_list, num);
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
			       src->prot_engine, src->prot_version,
			       src->player_allow_list, src->bot_allow_list,
			       src->spectators_allowed,
			       src->desc, src->author, src->url);
	
	return new;
}


void  _ggzcore_gametype_destroy(void* p)
{
	_ggzcore_gametype_free(p);
}


/* Private functions internal to this file */





