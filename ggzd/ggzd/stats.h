/*
 * File: stats.h
 * Author: GGZ Dev Team
 * Project: GGZ Server
 * Date: 10/27/2002
 * Desc: Functions for calculating statistics
 * $Id: stats.h 10009 2008-05-26 22:37:19Z josef $
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

#ifndef _GGZ_STATS_H
#define _GGZ_STATS_H

#include "ggzdmod-ggz.h"

/* Perform a lookup on the stats.  stats->player and stats->game should
 * already have been set.  Returns GGZ_ERROR on fatal DB error. */
GGZReturn stats_lookup(ggzdbPlayerGameStats *stats);

void report_statistics(int room, int gametype,
		       const GGZdModGameReportData *report, const char *savegame);

void report_savegame(int gametype, const char *owner, const char *savegame);

typedef struct {
	char *name;
	int position;
	int score;
} GGZRanking;

/* Returns (allocated) list with GGZRanking entries */
GGZList *toprankings(int gametype);

/* Real-time server statistics */
void stats_rt_init(void);
void stats_rt_report(void);
void stats_rt_report_chat(int room);
void stats_rt_shutdown(void);

#endif /* _GGZ_STATS_H */
