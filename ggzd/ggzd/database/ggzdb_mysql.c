/*
 * File: ggzdb_mysql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 03.05.2002
 * Desc: Back-end functions for handling the postgresql style database
 * $Id: ggzdb_mysql.c 9555 2008-01-19 08:18:21Z josef $
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <mysql/mysql.h>

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"
#include "ggzdb_util.h"
#include "rankings.h"


/* Internal variables */
static MYSQL *conn = NULL;
static MYSQL_RES *iterres = NULL;
static int itercount;
static pthread_mutex_t mutex;

/* Internal functions */


/* Function to initialize the mysql database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	char query[4096];
	my_bool reconnect = true;

	if(conn) return GGZ_OK;

	pthread_mutex_init(&mutex, NULL);

	mysql_library_init(0, NULL, NULL);
	conn = mysql_init(conn);
	/*
	 * MYSQL_OPT_RECONNECT set to true here enables automatic reconnections to the
	 * server if the connection is lost. This has been disabled by default since 
	 * mysql 5.0.3. This option requires 5.0.13.
	 */
	mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect);
	conn = mysql_real_connect(conn, connection.host, connection.username,
		connection.password, connection.database, 0, NULL, 0);

	if(!conn)
	{
		err_msg("Couldn't initialize database.");
		return GGZ_ERROR;
	}

	snprintf(query, sizeof(query), "CREATE TABLE `users` "
		"(`id` int4 AUTO_INCREMENT PRIMARY KEY, `handle` varchar(255), `password` varchar(255), "
		"`name` varchar(255), `email` varchar(255), `lastlogin` int8, `perms` int8, `firstlogin` int8)");

	rc = mysql_query(conn, query);

	/* Hack. */
	return GGZ_OK;
}


/* Function to deinitialize the mysql database system */
void _ggzdb_close(void)
{
	/*
	 * Since _ggzdb_close() is called by the main thread on exit
	 * it is not really necessary to get a lock here, but it does
	 * keep the locking consistent and so keeps valgrind/helgrind 
	 * happy.
	 */
	pthread_mutex_lock(&mutex);
	mysql_close(conn);

	mysql_library_end();
	conn = NULL;
	pthread_mutex_unlock(&mutex);

	pthread_mutex_destroy(&mutex);
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/*mysql_thread_init();*/
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/*mysql_thread_end();*/
}


/* Function to initialize the player table */
GGZDBResult _ggzdb_init_player(void)
{
	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];

	snprintf(query, sizeof(query), "INSERT INTO `users` "
		"(`handle`, `password`, `name`, `email`, `lastlogin`, `perms`, `firstlogin`) VALUES "
		"('%s', '%s', '%s', '%s', %li, %u, %li)",
		pe->handle, pe->password, pe->name, pe->email, pe->last_login, pe->perms, time(NULL));

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if (rc) {
		/* FIXME: is this correct?  If not, how do we detect a
		   duplicate entry - and notify the calling code?  --JDS */
		return GGZDB_ERR_DUPKEY;
	}

	return GGZDB_NO_ERROR;
}


/* Function to retrieve a player record */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[4096];

	snprintf(query, sizeof(query), "SELECT "
		"`password`,`name`,`email`,`lastlogin`,`perms` FROM `users` WHERE "
		"`handle` = LOWER('%s')",
		pe->handle);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	if (!rc) {
		res = mysql_store_result(conn);
		pthread_mutex_unlock(&mutex);

		if (res && (mysql_num_rows(res) == 1)) {
			row = mysql_fetch_row(res);
			strncpy(pe->password, row[0], sizeof(pe->password));
			strncpy(pe->name, row[1], sizeof(pe->name));
			strncpy(pe->email, row[2], sizeof(pe->email));
			pe->last_login = atol(row[3]);
			pe->perms = atol(row[4]);
			mysql_free_result(res);
			return GGZDB_NO_ERROR;
		} else {
			/* This is supposed to happen if we look up a
			   nonexistent player. */
			mysql_free_result(res);
			return GGZDB_ERR_NOTFOUND;
		}
	} else {
		pthread_mutex_unlock(&mutex);
		err_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];

	snprintf(query, sizeof(query), "UPDATE `users` SET "
		"`password`='%s',`name`='%s',`email`='%s',`lastlogin`=%li,`perms`=%u WHERE "
		"`handle`=LOWER('%s')",
		pe->password, pe->name, pe->email, pe->last_login, pe->perms, pe->handle);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if(rc) {
		err_msg("Couldn't update player.");
		return GGZDB_ERR_DB;
	}

	return GGZDB_NO_ERROR;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int result;
	MYSQL_ROW row;
	char query[4096];

	if (iterres) {
		mysql_free_result(iterres);
	}

	snprintf(query, sizeof(query), "SELECT "
		"`id`,`handle`,`password`,`name`,`email`,`lastlogin`,`perms` FROM `users`");
	result = mysql_query(conn, query);

	if (!result) {
		iterres = mysql_store_result(conn);
		if (iterres && mysql_num_rows(iterres)) {
			row = mysql_fetch_row(iterres);
			pe->user_id = atoi(row[0]);
			strncpy(pe->handle, row[1], sizeof(pe->handle));
			strncpy(pe->password, row[2], sizeof(pe->password));
			strncpy(pe->name, row[3], sizeof(pe->name));
			strncpy(pe->email, row[4], sizeof(pe->email));
			pe->last_login = atol(row[5]);
			pe->perms = atol(row[6]);
			itercount = 0;
			return GGZDB_NO_ERROR;
		} else {
			err_msg("No entries found.");
			mysql_free_result(iterres);
			iterres = NULL;
			return GGZDB_NO_ERROR;
		}
	} else {
		err_msg("Couldn't lookup player.");
		iterres = NULL;
		return GGZDB_ERR_DB;
	}
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	MYSQL_ROW row;

	if (!iterres) {
		err_msg_exit("get_next called before get_first, dummy");
	}

	if(iterres && (itercount < mysql_num_rows(iterres) - 1)) {
		itercount++;
		row = mysql_fetch_row(iterres);
		pe->user_id = atoi(row[0]);
		strncpy(pe->handle, row[1], sizeof(pe->handle));
		strncpy(pe->password, row[2], sizeof(pe->password));
		strncpy(pe->name, row[3], sizeof(pe->name));
		strncpy(pe->email, row[4], sizeof(pe->email));
		pe->last_login = atol(row[5]);
		pe->perms = atol(row[6]);
		return GGZDB_NO_ERROR;
	} else {
		mysql_free_result(iterres);
		iterres = NULL;
		return GGZDB_ERR_NOTFOUND;
	}
}


void _ggzdb_player_drop_cursor(void)
{
	if(!iterres) {
		/* This isn't an error; since we clear the cursor at the end
		   of _ggzdb_player_get_next we should expect to end up
		   here.  --JDS */
		/*err_msg_exit("drop_cursor called before get_first, dummy");*/
		return;
	}

	mysql_free_result(iterres);
	iterres = 0;
}

unsigned int _ggzdb_player_next_uid(void)
{
	/* SQL handles id's automatically */
	return 0;
}

GGZDBResult _ggzdb_init_stats(void)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	char query[4096];
	int rc;
	GGZDBResult ret = GGZDB_ERR_DB;
	char *player_quoted;

	player_quoted = _ggz_sql_escape(stats->player);

	snprintf(query, sizeof(query),
		"UPDATE `stats` "
		"SET `wins`=%i,`losses`=%i,`ties`=%i,`forfeits`=%i,"
		"`rating`=%f,`ranking`=%u,`highscore`=%li "
		"WHERE `handle` = LOWER('%s') AND `game`='%s'",
		stats->wins, stats->losses, stats->ties, stats->forfeits,
		stats->rating, stats->ranking, stats->highest_score,
		player_quoted, stats->game);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	if(!rc){
		if (!mysql_affected_rows(conn)) {
			snprintf(query, sizeof(query),
				"INSERT INTO `stats` "
				"(`handle`,`game`,`wins`,`losses`,`ties`,`forfeits`,`rating`,`ranking`,`highscore`) VALUES "
				"('%s', '%s', %i, %i, %i, %i, %f, %u, %li)",
				player_quoted, stats->game,
				stats->wins, stats->losses, stats->ties, stats->forfeits,
				stats->rating, stats->ranking, stats->highest_score);

			rc = mysql_query(conn, query);

			if(rc){
				err_msg("couldn't insert stats");
			}
			else ret = GGZDB_NO_ERROR;
		}
		else ret = GGZDB_NO_ERROR;

		pthread_mutex_unlock(&mutex);
	} else {
		pthread_mutex_unlock(&mutex);
		err_msg("couldn't update stats");
		ret = GGZDB_ERR_DB;
	}

	ggz_free(player_quoted);

	_ggzdb_stats_match(stats);

	return ret;
}

GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	char query[4096];
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *player_quoted;

	player_quoted = _ggz_sql_escape(stats->player);
	
	snprintf(query, sizeof(query),
			"SELECT "
			"`wins`,`losses`,`ties`,`forfeits`,`rating`,`ranking`,`highscore` "
			"FROM `stats` WHERE `handle`=LOWER('%s') AND `game`='%s'",
			player_quoted, stats->game);
	ggz_free(player_quoted);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	if(!rc){
		res = mysql_store_result(conn);
		pthread_mutex_unlock(&mutex);

		if (res && (mysql_num_rows(res) == 1)) {
			row = mysql_fetch_row(res);
			stats->wins = atoi(row[0]);
			stats->losses = atoi(row[1]);
			stats->ties = atoi(row[2]);
			stats->forfeits = atoi(row[3]);
			stats->rating = atof(row[4]);
			stats->ranking = atol(row[5]);
			stats->highest_score = atol(row[6]);
			mysql_free_result(res);
			return GGZDB_NO_ERROR;
		} else {
			/* This is supposed to happen if we look up a
			   nonexistent stat. */
			mysql_free_result(res);
			return GGZDB_ERR_NOTFOUND;
		}
	} else {
		pthread_mutex_unlock(&mutex);
		err_msg("Couldn't lookup stats.");
		return GGZDB_ERR_DB;
	}
}

GGZDBResult _ggzdb_stats_match(ggzdbPlayerGameStats *stats)
{
	char query[4096];
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *number, *playertype;
	char *player_quoted;

	snprintf(query, sizeof(query),
		"SELECT MAX(`id`) FROM `matches`");

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	if(rc) {
		err_msg("couldn't read match");
		number = NULL;

		pthread_mutex_unlock(&mutex);
		return GGZDB_ERR_DB;
	}
	else {
		res = mysql_store_result(conn);

		if (res && mysql_num_rows(res) == 1){
			row = mysql_fetch_row(res);
			number = row[0];
			/* mysql_free_result() occurs further down */
		} else {
			pthread_mutex_unlock(&mutex);
			if(res) {
				mysql_free_result(res);
			}
			return GGZDB_ERR_DB;
		}
	}

	playertype = "";
	if(stats->player_type == GGZ_PLAYER_GUEST) playertype = "guest";
	else if(stats->player_type == GGZ_PLAYER_NORMAL) playertype = "registered";
	else if(stats->player_type == GGZ_PLAYER_BOT) playertype = "bot";

	player_quoted = _ggz_sql_escape(stats->player);

	snprintf(query, sizeof(query),
		"INSERT INTO `matchplayers` "
		"(`match`,`handle`,`playertype`) VALUES "
		"(%s, '%s', '%s')",
		number, player_quoted, playertype);

	/*
	 * res must not be freed any earlier than this because 
	 * it will make "number" point to freed memory.
	 */
	mysql_free_result(res);
	ggz_free(player_quoted);

	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if(rc){
		err_msg("couldn't insert matchplayer");
		return GGZDB_ERR_DB;
	}else{
		return GGZDB_NO_ERROR;
	}
}

GGZDBResult _ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame)
{
	char query[4096];
	char *winner_quoted;
	int rc;

	snprintf(query, sizeof(query),
		"DELETE FROM `savegames` "
		"WHERE `game`='%s' AND `savegame`='%s'",
		game, savegame);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	winner_quoted = _ggz_sql_escape(winner);
	snprintf(query, sizeof(query),
		"INSERT INTO `matches` "
		"(`date`,`game`,`winner`,`savegame`) VALUES "
		"(%li,'%s','%s','%s')",
		time(NULL), game, winner_quoted, savegame);

	ggz_free(winner_quoted);

	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);
	if(!rc){
		return GGZDB_NO_ERROR;
	} else {
		err_msg("Couldn't insert match.");
		return GGZDB_ERR_DB;
	}
}

GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame)
{
	char query[4096];
	int rc;
	char *owner_quoted;

	owner_quoted = _ggz_sql_escape(owner);

	snprintf(query, sizeof(query),
		"INSERT INTO `savegames`"
		"(`date`,`game`,`owner`,`savegame`) VALUES "
		"(%li, '%s', '%s', '%s')",
		time(NULL), game, owner, savegame);

	ggz_free(owner_quoted);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if(rc){
		err_msg("couldn't insert savegame");
		return GGZDB_ERR_DB;
	} else {
		return GGZDB_NO_ERROR;
	}
}

GGZDBResult _ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[4096];
	char *handle_quoted;

	handle_quoted = _ggz_sql_escape(pe->handle);

	snprintf(query, sizeof(query),
		 "SELECT "
		 "`id`,`photo` "
		 "FROM `userinfo` WHERE `handle`=LOWER('%s')",
		 handle_quoted);

	ggz_free(handle_quoted);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	if(!rc){
		res = mysql_store_result(conn);
		pthread_mutex_unlock(&mutex);

		if(res && (mysql_num_rows(res) == 1)){
			row = mysql_fetch_row(res);

			pe->user_id = atol(row[0]);
			strncpy(pe->photo, row[1], sizeof(pe->photo));
			mysql_free_result(res);
			return GGZDB_NO_ERROR;
		} else	{
			mysql_free_result(res);
			return GGZDB_ERR_NOTFOUND;
		}
	} else {
		/* FIXME: shouldn't we unlock the mutex here? */
		err_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}
}

GGZDBResult _ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	ggzdbPlayerGameStats *stats;
	int i;

	snprintf(query, sizeof(query),
		"SELECT `wins`, `losses`, `ties`, `forfeits`, `rating`, `ranking`, `highscore`, `handle` FROM `stats` "
		"WHERE `game` = '%s' AND `ranking` <> 0 ORDER BY `ranking` ASC LIMIT %i",
		game, number);

	rc = mysql_query(conn, query);

	if (rc) {
		err_msg("couldn't read rankings");
	} else {
		rc = GGZDB_NO_ERROR;

		res = mysql_store_result(conn);
		for(i = 0; i < mysql_num_rows(res); i++) {
			row = mysql_fetch_row(res);

			stats = rankings[i];
			stats->wins = atoi(row[0]);
			stats->losses = atoi(row[1]);
			stats->ties = atoi(row[2]);
			stats->forfeits = atoi(row[3]);
			stats->rating = atof(row[4]);
			stats->ranking = atol(row[5]);
			stats->highest_score = atol(row[6]);
			snprintf(stats->player, MAX_USER_NAME_LEN, row[7]);
		}

		mysql_free_result(res);
	}

	return rc;
}

GGZDBResult _ggzdb_stats_calcrankings(const char *game)
{
	char dscpath[1024];

	snprintf(dscpath, sizeof(dscpath), "%s/games", GGZDCONFDIR);

	pthread_mutex_lock(&mutex);

	GGZRankings *rankings = rankings_init();
	rankings_loadmodels(rankings, dscpath);
	rankings_setconnection(rankings, conn);
	rankings_recalculate_game(rankings, game);
	rankings_destroy(rankings);

	pthread_mutex_unlock(&mutex);

	return GGZDB_NO_ERROR;
}

