/*
 * File: stats.h
 * Author: GGZ Dev Team
 * Project: GGZ Server
 * Date: 10/27/2002
 * Desc: Functions for calculating statistics
 * $Id: stats.c 5072 2002-10-27 23:16:47Z jdorje $
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

void report_statistics(int room, int gametype,
		       GGZdModGameReportData *report)
{
	int i;
	char game_name[MAX_GAME_NAME_LEN + 1];
	unsigned char records;
	ggzdbPlayerGameStats stats[report->num_players];

	pthread_rwlock_rdlock(&game_types[gametype].lock);
	strcpy(game_name, game_types[gametype].name);
	records = game_types[gametype].stats_records;
	pthread_rwlock_unlock(&game_types[gametype].lock);

	/* First, check if we use *any* stats. */
	if (!records)
		return;

	/* Retrieve the old stats. */
	for (i = 0; i < report->num_players; i++) {
		GGZDBResult status;

		strcpy(stats[i].player, report->names[i]);
		strcpy(stats[i].game, game_name);

		status = ggzdb_stats_lookup(&stats[i]);

		/* NOT_FOUND case is handled in lookup */
		if (status != GGZDB_NO_ERROR) {
			err_msg("DB error %d in table_game_report", status);
			return;
		}
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
		pthread_rwlock_unlock(&player->stats_lock);
		pthread_rwlock_unlock(&player->lock);

		room_update_event(report->names[i],
				  GGZ_PLAYER_UPDATE_STATS,
				  room);
	}
}
