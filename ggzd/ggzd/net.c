/*
 * File: net.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 8/27/01
 * Desc: Functions for handling network IO
 * $Id: net.c 2472 2001-09-14 00:04:35Z bmh $
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
#include <ggzdmod.h>
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

static int _net_send_result(GGZNetIO *net, char *action, char code);
static int _net_send_error(GGZNetIO *net);
static int _net_send_login_normal_status(GGZNetIO *net, char status);
static int _net_send_login_anon_status(GGZNetIO *net, char status);
static int _net_send_login_new_status(GGZNetIO *net, char status, char *password);
static int _net_send_table_status(GGZNetIO *net, GGZTable *table);
static int _net_send_table_seat(GGZNetIO *net, GGZTable *table, int num);
static int _net_send_seat(GGZNetIO *net, GGZTable *table, int num);
static int _net_send_line(GGZNetIO *net, char *line, ...);
static int _net_send_string(GGZNetIO *net, char *fmt, ...);
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
	_net_send_line(net, "<SESSION>");
	_net_send_line(net, "\t<SERVER ID='GGZ-%s' NAME='%s' VERSION='%d' STATUS='%s'>", VERSION, "Brent&apos;s Server", GGZ_CS_PROTO_VERSION, "ok");
	_net_send_line(net, "\t\t<OPTIONS CHATLEN='%d'/>", MAX_CHAT_LEN);
	_net_send_line(net, "\t</SERVER>");
			
	return 0;
}
 

int net_send_server_full(GGZNetIO *net)
{
	_net_send_line(net, "<SESSION>");
	_net_send_line(net, "\t<SERVER ID='GGZ-%s' NAME='%s' VERSION='%d' STATUS='%s'/>", VERSION, "Brent&apos;s Server", GGZ_CS_PROTO_VERSION, "full");
			
	return 0;
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
	
	_net_send_line(net, "<MOTD PRIORITY='normal'><![CDATA[");

	num = motd_get_num_lines();
		
	for (i = 0; i < num; i++) {
		motd_get_line(i, line, sizeof(line));
		_net_send_line(net, line);
	}

	_net_send_line(net, "]]></MOTD>");
	return 0;
}


int net_send_motd_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, "motd", status);
}


int net_send_room_list_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, "list", status);
}


int net_send_room_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>", 0);
	_net_send_line(net, "<LIST TYPE='room'>");
	return 0;
}


int net_send_room(GGZNetIO *net, int index, RoomStruct *room, char verbose)
{
	_net_send_line(net, "<ROOM ID='%d' NAME='%s' GAME='%d'>",
		       index, room->name, room->game_type);
	if (verbose && room->description)
		_net_send_line(net, "<DESC>%s</DESC>", room->description);
	_net_send_line(net, "</ROOM>");

	return 0;
}


int net_send_room_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	_net_send_line(net, "</RESULT>");
	return 0;
}


int net_send_type_list_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, "list", status);
}


int net_send_type_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>", 0);
	_net_send_line(net, "<LIST TYPE='game'>");
	return 0;
}


int net_send_type(GGZNetIO *net, int index, GameInfo *type, char verbose)
{
	_net_send_line(net, "<GAME ID='%d' NAME='%s' VERSION='%s'>",
		       index, type->name, type->version);
	_net_send_line(net, "<PROTOCOL ENGINE='%s' VERSION='%s'/>",
		       type->p_engine, type->p_version);
	_net_send_line(net, "<ALLOW PLAYERS='%d' BOTS='%d'/>",
		       type->player_allow_mask, type->bot_allow_mask);
	
	if (verbose) {
		_net_send_line(net, "<ABOUT AUTHOR='%s' URL='%s'/>",
			       type->author, type->homepage);
		_net_send_line(net, "<DESC>%s</DESC>", type->desc);
	}
	_net_send_line(net, "</GAME>");
	return 0;
}


int net_send_type_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	_net_send_line(net, "</RESULT>");
	return 0;
}

int net_send_player_list_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, "list", status);
}


int net_send_player_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>", 0);
	_net_send_line(net, "<LIST TYPE='player' ROOM=''>");
	return 0;
}


int net_send_player(GGZNetIO *net, GGZPlayer *p2)
{
	return _net_send_line(net, 
			      "<PLAYER ID='%s' TYPE='guest' TABLE='%d' />",
			      p2->name, p2->table);
}


int net_send_player_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	_net_send_line(net, "</RESULT>");
	return 0;
}


int net_send_table_list_error(GGZNetIO *net, char status)
{
	return _net_send_result(net, "list", status);
}


int net_send_table_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>", 0);
	_net_send_line(net, "<LIST TYPE='table' ROOM=''>");
	return 0;
}


int net_send_table(GGZNetIO *net, GGZTable *table)
{
	int i;

	_net_send_line(net, "<TABLE ID='%d' GAME='%d' STATUS='%d' SEATS='%d'>",
		       table->index, table->type, table->state, 
		       seats_num(table));

	_net_send_line(net, "<DESC>%s</DESC>", table->desc);
	
	for (i = 0; i < seats_num(table); i++)
		_net_send_seat(net, table, i);
	
	_net_send_line(net, "</TABLE>");

	return 0;
}


int net_send_table_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	_net_send_line(net, "</RESULT>");
	return 0;
}


int net_send_room_join(GGZNetIO *net, char status)
{
	return _net_send_result(net, "enter", status);
}


int net_send_chat(GGZNetIO *net, unsigned char opcode, char *name, char *msg)
{
	char *type = NULL;

	switch (opcode) {
	case GGZ_CHAT_NORMAL:
		type = "normal";
		break;
	case GGZ_CHAT_ANNOUNCE:
		type = "announce";
		break;
	case GGZ_CHAT_BEEP:
		type = "beep";
		break;
	case GGZ_CHAT_PERSONAL:
		type = "private";
		break;
	}

	if (opcode & GGZ_CHAT_M_MESSAGE) {
		_net_send_line(net, "<CHAT TYPE='%s' FROM='%s'><![CDATA[%s]]></CHAT>", 
			       type, name, msg);
	}
	else 
		_net_send_line(net, "<CHAT TYPE='%s' FROM='%s'/>", type, name);
			       

	return 0;
}


int net_send_chat_result(GGZNetIO *net, char status)
{
	return _net_send_result(net, "chat", status);
}


int net_send_table_launch(GGZNetIO *net, char status)
{
	return _net_send_result(net, "launch", status);
}


int net_send_table_join(GGZNetIO *net, char status)
{
	return _net_send_result(net, "join", status);
}


int net_send_table_leave(GGZNetIO *net, char status)
{
	return _net_send_result(net, "leave", status);
}


int net_send_player_update(GGZNetIO *net, unsigned char opcode, char *name)
{
	char *action = NULL;
	
	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		action = "delete";
		break;
	case GGZ_UPDATE_ADD:
		action = "add";
		break;
	}
	_net_send_line(net, "<UPDATE TYPE='player' ACTION='%s' ROOM=''>",
		       action);
	_net_send_line(net, "<PLAYER ID='%s' TYPE='guest' />", name);
	_net_send_line(net, "</UPDATE>");
	
	return 0;
}


int net_send_table_update(GGZNetIO *net, unsigned char opcode, GGZTable *table, int seat)
{
	char *action = NULL;

	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		action = "delete";
		break;
	case GGZ_UPDATE_ADD:
		action = "add";
		break;
	case GGZ_UPDATE_LEAVE:
		action = "leave";
		break;
	case GGZ_UPDATE_JOIN:
		action = "join";
		break;
	case GGZ_UPDATE_STATE:
		action = "status";
		break;
	}

	/* Always send opcode */
	_net_send_line(net, "<UPDATE TYPE='table' ACTION='%s' ROOM=''>",
		       action);

	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		net_send_table(net, table);
		break;
	case GGZ_UPDATE_ADD:
		net_send_table(net, table);
		break;
	case GGZ_UPDATE_LEAVE:
		_net_send_table_seat(net, table, seat);
		break;
	case GGZ_UPDATE_JOIN:
		_net_send_table_seat(net, table, seat);
		break;
	case GGZ_UPDATE_STATE:
		_net_send_table_status(net, table);
		break;
	}
	
	_net_send_line(net, "</UPDATE>");

	return 0;
}


int net_send_logout(GGZNetIO *net, char status)
{
	return _net_send_line(net, "</SESSION>");
}


int net_send_game_data(GGZNetIO *net, int size, char *data)
{
	int i;
	char buf[5];
	_net_send_string(net, "<DATA SIZE='%d'><![CDATA[", size);
	buf[0] = '\0';
	for (i = 0; i < size; i++) {
		sprintf(buf, "%d ", data[i]);
		write(net->fd, buf, strlen(buf));
	}
	_net_send_string(net, "]]></DATA>");

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

int _net_send_table_seat(GGZNetIO *net, GGZTable *table, int seat)
{
	_net_send_line(net, "<TABLE ID='%d' SEATS='%d'>", table->index, 
		       seats_num(table));
	_net_send_seat(net, table, seat);
	_net_send_line(net, "</TABLE>");
	
	return 0;
}


int _net_send_table_status(GGZNetIO *net, GGZTable *table)
{
	return _net_send_line(net, "<TABLE ID='%d' STATUS='%d' SEATS='%d'/>", 
			      table->index, table->state, seats_num(table));
}


int _net_send_seat(GGZNetIO *net, GGZTable *table, int num)
{
	int seat;
	char *type = NULL;
	char *name = NULL;

	seat = seats_type(table, num);
	switch (seat) {
	case GGZ_SEAT_OPEN:
		type = "open";
		break;
	case GGZ_SEAT_BOT:
		type = "bot";
		break;
	case GGZ_SEAT_RESV:
		type = "reserved";
		name = table->reserve[num];
		break;
	case GGZ_SEAT_PLAYER:
		type = "player";
		name = table->seats[num];
		break;
	}
	
	if (name)
		_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'>%s</SEAT>", 
			       num, type, name);
	else
		_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'/>", 
			       num, type);

	return 0;
}


static int _net_send_result(GGZNetIO *net, char *action, char code)
{
	return _net_send_line(net, "<RESULT ACTION='%s' CODE='%d'/>",
			      action, code);
}


static int _net_send_login_normal_status(GGZNetIO *net, char status)
{
	return _net_send_result(net, "login", status);
}


static int _net_send_login_anon_status(GGZNetIO *net, char status)
{
	return _net_send_result(net, "login", status);
}


static int _net_send_login_new_status(GGZNetIO *net, char status, char *password)
{
	/* Try to send login status */
	_net_send_line(net, "<RESULT ACTION='login' CODE='%d'>", status);
	
	/* Try to send checksum if successful */
	if (status == 0)
		_net_send_line(net, "<PASSWORD>%s</PASSWORD>", password);
	
	_net_send_line(net, "</RESULT>");
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


static int _net_send_line(GGZNetIO *net, char *line, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, line);
	vsprintf(buf, line, ap);
	va_end(ap);
	strcat(buf, "\n");
	return write(net->fd, buf, strlen(buf));
}


static int _net_send_string(GGZNetIO *net, char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	return write(net->fd, buf, strlen(buf));
}
