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
#include <unistd.h>

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

/* Network object structure */
struct _GGZNetIO {

	/* The player this object serves */
	struct _GGZPlayer *player;

	/* The actual socket */
	int fd;
};


/* Private internal functions */
static int _net_handle_login(GGZLoginType type, GGZNetIO *net);
static int _net_handle_logout(GGZNetIO *net);
static int _net_handle_room_join(GGZNetIO *net);
static int _net_handle_table_launch(GGZNetIO *net);
static int _net_handle_table_join(GGZNetIO *net);
static int _net_handle_table_leave(GGZNetIO *net);
static int _net_handle_list_players(GGZNetIO *net);
static int _net_handle_list_types(GGZNetIO *net);
static int _net_handle_list_rooms(GGZNetIO *net);
static int _net_handle_list_tables(GGZNetIO *net);
static int _net_handle_msg_from_sized(GGZNetIO *net);
static int _net_handle_chat(GGZNetIO *net);
static int _net_handle_motd(GGZNetIO *net);

static int _net_send_result(GGZNetIO *net, unsigned char opcode, char result);
static int _net_send_error(GGZNetIO *net);
static int _net_send_login_normal_status(GGZNetIO *net, char status);
static int _net_send_login_anon_status(GGZNetIO *net, char status);
static int _net_send_login_new_status(GGZNetIO *net, char status, char *password);

static int _net_read_name(int sock, char name[MAX_USER_NAME_LEN + 1]);


/* Create a new network IO object */
GGZNetIO* net_new(int fd, GGZPlayer *player)
{
	GGZNetIO *net = NULL;

	if ( (net = malloc(sizeof(GGZNetIO))) == NULL)
		err_sys_exit("malloc error in net_new()");

	net->fd = fd;
	net->player = player;

	return net;
}


/* Get filedescriptor for communication */
int net_get_fd(GGZNetIO* net)
{
	if (!net)
		return -1;
	else
		return net->fd;
}


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
int net_read_data(GGZNetIO* net)
{
	int status;
	UserToControl op;
	
	if (es_read_int(net->fd, (int *)&op) < 0)
		return GGZ_REQ_DISCONNECT;
	
	switch (op) {

	case REQ_LOGIN_NEW:
		status = _net_handle_login(GGZ_LOGIN_NEW, net);
		break;

	case REQ_LOGIN:
		status = _net_handle_login(GGZ_LOGIN, net);
		break;

	case REQ_LOGIN_ANON:
		status = _net_handle_login(GGZ_LOGIN_GUEST, net);
		break;

	case REQ_LOGOUT:
		status = _net_handle_logout(net);
		break;

	case REQ_TABLE_LAUNCH:
		status = _net_handle_table_launch(net);
		break;

	case REQ_TABLE_JOIN:
		status = _net_handle_table_join(net);
		break;

	case REQ_TABLE_LEAVE:
		status = _net_handle_table_leave(net);
		break;

	case REQ_LIST_PLAYERS:
		status = _net_handle_list_players(net);
		break;

	case REQ_LIST_TYPES:
		status = _net_handle_list_types(net);
		break;

	case REQ_LIST_TABLES:
		status = _net_handle_list_tables(net);
		break;

	case REQ_LIST_ROOMS:
		status = _net_handle_list_rooms(net);
		break;

	case REQ_ROOM_JOIN:
		status = _net_handle_room_join(net);
		break;

	case REQ_GAME:
		status = _net_handle_msg_from_sized(net); 
		break;
			
	case REQ_CHAT:
		status = _net_handle_chat(net);
		break;

	case REQ_MOTD:
		status = _net_handle_motd(net);
		break;
	  
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "%s requested unimplemented op %d", 
			net->player->name, op);
		
		status = _net_send_error(net);
	}

	return status;
}


/* Disconnect from network */
void net_disconnect(GGZNetIO* net)
{
	if (net && net->fd != -1) {
		close(net->fd);
		net->fd = -1;
	}
}


/* Free up resources used by net object */
void net_free(GGZNetIO* net)
{
	if (net)
		free(net);
}


int net_send_serverid(GGZNetIO *net)
{
	if (es_write_int(net->fd, MSG_SERVER_ID) < 0 
	    || es_va_write_string(net->fd, "GGZ-%s", VERSION) < 0
	    || es_write_int(net->fd, GGZ_CS_PROTO_VERSION) < 0
	    || es_write_int(net->fd, MAX_CHAT_LEN) < 0)
		return -1;

	return 0;
}
 

int net_send_server_full(GGZNetIO *net)
{
	return es_write_int(net->fd, MSG_SERVER_FULL);
}


int net_send_login(GGZNetIO *net, GGZLoginType type, char status, char *password)
{
	switch (type) {
	case GGZ_LOGIN:
		return _net_send_login_normal_status(net, status);
		break;
	case GGZ_LOGIN_GUEST:
		return _net_send_login_anon_status(net, status);
		break;
	case GGZ_LOGIN_NEW:
		return _net_send_login_new_status(net, status, password);
	}

	/* Should never get here */
	return -1;
}


int net_send_motd(GGZNetIO *net)
{
	int i, num;
	char line[1024];
	

	num = motd_get_num_lines();
		
	if (es_write_int(net->fd, MSG_MOTD) < 0
	    || es_write_int(net->fd, num) < 0)
		return -1;
	
	for (i = 0; i < num; i++) {
		motd_get_line(i, line, sizeof(line));
		if (es_write_string(net->fd, line) < 0)
			return -1;
	}

	return 0;
}


int net_send_motd_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_MOTD, status);
}


int net_send_room_list_error(GGZNetIO *net, char status)
{
	if (es_write_int(net->fd, RSP_LIST_ROOMS) < 0
	    || es_write_int(net->fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_room_list_count(GGZNetIO *net, int count)
{
	if (es_write_int(net->fd, RSP_LIST_ROOMS) < 0
	    || es_write_int(net->fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_room(GGZNetIO *net, int index, char *name, int game, char *desc)
{
	if (es_write_int(net->fd, index) < 0
	    || es_write_string(net->fd, name) < 0
	    || es_write_int(net->fd, game) < 0)
		return -1;

	if (desc && es_write_string(net->fd, desc) <0)
		return -1;

	return 0;
}


int net_send_type_list_error(GGZNetIO *net, char status)
{
	if (es_write_int(net->fd, RSP_LIST_TYPES) < 0
	    || es_write_int(net->fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_type_list_count(GGZNetIO *net, int count)
{
	if (es_write_int(net->fd, RSP_LIST_TYPES) < 0
	    || es_write_int(net->fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_type(GGZNetIO *net, int index, GameInfo *type, char verbose)
{
	if (es_write_int(net->fd, index) < 0
	    || es_write_string(net->fd, type->name) < 0
	    || es_write_string(net->fd, type->version) < 0
	    || es_write_string(net->fd, type->p_engine) < 0
	    || es_write_string(net->fd, type->p_version) < 0
	    || es_write_char(net->fd, type->player_allow_mask) < 0
	    || es_write_char(net->fd, type->bot_allow_mask) < 0)
		return -1;

	if (verbose) {
		if (es_write_string(net->fd, type->desc) < 0
		    || es_write_string(net->fd, type->author) < 0
		    || es_write_string(net->fd, type->homepage) < 0)
			return -1;
	}

	return 0;
}


int net_send_player_list_error(GGZNetIO *net, char status)
{
	if (es_write_int(net->fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(net->fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_player_list_count(GGZNetIO *net, int count)
{
	
	
	if (es_write_int(net->fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(net->fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_player(GGZNetIO *net, GGZPlayer *p2)
{
	if (es_write_string(net->fd, p2->name) < 0
	    || es_write_int(net->fd, p2->table) < 0)
		return -1;
	
	return 0;
}


int net_send_table_list_error(GGZNetIO *net, char status)
{
	if (es_write_int(net->fd, RSP_LIST_TABLES) < 0
	    || es_write_int(net->fd, (int)status) < 0)
		return -1;
	
	return 0;
}


int net_send_table_list_count(GGZNetIO *net, int count)
{
	if (es_write_int(net->fd, RSP_LIST_TABLES) < 0
	    || es_write_int(net->fd, count) < 0)
		return -1;
	
	return 0;
}


int net_send_table(GGZNetIO *net, GGZTable *table)
{
	char *name = NULL;
	int i, seat;
	
	if (es_write_int(net->fd, table->index) < 0
	    || es_write_int(net->fd, table->room) < 0
	    || es_write_int(net->fd, table->type) < 0
	    || es_write_string(net->fd, table->desc) < 0
	    || es_write_char(net->fd, table->state) < 0
	    || es_write_int(net->fd, seats_num(table)) < 0)
		return -1;
	
	for (i = 0; i < seats_num(table); i++) {
		seat = seats_type(table, i);
		if (es_write_int(net->fd, seat) < 0)
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
		
		if (es_write_string(net->fd, name) < 0)
			return -1;
	}

	return 0;
}


int net_send_room_join(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_ROOM_JOIN, status);
}


int net_send_chat(GGZNetIO *net, unsigned char opcode, char *name, char *msg)
{
	if (es_write_int(net->fd, MSG_CHAT) < 0
	    || es_write_char(net->fd, opcode) < 0
	    || es_write_string(net->fd, name) < 0)
		return -1;

	if (opcode & GGZ_CHAT_M_MESSAGE
	    && es_write_string(net->fd, msg) < 0)
		return -1;

	return 0;
}


int net_send_chat_result(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_CHAT, status);
}


int net_send_table_launch(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_TABLE_LAUNCH, status);
}


int net_send_table_join(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_TABLE_JOIN, status);
}


int net_send_table_leave(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_TABLE_LEAVE, status);
}


int net_send_player_update(GGZNetIO *net, unsigned char opcode, char *name)
{
	if (es_write_int(net->fd, MSG_UPDATE_PLAYERS) < 0
	    || es_write_char(net->fd, opcode) < 0
	    || es_write_string(net->fd, name) < 0)
		return -1;
	
	return 0;
}

int net_send_table_update(GGZNetIO *net, unsigned char opcode, GGZTable *table, int seat)
{
	/* Always send opcode */
	if (es_write_int(net->fd, MSG_UPDATE_TABLES) < 0 
	    || es_write_char(net->fd, opcode) < 0)
		return -1;
	
	/* If it's an add, all we do it send the table */
	if (opcode == GGZ_UPDATE_ADD)
		return net_send_table(net, table);

	/* For the others, we always send the index */
	if (es_write_int(net->fd, table->index) < 0)
		return -1;

	/* If it's a delete, we're done now */
	switch (opcode) {
	case GGZ_UPDATE_DELETE:  /* no more to do for delete */
		break;
	case GGZ_UPDATE_STATE:
		return es_write_char(net->fd, table->state);
		break;
	case GGZ_UPDATE_JOIN:
	case GGZ_UPDATE_LEAVE:
		if (es_write_int(net->fd, seat) < 0
		    || es_write_string(net->fd, table->seats[seat]) < 0)
			return -1;
	}

	/* If we get here it must have been OK */
	return 0;
}


int net_send_logout(GGZNetIO *net, char status)
{
	return _net_send_result(net, RSP_LOGOUT, status);
}


int net_send_game_data(GGZNetIO *net, int size, char *data)
{
	if (es_write_int(net->fd, RSP_GAME) < 0
	    || es_write_int(net->fd, size) < 0
	    || es_writen(net->fd, data, size) < 0)
		return -1;

	return 0;
}


/**************** Opcode Handlers **************************/

static int _net_handle_login(GGZLoginType type, GGZNetIO *net) 
{
	char name[MAX_USER_NAME_LEN + 1];
	char password[17];
		

	if (_net_read_name(net->fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	if (type == GGZ_LOGIN && es_read_string(net->fd, password, 17) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return login_player(type, net->player, name, password);
}


static int _net_handle_logout(GGZNetIO *net) 
{
	logout_player(net->player);
	return GGZ_REQ_DISCONNECT;
}


static int _net_handle_room_join(GGZNetIO *net)
{
	int room;
	
	/* Get the user's room request */
	if (es_read_int(net->fd, &room) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return room_handle_join(net->player, room);
}


static int _net_handle_table_launch(GGZNetIO *net) 
{
	int type, count, i;
	char desc[MAX_GAME_DESC_LEN + 1];
	int seats[MAX_TABLE_SIZE];
	char names[MAX_TABLE_SIZE][MAX_USER_NAME_LEN + 1];
	

	if (es_read_int(net->fd, &type) < 0
	    || es_read_string(net->fd, desc, (MAX_GAME_DESC_LEN + 1)) < 0
	    || es_read_int(net->fd, &count) < 0)
		return -1;

	/* Read in seat assignments */
	for (i = 0; i < count; i++) {
		if (es_read_int(net->fd, &seats[i]) < 0)
			return -1;
		if (seats[i] == GGZ_SEAT_RESV 
		    && _net_read_name(net->fd, names[i]) < 0)
			return -1;
	}

	/* Blank out the other seats */
	for (i = count; i < MAX_TABLE_SIZE; i++)
		seats[i] = GGZ_SEAT_NONE;

	return player_table_launch(net->player, type, desc, count, seats, names);
}


static int _net_handle_table_join(GGZNetIO *net) 
{
	int index;

	if (es_read_int(net->fd, &index) < 0)
		return -1;

	return player_table_join(net->player, index);
}


static int _net_handle_table_leave(GGZNetIO *net) 
{
	return player_table_leave(net->player);
}


static int _net_handle_list_players(GGZNetIO *net) 
{
	return player_list_players(net->player);
}


static int _net_handle_list_types(GGZNetIO *net) 
{
	char verbose;

	if (es_read_char(net->fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return player_list_types(net->player, verbose);
}


static int _net_handle_list_tables(GGZNetIO *net) 
{
	int type;
	char global;

	if (es_read_int(net->fd, &type) < 0
	    || es_read_char(net->fd, &global) < 0)
		return GGZ_REQ_DISCONNECT;

	return player_list_tables(net->player, type, global);
}


static int _net_handle_list_rooms(GGZNetIO *net)
{
	int game;
	char verbose;
	
	/* Get the options from teh client */
	if (es_read_int(net->fd, &game) < 0
	    || es_read_char(net->fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;

	return room_list_send(net->player, game, verbose);
}


static int _net_handle_msg_from_sized(GGZNetIO *net) 
{
	int size;
	char data[4096];	

	if (es_read_int(net->fd, &size) < 0
	    || es_readn(net->fd, data, size) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return player_msg_from_sized(net->player, size, data);
}
 

static int _net_handle_chat(GGZNetIO *net) 
{
	
	unsigned char subop;
	char *msg = NULL, *target = NULL;
	int status;

	if (es_read_char(net->fd, &subop) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Get arguments if they are used for this subop */
	if (subop & GGZ_CHAT_M_PLAYER) {
		if (es_read_string_alloc(net->fd, &target) < 0)
			return GGZ_REQ_DISCONNECT;
	}

	if (subop & GGZ_CHAT_M_MESSAGE) {
		if (es_read_string_alloc(net->fd, &msg) < 0) {
			return GGZ_REQ_DISCONNECT;
		}
	}

	status = player_chat(net->player, subop, target, msg);

	/* Free message now it's been dealt with  */
	if (msg)
		free(msg);
	
	return status;
}


static int _net_handle_motd(GGZNetIO *net) 
{
	return player_motd(net->player);
}


/************ Utility/Convenience functions *******************/

static int _net_send_result(GGZNetIO *net, unsigned char opcode, char result)
{
	if (es_write_int(net->fd, opcode) < 0
	    || es_write_char(net->fd, result) < 0)
		return -1;
	
	return 0;
}


static int _net_send_login_normal_status(GGZNetIO *net, char status)
{
	/* Try to send login status */
	if (_net_send_result(net, RSP_LOGIN, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 
	    && (es_write_int(net->fd, 265) < 0
		|| es_write_char(net->fd, 0) < 0))
		return -1;
	
	return 0;
}


static int _net_send_login_anon_status(GGZNetIO *net, char status)
{
	/* Try to send login status */
	if (_net_send_result(net, RSP_LOGIN_ANON, status) < 0)
		return -1;

	/* Try to send checksum if successful */
	if (status == 0 && es_write_int(net->fd, 265) < 0)
		return -1;
	
	
	return 0;
}


static int _net_send_login_new_status(GGZNetIO *net, char status, char *password)
{
	/* Try to send login status */
	if (_net_send_result(net, RSP_LOGIN_NEW, status) < 0)
		return -1;
	
	/* Try to send checksum if successful */
	if (status == 0 
	    && (es_write_string(net->fd, password) < 0
		|| es_write_int(net->fd, 265) < 0))
		return -1;
	
	return 0;
}


static int _net_send_error(GGZNetIO *net) 
{
	return (es_write_int(net->fd, MSG_ERROR));
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

