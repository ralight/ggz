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
#include <seats.h>
#include <table.h>


/* Private internal functions */
static int _net_handle_login(GGZLoginType type, GGZPlayer *player);
static int _net_handle_logout(GGZPlayer *player);
static int _net_handle_room_join(GGZPlayer *player);
static int _net_handle_table_launch(GGZPlayer *player);
static int _net_handle_table_join(GGZPlayer *player);
static int _net_handle_table_leave(GGZPlayer *player);
static int _net_handle_list_players(GGZPlayer *player);
static int _net_handle_list_types(GGZPlayer *player);
static int _net_handle_list_rooms(GGZPlayer *player);
static int _net_handle_list_tables(GGZPlayer *player);
static int _net_handle_msg_from_sized(GGZPlayer *player);
static int _net_handle_chat(GGZPlayer *player);
static int _net_handle_motd(GGZPlayer *player);

static int _net_send_result(GGZPlayer *player, unsigned char opcode, char result);
static int _net_send_error(GGZPlayer *player);
static int _net_send_login_normal_status(GGZPlayer *player, char status);
static int _net_send_login_anon_status(GGZPlayer *player, char status);
static int _net_send_login_new_status(GGZPlayer *player, char status, char *password);

static int _net_read_name(int sock, char name[MAX_USER_NAME_LEN + 1]);
static int _net_get_fd(GGZPlayer *player);


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
int net_read_data(GGZPlayer* player)
{
	int status;
	int fd = _net_get_fd(player);
	UserToControl op;
	
	if (es_read_int(fd, (int *)&op) < 0)
		return GGZ_REQ_DISCONNECT;
	
	switch (op) {

	case REQ_LOGIN_NEW:
		status = _net_handle_login(GGZ_LOGIN_NEW, player);
		break;

	case REQ_LOGIN:
		status = _net_handle_login(GGZ_LOGIN, player);
		break;

	case REQ_LOGIN_ANON:
		status = _net_handle_login(GGZ_LOGIN_GUEST, player);
		break;

	case REQ_LOGOUT:
		status = _net_handle_logout(player);
		break;

	case REQ_TABLE_LAUNCH:
		status = _net_handle_table_launch(player);
		break;

	case REQ_TABLE_JOIN:
		status = _net_handle_table_join(player);
		break;

	case REQ_TABLE_LEAVE:
		status = _net_handle_table_leave(player);
		break;

	case REQ_LIST_PLAYERS:
		status = _net_handle_list_players(player);
		break;

	case REQ_LIST_TYPES:
		status = _net_handle_list_types(player);
		break;

	case REQ_LIST_TABLES:
		status = _net_handle_list_tables(player);
		break;

	case REQ_LIST_ROOMS:
		status = _net_handle_list_rooms(player);
		break;

	case REQ_ROOM_JOIN:
		status = _net_handle_room_join(player);
		break;

	case REQ_GAME:
		status = _net_handle_msg_from_sized(player); 
		break;
			
	case REQ_CHAT:
		status = _net_handle_chat(player);
		break;

	case REQ_MOTD:
		status = _net_handle_motd(player);
		break;
	  
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "%s requested unimplemented op %d", 
			player->name, op);
		
		status = _net_send_error(player);
	}

	return status;
}


int net_send_serverid(GGZPlayer *player)
{
	int fd = _net_get_fd(player);

	if (es_write_int(fd, MSG_SERVER_ID) < 0 
	    || es_va_write_string(fd, "GGZ-%s", VERSION) < 0
	    || es_write_int(fd, GGZ_CS_PROTO_VERSION) < 0
	    || es_write_int(fd, MAX_CHAT_LEN) < 0)
		return -1;

	return 0;
}
 

int net_send_server_full(GGZPlayer *player)
{
	int fd = _net_get_fd(player);
	
	return es_write_int(fd, MSG_SERVER_FULL);
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
	int i, num;
	char line[1024];
	int fd = _net_get_fd(player);

	num = motd_get_num_lines();
		
	if (es_write_int(fd, MSG_MOTD) < 0
	    || es_write_int(fd, num) < 0)
		return -1;
	
	for (i = 0; i < num; i++) {
		motd_get_line(i, line, sizeof(line));
		if (es_write_string(fd, line) < 0)
			return -1;
	}

	return 0;
}


int net_send_motd_error(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_MOTD, status);
}


int net_send_room_list_error(GGZPlayer *player, char status)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_ROOMS) < 0
	    || es_write_int(fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_room_list_count(GGZPlayer *player, int count)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_ROOMS) < 0
	    || es_write_int(fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_room(GGZPlayer *player, int index, char *name, int game, char *desc)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, index) < 0
	    || es_write_string(fd, name) < 0
	    || es_write_int(fd, game) < 0)
		return -1;

	if (desc && es_write_string(fd, desc) <0)
		return -1;

	return 0;
}


int net_send_type_list_error(GGZPlayer *player, char status)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_TYPES) < 0
	    || es_write_int(fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_type_list_count(GGZPlayer *player, int count)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_TYPES) < 0
	    || es_write_int(fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_type(GGZPlayer *player, int index, GameInfo *type, char verbose)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, index) < 0
	    || es_write_string(fd, type->name) < 0
	    || es_write_string(fd, type->version) < 0
	    || es_write_string(fd, type->p_engine) < 0
	    || es_write_string(fd, type->p_version) < 0
	    || es_write_char(fd, type->player_allow_mask) < 0
	    || es_write_char(fd, type->bot_allow_mask) < 0)
		return -1;

	if (verbose) {
		if (es_write_string(fd, type->desc) < 0
		    || es_write_string(fd, type->author) < 0
		    || es_write_string(fd, type->homepage) < 0)
			return -1;
	}

	return 0;
}


int net_send_player_list_error(GGZPlayer *player, char status)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_player_list_count(GGZPlayer *player, int count)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_player(GGZPlayer *player, GGZPlayer *p2)
{
	int fd = _net_get_fd(player);
	
	if (es_write_string(fd, p2->name) < 0
	    || es_write_int(fd, p2->table) < 0)
		return -1;
	
	return 0;
}


int net_send_table_list_error(GGZPlayer *player, char status)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_table_list_count(GGZPlayer *player, int count)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_table(GGZPlayer *player, GGZTable *table)
{
	char *name = NULL;
	int i, seat, fd = _net_get_fd(player);
	
	
	if (es_write_int(fd, table->index) < 0
	    || es_write_int(fd, table->room) < 0
	    || es_write_int(fd, table->type) < 0
	    || es_write_string(fd, table->desc) < 0
	    || es_write_char(fd, table->state) < 0
	    || es_write_int(fd, seats_num(table)) < 0)
		return -1;
	
	for (i = 0; i < seats_num(table); i++) {
		seat = seats_type(table, i);
		if (es_write_int(fd, seat) < 0)
			return -1;
		
		switch(seat) {
		case GGZ_SEAT_OPEN:
		case GGZ_SEAT_BOT:
			continue;  /* no name for these */
		case GGZ_SEAT_RESV:
			name = table->reserve[i];
			break;
		case GGZ_SEAT_PLAYER:
			name = table->seats[i];
			break;
		}
		
		if (es_write_string(fd, name) < 0)
			return -1;
	}

	return 0;
}


int net_send_room_join(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_ROOM_JOIN, status);
}


int net_send_chat(GGZPlayer *player, unsigned char opcode, char *name, char *msg)
{
	int fd = _net_get_fd(player);

	if (es_write_int(fd, MSG_CHAT) < 0
	    || es_write_char(fd, opcode) < 0
	    || es_write_string(fd, name) < 0)
		return -1;

	if (opcode & GGZ_CHAT_M_MESSAGE
	    && es_write_string(fd, msg) < 0)
		return -1;

	return 0;
}


int net_send_chat_result(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_CHAT, status);
}


int net_send_table_launch(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_TABLE_LAUNCH, status);
}


int net_send_table_join(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_TABLE_JOIN, status);
}


int net_send_table_leave(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_TABLE_LEAVE, status);
}


int net_send_player_update(GGZPlayer *player, unsigned char opcode, char *name)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, MSG_UPDATE_PLAYERS) < 0
	    || es_write_char(fd, opcode) < 0
	    || es_write_string(fd, name) < 0)
		return -1;
	
	return 0;
}

int net_send_table_update(GGZPlayer *player, unsigned char opcode, GGZTable *table, int seat)
{
	int fd = _net_get_fd(player);

	/* Always send opcode */
	if (es_write_int(fd, MSG_UPDATE_TABLES) < 0 
	    || es_write_char(fd, opcode) < 0)
		return -1;
	
	/* If it's an add, all we do it send the table */
	if (opcode == GGZ_UPDATE_ADD)
		return net_send_table(player, table);

	/* For the others, we always send the index */
	if (es_write_int(fd, table->index) < 0)
		return -1;

	/* If it's a delete, we're done now */
	switch (opcode) {
	case GGZ_UPDATE_DELETE:  /* no more to do for delete */
		break;
	case GGZ_UPDATE_STATE:
		return es_write_char(fd, table->state);
		break;
	case GGZ_UPDATE_JOIN:
	case GGZ_UPDATE_LEAVE:
		if (es_write_int(fd, seat) < 0
		    || es_write_string(fd, table->seats[seat]) < 0)
			return -1;
	}

	/* If we get here it must have been OK */
	return 0;
}


int net_send_logout(GGZPlayer *player, char status)
{
	return _net_send_result(player, RSP_LOGOUT, status);
}


int net_send_game_data(GGZPlayer *player, int size, char *data)
{
	int fd = _net_get_fd(player);

	if (es_write_int(fd, RSP_GAME) < 0
	    || es_write_int(fd, size) < 0
	    || es_writen(fd, data, size) < 0)
		return -1;

	return 0;
}


/**************** Opcode Handlers **************************/

static int _net_handle_login(GGZLoginType type, GGZPlayer *player) 
{
	char name[MAX_USER_NAME_LEN + 1];
	char password[17];
	int fd = _net_get_fd(player);	

	if (_net_read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	if (type == GGZ_LOGIN && es_read_string(fd, password, 17) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return login_player(type, player, name, password);
}


static int _net_handle_logout(GGZPlayer *player) 
{
	logout_player(player);
	return GGZ_REQ_DISCONNECT;
}


static int _net_handle_room_join(GGZPlayer *player)
{
	int room, fd = _net_get_fd(player);
	
	/* Get the user's room request */
	if (es_read_int(fd, &room) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return room_handle_join(player, room);
}


static int _net_handle_table_launch(GGZPlayer *player) 
{
	int type, count, i;
	char desc[MAX_GAME_DESC_LEN + 1];
	int seats[MAX_TABLE_SIZE];
	char names[MAX_TABLE_SIZE][MAX_USER_NAME_LEN + 1];
	int fd = _net_get_fd(player);

	if (es_read_int(fd, &type) < 0
	    || es_read_string(fd, desc, (MAX_GAME_DESC_LEN + 1)) < 0
	    || es_read_int(fd, &count) < 0)
		return -1;

	/* Read in seat assignments */
	for (i = 0; i < count; i++) {
		if (es_read_int(fd, &seats[i]) < 0)
			return -1;
		if (seats[i] == GGZ_SEAT_RESV 
		    && _net_read_name(fd, names[i]) < 0)
			return -1;
	}

	/* Blank out the other seats */
	for (i = count; i < MAX_TABLE_SIZE; i++)
		seats[i] = GGZ_SEAT_NONE;

	return player_table_launch(player, type, desc, count, seats, names);
}


static int _net_handle_table_join(GGZPlayer *player) 
{
	int index, fd = _net_get_fd(player);

	if (es_read_int(fd, &index) < 0)
		return -1;

	return player_table_join(player, index);
}


static int _net_handle_table_leave(GGZPlayer *player) 
{
	return player_table_leave(player);
}


static int _net_handle_list_players(GGZPlayer *player) 
{
	return player_list_players(player);
}


static int _net_handle_list_types(GGZPlayer *player) 
{
	int fd = _net_get_fd(player);
	char verbose;

	if (es_read_char(fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return player_list_types(player, verbose);
}


static int _net_handle_list_tables(GGZPlayer *player) 
{
	int type, fd = _net_get_fd(player);
	char global;

	if (es_read_int(fd, &type) < 0
	    || es_read_char(fd, &global) < 0)
		return GGZ_REQ_DISCONNECT;

	return player_list_tables(player, type, global);
}


static int _net_handle_list_rooms(GGZPlayer *player)
{
	int fd = _net_get_fd(player);
	int game;
	char verbose;
	
	/* Get the options from teh client */
	if (es_read_int(fd, &game) < 0
	    || es_read_char(fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;

	return room_list_send(player, game, verbose);
}


static int _net_handle_msg_from_sized(GGZPlayer *player) 
{
	int size, fd = _net_get_fd(player);
	char data[4096];	

	if (es_read_int(fd, &size) < 0
	    || es_readn(fd, data, size) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return player_msg_from_sized(player, size, data);
}
 

static int _net_handle_chat(GGZPlayer *player) 
{
	int fd = _net_get_fd(player);
	unsigned char subop;
	char *msg = NULL, *target = NULL;
	int status;

	if (es_read_char(fd, &subop) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Get arguments if they are used for this subop */
	if (subop & GGZ_CHAT_M_PLAYER) {
		if (es_read_string_alloc(fd, &target) < 0)
			return GGZ_REQ_DISCONNECT;
	}

	if (subop & GGZ_CHAT_M_MESSAGE) {
		if (es_read_string_alloc(fd, &msg) < 0) {
			return GGZ_REQ_DISCONNECT;
		}
	}

	status = player_chat(player, subop, target, msg);

	/* Free message now it's been dealt with  */
	if (msg)
		free(msg);
	
	return status;
}


static int _net_handle_motd(GGZPlayer *player) 
{
	return player_motd(player);
}


/************ Utility/Convenience functions *******************/

static int _net_send_result(GGZPlayer *player, unsigned char opcode, char result)
{
	int fd = _net_get_fd(player);
	
	if (es_write_int(fd, opcode) < 0
	    || es_write_char(fd, result) < 0)
		return -1;
	
	return 0;
}


static int _net_send_login_normal_status(GGZPlayer *player, char status)
{
	int fd = _net_get_fd(player);

	/* Try to send login status */
	if (_net_send_result(player, RSP_LOGIN, status) < 0)
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
	int fd = _net_get_fd(player);
	
	/* Try to send login status */
	if (_net_send_result(player, RSP_LOGIN_ANON, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 && es_write_int(fd, 265) < 0)
		return -1;
	
	
	return 0;
}


static int _net_send_login_new_status(GGZPlayer *player, char status, char *password)
{
	int fd = _net_get_fd(player);
	
	/* Try to send login status */
	if (_net_send_result(player, RSP_LOGIN_NEW, status) < 0)
		return -1;
	
	/* Try to send checksum if successful */
	if (status == 0 
	    && (es_write_string(fd, password) < 0
		|| es_write_int(fd, 265) < 0))
		return -1;
	
	return 0;
}


static int _net_send_error(GGZPlayer *player) 
{
	int fd = _net_get_fd(player);

	return (es_write_int(fd, MSG_ERROR));
}


static int _net_get_fd(GGZPlayer *player)
{
	int fd;

	pthread_rwlock_rdlock(&player->lock);
	fd = player->fd;
	pthread_rwlock_unlock(&player->lock);

	return fd;
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

