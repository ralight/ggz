/*
 * File: stats.h
 * Author: GGZ Dev Team
 * Project: GGZ Server
 * Date: 10/27/2002
 * Desc: Functions for calculating statistics
 * $Id: stats.c 5996 2004-05-17 14:16:42Z josef $
 *
 * Copyright (C) 2002 GGZ Development Team.
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
# include <config.h>		/* Site specific config */
#endif

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"

#include "elo.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "hash.h"
#include "players.h"
#include "protocols.h"
#include "room.h"
#include "stats.h"


GGZReturn stats_lookup(ggzdbPlayerGameStats *stats)
{
	GGZDBResult status;

	status = ggzdb_stats_lookup(stats);

	if (status == GGZDB_ERR_NOTFOUND) {
		stats->wins = 0;
		stats->losses = 0;
		stats->ties = 0;
		stats->forfeits = 0;

		stats->rating = 1500.0;

		stats->ranking = 1; /* ? */
		stats->highest_score = 0; /* ? */

		return GGZ_OK;
	} else if (status != GGZDB_NO_ERROR) {
		err_msg("DB error %d in table_game_report", status);
		return GGZ_ERROR;
	}

	return GGZ_OK;
}

static void calculate_records(ggzdbPlayerGameStats *stats,
			      GGZdModGameReportData *report)
{
	int i;

	for (i = 0; i < report->num_players; i++) {
		switch (report->results[i]) {
		case GGZ_GAME_WIN:
			stats[i].wins++;
			break;
		case GGZ_GAME_LOSS:
			stats[i].losses++;
			break;
		case GGZ_GAME_TIE:
			stats[i].ties++;
			break;
		case GGZ_GAME_FORFEIT:
			stats[i].forfeits++;
			break;
		case GGZ_GAME_NONE:
			break;
		}
	}
}

static void calculate_ratings(ggzdbPlayerGameStats *stats,
			      GGZdModGameReportData *report,
			      const int num_teams)
{
	const int num_players = report->num_players;
	int i;
	int team_sizes[num_teams];
	float player_ratings[num_players];
	float team_scores[num_teams];
	float team_ratings[num_teams];
	float sum = 0.0;

	for (i = 0; i < num_players; i++) {
		player_ratings[i] = stats[i].rating;
	}

	/* First let's initialize thee team arrays */
	for (i = 0; i < num_teams; i++) {
		team_sizes[i] = 0;
		team_scores[i] = 0.0;
		team_ratings[i] = 0.0;
	}
	for (i = 0; i < num_players; i++) {
		const int team = report->teams[i];
		float winner_value = 0;
		if (team < 0) {
			err_msg("Invalid team %d.", team);
			return;
		}
		team_ratings[team] += player_ratings[i];
		team_sizes[team]++;

		switch (report->results[i]) {
		case GGZ_GAME_WIN:
			winner_value = 1.0;
			break;
		case GGZ_GAME_TIE:
		case GGZ_GAME_NONE: /* FIXME */
			winner_value = 0.5;
			break;
		case GGZ_GAME_LOSS:
		case GGZ_GAME_FORFEIT: /* FIXME */
			winner_value = 0;
			break;
		}
		if (team_sizes[team] > 1
		    && abs(winner_value - team_scores[team]) > 0.01)
			err_msg("Team given different winner values!");
		team_scores[team] = winner_value;
	}
	for (i = 0; i < num_teams; i++) {
		/* The team rating is the average of the individual ratings. */
		if (team_sizes == 0) {
			err_msg("Empty team %d!", i);
			return;
		}
		team_ratings[i] /= (float) team_sizes[i];
		sum += team_scores[i];
	}

	/* Make sure the winners sum to 1. */
	if (sum < 0.01) {
		/* Um, everybody lost.  Just make it all a tie... */
		sum = 0.0;
		for (i = 0; i < num_teams; i++) {
			team_scores[i] = 0.5;
			sum += 0.5;
		}
	}
	for (i = 0; i < num_teams; i++) {
		team_scores[i] /= sum;
	}
	sum = 1.0; /* sum /= sum; */

	/* TODO: team difficulty levels; for instance for 1-on-3 games.
	   This should probably just be handled as a straight rating
	   bonus for one team (i.e. "team 3's effective rating is +100") */

	elo_recalculate_ratings(num_players, player_ratings,
				report->teams, num_teams,
				team_ratings, team_scores);


	for (i = 0; i < num_players; i++) {
		stats[i].rating = player_ratings[i];
		dbg_msg(GGZ_DBG_STATS,
			"%s's rating becomes %f.\n", report->names[i],
		       stats[i].rating);
	}
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static int fill_in_teams(GGZdModGameReportData *report)
{
	int i;
	int max_team = -1;

	for (i = 0; i < report->num_players; i++) {
		/* ASSUMPTION: all team numbers are sequential. */
		max_team = MAX(max_team, report->teams[i]);
	}

	for (i = 0; i < report->num_players; i++) {
		if (report->teams[i] < 0) {
			max_team++;
			report->teams[i] = max_team;
		}
	}

	/* Return the number of teams */
	return max_team + 1;
}

void report_statistics(int room, int gametype,
		       GGZdModGameReportData *report)
{
	int i, num_teams;
	char game_name[MAX_GAME_NAME_LEN + 1];
	unsigned char records, ratings;
	ggzdbPlayerGameStats stats[report->num_players];
	char *winner;

	pthread_rwlock_rdlock(&game_types[gametype].lock);
	strcpy(game_name, game_types[gametype].name);
	records = game_types[gametype].stats_records;
	ratings = game_types[gametype].stats_ratings;
	pthread_rwlock_unlock(&game_types[gametype].lock);

	/* First, check if we use *any* stats. */
	if (!records && !ratings)
		return;

	/* Now see if this is a rated game. */
	for (i = 0; i < report->num_players; i++) {
		ggzdbPlayerEntry player;
		GGZDBResult status;

		if (report->types[i] == GGZ_SEAT_BOT)
			continue;
		else if (report->types[i] != GGZ_SEAT_PLAYER) {
			err_msg("Unknown player type %d in stats calc.",
				report->types[i]);
			return;
		}

		snprintf(player.handle, sizeof(player.handle),
			 report->names[i]);

		/* The player could have logged out - for instance if the
		   game server asks for a forfeit for a player who abandoned a
		   game.  So we need to look directly at the database... */
		status = ggzdb_player_get(&player);
		if (status == GGZDB_ERR_NOTFOUND) {
			/* Currently we don't track stats on games if there
			   are any guests at the table.  This should be
			   the default behavior.  But maybe some games will
			   want to allow it. */
			dbg_msg(GGZ_DBG_STATS,
				"Not tracking stats for guest player %s.",
				report->names[i]);
			return;
		} else if (status != GGZDB_NO_ERROR) {
			err_msg("Error %d accessing player %s for stats check",
				status, player.handle);
			return;
		}
		/* Looks like the player exists and is registered.  Great! */
	}

	/* Retrieve the old stats. */
	for (i = 0; i < report->num_players; i++) {
		strcpy(stats[i].game, game_name);

		if (report->types[i] == GGZ_SEAT_PLAYER) {
			snprintf(stats[i].player, sizeof(stats[i].player),
				 report->names[i]);
		} else if (report->types[i] == GGZ_SEAT_BOT) {
			snprintf(stats[i].player, sizeof(stats[i].player),
				 "%d|%s", i, report->names[i]);
		}

		if (stats_lookup(&stats[i]) != GGZ_OK)
			return;
	}

	/* Calculate the number of teams, and fill in empty teams. */
	num_teams = fill_in_teams(report);

	/* There's a potential threading problem here, but in
	   practice it shouldn't hurt.  Since we look up the
	   stats, then change them, then write them back, if
	   someone else tries to change them during this time
	   someone's data will be lost.  But as long as players
	   stats only change when *they* play, this will only
	   be a problem for bots - an acceptable tradeoff. */

	/* Check if there's a single winner */
	winner = NULL;
	for (i = 0; i < report->num_players; i++) {
		if(report->results[i] == GGZ_GAME_WIN) {
			if(winner) winner = "";
			else winner = stats[i].player;
		}
	}
	if(!winner) winner = "";

	/* Calculate stats */
	if (records)
		calculate_records(stats, report);
	if (ratings)
		calculate_ratings(stats, report, num_teams);

	/* Rewrite the stats to the database. */
	ggzdb_stats_newmatch(game_name, winner);
	for (i = 0; i < report->num_players; i++) {
		ggzdb_stats_update(&stats[i]);
	}

	/* Queue updates for people in the room. */
	for (i = 0; i < report->num_players; i++) {
		const char *name = report->names[i];
		GGZPlayer *player;

		/* Send an update to the room. */
		/* This isn't particularly elegant, but it should work at
		   skipping over AI players, players who have left the
		   room, and logged-out players. */
		player = hash_player_lookup(name);
		if (!player)
			continue;
		if (player->room != room) {
			pthread_rwlock_unlock(&player->lock);
			continue;
		}
		pthread_rwlock_wrlock(&player->stats_lock);
		if (records) {
			player->wins = stats[i].wins;
			player->losses = stats[i].losses;
			player->ties = stats[i].ties;
		}
		if (ratings) {
			player->rating = (int)(stats[i].rating + 0.5);
		}
		pthread_rwlock_unlock(&player->stats_lock);
		pthread_rwlock_unlock(&player->lock);

		room_update_event(report->names[i],
				  GGZ_PLAYER_UPDATE_STATS,
				  room, -1);
	}
}
