/* 
 * File: ggz_stats.h
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 9/4/01
 * Desc: GGZ game module stat functions
 * $Id: ggz_stats.h 5996 2004-05-17 14:16:42Z josef $
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#ifndef __GGZ_STATS__
#define __GGZ_STATS__

#include "ggzdmod.h"

typedef struct GGZStats GGZStats;

/* Debugging type (used by ggz_debug) */
#define DBG_GGZSTATS "ggz-stats"


/* Constructor - call to create a new GGZ Stats tracker */
GGZStats *ggzstats_new(GGZdMod * ggzdmod);
void ggzstats_free(GGZStats * stats);

/* Call to re-read stats (as when players change, etc.) */
int ggzstats_reread(GGZStats * stats);

/* Call these once to initialize teams */
void ggzstats_set_num_teams(GGZStats * stats, int num_teams);
void ggzstats_set_team(GGZStats * stats, int player, int team);

/* Call this at the start of each game (to reset data for calculating stats
   about that game) */
void ggzstats_new_game(GGZStats * stats);

/* for display purposes.  Return 0 on success, -1 on failure. */
int ggzstats_get_record(GGZStats * stats, int player,
			int *wins, int *losses, int *ties);
int ggzstats_get_rating(GGZStats * stats, int player, int *rating);

/* score goes from 0-1 and indicates how much the player won that game.  0
   indicates a loss (the default), 1 a win, 1/n an n-way tie.  FIXME:
   currently you should use 1/n if a team with n players on it wins the game. 
 */
void ggzstats_set_game_winner(GGZStats * stats, int player, double win_pct);

/* Call this to recaculate win-loss records, and write the data to the
   database.  Must be called just once per game, after calling
   ggzd_set_game_winner for each player. Returns 0 on success, -1 on failure. 
 */
int ggzstats_recalculate_records(GGZStats * stats);

/* recalculate all ratings.  Must be called just once per game, after calling 
   ggzd_set_game_winner for each player.  Returns 0 on success, -1 on
   failure. */
int ggzstats_recalculate_ratings(GGZStats * stats);

void ggzstats_add_game(GGZStats * stats);

#endif /* __GGZ_STATS__ */
