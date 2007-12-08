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

#ifdef PGSQL_IN_PGSQLDIR
# include <pgsql/libpq-fe.h>
#else
# include <postgresql/libpq-fe.h>
#endif

/* Calculate rankings per game and write them back */
static void rankings_calculate(GGZRankings *rankings, const char *game, const char *mode, const char *types)
{
	PGresult *res, *res2;
	char query[256];
	int i, rank;
	char *handle;
	int considered;

	if(types)
	{
		snprintf(query, sizeof(query), "UPDATE stats SET ranking = 0 WHERE game = '%s'",
			game);
		res = PQexec((PGconn*)rankings->conn, query);
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

	res = PQexec((PGconn*)rankings->conn, query);
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		printf("  - %i players\n", PQntuples(res));
		rank = 1;
		for(i = 0; i < PQntuples(res); i++)
		{
			handle = PQgetvalue(res, i, 1);

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
						res2 = PQexec((PGconn*)rankings->conn, query);
						if(PQresultStatus(res2) == PGRES_TUPLES_OK)
						{
							if(strstr(types, "registered"))
								if(PQntuples(res2) == 1) considered = 1;
							if(strstr(types, "guest"))
								if(PQntuples(res2) == 0) considered = 1;
						}
						PQclear(res2);
					}
				}
				if(strstr(types, "bot"))
				{
					if(strstr(handle, "|AI")) considered = 1;
				}
			}
			else considered = 1;

			if(!considered) continue;

			snprintf(query, sizeof(query), "UPDATE stats SET ranking = %i WHERE id = %s",
				i + 1, PQgetvalue(res, i, 0));
			res2 = PQexec((PGconn*)rankings->conn, query);
			PQclear(res2);

			rank++;
		}
		if(types)
			printf("  - considered %i players\n", rank - 1);
	}
	else printf("* Error: Could not read statistics entries for game %s.\n", game);

	PQclear(res);
}

/* Recalculation for one game only */
void rankings_recalculate_game(GGZRankings *rankings, const char *game)
{
	PGresult *res;
	const char *mode;

	res = PQexec((PGconn*)rankings->conn, "BEGIN TRANSACTION");
	PQclear(res);

	mode = rankings_model(rankings, game);
	printf("* %s (%s)\n", game, mode);
	if(!strcmp(mode, ""))
		printf("* Error: game %s doesn't support statistics.\n", game);
	else
		rankings_calculate(rankings, game, mode, NULL);

	res = PQexec((PGconn*)rankings->conn, "COMMIT");
	PQclear(res);
}

/* Iterate over all games for further calculation */
void rankings_recalculate_all(GGZRankings *rankings, const char *types)
{
	PGresult *res;
	int i;
	char *game;
	const char *mode;
	int newentries, allentries;

	res = PQexec((PGconn*)rankings->conn, "SELECT COUNT(id) FROM stats WHERE ranking = 0");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
		newentries = atoi(PQgetvalue(res, 0, 0));
	else
		newentries = -1;
	PQclear(res);

	res = PQexec((PGconn*)rankings->conn, "SELECT COUNT(id) FROM stats");
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

	res = PQexec((PGconn*)rankings->conn, "BEGIN TRANSACTION");
	PQclear(res);

	res = PQexec((PGconn*)rankings->conn, "SELECT DISTINCT game FROM stats");
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

	res = PQexec((PGconn*)rankings->conn, "COMMIT");
	PQclear(res);

	printf("== All rankings calculated.\n");
}

