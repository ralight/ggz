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

/* Header file */
#include "rankings.h"

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include <mysql/mysql.h>

/* Calculate rankings per game and write them back */
static void rankings_calculate(GGZRankings *rankings, const char *game, const char *mode, const char *types)
{
	MYSQL_RES *res, *res2;
	MYSQL_ROW row;
	int rc;
	char query[256];
	int i, rank;
	char *handle;
	int considered;

	if(types)
	{
		snprintf(query, sizeof(query), "UPDATE stats SET ranking = 0 WHERE game = '%s'",
			game);
		res = NULL;
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

	rc = mysql_query((MYSQL*)rankings->conn, query);
	if(!rc)
	{
		res = mysql_store_result((MYSQL*)rankings->conn);
		printf("  - %i players\n", (int)mysql_num_rows(res));
		rank = 1;
		for(i = 0; i < mysql_num_rows(res); i++)
		{
			row = mysql_fetch_row(res);
			handle = row[1];
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
						rc = mysql_query((MYSQL*)rankings->conn, query);
						if(!rc)
						{
							res2 = mysql_store_result((MYSQL*)rankings->conn);
							if(strstr(types, "registered"))
								if(mysql_num_rows(res2) == 1) considered = 1;
							if(strstr(types, "guest"))
								if(mysql_num_rows(res2) == 0) considered = 1;
							mysql_free_result(res2);
						}

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
				i + 1, row[0]);
			mysql_query((MYSQL*)rankings->conn, query);

			rank++;
		}
		if(types)
			printf("  - considered %i players\n", rank - 1);
		mysql_free_result(res);
	}
	else printf("* Error: Could not read statistics entries for game %s.\n", game);
}

/* Recalculation for one game only */
void rankings_recalculate_game(GGZRankings *rankings, const char *game)
{
	const char *mode;

	mysql_query((MYSQL*)rankings->conn, "BEGIN TRANSACTION");

	mode = rankings_model(rankings, game);
	printf("* %s (%s)\n", game, mode);
	if(!strcmp(mode, ""))
		printf("* Error: game %s doesn't support statistics.\n", game);
	else
		rankings_calculate(rankings, game, mode, NULL);

	mysql_query((MYSQL*)rankings->conn, "COMMIT");
}

/* Iterate over all games for further calculation */
void rankings_recalculate_all(GGZRankings *rankings, const char *types)
{
	/* FIXME: Total recalculation via 'calcrankings' 
	   tool not supported for MySQL yet. */
}
