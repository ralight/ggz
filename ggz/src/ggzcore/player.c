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


/* Utility functions used by _ggzcore_list */
static int   _ggzcore_player_compare(void* p, void* q);
static void* _ggzcore_player_create(void* p);
static void  _ggzcore_player_destroy(void* p);


/* Publicly exported functions */

char* ggzcore_player_get_name(GGZPlayer *player)
{
	if (!player)
		return NULL;

	return _ggzcore_player_get_name(player);
}


GGZTable* ggzcore_player_get_table(GGZPlayer *player)
{
	if (!player)
		return NULL;

	return _ggzcore_player_get_table(player);
}


/* 
 * Internal library functions (prototypes in player.h) 
 * NOTE:All of these functions assume valid inputs!
 */

struct _ggzcore_list* _ggzcore_player_list_new(void)
{
	return _ggzcore_list_create(_ggzcore_player_compare,
				    _ggzcore_player_create,
				    _ggzcore_player_destroy,
				    0);
}

void _ggzcore_player_init(struct _GGZPlayer *player, const char *name, 
			  struct _GGZTable *table)
{
	player->name = strdup(name);
	player->table = table;
}


char* _ggzcore_player_get_name(struct _GGZPlayer *player)
{
	return player->name;
}


GGZTable* _ggzcore_player_get_table(struct _GGZPlayer *player)
{
	return player->table;
}


/* Static functions internal to this file */


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

