/* 
 * File: messages.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data for messaging system
 * $Id: message.c 8427 2006-07-31 22:50:50Z jdorje $
 *
 * Right now GGZCards uses a super-generic "messaging" system.  Text
 * messages are generated by the server and sent to the client for
 * direct display.  This makes i18n basically impossible.  It also
 * makes really cool client displays basically impossible.  However,
 * it makes it really easy to add new games - you don't even have to
 * change the client!
 *
 * A more complicated alternate messaging system is in planning.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "message.h"
#include "score.h"
#include "types.h"

#define NUM_SCORERS ((game.num_teams == 0) \
		     ? game.num_players : game.num_teams)
static int *cumulative_scores = NULL;

/* Surely there's some library function to do this... */
static int int_len(int x)
{
	int len = x < 0 ? 1 : 0;

	for (x = abs(x); x > 0; x /= 10)
		len++;

	return len;
}

/* This function is extra-ordinarily overcomplicated.  I hate string
   handling... */
static void send_cumulative_scores(void)
{
	char buf[4096];
	int buf_len = 0, r;
	int widths[game.num_players];

	if (!game.cumulative_scores)
		return;

	/* First, put up the list of players/teams and calculate the string
	   width of each.  This is a pretty complicated process... */
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len,
			    " # ");
	if (game.num_teams == 0) {
		player_t p;
		for (p = 0; p < game.num_players; p++) {
			widths[p] = strlen(get_player_name(p));
			if (widths[p] < 4)
				widths[p] = 4;
			buf_len +=
				snprintf(buf + buf_len, sizeof(buf) - buf_len,
					 "%*s%s", widths[p],
					 get_player_name(p),
					 p ==
					 game.num_players - 1 ? "" : " ");
		}
	} else {
		team_t t;

		memset(widths, 0, sizeof(widths));

		for (t = 0; t < game.num_teams; t++) {
			int num = 0;
			player_t p;
			char *orig = buf + buf_len;

			for (p = 0; p < game.num_players; p++) {
				if (game.players[p].team == t) {
					if (num != 0)
						buf_len +=
							snprintf(buf +
								 buf_len,
								 sizeof(buf) -
								 buf_len,
								 "/");

					buf_len +=
						snprintf(buf + buf_len,
							 sizeof(buf) -
							 buf_len, "%s",
							 get_player_name(p));

					num++;
				}
			}

			widths[t] = strlen(orig);

			if (t != game.num_teams - 1) {
				buf_len +=
					snprintf(buf + buf_len,
						 sizeof(buf) - buf_len, " ");
			}
		}
	}
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r < game.hand_num + 1; r++) {
		int x;		/* player or team... */
		const int max = NUM_SCORERS;

		buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len,
				    "%2d ", r + 1);

		for (x = 0; x < max; x++) {
			int score = cumulative_scores[r * max + x];
			int iw = int_len(score);
			int extra = widths[x] - iw;

			/* this overcomplicated bit of hackery is intended to
			   center the score under the name. Unfortunately, it
			   assumes the number isn't longer than 3 characters. */
			buf_len +=
				snprintf(buf + buf_len, sizeof(buf) - buf_len,
					 "%*s%*d%*s%s", extra / 2, "", iw,
					 score, (extra + 1) / 2, "",
					 (x == max - 1) ? "" : " ");
		}
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");
	}

	set_global_message("Scores", "%s", buf);
}

void init_cumulative_scores(void)
{
	update_cumulative_scores();
}

/* add on this hand's scores */
void update_cumulative_scores(void)
{
	int *scores;

	if (!game.cumulative_scores)
		return;

	cumulative_scores = ggz_realloc(cumulative_scores,
					(game.hand_num + 1) * NUM_SCORERS
					* sizeof(*cumulative_scores));
	scores = cumulative_scores + (game.hand_num) * NUM_SCORERS;

	players_iterate(p) {
		if (game.num_teams == 0) {
			scores[p] = game.players[p].score;
		} else {
			scores[game.players[p].team]
			  = game.players[p].score;
		}
	} players_iterate_end;

	send_cumulative_scores();
}

