/*
 * File: login.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 6/22/00
 * Desc: Functions for handling player logins
 * $Id: login.c 5923 2004-02-14 21:12:29Z jdorje $
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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err_func.h"
#include "ggzdb.h"
#include "hash.h"
#include "login.h"
#include "motd.h"
#include "net.h"
#include "players.h"
#include "protocols.h"
#include "perms.h"
#include "client.h"


static void login_generate_password(char *pw);
static GGZReturn login_add_user(ggzdbPlayerEntry *entry,
				char *name, char *password);
static bool is_valid_username(char *);


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
GGZPlayerHandlerStatus login_player(GGZLoginType type, GGZPlayer* player,
                                    char *name, char *password)
{
	char *ip_addr;
	bool name_ok;
	char new_pw[17];
	ggzdbPlayerEntry db_pe;
	char *login_type=NULL;
	GGZDBResult db_status;

	new_pw[0] = '\0';

	dbg_msg(GGZ_DBG_CONNECTION, "Player %p attempting login as %d",
	        player, type);
	
	/* Sanity safety feature: don't send on any names longer than the
	   code expects.  The code *should* be able to deal with it if
	   this does happen, but currently it *can't*.  --JDS */
	if (strlen(name) > MAX_USER_NAME_LEN)
		name[MAX_USER_NAME_LEN] = 0;

	/* Validate the username */
	if(!is_valid_username(name)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s",
			name);
		/* FIXME: We should have a specific error code for this */
		if (net_send_login(player->client->net, type, E_ALREADY_LOGGED_IN, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}


	/* Start off assuming name is good */
	name_ok = true;
	
	/* Check guest names vs. the database */
	snprintf(db_pe.handle, sizeof(db_pe.handle), "%s", name);
	db_status = ggzdb_player_get(&db_pe);
	if(type == GGZ_LOGIN_GUEST && db_status != GGZDB_ERR_NOTFOUND) {
		dbg_msg(GGZ_DBG_CONNECTION,
		        "Guest player trying to use actual login name %s.",
		        name);
		name_ok = false;
	}
	
	/* Add the player name to the hash table */
	if (name_ok && !hash_player_add(name, player)) {
		dbg_msg(GGZ_DBG_CONNECTION, "Could not add player %s to hash.",
		        name);
		name_ok = false;
	}


	/* Error if the name is already in the hash table or guest
	   name in the DB */
	if (!name_ok) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful login of %s", name);
		if (net_send_login(player->client->net, type, E_USR_LOOKUP, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Verify password for normal users */
	if (type == GGZ_LOGIN) {
		/* Check password */
		if(db_status == GGZDB_ERR_NOTFOUND) {
			dbg_msg(GGZ_DBG_CONNECTION,
				"Unsuccessful login of %s - no account", name);
			name_ok = false;
		} else if(ggzdb_compare_password(password, db_pe.password) != 1) {
			dbg_msg(GGZ_DBG_CONNECTION,
				"Unsuccessful login of %s - bad password",name);
			log_msg(GGZ_LOG_SECURITY, "BADPWD from %s for %s",
				player->client->addr, name);
			name_ok = false;
		}
		if(!name_ok) {
			hash_player_delete(name);
			if (net_send_login(player->client->net, type, E_USR_LOOKUP, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}

		/* Password is verified, update their last login */
		snprintf(db_pe.handle, sizeof(db_pe.handle), "%s", name);
		db_pe.last_login = time(NULL);
		if (ggzdb_player_update(&db_pe) != 0)
			err_msg("Player database update failed (%s)", name);

		login_type = " registered player";
	}
	else if (type == GGZ_LOGIN_NEW) {
		/* At this point, we know the name is not currently in
                   use, so try adding it to the database*/
		db_pe.user_id = ggzdb_player_next_uid();
		if(db_status != GGZDB_ERR_NOTFOUND
		   || login_add_user(&db_pe, name, new_pw) < 0) {
			hash_player_delete(name);
			if (net_send_login(player->client->net, type, E_USR_LOOKUP, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
		log_msg(GGZ_LOG_SECURITY, "NEWACCT (%u) from %s for %s",
			db_pe.user_id, player->client->addr, name);
		login_type = " newly registered player";
	} else
		login_type = "n anonymous player";
	
	/* Setup the player's information */
	pthread_rwlock_wrlock(&player->lock);
	if (type == GGZ_LOGIN_GUEST) {
		player->login_status = GGZ_LOGIN_ANON;
		log_login_anon();
	} else {
		/* Setup initial registered player info */
		player->login_status = GGZ_LOGIN_REGISTERED;
		player->uid = db_pe.user_id;
		perms_init(player, &db_pe);
		log_login_regd();
	}
	snprintf(player->name, sizeof(player->name), "%s", name);
	ip_addr = player->client->addr;
	player->login_time = (long) time(NULL);
	player->next_ping = time(NULL) + 5;
	/* Don't send any room updates until the player gets a room list. */
	pthread_rwlock_unlock(&player->lock);
	
	/* Notify user of success and give them their password (if new) */
	if (net_send_login(player->client->net, type, 0, new_pw) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_is_defined() && net_send_motd(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	dbg_msg(GGZ_DBG_CONNECTION, "Successful login of %s", name);

	/* Log the connection */
	log_msg(GGZ_LOG_CONNECTION_INFO, "LOGIN %s from %s as a%s", name,
		ip_addr, login_type);
	
	
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


static GGZReturn login_add_user(ggzdbPlayerEntry *db_entry,
				char *name, char *password)
{
	/*  Initialize player entry */
	login_generate_password(password);
	snprintf(db_entry->handle, sizeof(db_entry->handle), "%s", name);
	snprintf(db_entry->password, sizeof(db_entry->password), "%s", password);
	snprintf(db_entry->name, sizeof(db_entry->name), "N/A");
	snprintf(db_entry->email, sizeof(db_entry->email), "N/A");
	db_entry->perms = PERMS_DEFAULT_SETTINGS;
	db_entry->last_login = time(NULL);
	
	/* If we tried to overwrite a value, then we know it existed */
	if (ggzdb_player_add(db_entry) == GGZDB_ERR_DUPKEY) {
		dbg_msg(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		return GGZ_ERROR;
	}

	return GGZ_OK;
}


/* This routine validates the username request */
static bool is_valid_username(char *name)
{
	char *p;

	/* "<none>" is invalid */
	if(!strcmp(name, "<none>"))
		return false;

	/* Nothing less than a space and no extended ASCII */
	/* & - can mess with M$ Windows labels, etc */
	/* % - can screw up log and debug's printf()s */
	/* \ - can screw up log and debug's printf()s */
	for(p=name; *p!='\0'; p++)
		if(*p < 33 || *p == '%' || *p == '&' || *p == '\\' || *p > 126)
			return false;

	return true;
}
