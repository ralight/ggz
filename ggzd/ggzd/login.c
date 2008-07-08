/*
 * File: login.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 6/22/00
 * Desc: Functions for handling player logins
 * $Id: login.c 10204 2008-07-08 06:39:36Z jdorje $
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

#ifdef WITH_CRACKLIB
# include <crack.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# ifndef CRACKLIB_DICTPATH
#  define CRACKLIB_DICTPATH "/var/cache/cracklib/cracklib_dict"
# endif
#endif
#ifdef WITH_OMNICRACKLIB
# include <omnicrack.h>
#endif

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
#include "unicode.h"
#include "stats.h"


#if 0
static void login_generate_password(char *pw);
#endif
static GGZReturn login_add_user(ggzdbPlayerEntry *entry,
				const char *name, char *password, const char *email);
static bool is_valid_username(const char *name);
static bool is_valid_password(const char *password);


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
GGZPlayerHandlerStatus login_player(GGZLoginType type, GGZPlayer *player,
                                    char *name, const char *password, const char *email)
{
	char *ip_addr;
	bool name_ok;
	char new_pw[17];
	ggzdbPlayerEntry db_pe;
	char *login_type = NULL;
	GGZDBResult db_status;
	GGZPlayerHandlerStatus reason;

	new_pw[0] = '\0';
	if(password)
		snprintf(new_pw, sizeof(new_pw), password);

	ggz_debug(GGZ_DBG_CONNECTION, "Player %p attempting login as %d",
		  (void *)player, type);

	/* If we disallow new registrations, we ignore logins straight away */
	if(type != GGZ_LOGIN && opt.registration_policy == GGZ_REGISTRATION_CLOSED) {
		ggz_debug(GGZ_DBG_CONNECTION,
		        "Registration/guest login attempt of name %s while it is prohibited.",
		        name);
		if (net_send_login(player->client->net, type,
				   E_USR_TYPE, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* A too-long username gives an error.  We used to just truncate it
	   but that would probably just confuse the user. */
	if (strlen(name) > MAX_USER_NAME_LEN) {
		ggz_debug(GGZ_DBG_CONNECTION, "Too-long username of %s", name);
		if (net_send_login(player->client->net, type,
				   E_TOO_LONG, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Validate the username */
	if(!is_valid_username(name)) {
		ggz_debug(GGZ_DBG_CONNECTION, "Invalid login name of %s",
			name);
		if (net_send_login(player->client->net, type,
				   E_BAD_USERNAME, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Validate the password */
	if (type == GGZ_LOGIN || type == GGZ_LOGIN_NEW) {
		if(!is_valid_password(new_pw)) {
			ggz_debug(GGZ_DBG_CONNECTION, "Insecure password from %s",
				name);
			if (net_send_login(player->client->net, type,
					   E_BAD_PASSWORD, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
	}

	/* Start off assuming name is good */
	name_ok = true;
	reason = E_NO_STATUS;

	/* Check guest names vs. the database */
	snprintf(db_pe.handle, sizeof(db_pe.handle), "%s", name);
	db_status = ggzdb_player_get(&db_pe);
	if(type == GGZ_LOGIN_GUEST && db_status != GGZDB_ERR_NOTFOUND) {
		ggz_debug(GGZ_DBG_CONNECTION,
		        "Guest player trying to use actual login name %s.",
		        name);
		name_ok = false;
		reason = E_USR_TYPE;
	}

	/* Add the player name to the hash table */
	if (name_ok && !hash_player_add(name, player)) {
		ggz_debug(GGZ_DBG_CONNECTION, "Could not add player %s to hash.",
		        name);
		name_ok = false;
		reason = E_ALREADY_LOGGED_IN;
	}

	/* Error if the name is already in the hash table or guest
	   name in the DB */
	if (!name_ok) {
		ggz_debug(GGZ_DBG_CONNECTION, "Unsuccessful login of %s", name);
		if (net_send_login(player->client->net, type, reason, NULL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Verify password for normal users */
	if (type == GGZ_LOGIN) {
		/* Check password */
		if(db_status == GGZDB_ERR_NOTFOUND) {
			ggz_debug(GGZ_DBG_CONNECTION,
				"Unsuccessful login of %s - no account", name);
			name_ok = false;
			reason = E_USR_FOUND;
		} else if(ggzdb_compare_password(password, db_pe.password) != 1) {
			ggz_debug(GGZ_DBG_CONNECTION,
				"Unsuccessful login of %s - bad password", name);
			log_msg(GGZ_LOG_SECURITY, "BADPWD from %s for %s",
				player->client->addr, name);
			name_ok = false;
			reason = E_USR_LOOKUP;
		} else if(!db_pe.confirmed) {
			if(opt.registration_policy == GGZ_REGISTRATION_CONFIRM) {
				ggz_debug(GGZ_DBG_CONNECTION,
					"Unsuccessful login of %s - account unconfirmed", name);
				name_ok = false;
				reason = E_USR_LOOKUP;
			}
		}
		if(!name_ok) {
			hash_player_delete(name);
			if (net_send_login(player->client->net, type,
					   reason, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}

		/* Password is verified, update their last login */
		snprintf(db_pe.handle, sizeof(db_pe.handle), "%s", name);
		db_pe.last_login = time(NULL);
		if (ggzdb_player_update(&db_pe) != 0)
			ggz_error_msg("Player database update failed (%s)", name);

		login_type = " registered player";
	}
	else if (type == GGZ_LOGIN_NEW) {
		/* At this point, we know the name is not currently in
                   use, so try adding it to the database*/

		if (new_pw[0] == '\0') {
			hash_player_delete(name);
			if (net_send_login(player->client->net, type,
					   E_BAD_PASSWORD, NULL) < 0)
			  return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}

		db_pe.user_id = ggzdb_player_next_uid();
		if(db_status != GGZDB_ERR_NOTFOUND
		   || login_add_user(&db_pe, name, new_pw, email) < 0) {
			hash_player_delete(name);
			if (net_send_login(player->client->net, type,
					   E_USR_TAKEN, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
		log_msg(GGZ_LOG_SECURITY, "NEWACCT (%u) from %s for %s",
			db_pe.user_id, player->client->addr, name);
		login_type = " newly registered player";

		if(opt.registration_policy == GGZ_REGISTRATION_CONFIRM) {
			if (net_send_login(player->client->net, type,
					   E_USR_LOOKUP, NULL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
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
		perms_init_from_db(&player->perms, &db_pe);
		log_login_regd();
	}
	snprintf(player->name, sizeof(player->name), "%s", name);
	ip_addr = player->client->addr;

	/* FIXME: not all of these values need to be set with
	   the lock on. */
	player->login_time = (long) time(NULL);
	player->next_ping_time = get_current_time() + 5.0;
	player->is_room_update_needed = true;
	player->next_room_update_time
	  = get_current_time() + opt.room_update_freq;

	/* Don't send any room updates until the player gets a room list. */
	pthread_rwlock_unlock(&player->lock);

	/* Notify user of success and give them their password (if new) */
	if (net_send_login(player->client->net, type, 0, new_pw) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_is_defined() && net_send_motd(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	ggz_debug(GGZ_DBG_CONNECTION, "Successful login of %s", name);

	/* Log the connection */
	log_msg(GGZ_LOG_CONNECTION_INFO, "LOGIN %s from %s as a%s", name,
		ip_addr, login_type);

	stats_rt_report();

	return GGZ_REQ_OK;
}


#if 0
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
#endif

static GGZReturn login_add_user(ggzdbPlayerEntry *db_entry,
				const char *name, char *password,
				const char *email)
{
	/*  Initialize player entry */
#if 0
	if (!password[0])
		login_generate_password(password);
#endif
	ggz_strncpy(db_entry->handle, name, sizeof(db_entry->handle));
	ggz_strncpy(db_entry->password, password, sizeof(db_entry->password));
	ggz_strncpy(db_entry->email, email, sizeof(db_entry->email));
	ggz_strncpy(db_entry->name, "", sizeof(db_entry->name));
	ggz_perms_init_from_list(&db_entry->perms,
			     perms_default, num_perms_default);
	db_entry->last_login = time(NULL);

	if((opt.registration_policy == GGZ_REGISTRATION_OPEN)
	|| (opt.registration_policy == GGZ_REGISTRATION_CONFIRMLATER)) {
		db_entry->confirmed = 1;
	} else {
		db_entry->confirmed = 0;
	}
	
	/* If we tried to overwrite a value, then we know it existed */
	if (ggzdb_player_add(db_entry) == GGZDB_ERR_DUPKEY) {
		ggz_debug(GGZ_DBG_CONNECTION, "Unsuccessful new login of %s", 
			name);
		return GGZ_ERROR;
	}

	return GGZ_OK;
}


/* This routine validates the username request */
static bool is_valid_username(const char *name)
{
	/* "<none>" is invalid */
	/* FIXME: why would this need special handling at all? */
	if (strcasecmp(name, "<none>") == 0) {
		return false;
	}

	if (!username_allowed(name)) {
		return false;
	}

	return true;
}

/* This routine validates the password request */
static bool is_valid_password(const char *password)
{
#ifdef WITH_CRACKLIB
	const char *res;
	const char *dicts[] = {
		CRACKLIB_DICTPATH,
		"/usr/lib/cracklib_dict.pw",
		NULL
	};
	const char *dict = NULL;
	struct stat st;
	int ret;
	int i;

	for (i = 0; dicts[i]; i++)
	{
		ret = stat(dicts[i], &st);
		if (ret == 0) {
			dict = dicts[i];
			break;
		}
	}

	if (dict) {
		res = FascistCheck(password, dict);
		if (res) {
			return false;
		}
	} else {
		ggz_error_msg("Cracklib password database not found.");
		return false;
	}
#endif
#ifdef WITH_OMNICRACKLIB
	int res;

	res = omnicrack_checkstrength(password, OMNICRACK_STRATEGY_ALL - OMNICRACK_STRATEGY_DICT, NULL);
	if (res != OMNICRACK_RESULT_OK) {
		return false;
	}
#endif
	return true;
}
