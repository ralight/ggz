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
 *
 * Needs a local ggzd installation.
 * Otherwise (undefined WITH_GGZ), ranking models might not be accurate.
 */

/* Features */
#define WITH_GGZ 1

/* Include files */
#include <libpq-fe.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#ifdef WITH_GGZ
#include <ggz.h>
#include <sys/types.h>
#include <dirent.h>
#endif

/* Calculation model per game */
static const char *rankingmodels_copy[14][2] =
{
	{"TicTacToe", "wins/losses"},
	{"TEG", "highscore"},
	{"Reversi", "wins/losses"},
	{"Dots", "wins/losses"},
	{"Chess", "rating"},
	{"GGZCards-FortyTwo", "rating"},
	{"GGZCards-Hearts", "rating"},
	{"GGZCards-Spades", "rating"},
	{"ccheckers", "wins/losses"},
	{"", ""},
	{"", ""},
	{"", ""},
	{"", ""},
	{NULL, NULL}
};

/* Global variables */
static PGconn *conn = NULL;
static char ***rankingmodels = NULL;

/* Load the list of ranking models */
static int rankings_loadmodels(const char *path)
{
#ifdef WITH_GGZ
	DIR *d;
	struct dirent *e;
	int handle;
	int records, ratings, highscores;
	char *name;
	int count;
	char filename[1024];
	char *model;

	count = 0;
	if(!path) return 0;
	d = opendir(path);
	if(!d) return 0;
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

		rankingmodels = (char***)ggz_realloc(rankingmodels, (count + 2) * sizeof(char**));
		rankingmodels[count] = (char**)ggz_malloc(2 * sizeof(char*));
		rankingmodels[count][0] = ggz_strdup(name);
		rankingmodels[count][1] = ggz_strdup(model);
		rankingmodels[count + 1] = NULL;

		ggz_free(name);
		ggz_conf_close(handle);

		count++;
	}
	closedir(d);

	return count;
#else
	return 0;
#endif
}

/* Find calculation model per game */
static const char *rankings_model(const char *game)
{
	int i;

	if(!rankingmodels)
	{
		for(i = 0; rankingmodels_copy[i][0]; i++)
		{
			rankingmodels = (char***)realloc(rankingmodels, (i + 2) * sizeof(char**));
			rankingmodels[i] = (char**)malloc(2 * sizeof(char*));
			rankingmodels[i][0] = strdup(rankingmodels_copy[i][0]);
			rankingmodels[i][1] = strdup(rankingmodels_copy[i][1]);
			rankingmodels[i + 1] = NULL;
		}
	}

	for(i = 0; rankingmodels[i]; i++)
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
static void rankings_calculate(const char *game, const char *mode, const char *types)
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
		res = PQexec(conn, query);
	}

	if((!strcmp(mode, "highscore")) || (!strcmp(mode, "rating")))
	{
		snprintf(query, sizeof(query), "SELECT id, handle FROM stats WHERE game = '%s' ORDER BY %s DESC",
			game, mode);
	}
	else if(!strcmp(mode, "wins/losses"))
	{
		snprintf(query, sizeof(query),
			"SELECT id, handle FROM stats WHERE game = '%s' GROUP BY id, handle ORDER BY SUM(wins - losses) DESC",
			game);
	}
	else return;

	res = PQexec(conn, query);
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
						res2 = PQexec(conn, query);
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
			PQexec(conn, query);

			rank++;
		}
		if(types)
			printf("  - considered %i players\n", rank - 1);
	}
	else printf("* Error: Could not read statistics entries for game %s.\n", game);
	PQclear(res);
}

/* Iterate over all games for further calculation */
static void rankings_recalculate_all(const char *types)
{
	PGresult *res;
	int i;
	char *game;
	const char *mode;
	int newentries, allentries;

	res = PQexec(conn, "SELECT COUNT(id) FROM stats WHERE ranking = 0");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
		newentries = atoi(PQgetvalue(res, 0, 0));
	else
		newentries = -1;
	PQclear(res);

	res = PQexec(conn, "SELECT COUNT(id) FROM stats");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
		allentries = atoi(PQgetvalue(res, 0, 0));
	else
		allentries = -1;
	PQclear(res);

	if(newentries == 0)
	{
		printf("== No new players found for calculation.\n");
		printf("== Proceeding to recalculate existing players.\n");
		/*exit(0);*/
	}
	else printf("== Calculate %i players, including %i new players.\n", allentries, newentries);
	printf("== Calculation types: %s.\n", types);

	res = PQexec(conn, "SELECT DISTINCT game FROM stats");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		for(i = 0; i < PQntuples(res); i++)
		{
			game = PQgetvalue(res, i, 0);
			mode = rankings_model(game);
			printf("* %s (%s)\n", game, mode);
			if(!mode)
				printf("* Error: game %s doesn't support statistics.\n", game);
			else
				rankings_calculate(game, mode, types);
		}
	}
	PQclear(res);

	printf("== All rankings calculated.\n");
}

/* Program startup, options processing and calculation invokation */
int main(int argc, char *argv[])
{
	int c, optindex;
	int help;
	char *host, *name, *user, *password, *dscpath, *types;
	int ret;
	struct option options[] =
	{
		{"host", 1, 0, 'h'},
		{"user", 1, 0, 'u'},
		{"name", 1, 0, 'n'},
		{"password", 1, 0, 'p'},
		{"dscpath", 1, 0, 'd'},
		{"types", 1, 0, 't'},
		{"help", 0, 0, '?'},
		{0, 0, 0, 0}
	};

	help = 0;
	host = "localhost";
	name = NULL;
	user = NULL;
	password = NULL;
	dscpath = NULL;
	types = NULL;

	while(1)
	{
		c = getopt_long(argc, argv, "h:n:u:p:d:t:?", options, &optindex);
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
			case 'd':
				dscpath = optarg;
				break;
			case 't':
				types = optarg;
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
#ifdef WITH_GGZ
		printf("[-d | --dscpath <path>      ] Game server description directory\n");
#endif
		printf("[-t | --types <type,type,..>] Consider only selected player types (guest, bot, registered)\n");
		printf("[-? | --help                ] This help screen\n");
		printf("\n");
		return 0;
	}

	ret = rankings_loadmodels(dscpath);
	if(ret) printf("== All %i game descriptions loaded.\n", ret);
	else printf("== Error: game descriptions could not be loaded, using internal copy.\n");

	rankings_connect(host, name, user, password);
	if(conn) rankings_recalculate_all(types);
	else
	{
		fprintf(stderr, "Error: Connection to database failed.\n");
		return -1;
	}

	return 0;
}

