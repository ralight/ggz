/*
 * File: ggz_stats.h
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 9/4/01
 * Desc: GGZ game module stat functions
 * $Id: ggz_stats.h 2364 2001-09-05 06:47:26Z jdorje $
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

typedef enum {
	GGZ_ELO_RATING
} GGZRatingSystem;

/* Set the rating system; i.e. ELO or Glicko-1.
 * ELO is the easiest to implement. */
void ggzd_set_rating_system(GGZRatingSystem system);

/* for display purposes only */
int ggzd_get_rating(int player);

void ggzd_set_num_teams(int num_teams);
void ggzd_set_team(int player, int team);

/* score goes from 0-1 and indicates how much the
 * player won that game.  0 indicates a loss (the
 * default), 1 a win, 1/n an n-way tie. */
void ggzd_set_game_winner(int player, double score);

/* recalculate all ratings.  This function can
 * fail if for instance the scores don't sum to
 * 1.  Ultimately, this should extract the data
 * from the DB (with locking), recalculate, and
 * send the data back to the database.  In the
 * short-term there is no database so the data
 * should be held locally for the next game. */
int ggzd_recalculate_ratings();
