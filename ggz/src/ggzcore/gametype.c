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


#include <config.h>
#include <gametype.h>
#include <lists.h>
#include <msg.h>

#include <stdlib.h>
#include <string.h>


/* 
 * The game type structure is meant to be a node in a linked list of
 * the game types on the server 
 */
struct _GGZGameType {
	
	/* Game ID (index) */
	unsigned int id;

	/* Name of game */
	char *name;	

	/* Version of game*/
	char *game;

	/* Players allowed */
	unsigned int players;
	
	/* Bots allowed */
	unsigned int bots;

	/* Game description */
	char *desc;

	/* Game author */
	char *author;

	/* Game website */
	char *url;
};

/* List of game types on the server */
static struct _ggzcore_list *gametype_list;
static unsigned int num_gametypes;


/* Local functions for manipulating gametype list */
static void _ggzcore_gametype_list_print(void);
static void _ggzcore_gametype_print(struct _GGZGameType*);

/* Utility functions used by _ggzcore_list */
static int   _ggzcore_gametype_compare(void* p, void* q);
static void* _ggzcore_gametype_create(void* p);
static void  _ggzcore_gametype_destroy(void* p);


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
	new->game = strdup(src->game);
	new->players = src->players;
	new->bots = src->bots;
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
	struct _GGZGameType *gametype = p;

	if (gametype->name)
		free(gametype->name);
	if (gametype->game)
		free(gametype->game);
	if (gametype->desc)
		free(gametype->desc);
	if (gametype->author)
		free(gametype->author);
	if (gametype->url)
		free(gametype->url);

	free(p);
}



static void _ggzcore_gametype_list_print(void)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(gametype_list); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_gametype_print(_ggzcore_list_get_data(cur));
}


static void _ggzcore_gametype_print(struct _GGZGameType *gametype)
{
	ggzcore_debug(GGZ_DBG_GAMETYPE, "Game type %d:", gametype->id);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  name:    %s", gametype->name);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  game:    %s", gametype->game);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  players: %d", gametype->players);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  bots:    %d", gametype->bots);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  game:    %s", gametype->game);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  desc:    %d", gametype->desc);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  author:  %s", gametype->author);
	ggzcore_debug(GGZ_DBG_GAMETYPE, "  url:     %d", gametype->url);
}



/* Lookup Functions */

unsigned int ggzcore_gametype_get_num(void)
{
	return num_gametypes;
}


char* ggzcore_gametype_get_name(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZGameType data, *gametype;

	data.id = id;
	if (!(entry = _ggzcore_list_search(gametype_list, &data)))
		return NULL;

	gametype = _ggzcore_list_get_data(entry);
	
	return gametype->name;
}


char* ggzcore_gametype_get_author(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZGameType data, *gametype;

	data.id = id;
	if (!(entry = _ggzcore_list_search(gametype_list, &data)))
		return NULL;

	gametype = _ggzcore_list_get_data(entry);
	
	return gametype->author;
}


char* ggzcore_gametype_get_url(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZGameType data, *gametype;

	data.id = id;
	if (!(entry = _ggzcore_list_search(gametype_list, &data)))
		return NULL;

	gametype = _ggzcore_list_get_data(entry);
	
	return gametype->url;
}


char* ggzcore_gametype_get_desc(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZGameType data, *gametype;

	data.id = id;
	if (!(entry = _ggzcore_list_search(gametype_list, &data)))
		return NULL;

	gametype = _ggzcore_list_get_data(entry);
	
	return gametype->desc;
}


char** ggzcore_gametype_get_names(void)
{
	int i = 0;
	char **names = NULL;
	struct _ggzcore_list_entry *cur;
	struct _GGZGameType *gametype;

	if (num_gametypes >= 0) {
		if (!(names = calloc((num_gametypes + 1), sizeof(char*))))
			ggzcore_error_sys_exit("calloc() failed in gametype_get_names");
		cur = _ggzcore_list_head(gametype_list);
		while (cur) {
			gametype = _ggzcore_list_get_data(cur);
			names[i++] = gametype->name;
			cur = _ggzcore_list_next(cur);
		}
	}
				
	return names;
}


