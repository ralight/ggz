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

/*
 * Calculation models:
 * - wins/losses: all wins minus all losses, highest values
 * - ratings: precalculated rating column, highest values
 * - highscore: precalculated highscore column, highest values
 *
 * Needs a local ggzd installation.
 * Otherwise (undefined WITH_GGZ), ranking models might not be accurate.
 */

/* Header file */
#include "rankings.h"

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>
#include <sys/types.h>
#include <dirent.h>

/* Database initialization */
void rankings_setconnection(GGZRankings *rankings, void *connection)
{
	rankings->conn = connection;
}

/* Helper function: Find calculation model per game */
const char *rankings_model(GGZRankings *rankings, const char *game)
{
	int i;
	char ***rankingmodels = rankings->rankingmodels;

	if(!rankingmodels)
		return "";

	for(i = 0; rankingmodels[i]; i++)
		if(!strcmp(rankingmodels[i][0], game))
			return rankingmodels[i][1];
	return "";
}

/* Load the list of ranking models */
int rankings_loadmodels(GGZRankings *rankings, const char *path)
{
	DIR *d;
	struct dirent *e;
	int handle;
	int records, ratings, highscores;
	char *name;
	int count;
	char filename[1024];
	char *model;
	char ***rankingmodels = NULL;

	if(rankings->conn)
		return 0;

	count = 0;
	if(!path)
		return 0;

	d = opendir(path);
	if(!d)
		return 0;

	while((e = readdir(d)))
	{
		if((!strcmp(e->d_name, ".")) || (!strcmp(e->d_name, ".."))) continue;

		snprintf(filename, sizeof(filename), "%s/%s", path, e->d_name);
		handle = ggz_conf_parse(filename, GGZ_CONF_RDONLY);
		if(handle == -1) continue;
		name = ggz_conf_read_string(handle, "GameInfo", "Name", NULL);
		records = ggz_conf_read_int(handle, "Statistics", "Records", 0);
		ratings = ggz_conf_read_int(handle, "Statistics", "Ratings", 0);
		highscores = ggz_conf_read_int(handle, "Statistics", "Highscores", 0);

		model = NULL;
		if(highscores == 1) model = "highscore";
		else if(ratings == 1) model = "rating";
		else if(records == 1) model = "wins/losses";

		if((name) && (model))
		{
			rankingmodels = (char***)ggz_realloc(rankingmodels, (count + 2) * sizeof(char**));
			rankingmodels[count] = (char**)ggz_malloc(2 * sizeof(char*));
			rankingmodels[count][0] = ggz_strdup(name);
			rankingmodels[count][1] = ggz_strdup(model);
			rankingmodels[count + 1] = NULL;

			count++;
		}

		if(name)
			ggz_free(name);
		ggz_conf_close(handle);
	}
	closedir(d);

	rankings->rankingmodels = rankingmodels;

	return count;
}

/* Entry point */
GGZRankings *rankings_init(void)
{
	GGZRankings *rankings = ggz_malloc(sizeof(GGZRankings));
	rankings->conn = NULL;
	rankings->rankingmodels = NULL;
	return rankings;
}

/* Exit point */
void rankings_destroy(GGZRankings *rankings)
{
	ggz_free(rankings);
}

