/*
 * File: player.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: player.h 6116 2004-07-16 19:07:48Z jdorje $
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


#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "room.h"

/* 
 * The Player structure is meant to be a node in a linked list of
 * the players in the current room .
 */
struct _GGZPlayer {
	
	/* Name of player */
	char* name;	

	/* Type of player */
	GGZPlayerType type;

	/* Pointer to room player is in */
	struct _GGZRoom *room;

	/* Server ID of table player is at */
	int table;

	/* Lag of the player */
	int lag;

	/* Record of the player */
#define NO_RECORD -1
	int wins, losses, ties, forfeits;

	/* Rating of the player */
#define NO_RATING 0
	int rating;

	/* Ranking of the player */
#define NO_RANKING 0
	int ranking;

	/* Player's highest score */
#define NO_HIGHSCORE -1
	int highscore;
};


struct _GGZPlayer* _ggzcore_player_new(void);

void _ggzcore_player_free(struct _GGZPlayer *player);

void _ggzcore_player_init(struct _GGZPlayer *player, 
			  const char *name, 
			  struct _GGZRoom *room,
			  const int table,
			  const GGZPlayerType type,
			  const int lag);
void _ggzcore_player_init_stats(GGZPlayer *player,
				const int wins,
				const int losses,
				const int ties,
				const int forfeits,
				const int rating,
				const int ranking,
				const long highscore);


void _ggzcore_player_set_table(struct _GGZPlayer *player, const int table);

void _ggzcore_player_set_lag(struct _GGZPlayer *player, const int lag);

char* _ggzcore_player_get_name(struct _GGZPlayer *player);

GGZPlayerType _ggzcore_player_get_type(struct _GGZPlayer *player);

struct _GGZTable* _ggzcore_player_get_table(struct _GGZPlayer *player);

int _ggzcore_player_get_lag(struct _GGZPlayer *player);
int _ggzcore_player_get_record(GGZPlayer *player,
			       int *wins, int *losses,
			       int *ties, int *forfeits);
int _ggzcore_player_get_rating(GGZPlayer *player, int *rating);
int _ggzcore_player_get_ranking(GGZPlayer *player, int *ranking);
int _ggzcore_player_get_highscore(GGZPlayer *player, int *highscore);

/* Utility functions used by _ggzcore_list */
int   _ggzcore_player_compare(void* p, void* q);
void* _ggzcore_player_create(void* p);
void  _ggzcore_player_destroy(void* p);


#endif /* __PLAYER_H_ */
