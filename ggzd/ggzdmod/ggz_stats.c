/* 
 * File: ggz_stats.c
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 9/4/01
 * Desc: GGZ game module stat functions
 * $Id: ggz_stats.c 4182 2002-05-07 16:51:35Z jdorje $
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

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "elo.h"
#include "ggzdmod.h"
#include "ggz_stats.h"

struct GGZStats {
	GGZdMod *ggz;
	int num_players;

	float *ratings;
	int *wins, *losses, *ties;

	int teams;
	int *team_list;

	double *winners;
};

#define lock_name "ggz-stats-lock"
#define stats_name "ggz-stats"


int read_stats(GGZStats * stats)
{
	int num = stats->num_players, player, handle;

	assert(num == ggzdmod_get_num_seats(stats->ggz));

	/* FIXME: use database */
	ggz_debug(DBG_GGZSTATS, "Reading stats data for %d players.", num);

	while (mkdir(lock_name, S_IRWXU) < 0) {
		ggz_error_sys("Couldn't access lock "
			      "while reading ggz_stats data.");
		sleep(1);
	}

	handle = ggz_conf_parse(stats_name, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if (handle < 0) {
		ggz_error_sys("Couldn't parse conf file "
			      "while reading ggz_stats data.");
		if (rmdir(lock_name) < 0)
			assert(0);
		return -1;
	}

	for (player = 0; player < num; player++) {
		GGZSeat seat = ggzdmod_get_seat(stats->ggz, player);
		char player_name[128];

		if (seat.type != GGZ_SEAT_PLAYER && seat.type != GGZ_SEAT_BOT)
			continue;

		if (seat.type == GGZ_SEAT_PLAYER) {
			int i;
			snprintf(player_name, sizeof(player_name), "%s",
				 seat.name);
			for (i = 0; i < strlen(player_name); i++)
				player_name[i] = tolower(player_name[i]);
		} else
			snprintf(player_name, sizeof(player_name),
				 "%s(AI)(%d)", seat.name, seat.num);

		stats->ratings[player] = ggz_conf_read_int(handle, "ratings", player_name, 150000);
		stats->ratings[player] /= 100;
		
		stats->wins[player] =
			ggz_conf_read_int(handle, "wins", player_name, 0);
		stats->losses[player] =
			ggz_conf_read_int(handle, "losses", player_name, 0);
		stats->ties[player] =
			ggz_conf_read_int(handle, "ties", player_name, 0);
	}

	ggz_conf_cleanup();	/* Hope nobody else is using this! */

	if (rmdir(lock_name) < 0) {
		ggz_error_sys("Couldn't remove lock "
			      "while reading ggz_stats data.");
		assert(0);
	}

	return 0;
}

int write_stats(GGZStats * stats)
{
	int num = stats->num_players, handle, player;

	assert(num == ggzdmod_get_num_seats(stats->ggz));

	/* FIXME: use database */
	ggz_debug(DBG_GGZSTATS, "Writing stats for %d players.", num);

	while (mkdir(lock_name, S_IRWXU) < 0) {
		ggz_error_sys("Couldn't access lock "
			      "while writing ggz_stats data.");
		sleep(1);
	}

	handle = ggz_conf_parse(stats_name, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if (handle < 0) {
		ggz_error_sys("Couldn't parse conf file "
			      "while writing ggz_stats data.");
		if (rmdir(lock_name) < 0)
			assert(0);
		return -1;
	}

	for (player = 0; player < num; player++) {
		GGZSeat seat = ggzdmod_get_seat(stats->ggz, player);
		char player_name[128];
		int rating = (float)(100.0 * stats->ratings[player] + 0.5);

		if (seat.type != GGZ_SEAT_PLAYER && seat.type != GGZ_SEAT_BOT)
			continue;

		if (seat.type == GGZ_SEAT_PLAYER) {
			int i;
			snprintf(player_name, sizeof(player_name), "%s",
				 seat.name);
			for (i = 0; i < strlen(player_name); i++)
				player_name[i] = tolower(player_name[i]);
		} else
			snprintf(player_name, sizeof(player_name),
				 "%s(AI)(%d)", seat.name, seat.num);

		ggz_conf_write_int(handle, "ratings", player_name, rating);

		ggz_conf_write_int(handle, "wins", player_name,
				   stats->wins[player]);
		ggz_conf_write_int(handle, "losses", player_name,
				   stats->losses[player]);
		ggz_conf_write_int(handle, "ties", player_name,
				   stats->ties[player]);
	}

	ggz_conf_commit(handle);
	ggz_conf_cleanup();	/* Hope nobody else is using this! */

	if (rmdir(lock_name) < 0) {
		ggz_error_sys("Couldn't remove lock "
			      "while writing ggz_stats data.");
		assert(0);
	}

	return 0;
}

GGZStats *ggzstats_new(GGZdMod * ggzdmod)
{
	GGZStats *stats;
	int num_players = ggzdmod_get_num_seats(ggzdmod);
	
	if (!ggzdmod) {
		ggz_error_msg("ggzstats_new: " "invalid param");
		return NULL;
	}
	
	ggz_debug(DBG_GGZSTATS, "Creating new stats object for %d players.", num_players);

	stats = ggz_malloc(sizeof(*stats));
	stats->ggz = ggzdmod;
	stats->num_players = num_players;
	stats->winners = ggz_malloc(num_players * sizeof(*stats->winners));

	stats->wins = ggz_malloc(num_players * sizeof(*stats->wins));
	stats->losses = ggz_malloc(num_players * sizeof(*stats->losses));
	stats->ties = ggz_malloc(num_players * sizeof(*stats->ties));
	stats->ratings = ggz_malloc(num_players * sizeof(*stats->ratings));

	if (read_stats(stats) < 0) {
		ggz_error_msg("ggzstats_new: " "couldn't read stats!!");
		ggzstats_free(stats);
		return NULL;
	}

	return stats;
}

void ggzstats_free(GGZStats * stats)
{
	if (!stats) {
		ggz_error_msg("ggzstats_free: " "invalid param");
		return;
	}
	
	ggz_debug(DBG_GGZSTATS, "Freeing ggzstats object.");

	ggz_free(stats->ratings);
	ggz_free(stats->wins);
	ggz_free(stats->losses);
	ggz_free(stats->ties);

	ggz_free(stats->winners);

	ggz_realloc(stats->team_list, 0);
	ggz_free(stats);
}

int ggzstats_reread(GGZStats * stats)
{
	if (!stats) {
		ggz_error_msg("ggzstats_reread: " "invalid param");
		return -1;
	}

	ggz_debug(DBG_GGZSTATS, "Re-reading stats.");

	if (read_stats(stats) < 0) {
		ggz_error_msg("ggzstats_reread: " "couldn't read stats!!");
		return -1;
	}
	return 0;
}

void ggzstats_set_num_teams(GGZStats * stats, int num_teams)
{
	int player;
	
	if (!stats || num_teams < 0 || num_teams >= stats->num_players) {
		ggz_debug(DBG_GGZSTATS,
			  "ggzstats_set_num_teams: " "invalid number %d.",
			  num_teams);
		return;
	}
	
	ggz_debug(DBG_GGZSTATS, "Setting number of teams to %d.", num_teams);

	stats->teams = num_teams;
	stats->team_list = ggz_realloc(stats->team_list, stats->num_players * sizeof(*stats->team_list));
	for (player = 0; player < stats->num_players; player++)
		stats->team_list[player] = -1;	/* no team */
}

void ggzstats_set_team(GGZStats * stats, int player, int team)
{
	if (!stats || player < 0 || player >= stats->num_players || team < 0 || team > stats->teams) {
		ggz_error_msg("ggzstats_set_team: " "invalid params %d/%d.", player, team);
		return;
	}
	
	ggz_debug(DBG_GGZSTATS, "Putting player %d on team %d.", player, team);

	stats->team_list[player] = team;
}

void ggzstats_new_game(GGZStats * stats)
{
	int i;
	
	if (!stats) {
		ggz_error_msg("ggzstats_new_game: " "invalid param");
		return;
	}
	
	ggz_debug(DBG_GGZSTATS, "New game.");

	for (i = 0; i < stats->num_players; i++)
		stats->winners[i] = 0.0;
}

int ggzstats_get_record(GGZStats * stats, int player,
			int *wins, int *losses, int *ties)
{
	if (!stats || player < 0 || player >= stats->num_players) {
		ggz_error_msg("ggzstats_get_record: " "invalid player %d.", player);
		return -1;
	}
	
	*wins = stats->wins[player];
	*losses = stats->losses[player];
	*ties = stats->ties[player];
	
	ggz_debug(DBG_GGZSTATS, "Retrieving record for player %d: %d-%d-%d.", player, *wins, *losses, *ties);
	
	return 0;
}

int ggzstats_get_rating(GGZStats * stats, int player, int *rating)
{
	if (!stats || player < 0 || player >= stats->num_players) {
		ggz_error_msg("ggzstats_get_rating: " "invalid player %d.", player);
		return -1;
	}

	*rating = (int) (stats->ratings[player] + 0.5);
	
	ggz_debug(DBG_GGZSTATS, "Retrieving rating for player %d: %d.", player, *rating);
	
	return 0;
}

void ggzstats_set_game_winner(GGZStats * stats, int player, double win_pct)
{
	if (!stats || player < 0 || player >= stats->num_players
	    || win_pct < 0.0 || win_pct > 1.0) {
		ggz_error_msg("ggzstats_set_game_winner: "
			      "invalid params %d/%f.", player, win_pct);
		return;
	}
	
	ggz_debug(DBG_GGZSTATS, "Setting player %d to win_pct %f.", player, win_pct);

	stats->winners[player] = win_pct;
}

/* NOTE Teams are handled like this: a team effectively has a rating equal to
   the average of all players on that team.  This is put into ELO to
   determine the chance of each team winning.  However, the rating change of
   each player is dependent on that players's rating (or variance), not the
   team's, according to the standard ELO (or Glicko) formula. */

/* This function is pretty damned ugly. */
int ggzstats_recalculate_ratings(GGZStats * stats)
{
	int i;
	int num_players = ggzdmod_get_num_seats(stats->ggz);
	int num_teams = stats->teams > 0 ? stats->teams : num_players;
	float sum = 0;

	float team_scores[num_teams];
	float team_ratings[num_teams];
	int team_sizes[num_teams];
	int player_teams[num_players];

	/* check to see if all data is initted */
	if (!stats) {
		ggz_error_msg("ggzstats_recalculate_ratings: "
			      "invalid rating data.");
		return -1;
	}
	
	ggz_debug(DBG_GGZSTATS, "Recalculating ratings.");

	/* first lets initialize the team arrays */
	for (i = 0; i < num_teams; i++) {
		team_sizes[i] = 0;
		team_scores[i] = 0.0;
		team_ratings[i] = 0.0;
	}
	for (i = 0; i < num_players; i++) {
		int team = stats->teams > 0 ? stats->team_list[i] : i;
		team_ratings[team] += stats->ratings[i];
		team_sizes[team]++;
		team_scores[team] += stats->winners[i];
		player_teams[i] = team;
	}
	for (i = 0; i < num_teams; i++) {
		/* the team rating is the average of the individual ratings. */
		team_ratings[i] /= (float) team_sizes[i];	/* FIXME */
		ggz_debug(DBG_GGZSTATS, "Team rating for team %d is %f.",
			  i, team_ratings[i]);
		sum += team_scores[i];
	}

	/* Verify there is exactly 1 winner. */
	if (sum != 1.0) {
		/* FIXME: rounding error might cause this */
		ggz_error_msg("ggzstats_recalculate_ratings: "
			      "winner sums add to %f.", sum);
		return -1;
	}

	elo_recalculate_ratings(num_players, stats->ratings, player_teams,
				num_teams, team_ratings, team_scores);

	for (i = 0; i < num_players; i++) {
		int team = player_teams[i];
		if (team_scores[team] >= 1.0)
			stats->wins[i]++;
		else if (team_scores[team] <= 0.0)
			stats->losses[i]++;
		else
			stats->ties[i]++;
	}

	if (write_stats(stats) < 0)
		ggz_error_msg("ggzstats_recalculate_ratings: "
			      "couldn't write stats!!");

	return 0;		/* success */
}
