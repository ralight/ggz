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

#include <err_func.h>
#include <ggzdb.h>
#include <hash.h>
#include <login.h>
#include <motd.h>
#include <net.h>
#include <players.h>
#include <protocols.h>

static void login_generate_password(char *);
static int login_check_password(char *name, char *password);
static int login_add_user(char *name, char *password);
static int  validate_username(char *);


/*
 * login_player() implements the following exchange:
 * 
 * REQ_LOGIN
 *  str: login name
 *  str: password
 * RSP_NEW_LOGIN
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  int: game type checksum (if success)
 *  chr: reservation flag (if success)
 */
int login_player(GGZLoginType type, GGZPlayer* player, char *name, char *password)
{
	char *ip_addr, *src, *dest;
	int name_ok;
	char lc_name[MAX_USER_NAME_LEN + 1], new_pw[17];
	ggzdbPlayerEntry db_pe;

	dbg_msg(GGZ_DBG_CONNECTION, "Player %p attempting login", player);

	/* Can't login twice */
	if (player->uid != GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CONNECTION, "%s attempted to log in again", 
			player->name);
		if (net_send_login(player->net, type, E_ALREADY_LOGGED_IN, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Add '(G)' to guest logins */
	/* FIXME: remove this as soon as we implement XML protocol */
	if (type == GGZ_LOGIN_GUEST) {
		if(strlen(name) <= MAX_USER_NAME_LEN - 4)
			strcat(name, "(G)");
		else {
			name[MAX_USER_NAME_LEN-3] = '(';
			name[MAX_USER_NAME_LEN-2] = 'G';
			name[MAX_USER_NAME_LEN-1] = ')';
			name[MAX_USER_NAME_LEN] = '\0';
		}
	}


	/* Convert name to lowercase for comparisons */
	for(src=name, dest=lc_name; *src!='\0'; src++, dest++)
		*dest = tolower(*src);
	*dest = '\0';
	
	/* Validate the username */
	if(!validate_username(lc_name)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s",
			name);
		/* FIXME: We should have a specific error code for this */
		if (net_send_login(player->net, type, E_ALREADY_LOGGED_IN, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}


	/* Start off assuming name is good */
	name_ok = 1;
	
	/* Check guest names vs. the database */
	if (type == GGZ_LOGIN_GUEST) {
		strcpy(db_pe.handle, lc_name);
		if (ggzdb_player_get(&db_pe) != GGZDB_ERR_NOTFOUND)
			name_ok = 0;
	}
	
	/* Add the player name to the hash table */
	if (name_ok)
		name_ok = hash_player_add(lc_name, player);
	if (!name_ok) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful login of %s", name);
		if (net_send_login(player->net, type, E_USR_LOOKUP, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Verify password for normal users */
	if (type == GGZ_LOGIN) {

		/* Check password */
		if (login_check_password(lc_name, password) < 0) {
			hash_player_delete(lc_name);
			if (net_send_login(player->net, type, E_USR_LOOKUP, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}

		/* Password is verified, update their last login */
		strcpy(db_pe.handle, lc_name);
		db_pe.last_login = time(NULL);
		if (ggzdb_player_update(&db_pe) != 0)
			err_msg("Player database update failed (%s)", name);

	}
	else if (type == GGZ_LOGIN_NEW) {
		/* At this point, we know the name is not currently in
                   use, so try adding it to the database*/
		if (login_add_user(lc_name, new_pw) < 0) {
			hash_player_delete(lc_name);
			if (net_send_login(player->net, type, E_USR_LOOKUP, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
	}
	
	/* Setup the player's information */
	pthread_rwlock_wrlock(&player->lock);
	if (type == GGZ_LOGIN_GUEST) 
		player->uid = GGZ_UID_ANON;
	else
		player->uid = 0;
	strncpy(player->name, name, MAX_USER_NAME_LEN + 1);
	ip_addr = player->addr;
	pthread_rwlock_unlock(&player->lock);
	
	/* Notify user of success and give them their password (if new) */
	if (net_send_login(player->net, type, 0, new_pw) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_is_defined() && net_send_motd(player->net) < 0)
		return GGZ_REQ_DISCONNECT;

	dbg_msg(GGZ_DBG_CONNECTION, "Successful login of %s", name);

	/* Log the connection */
	log_msg(GGZ_LOG_CONNECTION_INFO, "Player %s logged in from %s", name,
		ip_addr);
	
	
	return GGZ_REQ_OK;
}



/*
 * logout_player implements the following exchange:
 * 
 * REQ_LOGOUT
 * RSP_LOGOUT
 *  chr: success flag (0 for success, -1 for error )
 */
int logout_player(GGZPlayer* player)
{
	dbg_msg(GGZ_DBG_CONNECTION, "Handling logout for %s", player->name);

	/* FIXME: Saving of stats and other things */
	if (net_send_logout(player->net, 0) < 0)
		return GGZ_REQ_DISCONNECT;

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


static int login_check_password(char *name, char *password)
{
	ggzdbPlayerEntry db_entry;

	/* Lookup the player in the database so we can verify the password */
	strcpy(db_entry.handle, name);

	/* If they aren't found, return an error */
	if (ggzdb_player_get(&db_entry) == GGZDB_ERR_NOTFOUND) {
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful login of %s - no account", name);
		return -1;
	}

	/* They were found, so verify the password */
	if(strcmp(db_entry.password, password)) {
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful login of %s - bad password", name);
		return -1;
	}

	return 0;
}


static int login_add_user(char *name, char *password)
{
	ggzdbPlayerEntry db_entry;
	
	/*  Initialize player entry */
	login_generate_password(password);
	strcpy(db_entry.password, password);
	strcpy(db_entry.handle, name);
	strcpy(db_entry.name, "N/A");
	strcpy(db_entry.email, "N/A");
	db_entry.last_login = time(NULL);
	
	/* If we tried to overwrite a value, then we know it existed */
	if (ggzdb_player_add(&db_entry) == GGZDB_ERR_DUPKEY) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		return -1;
	}
	
	return 0;
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

