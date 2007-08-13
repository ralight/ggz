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
#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

static GGZRankings *rankings = NULL;

/* Database initialization */
static void rankings_connect(const char *host, const char *dbname, const char *user, const char *password)
{
	char conninfo[128];
	PGconn *conn;

	snprintf(conninfo, sizeof(conninfo),
		"host=%s dbname=%s user=%s password=%s", host, dbname, user, password);
	conn = PQconnectdb(conninfo);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Error: Connection to database failed.\n");
		exit(-1);
	}

	rankings_setconnection(rankings, conn);
}

/* Program startup, options processing and calculation invokation */
int main(int argc, char *argv[])
{
	int c, optindex;
	int help;
	char *host, *name, *user, *password, *dscpath, *types;
	char *configpath;
	int ret;
	int rc;
	char configbuffer[1024];
	struct option options[] =
	{
		{"host", 1, 0, 'H'},
		{"user", 1, 0, 'u'},
		{"name", 1, 0, 'n'},
		{"password", 1, 0, 'p'},
		{"dscpath", 1, 0, 'd'},
		{"types", 1, 0, 't'},
		{"help", 0, 0, 'h'},
		{"config", 1, 0, 'c'},
		{0, 0, 0, 0}
	};

	help = 0;
	host = NULL;
	name = NULL;
	user = NULL;
	password = NULL;
	dscpath = NULL;
	types = NULL;
	configpath = NULL;

	while(1)
	{
		c = getopt_long(argc, argv, "H:n:u:p:d:t:hc:", options, &optindex);
		if(c == -1) break;

		switch(c)
		{
			case 'H':
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
			case 'h':
				help = 1;
				break;
			case 'c':
				configpath = optarg;
				break;
			default:
				help = 1;
		}
	}

	if(help)
	{
		printf("GGZ Rankings Calculator\n");
		printf("Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>\n");
		printf("\n");
		printf("Options:\n");
		printf("[-H | --host <host>         ] Database host\n");
		printf("[-n | --name <name>         ] Database name\n");
		printf("[-u | --user <user>         ] Database username\n");
		printf("[-p | --password <password> ] Database password\n");
		printf("[-c | --config <path>       ] Configuration path of ggzd\n");
		printf("[-d | --dscpath <path>      ] Game server description directory\n");
		printf("[-t | --types <type,type,..>] Consider only selected player types (guest, bot, registered)\n");
		printf("[-h | --help                ] This help screen\n");
		printf("\n");
		return 0;
	}

	if(!configpath)
	{
		configpath = GGZDCONFDIR;
		printf("== Setting configuration directory to '%s'.\n", configpath);
	}

	if(configpath)
	{
		snprintf(configbuffer, sizeof(configbuffer), "%s/ggzd.conf", configpath);
		rc = ggz_conf_parse(configbuffer, GGZ_CONF_RDONLY);
		if(!host)
			host = ggz_conf_read_string(rc, "General", "DatabaseHost", NULL);
		if(!name)
			name = ggz_conf_read_string(rc, "General", "DatabaseName", NULL);
		if(!user)
			user = ggz_conf_read_string(rc, "General", "DatabaseUsername", NULL);
		if(!password)
			password = ggz_conf_read_string(rc, "General", "DatabasePassword", NULL);
		ggz_conf_close(rc);

		if(!dscpath)
		{
			snprintf(configbuffer, sizeof(configbuffer), "%s/games", configpath);
			dscpath = strdup(configbuffer);
		}
	}

	if(!dscpath)
	{
		printf("== Error: no path to game description files given.\n");
		return -1;
	}

	rankings = rankings_init();

	ret = rankings_loadmodels(rankings, dscpath);
	if(ret) printf("== All %i game descriptions loaded.\n", ret);
	else
	{
		printf("== Error: game descriptions could not be loaded.\n");
		return -1;
	}

	rankings_connect(host, name, user, password);
	rankings_recalculate_all(rankings, types);

	rankings_destroy(rankings);

	return 0;
}

