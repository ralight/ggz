/*
 * File: stats.h
 * Author: GGZ Dev Team
 * Project: GGZ Server
 * Date: 10/27/2002
 * Desc: Functions for calculating statistics
 * $Id: stats.c 5073 2002-10-28 00:09:53Z jdorje $
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
#include <string.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "hash.h"
#include "players.h"
#include "protocols.h"
#include "room.h"
#include "stats.h"


/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];


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
		}
	}
}

static void calculate_ratings(ggzdbPlayerGameStats *stats,
			      GGZdModGameReportData *report)
{
	err_msg("Ratings not implemented!");
}

void report_statistics(int room, int gametype,
		       GGZdModGameReportData *report)
{
	int i;
	char game_name[MAX_GAME_NAME_LEN + 1];
	unsigned char records, ratings;
	ggzdbPlayerGameStats stats[report->num_players];

	pthread_rwlock_rdlock(&game_types[gametype].lock);
	strcpy(game_name, game_types[gametype].name);
	records = game_types[gametype].stats_records;
	ratings = game_types[gametype].stats_ratings;
	pthread_rwlock_unlock(&game_types[gametype].lock);

	/* First, check if we use *any* stats. */
	if (!records && !ratings)
		return;

	/* Retrieve the old stats. */
	for (i = 0; i < report->num_players; i++) {
		strcpy(stats[i].player, report->names[i]);
		strcpy(stats[i].game, game_name);

		if (stats_lookup(&stats[i]) != GGZ_OK)
			return;
	}

	/* There's a potential threading problem here, but in
	   practice it shouldn't hurt.  Since we look up the
	   stats, then change them, then write them back, if
	   someone else tries to change them during this time
	   someone's data will be lost.  But as long as players
	   stats only change when *they* play, this will only
	   be a problem for bots - an acceptable tradeoff. */

	/* Calculate stats */
	if (records)
		calculate_records(stats, report);
	if (ratings)
		calculate_ratings(stats, report);

	/* Rewrite the stats to the database. */
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
			player->rating = stats[i].rating;
		}
		pthread_rwlock_unlock(&player->stats_lock);
		pthread_rwlock_unlock(&player->lock);

		room_update_event(report->names[i],
				  GGZ_PLAYER_UPDATE_STATS,
				  room);
	}
}
