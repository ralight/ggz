/* 
 * File: messages.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data for messaging system
 * $Id: score.c 8987 2007-02-19 02:50:30Z jdorje $
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
#  include <config.h>	/* Site-specific config */
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "message.h"
#include "net.h"
#include "score.h"
#include "team.h"
#include "types.h"

int get_team_score(team_t t)
{
	assert(t >= 0 && t < game.num_teams);
	return game.teams[t].score.score;
}

int get_player_score(player_t p)
{
	return get_team_score(get_player_team(p));
}


void clear_score(team_t t)
{
	int i;

	game.teams[t].score.score = 0;
	for (i = 0; i < SCORE_EXTRAS; i++) {
		game.teams[t].score.extra[i] = 0;
	}
}

void change_score(team_t t, int scorediff)
{
	game.teams[t].score.score += scorediff;
}

/** @brief Change the full scores for this team. */
void change_score_full(team_t t, score_data_t scorediff)
{
	int i;

	game.teams[t].score.score += scorediff.score;
	for (i = 0; i < SCORE_EXTRAS; i++) {
		game.teams[t].score.extra[i] += scorediff.extra[i];
	}
}

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
	int widths[game.num_teams];

	if (!game.cumulative_scores)
		return;

	/* First, put up the list of players/teams and calculate the string
	   width of each.  This is a pretty complicated process... */
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, " # ");

	memset(widths, 0, sizeof(widths));

	teams_iterate(t) {
		int num = 0;
		player_t p;
		char *orig = buf + buf_len;

		for (p = 0; p < game.num_players; p++) {
			if (game.players[p].team == t) {
				if (num != 0) {
					buf_len +=
					    snprintf(buf + buf_len,
						     sizeof(buf) - buf_len,
						     "/");
				}

				buf_len +=
				    snprintf(buf + buf_len,
					     sizeof(buf) - buf_len, "%s",
					     get_player_name(p));
				num++;
			}
		}

		widths[t] = strlen(orig);

		if (t != game.num_teams - 1) {
			buf_len +=
			    snprintf(buf + buf_len, sizeof(buf) - buf_len,
				     " ");
		}
	}
	teams_iterate_end;

	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r < game.hand_num + 1; r++) {
		buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len,
				    "%2d ", r + 1);

		teams_iterate(t) {
			int score = game.teams[t].scores[r].score;
			int iw = int_len(score);
			int extra = widths[t] - iw;

			/* this overcomplicated bit of hackery is intended to
			   center the score under the name. Unfortunately, it
			   assumes the number isn't longer than 3 characters. */
			buf_len +=
			    snprintf(buf + buf_len, sizeof(buf) - buf_len,
				     "%*s%*d%*s%s", extra / 2, "", iw,
				     score, (extra + 1) / 2, "",
				     (t == game.num_teams - 1) ? "" : " ");
		}
		teams_iterate_end;
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
	teams_iterate(t) {
		game.teams[t].scores = ggz_realloc(game.teams[t].scores,
						   (game.hand_num +
						    1) *
						   sizeof(*game.teams[t].
							  scores));
		game.teams[t].scores[game.hand_num] = game.teams[t].score;
	}
	teams_iterate_end;

	net_broadcast_scores(game.hand_num);

	send_cumulative_scores();
}
