/*
 * File: player.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling players
 *
 * Copyright (C) 1998 Brent Hendricks.
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
#include <player.h>
#include <lists.h>
#include <msg.h>

#include <stdlib.h>
#include <string.h>

/* 
 * The Player structure is meant to be a node in a linked list of
 * the players in the current room .
 */
struct _GGZPlayer {
	
	/* Name of player */
	char* name;	

	/* Index of table at which this player is "sitting" */
	int table;
};


/* List of players in current room */
static struct _ggzcore_list *player_list;

/* Local functions for manipulating player list */
static void _ggzcore_player_list_print(void);
static void _ggzcore_player_print(struct _GGZPlayer*);

/* Utility functions used by _ggzcore_list */
static int   _ggzcore_player_compare(void* p, void* q);
static void* _ggzcore_player_create(void* p);
static void  _ggzcore_player_destroy(void* p);


void _ggzcore_player_list_clear(void)
{
	if (player_list)
		_ggzcore_list_destroy(player_list);
	
	player_list = _ggzcore_list_create(_ggzcore_player_compare,
					   _ggzcore_player_create,
					   _ggzcore_player_destroy,
					   0);
}


int _ggzcore_player_list_add(const char* name, const int table)
{
	int status;
	struct _GGZPlayer player;

	ggzcore_debug(GGZ_DBG_PLAYER, "Adding %s to player list", name);
	
	player.name = (char*)name;
	player.table = table;

	status = _ggzcore_list_insert(player_list, (void*)&player);
	_ggzcore_player_list_print();

	return status;
}


int _ggzcore_player_list_remove(const char* name)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZPlayer player;

	ggzcore_debug(GGZ_DBG_PLAYER, "Removing %s from player list", name);
	
	player.name = (char*)name;
	if (!(entry = _ggzcore_list_search(player_list, &player)))
		return -1;

	_ggzcore_list_delete_entry(player_list, entry);
	_ggzcore_player_list_print();

	return 0;
}


int _ggzcore_player_list_replace(const char* name, const int table)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZPlayer data, *player;
	
	ggzcore_debug(GGZ_DBG_PLAYER, "Updating player info for %s", name);

	data.name = (char*)name;
	if (!(entry = _ggzcore_list_search(player_list, &data)))
		return -1;

	/* Update information */
	player = _ggzcore_list_get_data(entry);
	player->table = table;

	_ggzcore_player_list_print();

	return 0;
}


static int _ggzcore_player_compare(void* p, void* q)
{
	return strcmp(((struct _GGZPlayer*)p)->name, 
		      ((struct _GGZPlayer*)q)->name);
}


static void* _ggzcore_player_create(void* p)
{
	struct _GGZPlayer *new, *src = p;

	if (!(new = malloc(sizeof(struct _GGZPlayer))))
		ggzcore_error_sys_exit("malloc failed in player_create");

	new->name = strdup(src->name);
	new->table = src->table;

	return (void*)new;
}


static void  _ggzcore_player_destroy(void* p)
{
	struct _GGZPlayer *player = p;

	if (player->name)
		free(player->name);
	free(p);
}


static void _ggzcore_player_list_print(void)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(player_list); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_player_print(_ggzcore_list_get_data(cur));
}


static void _ggzcore_player_print(struct _GGZPlayer *player)
{
	ggzcore_debug(GGZ_DBG_PLAYER, "Player name: %s", player->name);
	ggzcore_debug(GGZ_DBG_PLAYER, "Player table: %d", player->table);
}
