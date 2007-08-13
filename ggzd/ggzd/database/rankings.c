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

/* Global variables */
struct GGZRankings
{
#ifdef WITH_PGSQL
	PGconn *conn;
#elif WITH_MYSQL
	MYSQL *conn;
#endif
	char ***rankingmodels;
};


/* Helper function: Find calculation model per game */
static const char *rankings_model(GGZRankings *rankings, const char *game)
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

/* Database initialization */
#ifdef WITH_PGSQL
void rankings_setconnection(GGZRankings *rankings, PGconn *connection)
{
	rankings->conn = connection;
}
#elif WITH_MYSQL
void rankings_setconnection(GGZRankings *rankings, MYSQL *connection)
{
	rankings->conn = connection;
}
#endif

/* Calculate rankings per game and write them back */
static void rankings_calculate(GGZRankings *rankings, const char *game, const char *mode, const char *types)
{
#ifdef WITH_PGSQL
	PGresult *res, *res2;
#elif WITH_MYSQL
	MYSQL_RES *res, *res2;
	MYSQL_ROW row;
	int rc;
#endif
	char query[256];
	int i, rank;
	char *handle;
	int considered;

	if(types)
	{
		snprintf(query, sizeof(query), "UPDATE stats SET ranking = 0 WHERE game = '%s'",
			game);
#ifdef WITH_PGSQL
		res = PQexec(rankings->conn, query);
#elif WITH_MYSQL
		res = NULL;
#endif
	}

	if((!strcmp(mode, "highscore")) || (!strcmp(mode, "rating")))
	{
		snprintf(query, sizeof(query), "SELECT id, handle FROM stats "
			"WHERE game = '%s' ORDER BY %s DESC",
			game, mode);
	}
	else if(!strcmp(mode, "wins/losses"))
	{
		snprintf(query, sizeof(query),
			"SELECT id, handle FROM stats WHERE game = '%s' "
			"GROUP BY id, handle ORDER BY SUM(wins - losses) DESC",
			game);
	}
	else return;

	/* FIXME: This (ifdef'd control structures) is hyper-ugly but quite efficient... */
#ifdef WITH_PGSQL
	res = PQexec(rankings->conn, query);
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		printf("  - %i players\n", PQntuples(res));
		rank = 1;
		for(i = 0; i < PQntuples(res); i++)
		{
			handle = PQgetvalue(res, i, 1);

#elif WITH_MYSQL
	rc = mysql_query(rankings->conn, query);
	if(!rc)
	{
		res = mysql_store_result(rankings->conn);
		printf("  - %i players\n", (int)mysql_num_rows(res));
		rank = 1;
		for(i = 0; i < mysql_num_rows(res); i++)
		{
			row = mysql_fetch_row(res);
			handle = row[1];
#endif
			if(types)
			{
				considered = 0;

				if((strstr(types, "registered")) || (strstr(types, "guest")))
				{
					if(!strstr(handle, "|AI"))
					{
						snprintf(query, sizeof(query),
							"SELECT handle FROM users WHERE handle = '%s'",
							handle);
#ifdef WITH_PGSQL
						res2 = PQexec(rankings->conn, query);
						if(PQresultStatus(res2) == PGRES_TUPLES_OK)
						{
							if(strstr(types, "registered"))
								if(PQntuples(res2) == 1) considered = 1;
							if(strstr(types, "guest"))
								if(PQntuples(res2) == 0) considered = 1;
						}
						PQclear(res2);
#elif WITH_MYSQL
						rc = mysql_query(rankings->conn, query);
						if(!rc)
						{
							res2 = mysql_store_result(rankings->conn);
							if(strstr(types, "registered"))
								if(mysql_num_rows(res2) == 1) considered = 1;
							if(strstr(types, "guest"))
								if(mysql_num_rows(res2) == 0) considered = 1;
							mysql_free_result(res2);
						}

#endif
					}
				}
				if(strstr(types, "bot"))
				{
					if(strstr(handle, "|AI")) considered = 1;
				}
			}
			else considered = 1;

			if(!considered) continue;

#ifdef WITH_PGSQL
			snprintf(query, sizeof(query), "UPDATE stats SET ranking = %i WHERE id = %s",
				i + 1, PQgetvalue(res, i, 0));
			res2 = PQexec(rankings->conn, query);
			PQclear(res2);
#elif WITH_MYSQL
			snprintf(query, sizeof(query), "UPDATE stats SET ranking = %i WHERE id = %s",
				i + 1, row[0]);
			mysql_query(rankings->conn, query);
#endif

			rank++;
		}
		if(types)
			printf("  - considered %i players\n", rank - 1);
#ifdef WITH_MYSQL
		mysql_free_result(res);
#endif
	}
	else printf("* Error: Could not read statistics entries for game %s.\n", game);

#ifdef WITH_PGSQL
	PQclear(res);
#endif
}

/* Recalculation for one game only */
void rankings_recalculate_game(GGZRankings *rankings, const char *game)
{
#ifdef WITH_PGSQL
	PGresult *res;
#endif
	const char *mode;

#ifdef WITH_PGSQL
	res = PQexec(rankings->conn, "BEGIN TRANSACTION");
	PQclear(res);
#elif WITH_MYSQL
	mysql_query(rankings->conn, "BEGIN TRANSACTION");
#endif

	mode = rankings_model(rankings, game);
	printf("* %s (%s)\n", game, mode);
	if(!strcmp(mode, ""))
		printf("* Error: game %s doesn't support statistics.\n", game);
	else
		rankings_calculate(rankings, game, mode, NULL);

#ifdef WITH_PGSQL
	res = PQexec(rankings->conn, "COMMIT");
	PQclear(res);
#elif WITH_MYSQL
	mysql_query(rankings->conn, "COMMIT");
#endif
}

/* Iterate over all games for further calculation */
void rankings_recalculate_all(GGZRankings *rankings, const char *types)
{
#ifdef WITH_PGSQL
	PGresult *res;
	int i;
	char *game;
	const char *mode;
	int newentries, allentries;

	res = PQexec(rankings->conn, "SELECT COUNT(id) FROM stats WHERE ranking = 0");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
		newentries = atoi(PQgetvalue(res, 0, 0));
	else
		newentries = -1;
	PQclear(res);

	res = PQexec(rankings->conn, "SELECT COUNT(id) FROM stats");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
		allentries = atoi(PQgetvalue(res, 0, 0));
	else
		allentries = -1;
	PQclear(res);

	if(newentries == 0)
	{
		printf("== No new players found for calculation.\n");
		printf("== Proceeding to recalculate existing %i players.\n",
			allentries);
	}
	else
	{
		printf("== Calculate %i players, including %i new players.\n",
			allentries, newentries);
	}

	if(types)
		printf("== Calculation types filter: %s.\n", types);

	res = PQexec(rankings->conn, "BEGIN TRANSACTION");
	PQclear(res);

	res = PQexec(rankings->conn, "SELECT DISTINCT game FROM stats");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		for(i = 0; i < PQntuples(res); i++)
		{
			game = PQgetvalue(res, i, 0);
			mode = rankings_model(rankings, game);
			printf("* %s (%s)\n", game, mode);
			if(!strcmp(mode, ""))
				printf("* Error: game %s doesn't support statistics.\n", game);
			else
				rankings_calculate(rankings, game, mode, types);
		}
	}
	PQclear(res);

	res = PQexec(rankings->conn, "COMMIT");
	PQclear(res);

	printf("== All rankings calculated.\n");
#elif WITH_MYSQL
#warning "Total recalculation via 'calcrankings' tool not supported for MySQL yet."
#endif
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

