/*
 * GGZ Rankings Calculator
 * Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
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
 */

/* Include files */
#include <libpq-fe.h>
#include <stdio.h>
#include <getopt.h>

/* Calculation model per game */
static const char *rankingmodels[14][2] =
{
	{"TicTacToe", "wins/losses"},
	{"TEG", "highscore"},
	{"Reversi", "wins/losses"},
	{"Dots", "wins/losses"},
	{"Chess", "rating"},
	{"GGZCards-FortyTwo", "rating"},
	{"GGZCards-Hearts", "rating"},
	{"GGZCards-Spades", "rating"},
	{"", ""},
	{"", ""},
	{"", ""},
	{"", ""},
	{"", ""},
	{NULL, NULL}
};

/* Global variables */
static PGconn *conn = NULL;

/* Find calculation model per game */
static const char *rankings_mode(const char *game)
{
	int i;
	for(i = 0; rankingmodels[i][0]; i++)
		if(!strcmp(rankingmodels[i][0], game))
			return rankingmodels[i][1];
	return "";
}

/* Database initialization */
static void rankings_connect(const char *host, const char *dbname, const char *user, const char *password)
{
	char conninfo[128];

	snprintf(conninfo, sizeof(conninfo),
		"host=%s dbname=%s user=%s password=%s", host, dbname, user, password);
	conn = PQconnectdb(conninfo);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		conn = NULL;
		return;
	}
}

/* Calculate rankings per game and write them back */
static void rankings_calculate(const char *game, const char *mode)
{
	PGresult *res;
	char query[256];
	int i;

	if((!strcmp(mode, "highscore")) || (!strcmp(mode, "rating")))
	{
		snprintf(query, sizeof(query), "SELECT id FROM stats WHERE game = '%s' ORDER BY %s DESC",
			game, mode);
	}
	else if(!strcmp(mode, "wins/losses"))
	{
		snprintf(query, sizeof(query),
			"SELECT id FROM stats WHERE game = '%s' GROUP BY id ORDER BY SUM(wins - losses) DESC",
			game);
	}
	else return;

	res = PQexec(conn, query);
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		printf("  - %i players\n", PQntuples(res));
		for(i = 0; i < PQntuples(res); i++)
		{
			snprintf(query, sizeof(query), "UPDATE stats SET ranking = %i WHERE id = %s",
				i + 1, PQgetvalue(res, i, 0));
			PQexec(conn, query);
		}
	}
}

/* Iterate over all games for further calculation */
static void rankings_recalculate_all(void)
{
	PGresult *res;
	int i;
	char *game;
	const char *mode;

	res = PQexec(conn, "SELECT DISTINCT game FROM stats");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		for(i = 0; i < PQntuples(res); i++)
		{
			game = PQgetvalue(res, i, 0);
			mode = rankings_mode(game);
			printf("* %s (%s)\n", game, mode);
			rankings_calculate(game, mode);
		}
	}                                                                         
}

/* Program startup, options processing and calculation invokation */
int main(int argc, char *argv[])
{
	int c, optindex;
	int help;
	char *host, *name, *user, *password;
	struct option options[] =
	{
		{"host", 1, 0, 0},
		{"user", 1, 0, 0},
		{"name", 1, 0, 0},
		{"password", 1, 0, 0},
		{"help", 0, 0, 0},
		{0, 0, 0, 0}
	};

	help = 0;
	host = "localhost";
	name = NULL;
	user = NULL;
	password = NULL;

	while(1)
	{
		c = getopt_long(argc, argv, "h:n:u:p:?", options, &optindex);
		if(c == -1) break;

		switch(c)
		{
			case 'h':
				host = optarg;
				break;
			case 'n':
				name = optarg;
				break;
			case 'u':
				user = optarg;
				break;
			case 'p':
				password = optarg;
				break;
			case '?':
				help = 1;
				break;
			default:
				help = 1;
		}
	}

	if(help)
	{
		printf("GGZ Rankings Calculator\n");
		printf("Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>\n");
		printf("\n");
		printf("Options:\n");
		printf("[-h | --host <host>         ] Database host\n");
		printf("[-n | --name <name>         ] Database name\n");
		printf("[-u | --user <user>         ] Database username\n");
		printf("[-p | --password <password> ] Database password\n");
		printf("[-? | --help                ] This help screen\n");
		printf("\n");
		return 0;
	}

	rankings_connect(host, name, user, password);
	if(conn) rankings_recalculate_all();
	else
	{
		fprintf(stderr, "Error: Connection to database failed.\n");
		return -1;
	}

	return 0;
}

