/*
 * File: login.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 6/22/00
 * Desc: Functions for handling player logins
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

#include <config.h>

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <easysock.h>
#include <err_func.h>
#include <ggzdb.h>
#include <hash.h>
#include <login.h>
#include <motd.h>
#include <players.h>
#include <protocols.h>

static int  login_player_anon(GGZPlayer* player, int fd);
static int  login_player_new(GGZPlayer* player, int fd);
static int  login_player_normal(GGZPlayer* player, int fd);
static int  logout_player(GGZPlayer* player, int fd);
static void login_generate_password(char *);
static int read_name(int sock, char name[MAX_USER_NAME_LEN + 1]);
static int  validate_username(char *);

/* Handle opcodes from player_handle() */
int login_handle_request(const unsigned int request, GGZPlayer* player, 
			 const int p_fd)
{
	int status = GGZ_REQ_OK;

	switch (request) {
	case REQ_LOGIN_NEW:
		status = login_player_new(player, p_fd);
		break;

	case REQ_LOGIN:
		status = login_player_normal(player, p_fd);
		break;

	case REQ_LOGIN_ANON:
		status = login_player_anon(player, p_fd);
		break;

	case REQ_LOGOUT:
		logout_player(player, p_fd);
		status = GGZ_REQ_DISCONNECT;
		break;
		
	}
	return status;
}


/*
 * login_player_normal implements the following exchange:
 * 
 * REQ_LOGIN
 *  str: login name
 *  str: password
 * RSP_NEW_LOGIN
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  int: game type checksum (if success)
 *  chr: reservation flag (if success)
 */
static int login_player_normal(GGZPlayer* player, int fd)
{
	char name[MAX_USER_NAME_LEN + 1];
	char password[17];
	ggzdbPlayerEntry db_pe;
	char *ip_addr;
	int name_ok, rc;
	char lc_name[MAX_USER_NAME_LEN + 1];
	char *src, *dest;

	dbg_msg(GGZ_DBG_CONNECTION, "Player %p attempting login", player);

	if (read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;
	if(es_read_string(fd, password, 17) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Can't login twice */
	if (player->uid != GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CONNECTION, "%s attempted to log in again", 
			player->name);
		if (es_write_int(fd, RSP_LOGIN) < 0
		    || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Convert name to lowercase for comparisons */
	for(src=name,dest=lc_name; *src!='\0'; src++,dest++)
		*dest = tolower(*src);
	*dest = '\0';

	/* Validate the username */
	if(!validate_username(lc_name)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s",
			name);
		/* FIXME: We should have a specific error code for this */
		if(es_write_int(fd, RSP_LOGIN) < 0
		   || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Add the player name to the hash table */
	name_ok = hash_player_add(lc_name, player);
	if (!name_ok) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		if (es_write_int(fd, RSP_LOGIN) < 0
		    || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Lookup the player in the database so we can verify the password */
	strcpy(db_pe.handle, lc_name);
	rc = ggzdb_player_get(&db_pe);

	/* If they aren't found, return an error */
	if(rc == GGZDB_ERR_NOTFOUND) {
		hash_player_delete(lc_name);
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful login of %s - no account", name);
		if (es_write_int(fd, RSP_LOGIN) < 0
		    || es_write_char(fd, E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* They were found, so verify the password */
	if(strcmp(db_pe.password, password)) {
		hash_player_delete(lc_name);
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful login of %s - bad password", name);
		if (es_write_int(fd, RSP_LOGIN) < 0
		    || es_write_char(fd, E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Password is verified, update their last login */
	db_pe.last_login = time(NULL);
	rc = ggzdb_player_update(&db_pe);
	if(rc != 0)
		err_msg("Player database update failed (%s)", name);

	/* Setup the player's information */
	pthread_rwlock_wrlock(&player->lock);
	player->uid = 0;
	strncpy(player->name, name, MAX_USER_NAME_LEN + 1);
	ip_addr = player->addr;
	pthread_rwlock_unlock(&player->lock);

	/* Notify user of success */
	if (es_write_int(fd, RSP_LOGIN) < 0 
	    || es_write_char(fd, 0) < 0 
	    || es_write_int(fd, 265) < 0
	    || es_write_char(fd, 0) < 0) {
		return GGZ_REQ_DISCONNECT;
	}

	/* Send off the Message Of The Day */
	if (motd_info.use_motd
	    && (es_write_int(fd, MSG_MOTD) < 0
		|| motd_send_motd(fd) < 0)) {
		return GGZ_REQ_DISCONNECT;
	}

	dbg_msg(GGZ_DBG_CONNECTION, "Successful login of %s", name);

	/* Log the connection */
	log_msg(GGZ_LOG_CONNECTION_INFO,
		"Player %s logged in from %s", name, ip_addr);

	return GGZ_REQ_OK;
}


/*
 * login_player_new implements the following exchange:
 * 
 * REQ_NEW_LOGIN
 *  str: login name
 * RSP_NEW_LOGIN
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  str: initial password (if success)
 *  int: game type checksum (if success)
 */
static int login_player_new(GGZPlayer* player, int fd)
{
	char name[MAX_USER_NAME_LEN + 1];
	ggzdbPlayerEntry db_pe;
	char *ip_addr;
	int name_ok, rc;
	char lc_name[MAX_USER_NAME_LEN + 1];
	char *src, *dest;

	dbg_msg(GGZ_DBG_CONNECTION, "Creating new login for player %p", 
		player);

	if (read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Can't login twice */
	if (player->uid != GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CONNECTION, "%s attempted to log in again", 
			player->name);
		if (es_write_int(fd, RSP_LOGIN_NEW) < 0
		    || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Convert name to lowercase for comparisons */
	for(src=name,dest=lc_name; *src!='\0'; src++,dest++)
		*dest = tolower(*src);
	*dest = '\0';

	/* Validate the username */
	if(!validate_username(lc_name)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s",
			name);
		/* FIXME: We should have a specific error code for this */
		if(es_write_int(fd, RSP_LOGIN) < 0
		   || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Add the player name to the hash table */
	name_ok = hash_player_add(lc_name, player);
	if (!name_ok) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		if (es_write_int(fd, RSP_LOGIN_NEW) < 0
		    || es_write_char(fd, E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* At this point, we know the name is not currently in use, so */
	/* try adding it to the database - first generate a password */
	login_generate_password(db_pe.password);
	strcpy(db_pe.handle, lc_name);
	strcpy(db_pe.name, "N/A");
	strcpy(db_pe.email, "N/A");
	db_pe.last_login = time(NULL);
	rc = ggzdb_player_add(&db_pe);

	/* If we tried to overwrite a value, then we know it existed */
	if(rc == GGZDB_ERR_DUPKEY) {
		hash_player_delete(lc_name);
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		if (es_write_int(fd, RSP_LOGIN_NEW) < 0
		    || es_write_char(fd, E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Setup the player's information */
	pthread_rwlock_wrlock(&player->lock);
	player->uid = 0;
	strncpy(player->name, name, MAX_USER_NAME_LEN + 1);
	ip_addr = player->addr;
	pthread_rwlock_unlock(&player->lock);

	/* Notify user of success and give them their password */
	if (es_write_int(fd, RSP_LOGIN_NEW) < 0 
	    || es_write_char(fd, 0) < 0 
	    || es_write_string(fd, db_pe.password) < 0
	    || es_write_int(fd, 265) < 0) {
		return GGZ_REQ_DISCONNECT;
	}

	/* Send off the Message Of The Day */
	if (motd_info.use_motd
	    && (es_write_int(fd, MSG_MOTD) < 0
		|| motd_send_motd(fd) < 0)) {
		return GGZ_REQ_DISCONNECT;
	}

	dbg_msg(GGZ_DBG_CONNECTION, "Successful new login of %s", name);

	/* Log the connection */
	log_msg(GGZ_LOG_CONNECTION_INFO,
		"New player %s logged in from %s", name, ip_addr);
	
	return GGZ_REQ_OK;
}


static char *pw_words[] = { "apple", "horse", "turtle", "orange", "tree",
			    "carrot", "dingo", "gnu", "bunny", "wombat" };

/* This generates a password for the user */
static void login_generate_password(char *pw)
{
	int word, d1, d2;

	word = random() % 10;
	d1 = random() % 10;
	d2 = random() % 10;
	snprintf(pw, 17, "%s%d%d", pw_words[word], d1, d2);
}


/*
 * login_player_anon implements the following exchange:
 * 
 * REQ_LOGIN_ANON
 *  str: login name
 * RSP_LOGIN_ANON
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  int: game type checksum (if success)
 */
static int login_player_anon(GGZPlayer* player, int fd)
{
	char name[MAX_USER_NAME_LEN + 1];
	char *ip_addr;
	int name_ok;
	ggzdbPlayerEntry db_pe;
	int rc;
	char lc_name[MAX_USER_NAME_LEN + 1];
	char *src, *dest;

	/* Read this first to get it out of the socket */
	if (read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Can't login twice */
	if (player->uid != GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CONNECTION, "%s attempted to log in again", 
			player->name);
		if (es_write_int(fd, RSP_LOGIN_ANON) < 0
		    || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	dbg_msg(GGZ_DBG_CONNECTION, "Creating guest login for player %p", 
		player);

	if(strlen(name) <= MAX_USER_NAME_LEN - 4)
		strcat(name, "(G)");
	else {
		name[MAX_USER_NAME_LEN-3] = '(';
		name[MAX_USER_NAME_LEN-2] = 'G';
		name[MAX_USER_NAME_LEN-1] = ')';
		name[MAX_USER_NAME_LEN] = '\0';
	}

	/* Convert name to lowercase for comparisons */
	for(src=name,dest=lc_name; *src!='\0'; src++,dest++)
		*dest = tolower(*src);
	*dest = '\0';

	/* Validate the username */
	if(!validate_username(lc_name)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s",
			name);
		/* FIXME: We should have a specific error code for this */
		if(es_write_int(fd, RSP_LOGIN) < 0
		   || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Check the name vs. the database */
	name_ok = 1;
	strcpy(db_pe.handle, lc_name);
	rc = ggzdb_player_get(&db_pe);
	if(rc != GGZDB_ERR_NOTFOUND)
		name_ok = 0;

	/* Add the player name to the hash table */
	if(name_ok)
		name_ok = hash_player_add(lc_name, player);

	if (!name_ok) {
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful anonymous login of %s", name);
		if (es_write_int(fd, RSP_LOGIN_ANON) < 0
		    || es_write_char(fd, E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	pthread_rwlock_wrlock(&player->lock);
	player->uid = GGZ_UID_ANON;
	strncpy(player->name, name, MAX_USER_NAME_LEN + 1);
	ip_addr = player->addr;
	pthread_rwlock_unlock(&player->lock);

	if (es_write_int(fd, RSP_LOGIN_ANON) < 0
	    || es_write_char(fd, 0) < 0
	    || es_write_int(fd, 265) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_info.use_motd
	    && (es_write_int(fd, MSG_MOTD) < 0
		|| motd_send_motd(fd) < 0)) {
		return GGZ_REQ_DISCONNECT;
	}
	
	dbg_msg(GGZ_DBG_CONNECTION, "Successful anonymous login of %s", name);

	log_msg(GGZ_LOG_CONNECTION_INFO,
		"Anonymous player %s logged in from %s", name, ip_addr);

	return 0;
}


/*
 * logout_player implements the following exchange:
 * 
 * REQ_LOGOUT
 * RSP_LOGOUT
 *  chr: success flag (0 for success, -1 for error )
 */
static int logout_player(GGZPlayer* player, int fd)
{
	dbg_msg(GGZ_DBG_CONNECTION, "Handling logout for %s", player->name);
	
	/* FIXME: Saving of stats and other things */
	if (es_write_int(fd, RSP_LOGOUT) < 0 
	    || es_write_char(fd, 0) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return GGZ_REQ_OK;
}


static int read_name(int sock, char name[MAX_USER_NAME_LEN + 1])
{
	char *tmp;

	if (es_read_string_alloc(sock, &tmp) < 0)
		return -1;

	strncpy(name, tmp, MAX_USER_NAME_LEN);
	free(tmp);

	/* Make sure names are null-terminated */
	name[MAX_USER_NAME_LEN] = '\0';
	
	return 0;    ;
}


/* This routine validates the username request */
static int validate_username(char *name)
{
	char *p;

	/* Nothing less than a space and no extended ASCII */
	/* & - can mess with M$ Windows labels, etc */
	/* % - can screw up log and debug's printf()s */
	/* \ - can screw up log and debug's printf()s */
	for(p=name; *p!='\0'; p++)
		if(*p < 33 || *p == '%' || *p == '&' || *p == '\\' || *p > 126)
			return 0;

	return 1;
}
