/*
 * File: net.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 8/27/01
 * Desc: Functions for handling network IO
 *
 * Copyright (C) 1999-2001 Brent Hendricks.
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

#include <string.h>
#include <easysock.h>
#include <stdlib.h>

#include <err_func.h>
#include <ggzd.h>
#include <login.h>
#include <motd.h>
#include <net.h>
#include <players.h>
#include <protocols.h>
#include <room.h>


/* Private internal functions */
static int _net_handle_login(GGZLoginType type, GGZPlayer *player, int fd);
static int _net_handle_logout(GGZPlayer *player, int fd);
static int _net_handle_table_launch(GGZPlayer *player, int fd);
static int _net_handle_table_join(GGZPlayer *player, int fd);
static int _net_handle_table_leave(GGZPlayer *player, int fd);
static int _net_handle_list_players(GGZPlayer *player, int fd);
static int _net_handle_list_types(GGZPlayer *player, int fd);
static int _net_handle_list_tables(GGZPlayer *player, int fd);
static int _net_handle_room_request(int op, GGZPlayer *Player, int fd);
static int _net_handle_msg_from_sized(GGZPlayer *player);
static int _net_handle_chat(GGZPlayer *player, int fd);
static int _net_handle_motd(GGZPlayer *player, int fd);

static int _net_send_error(GGZPlayer *player, int fd);
static int _net_send_login_normal_status(GGZPlayer *player, char status);
static int _net_send_login_anon_status(GGZPlayer *player, char status);
static int _net_send_login_new_status(GGZPlayer *player, char status, char *password);

static int _net_read_name(int sock, char name[MAX_USER_NAME_LEN + 1]);

/*
 * net_data_read() reads a single opcoode from the client, and then
 * dispatches the correct function to read in the data from the client
 * and handle the request.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure 
 * int fd            : player's fd
 *
 * Returns: 
 * int : one of
 *  GGZ_REQ_TABLE_JOIN   : player is now in a game.  fd of the game 
 *                         is now pointed to by t_fd.
 *  GGZ_REQ_TABLE_LEAVE  : player has completed game
 *  GGZ_REQ_DISCONNECT   : player is being logged out (possbily due to error)
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK : nothing special */
int net_read_data(GGZPlayer* player, int fd)
{
	int status;
	UserToControl op;

	if (es_read_int(fd, (int *)&op) < 0)
		return GGZ_REQ_DISCONNECT;
	
	switch (op) {

	case REQ_LOGIN_NEW:
		status = _net_handle_login(GGZ_LOGIN_NEW, player, fd);
		break;

	case REQ_LOGIN:
		status = _net_handle_login(GGZ_LOGIN, player, fd);
		break;

	case REQ_LOGIN_ANON:
		status = _net_handle_login(GGZ_LOGIN_GUEST, player, fd);
		break;

	case REQ_LOGOUT:
		status = _net_handle_logout(player, fd);
		break;

	case REQ_TABLE_LAUNCH:
		status = _net_handle_table_launch(player, fd);
		break;

	case REQ_TABLE_JOIN:
		status = _net_handle_table_join(player, fd);
		break;

	case REQ_TABLE_LEAVE:
		status = _net_handle_table_leave(player, fd);
		break;

	case REQ_LIST_PLAYERS:
		status = _net_handle_list_players(player, fd);
		break;

	case REQ_LIST_TYPES:
		status = _net_handle_list_types(player, fd);
		break;

	case REQ_LIST_TABLES:
		status = _net_handle_list_tables(player, fd);
		break;

	case REQ_LIST_ROOMS:
	case REQ_ROOM_JOIN:
		status = _net_handle_room_request((int)op, player, fd);
		break;

	case REQ_GAME:
		status = _net_handle_msg_from_sized(player); 
		break;
			
	case REQ_CHAT:
		status = _net_handle_chat(player, fd);
		break;

	case REQ_MOTD:
		status = _net_handle_motd(player, fd);
		break;
	  
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "%s requested unimplemented op %d", 
			player->name, op);
		
		status = _net_send_error(player, fd);
	}

	return status;
}


int net_send_login(GGZLoginType type, GGZPlayer *player, char status, char *password)
{
	switch (type) {
	case GGZ_LOGIN:
		return _net_send_login_normal_status(player, status);
		break;
	case GGZ_LOGIN_GUEST:
		return _net_send_login_anon_status(player, status);
		break;
	case GGZ_LOGIN_NEW:
		return _net_send_login_new_status(player, status, password);
	}

	/* Should never get here */
	return -1;
}


int net_send_motd(GGZPlayer *player)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);
	
	if (es_write_int(fd, MSG_MOTD) < 0
	    || motd_send_motd(fd) < 0)
		return -1;

	return 0;
}


int net_send_logout(GGZPlayer *player, char status)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);

	if (es_write_int(fd, RSP_LOGOUT) < 0 
	    || es_write_char(fd, 0) < 0)
		return 1;

	return 0;
}


static int _net_send_login_normal_status(GGZPlayer *player, char status)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);
	
	/* Try to send login status */
	if (es_write_int(fd, RSP_LOGIN) < 0 
	    || es_write_char(fd, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 
	    && (es_write_int(fd, 265) < 0
		|| es_write_char(fd, 0) < 0))
		return -1;
	
	return 0;
}


static int _net_send_login_anon_status(GGZPlayer *player, char status)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);
	
	/* Try to send login status */
	if (es_write_int(fd, RSP_LOGIN_ANON) < 0 
	    || es_write_char(fd, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 && es_write_int(fd, 265) < 0)
		return -1;
	
	
	return 0;
}


static int _net_send_login_new_status(GGZPlayer *player, char status, char *password)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);
	
	/* Try to send login status */
	if (es_write_int(fd, RSP_LOGIN_NEW) < 0 
	    || es_write_char(fd, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 
	    && (es_write_string(fd, password) < 0
		|| es_write_int(fd, 265) < 0))
		return -1;
	
	return 0;
}


static int _net_handle_login(GGZLoginType type, GGZPlayer *player, int fd) 
{
	char name[MAX_USER_NAME_LEN + 1];
	char password[17];
	
	if (_net_read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	if (type == GGZ_LOGIN && es_read_string(fd, password, 17) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return login_player(type, player, name, password);
}


static int _net_handle_logout(GGZPlayer *player, int fd) 
{
	logout_player(player, fd);
	return GGZ_REQ_DISCONNECT;
}


static int _net_handle_table_launch(GGZPlayer *player, int fd) 
{
	return player_table_launch(player, fd);
}


static int _net_handle_table_join(GGZPlayer *player, int fd) 
{
	return player_table_join(player, fd);
}


static int _net_handle_table_leave(GGZPlayer *player, int fd) 
{
	return player_table_leave(player, fd);
}


static int _net_handle_list_players(GGZPlayer *player, int fd) 
{
	return player_list_players(player, fd);
}


static int _net_handle_list_types(GGZPlayer *player, int fd) 
{
	return player_list_types(player, fd);
}


static int _net_handle_list_tables(GGZPlayer *player, int fd) 
{
	return player_list_tables(player, fd);;
}


static int _net_handle_room_request(int op, GGZPlayer *player, int fd) 
{
	return room_handle_request(op, player, fd);
}


static int _net_handle_msg_from_sized(GGZPlayer *player) 
{
	return player_msg_from_sized(player);
}
 

static int _net_handle_chat(GGZPlayer *player, int fd) 
{
	return player_chat(player, fd);
}


static int _net_handle_motd(GGZPlayer *player, int fd) 
{
	return player_motd(player, fd);
}


static int _net_send_error(GGZPlayer *player, int fd) 
{
	return (es_write_int(fd, MSG_ERROR));
}


static int _net_read_name(int sock, char name[MAX_USER_NAME_LEN + 1])
{
	char *tmp;

	if (es_read_string_alloc(sock, &tmp) < 0)
		return -1;

	strncpy(name, tmp, MAX_USER_NAME_LEN);
	free(tmp);

	/* Make sure names are null-terminated */
	name[MAX_USER_NAME_LEN] = '\0';
	
	return 0;
}

