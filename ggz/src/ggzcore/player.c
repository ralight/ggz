/*
 * File: player.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: player.c 6785 2005-01-21 18:48:01Z jdorje $
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



int ggzcore_player_get_record(GGZPlayer *player,
			      int *wins, int *losses,
			      int *ties, int *forfeits)
{
	if (!player || !wins || !losses || !ties || !forfeits)
		return 0;
	return _ggzcore_player_get_record(player, wins, losses,
					  ties, forfeits);
}

int ggzcore_player_get_rating(GGZPlayer *player, int *rating)
{
	if (!player || !rating)
		return 0;
	return _ggzcore_player_get_rating(player, rating);
}

int ggzcore_player_get_ranking(GGZPlayer *player, int *ranking)
{
	if (!player || !ranking)
		return 0;
	return _ggzcore_player_get_ranking(player, ranking);
}

int ggzcore_player_get_highscore(GGZPlayer *player, int *highscore)
{
	if (!player || !highscore)
		return 0;
	return _ggzcore_player_get_highscore(player, highscore);
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

	player->wins = NO_RECORD;
	player->losses = NO_RECORD;
	player->ties = NO_RECORD;
	player->forfeits = NO_RECORD;
	player->rating = NO_RATING;
	player->ranking = NO_RANKING;
	player->highscore = NO_HIGHSCORE;

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


void _ggzcore_player_init_stats(GGZPlayer *player,
				const int wins,
				const int losses,
				const int ties,
				const int forfeits,
				const int rating,
				const int ranking,
				const long highscore)
{
	player->wins = wins;
	player->losses = losses;
	player->ties = ties;
	player->forfeits = forfeits;
	player->rating = rating;
	player->ranking = ranking;
	player->highscore = highscore;
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
	
	return ggzcore_room_get_table_by_id(player->room, player->table);
}


int _ggzcore_player_get_lag(struct _GGZPlayer *player)
{
	return player->lag;
}


int _ggzcore_player_get_record(GGZPlayer *player,
				int *wins, int *losses,
				int *ties, int *forfeits)
{
	if (player->wins == NO_RECORD
	    && player->losses == NO_RECORD
	    && player->ties == NO_RECORD
	    && player->forfeits == NO_RECORD)
		return 0;

#ifndef MAX
#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

	/* NO_RECORD is -1.  If we have a stat for anything, we should
	   return all stats and assume 0 for any we don't know. */

	*wins = MAX(player->wins, 0);
	*losses = MAX(player->losses, 0);
	*ties = MAX(player->ties, 0);
	*forfeits = MAX(player->forfeits, 0);

	return 1;
}


int _ggzcore_player_get_rating(GGZPlayer *player,
				int *rating)
{
	if (player->rating == NO_RATING)
		return 0;
	*rating = player->rating;
	return 1;
}


int _ggzcore_player_get_ranking(GGZPlayer *player,
				int *ranking)
{
	if (player->ranking == NO_RANKING)
		return 0;
	*ranking = player->ranking;
	return 1;
}


int _ggzcore_player_get_highscore(GGZPlayer *player, int *highscore)
{
	if (player->highscore == NO_HIGHSCORE)
		return 0;
	*highscore = player->highscore;
	return 1;
}


int _ggzcore_player_compare(const void *p, const void *q)
{
	const GGZPlayer *s_p = p;
	const GGZPlayer *s_q = q;

	return strcmp(s_p->name, s_q->name);
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



