/*
 * GGZ Rankings Calculator
 * Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef RANKINGS_H
#define RANKINGS_H

#include "config.h"

/* Global types */
struct GGZRankings
{
	void *conn;
	char ***rankingmodels;
};
typedef struct GGZRankings GGZRankings;

/* Entry point */
GGZRankings *rankings_init(void);

/* Load the list of ranking models */
int rankings_loadmodels(GGZRankings *rankings, const char *path);

/* Database initialization */
void rankings_setconnection(GGZRankings *rankings, void *connection);

/* Calculate rankings per game and write them back */
void rankings_recalculate_game(GGZRankings *rankings, const char *game);

/* Iterate over all games for further calculation */
void rankings_recalculate_all(GGZRankings *rankings, const char *types);

/* Exit point */
void rankings_destroy(GGZRankings *rankings);

/* Helper function */
const char *rankings_model(GGZRankings *rankings, const char *game);

#endif
