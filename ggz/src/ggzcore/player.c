/*
 * File: player.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: player.c 5062 2002-10-27 12:46:20Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "player.h"
#include "room.h"

/* Publicly exported functions */

char* ggzcore_player_get_name(GGZPlayer *player)
{
	if (!player)
		return NULL;

	return _ggzcore_player_get_name(player);
}


GGZPlayerType ggzcore_player_get_type(GGZPlayer *player)
{
	if (!player)
		return -1;

	return _ggzcore_player_get_type(player);
}


GGZTable* ggzcore_player_get_table(GGZPlayer *player)
{
	if (!player)
		return NULL;

	return _ggzcore_player_get_table(player);
}


int ggzcore_player_get_lag(GGZPlayer *player)
{
	if (!player)
		return 0;

	return _ggzcore_player_get_lag(player);
}



void ggzcore_player_get_record(GGZPlayer *player,
			       int *wins, int *losses, int *ties)
{
	if (!player)
		return;

	_ggzcore_player_get_record(player, wins, losses, ties);
}


/* 
 * Internal library functions (prototypes in player.h) 
 * NOTE:All of these functions assume valid inputs!
 */

struct _GGZPlayer* _ggzcore_player_new(void)
{
	struct _GGZPlayer *player;

	player = ggz_malloc(sizeof(struct _GGZPlayer));

	/* Set to invalid table */
	player->table = -1;

	/* Assume no lag */
	player->lag = -1;

	player->wins = -1;
	player->losses = -1;
	player->ties = -1;
	player->rating = -1;

	return player;
}


void _ggzcore_player_init(struct _GGZPlayer *player, 
			  const char *name, 
			  struct _GGZRoom *room,
			  const int table,
			  const GGZPlayerType type,
			  const int lag)
{
	player->name = ggz_strdup(name);
	player->room = room;
	player->table = table;
	player->type = type;
	player->lag = lag;
}


void _ggzcore_player_free(struct _GGZPlayer *player)
{
	if (player->name)
		ggz_free(player->name);

	ggz_free(player);
}


void _ggzcore_player_set_table(struct _GGZPlayer *player, const int table)
{
	player->table = table;
}


void _ggzcore_player_set_lag(struct _GGZPlayer *player, const int lag)
{
	player->lag = lag;
}


char* _ggzcore_player_get_name(struct _GGZPlayer *player)
{
	return player->name;
}


GGZPlayerType _ggzcore_player_get_type(struct _GGZPlayer *player)
{
	return player->type;
}


GGZTable* _ggzcore_player_get_table(struct _GGZPlayer *player)
{
	if (player->table == -1)
		return NULL;
	
	return _ggzcore_room_get_table_by_id(player->room, player->table);
}


int _ggzcore_player_get_lag(struct _GGZPlayer *player)
{
	return player->lag;
}


void _ggzcore_player_get_record(GGZPlayer *player,
				int *wins, int *losses, int *ties)
{
	if (wins)
		*wins = player->wins;
	if (losses)
		*losses = player->losses;
	if (ties)
		*ties = player->ties;
}


int _ggzcore_player_compare(void* p, void* q)
{
	return strcmp(((struct _GGZPlayer*)p)->name, 
		      ((struct _GGZPlayer*)q)->name);
}


void* _ggzcore_player_create(void* p)
{
	struct _GGZPlayer *new, *src = p;

	new = _ggzcore_player_new();
	_ggzcore_player_init(new, src->name, src->room, src->table, src->type, src->lag);

	return (void*)new;
}


void  _ggzcore_player_destroy(void* p)
{
	_ggzcore_player_free(p);
}

/* Static functions internal to this file */



