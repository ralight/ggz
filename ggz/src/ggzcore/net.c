/*
 * File: net.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for performing network I/O
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


#include "config.h"
#include "ggzcore.h"
#include "net.h"
#include "msg.h"
#include "protocol.h"
#include "easysock.h"
#include "player.h"
#include "room.h"
#include "state.h"
#include "table.h"
#include "gametype.h"

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>


/* GGZNet structure for handling the network connection to the server */
struct _GGZNet {
	
	/* Server structure handling this session */
	struct _GGZServer *server;
	
	/* Host name of server */
	char *host;

	/* Port on which GGZ server in running */
	unsigned int port;

	/* File descriptor for communication with server */
	int fd;

	/* Room to which we are transitioning */
	struct _GGZRoom *new_room;

	/* Room verbosity (need to save) */
	char room_verbose;

	/* Gametype verbosity (need to save) */
	char gametype_verbose;
};


/* Datatype for a message from the server */
struct _GGZServerMsg {

	/* Opcode from server */
	GGZServerOp opcode;

	/* Name of opcode */
	char* name;

	/* Handler */
	void (*handler)(struct _GGZNet *net); 
};


/* Handlers for various server commands */
static void _ggzcore_net_handle_server_id(struct _GGZNet *net);
static void _ggzcore_net_handle_login(struct _GGZNet *net);
static void _ggzcore_net_handle_login_anon(struct _GGZNet *net);
static void _ggzcore_net_handle_motd(struct _GGZNet *net);
static void _ggzcore_net_handle_logout(struct _GGZNet *net);
static void _ggzcore_net_handle_list_rooms(struct _GGZNet *net);
static void _ggzcore_net_handle_room_join(struct _GGZNet *net);
static void _ggzcore_net_handle_list_players(struct _GGZNet *net);
static void _ggzcore_net_handle_rsp_chat(struct _GGZNet *net);
static void _ggzcore_net_handle_chat(struct _GGZNet *net);
static void _ggzcore_net_handle_update_players(struct _GGZNet *net);
static void _ggzcore_net_handle_update_tables(struct _GGZNet *net);
static void _ggzcore_net_handle_list_tables(struct _GGZNet *net);
static void _ggzcore_net_handle_list_types(struct _GGZNet *net);
static void _ggzcore_net_handle_table_launch(struct _GGZNet *net);
static void _ggzcore_net_handle_table_join(struct _GGZNet *net);
static void _ggzcore_net_handle_table_leave(struct _GGZNet *net);
static void _ggzcore_net_handle_rsp_game(struct _GGZNet *net);
static struct _GGZTable* _ggzcore_net_handle_table(struct _GGZNet *net);


/* Useful(?) functions to do the data reading */
static int _ggzcore_net_read_opcode(const unsigned int fd, GGZServerOp *op);
static int _ggzcore_net_read_server_id(const unsigned int fd, int *protocol);
static int _ggzcore_net_read_login_anon(const unsigned int fd, char *status);
static int _ggzcore_net_read_motd(const unsigned int fd, int *lines, char ***buffer);
static int _ggzcore_net_read_logout(const unsigned int fd, char *status);
static int _ggzcore_net_read_num_rooms(const unsigned int fd, int *num);
static int _ggzcore_net_read_room(struct _GGZNet *net,
			   int *id,
			   char **name,
			   int *game,
			   char **desc);

static int _ggzcore_net_read_num_types(const unsigned int fd, int *num);
static int _ggzcore_net_read_type(const unsigned int fd, struct _GGZGameType *type);

static int _ggzcore_net_read_room_join(const unsigned int fd, char *status);
static int _ggzcore_net_read_num_players(const unsigned int fd, int *num);
static int _ggzcore_net_read_player(const unsigned int fd, char **name, int *table);

static int _ggzcore_net_read_num_tables(const unsigned int fd, int *num);
static int _ggzcore_net_read_table(const unsigned int fd, int *id, int *room, int *type, char **desc, char *state, int *seats);
static int _ggzcore_net_read_seat(const unsigned int fd, GGZSeatType *seat, char **name);

static int _ggzcore_net_read_chat(const unsigned int fd, 
			   GGZChatOp *op, 
			   char **name, 
			   char **msg);
static int _ggzcore_net_read_update_players(const unsigned int fd, 
				     GGZUpdateOp *op,
				     char **name);
static int _ggzcore_net_read_update_table_op(const unsigned int fd, GGZUpdateOp *op);
static int _ggzcore_net_read_table_id(const unsigned int fd, int *id);
static int _ggzcore_net_read_table_state(const unsigned int fd, int *id, char *state);
static int _ggzcore_net_read_table_seat(const unsigned int fd, int *id, int *seat, char **player);
static int _ggzcore_net_read_update_tables(const unsigned int fd, 
				    GGZUpdateOp *op,
				    int *table);

static int _ggzcore_net_read_table_launch(const unsigned int fd, char *status);
static int _ggzcore_net_read_table_join(const unsigned int fd, char *status);
static int _ggzcore_net_read_table_leave(const unsigned int fd, char *status);

static int _ggzcore_net_read_rsp_chat(const unsigned int fd, char *status);

static int _ggzcore_net_read_game_data(const unsigned int fd, int *size, char *buffer);


/* Lookup table of server msgs -> handler functions */
static struct _GGZServerMsg _ggzcore_server_msgs[] = {
	{MSG_SERVER_ID,      "msg_server_id", _ggzcore_net_handle_server_id},
	{MSG_SERVER_FULL,    "msg_server_full", NULL },
	{MSG_MOTD,           "msg_motd", _ggzcore_net_handle_motd},
	{MSG_CHAT,           "msg_chat", _ggzcore_net_handle_chat},
	{MSG_UPDATE_PLAYERS, "msg_update_players", _ggzcore_net_handle_update_players},
	{MSG_UPDATE_TYPES,   "msg_update_types", NULL},
	{MSG_UPDATE_TABLES,  "msg_update_tables", _ggzcore_net_handle_update_tables},
	{MSG_UPDATE_ROOMS,   "msg_update_rooms", NULL},
	{MSG_ERROR,          "msg_error", NULL},
	{RSP_LOGIN_NEW,      "rsp_login_new", NULL},
	{RSP_LOGIN,          "rsp_login", _ggzcore_net_handle_login},
	{RSP_LOGIN_ANON,     "rsp_login_anon", _ggzcore_net_handle_login_anon},
	{RSP_LOGOUT,         "rsp_logout", _ggzcore_net_handle_logout},
	{RSP_PREF_CHANGE,    "rsp_pref_change", NULL},
	{RSP_REMOVE_USER,    "rsp_remove_user", NULL},
	{RSP_LIST_PLAYERS,   "rsp_list_players", _ggzcore_net_handle_list_players},
	{RSP_LIST_TYPES,     "rsp_list_types", _ggzcore_net_handle_list_types},
	{RSP_LIST_TABLES,    "rsp_list_tables", _ggzcore_net_handle_list_tables},
	{RSP_LIST_ROOMS,     "rsp_list_rooms", _ggzcore_net_handle_list_rooms},
	{RSP_TABLE_OPTIONS,  "rsp_table_options", NULL},
	{RSP_USER_STAT,      "rsp_user_stat", NULL},
	{RSP_TABLE_LAUNCH,   "rsp_table_launch", _ggzcore_net_handle_table_launch},
	{RSP_TABLE_JOIN,     "rsp_table_join", _ggzcore_net_handle_table_join},
	{RSP_TABLE_LEAVE,    "rsp_table_leave", _ggzcore_net_handle_table_leave},
	{RSP_GAME,           "rsp_game", _ggzcore_net_handle_rsp_game},
	{RSP_CHAT,           "rsp_chat", _ggzcore_net_handle_rsp_chat},
        {RSP_MOTD,           "rsp_motd", _ggzcore_net_handle_motd},
	{RSP_ROOM_JOIN,      "rsp_room_join", _ggzcore_net_handle_room_join}
};


/* Total number of recognized messages */
static unsigned int _ggzcore_num_messages = sizeof(_ggzcore_server_msgs)/sizeof(struct _GGZServerMsg);


/* Internal library functions (prototypes in net.h) */

struct _GGZNet* _ggzcore_net_new(void)
{
	struct _GGZNet *net;

	net = ggzcore_malloc(sizeof(struct _GGZNet));
	
	/* Set fd to invalid value */
	net->fd = -1;
	
	return net;
}


void _ggzcore_net_init(struct _GGZNet *net, struct _GGZServer *server, const char *host, unsigned int port)
{
	net->server = server;
	net->host = ggzcore_strdup(host);
	net->port = port;
	net->fd = -1;
}


/* FIXME: set a timeout for connecting */
int _ggzcore_net_connect(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Connecting to %s:%d", net->host, net->port);
	net->fd = es_make_socket(ES_CLIENT, net->port, net->host);
	
	if (net->fd >= 0)
		return 0;  /* success */
	else
		return -1; /* error */
}


void _ggzcore_net_free(struct _GGZNet *net)
{
	if (net->host)
		ggzcore_free(net->host);
	ggzcore_free(net);
}


char* _ggzcore_net_get_host(struct _GGZNet *net)
{
	return net->host;
}


unsigned int _ggzcore_net_get_port(struct _GGZNet *net)
{
	return net->port;
}


int _ggzcore_net_get_fd(struct _GGZNet *net)
{
	return net->fd;
}


void _ggzcore_net_disconnect(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Disconnecting");
	close(net->fd);
	net->fd = -1;
}


int _ggzcore_net_send_login(struct _GGZNet *net, 
			     GGZLoginType type, 
			     const char* login, 
			     const char* pass)
{
	int status = 0;

	switch (type) {
	case GGZ_LOGIN:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN");
		if (es_write_int(net->fd, REQ_LOGIN) < 0
		    || es_write_string(net->fd, login) < 0
		    || es_write_string(net->fd, pass) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_GUEST:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_GUEST");
		if (es_write_int(net->fd, REQ_LOGIN_ANON) < 0
		    || es_write_string(net->fd, login) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_NEW:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_NEW");
		if (es_write_int(net->fd, REQ_LOGIN_NEW) < 0
		    || es_write_string(net->fd, login) < 0)
			status = -1;
		break;
	}

	return status;
}


int _ggzcore_net_send_logout(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LOGOUT");	
	return es_write_int(net->fd, REQ_LOGOUT);
}


int _ggzcore_net_send_motd(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_MOTD");	
	return es_write_int(net->fd, REQ_MOTD);
}


int _ggzcore_net_send_list_rooms(struct _GGZNet *net,
				 const int type, 
				 const char verbose)
{	
	int status = 0;

	net->room_verbose = verbose;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_ROOMS");	
	if (es_write_int(net->fd, REQ_LIST_ROOMS) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_char(net->fd, verbose) < 0)
		status = -1;

	return status;
}


int _ggzcore_net_send_list_types(struct _GGZNet *net, const char verbose)
{
	int status = 0;

	net->gametype_verbose = verbose;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_TYPES");	
	if (es_write_int(net->fd, REQ_LIST_TYPES) < 0
	    || es_write_char(net->fd, verbose) < 0)
		status = -1;

	return status;
}


int _ggzcore_net_send_join_room(struct _GGZNet *net, const unsigned int room_num)
{
	int status = 0;
	struct _GGZRoom *room;

	room = ggzcore_server_get_nth_room(net->server, room_num);

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_ROOM_JOIN");	
	if (es_write_int(net->fd, REQ_ROOM_JOIN) < 0
	    || es_write_int(net->fd, room_num) < 0)
		status = -1;
	
	net->new_room = room;
	return status;
}


int _ggzcore_net_send_list_players(struct _GGZNet *net)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_PLAYERS");	
	return es_write_int(net->fd, REQ_LIST_PLAYERS);
}


int _ggzcore_net_send_list_tables(struct _GGZNet *net, const int type, const char global)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_TABLES");	
	if (es_write_int(net->fd, REQ_LIST_TABLES) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_char(net->fd, global) < 0)
		return -1;

	return 0;
}


int _ggzcore_net_send_chat(struct _GGZNet *net, 
			   const GGZChatOp op, 
			   const char* player, 
			   const char* msg)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_CHAT");	
	if (es_write_int(net->fd, REQ_CHAT) < 0
	    || es_write_char(net->fd, op) < 0)
		return -1;
	
	if (op & GGZ_CHAT_M_PLAYER)
		if (es_write_string(net->fd, player) < 0)
			return -1;
	
	if (op & GGZ_CHAT_M_MESSAGE)
		if (es_write_string(net->fd, msg) < 0)
			return -1;

	return 0;
}


int _ggzcore_net_send_table_launch(struct _GGZNet *net,
				   const int type,
				   char *desc,
				   const int num_seats)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_LAUNCH");
	if (es_write_int(net->fd, REQ_TABLE_LAUNCH) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_string(net->fd, desc) < 0
	    || es_write_int(net->fd, num_seats) < 0)
		return -1;

	return 0;
}


int _ggzcore_net_send_table_join(struct _GGZNet *net, const unsigned int num)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_JOIN");
	if (es_write_int(net->fd, REQ_TABLE_JOIN) < 0
	    || es_write_int(net->fd, num) < 0)
		return -1;

	return 0;
}


int _ggzcore_net_send_table_leave(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_LEAVE");
	return es_write_int(net->fd, REQ_TABLE_LEAVE);
}


int _ggzcore_net_send_seat(struct _GGZNet *net, 
			   GGZSeatType seat, 
			   char *name)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending seat");
	if (es_write_int(net->fd, seat) < 0)
		return -1;

	switch (seat) {
	case GGZ_SEAT_PLAYER:
	case GGZ_SEAT_RESERVED:
		es_write_string(net->fd, name);
		break;
	case GGZ_SEAT_BOT:
		break;
	default:
		break;
	}
	
	return 0;
}


int _ggzcore_net_send_game_data(struct _GGZNet *net, int size, char *buffer)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_GAME: %d bytes from game", size);
	if (es_write_int(net->fd, REQ_GAME) < 0
	    || es_write_int(net->fd, size) < 0
	    || es_writen(net->fd, buffer, size) < 0)
		return -1;

	return 0;
}


int _ggzcore_net_data_is_pending(struct _GGZNet *net)
{
	int pending = 0;
	struct pollfd fd[1] = {{net->fd, POLLIN, 0}};

	if (net && net->fd != -1) {
	
	ggzcore_debug(GGZ_DBG_POLL, "Checking for net events");	
	if ( (pending = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			pending = 0;
		else 
			ggzcore_error_sys_exit("poll failed in ggzcore_server_data_is_pending");
	}
	else if (pending)
		ggzcore_debug(GGZ_DBG_POLL, "Found a net event!");
	}

	return pending;
}


int _ggzcore_net_read_data(struct _GGZNet *net)
{
	GGZServerOp opcode;
	void (*handler)(struct _GGZNet *net);
	int status;

	if (!net)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "Processing network events");
	status = _ggzcore_net_read_opcode(net->fd, &opcode);
	
	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return -1;
	}
	
	if (opcode < 0 || opcode >= _ggzcore_num_messages) {
		ggzcore_debug(GGZ_DBG_NET, "Bad opcode %d from server", 
			      opcode);
		_ggzcore_server_protocol_error(net->server, "Bad opcode");
		return -1;
	}
	
	/* Call handler function */
	if (!(handler = _ggzcore_server_msgs[opcode].handler))
		ggzcore_debug(GGZ_DBG_NET, "Server msg: %s not handled yet", 
			      _ggzcore_server_msgs[opcode].name);
	else {
		ggzcore_debug(GGZ_DBG_NET, "Handling %s", 
			      _ggzcore_server_msgs[opcode].name);
		handler(net);
	}
	
	return 0;
}


static int _ggzcore_net_read_opcode(const unsigned int fd, GGZServerOp *op)
{
	return es_read_int(fd, (int*)op);
}
		
		
static int _ggzcore_net_read_server_id(const unsigned int fd, int *protocol)
{
	unsigned int size;
	int status = 0;
	char msg[4096];

	if (es_read_string(fd, (char*)&msg, 4096) < 0
	    || es_read_int(fd, protocol) < 0
	    || es_read_int(fd, &size) < 0)
		status = -1;
	else 
		ggzcore_debug(GGZ_DBG_NET, "%s : proto %d: chat %d", msg, 
			      *protocol, size);
	
	return status;
}


static int _ggzcore_net_read_login(const unsigned int fd)
{
	char status, res;
	int checksum;

	if (es_read_char(fd, &status) < 0)
		return -1;
	
	if (status == 0 && es_read_int(fd, &checksum) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGIN from server : %d, %d", 
		      status, checksum);

	if (status == 0 && es_read_char(fd, &res) < 0)
		return -1;

	return 0;
}


static int _ggzcore_net_read_login_anon(const unsigned int fd, char *status)
{
	int checksum = 0;

	if (es_read_char(fd, status) < 0)
		return -1;
	
	if (*status == 0 && es_read_int(fd, &checksum) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGIN_ANON from server : %d, %d", 
		      *status, checksum);
	
	return 0;
}


static int _ggzcore_net_read_motd(const unsigned int fd, int *lines, char ***buffer)
{
	int i;
	char **motd;

	if (es_read_int(fd, lines) < 0)
		return -1;

	if (!(*buffer = calloc((*lines + 1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_read_motd");
	
	motd = *buffer;
	
	for (i = 0; i < *lines; i++) {
		if (es_read_string_alloc(fd, &motd[i]) < 0)
			return -1;
	}
	ggzcore_debug(GGZ_DBG_NET, "MSG_MOTD from server : %d lines", *lines);

	return 0;
}


static int _ggzcore_net_read_logout(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGOUT from server : %d", *status);
	
	return 0;
}


static int _ggzcore_net_read_num_rooms(const unsigned int fd, int *num)
{
	return es_read_int(fd, num);
}


static int _ggzcore_net_read_room(struct _GGZNet *net,
			   int *id,
			   char **name,
			   int *game,
			   char **desc)
{
	int fd;

	fd = net->fd;
	
	if (es_read_int(fd, id) < 0
	    || es_read_string_alloc(fd, name) < 0
	    || es_read_int(fd, game) < 0)
		return  -1;
	
	if (net->room_verbose && es_read_string_alloc(fd, desc) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "Room: %d (%s) plays %d", *id, *name, 
		      *game);

	return 0;
}


static int _ggzcore_net_read_num_types(const unsigned int fd, int *num)
{
	return es_read_int(fd, num);
}


static int _ggzcore_net_read_type(const unsigned int fd, struct _GGZGameType *type)
{
	int id;
	char players, bots;
	char *name, *version, *desc, *author, *url;

	if (es_read_int(fd, &id) < 0
	    || es_read_string_alloc(fd, &name) < 0
	    || es_read_string_alloc(fd, &version) < 0
	    || es_read_char(fd, &players) < 0
	    || es_read_char(fd, &bots) < 0
	    || es_read_string_alloc(fd, &desc) < 0
	    || es_read_string_alloc(fd, &author) < 0
	    || es_read_string_alloc(fd, &url) < 0)
		return -1;
	    
	ggzcore_debug(GGZ_DBG_NET, "Read info for game %d: %s", id, name);

	_ggzcore_gametype_init(type, id, name, version, players, bots, desc,
			       author, url);
	
	return 0;
}


static int _ggzcore_net_read_room_join(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_ROOM_JOIN from server : %d", *status);
	
	return 0;
}
		

static int _ggzcore_net_read_num_players(const unsigned int fd, int *num)
{
	return es_read_int(fd, num);
}


static int _ggzcore_net_read_player(const unsigned int fd, char **name, int *table)
{
	if (es_read_string_alloc(fd, name) < 0
	    || es_read_int(fd, table) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "Player %s at table %d", *name, *table);

	return 0;
}


static int _ggzcore_net_read_num_tables(const unsigned int fd, int *num)
{
	return es_read_int(fd, num);
}


static int _ggzcore_net_read_table(const unsigned int fd, int *id, int *room, int *type, char **desc, char *state, int *seats)
{
	if (es_read_int(fd, id) < 0
	    || es_read_int(fd, room) < 0
	    || es_read_int(fd, type) < 0
	    || es_read_string_alloc(fd, desc) < 0
	    || es_read_char(fd, state) < 0
	    || es_read_int(fd, seats) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "Read info for table %d", *id);

	return 0;
}


static int _ggzcore_net_read_seat(const unsigned int fd, GGZSeatType *seat, char **name)
{
	int type;

	if (es_read_int(fd, &type) < 0)
		return -1;

	*seat = type;

	switch (*seat) {
	case GGZ_SEAT_PLAYER:
	case GGZ_SEAT_RESERVED:
		es_read_string_alloc(fd, name);
		break;
	case GGZ_SEAT_BOT:
		break;
	default:
		break;
	}
	
	return 0;
}


static int _ggzcore_net_read_rsp_chat(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_CHAT from server : %d",
		      *status);
	
	return 0;
}


static int _ggzcore_net_read_chat(const unsigned int fd, GGZChatOp *op, char **name, 
			   char **msg)
{
	unsigned char opcode;

	if (es_read_char(fd, &opcode) < 0
	    || es_read_string_alloc(fd, name) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "Chat opcode = %d", opcode);	
	*op = opcode;

	if (opcode & GGZ_CHAT_M_MESSAGE) 
		if (es_read_string_alloc(fd, msg) < 0)
			return -1;

	return 0;
}


static int _ggzcore_net_read_update_players(const unsigned int fd, GGZUpdateOp *op,
				     char **name)
{
	unsigned char opcode;

	if (es_read_char(fd, &opcode) < 0
	    || es_read_string_alloc(fd, name) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "Update opcode = %d", opcode);
	*op = opcode;

	return 0;
}


static int _ggzcore_net_read_update_table_op(const unsigned int fd, GGZUpdateOp *op)
{
	unsigned char opcode;

	if (es_read_char(fd, &opcode) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "Update opcode = %d", opcode);
	*op = opcode;

	return 0;
}


static int _ggzcore_net_read_table_id(const unsigned int fd, int *id)
{
	return es_read_int(fd, id);
}


static int _ggzcore_net_read_table_state(const unsigned int fd, int *id, char *state)
{
	if (es_read_int(fd, id) < 0
	    || es_read_char(fd, state) < 0)
		return -1;

	return 0;
}

static int _ggzcore_net_read_table_seat(const unsigned int fd, int *id, int *seat, char **player)
{
	if (es_read_int(fd, id) < 0
	    || es_read_int(fd, seat) < 0
	    || es_read_string_alloc(fd, player) < 0)
		return -1;
	
	return 0;
}



static int _ggzcore_net_read_update_tables(const unsigned int fd, GGZUpdateOp *op,
				    int *table)
{
	unsigned char opcode;

	if (es_read_char(fd, &opcode) < 0
	    || es_read_int(fd, table) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "Update opcode = %d", opcode);
	*op = opcode;

	return 0;
}


static int _ggzcore_net_read_table_launch(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_TABLE_LAUNCH from server : %d", *status);
	
	return 0;
}


static int _ggzcore_net_read_table_join(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_TABLE_JOIN from server : %d", *status);
	
	return 0;
}


static int _ggzcore_net_read_table_leave(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_TABLE_LEAVE from server : %d", *status);
	
	return 0;
}


static int _ggzcore_net_read_game_data(const unsigned int fd, int *size, char *buffer)
{
	if (es_read_int(fd, size) < 0
	    || es_readn(fd, buffer, *size) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_GAME: %d bytes from server", *size);

	return 0;
}


static void _ggzcore_net_handle_server_id(struct _GGZNet *net)
{
	int status, protocol;

	status = _ggzcore_net_read_server_id(net->fd, &protocol);

	if (status < 0)
		_ggzcore_server_net_error(net->server, NULL);
	else if (protocol == GGZ_CS_PROTO_VERSION) {
		_ggzcore_server_change_state(net->server, GGZ_TRANS_CONN_OK);
		_ggzcore_server_event(net->server, GGZ_NEGOTIATED, NULL);
	}
	else {
		_ggzcore_server_change_state(net->server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(net->server, GGZ_NEGOTIATE_FAIL, "Protocol mismatch");
	}
}


static void _ggzcore_net_handle_login(struct _GGZNet *net)
{
#if 0	
	switch (status) {
	case 0:
		reservations = (int)res;
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN, (void*)reservations, NULL);
		break;
	case E_ALREADY_LOGGED_IN:
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN_FAIL, 
				      "Already logged in", NULL);
		break;
	case E_USR_LOOKUP:
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN_FAIL, 
				      "Name taken", NULL);
		break;
	}

#endif
}

static void _ggzcore_net_handle_login_anon(struct _GGZNet *net)
{
	char ok;
	int status;

	status = _ggzcore_net_read_login_anon(net->fd, &ok);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	ggzcore_debug(GGZ_DBG_SERVER, "Status of login: %d", ok);

	switch (ok) {
	case 0:
		_ggzcore_server_change_state(net->server, GGZ_TRANS_LOGIN_OK);
		_ggzcore_server_event(net->server, GGZ_LOGGED_IN, NULL);
		break;
	case E_ALREADY_LOGGED_IN:
		_ggzcore_server_change_state(net->server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(net->server, GGZ_LOGIN_FAIL, "Already logged in");
		break;
	case E_USR_LOOKUP:
		_ggzcore_server_change_state(net->server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(net->server, GGZ_LOGIN_FAIL, "Name taken");
		break;
	}
}


static void _ggzcore_net_handle_logout(struct _GGZNet *net)
{
	int status;
	char ok;

	status = _ggzcore_net_read_logout(net->fd, &ok);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	_ggzcore_net_disconnect(net);
	net->fd = -1;

	_ggzcore_server_change_state(net->server, GGZ_TRANS_LOGOUT_OK);
	_ggzcore_server_event(net->server, GGZ_LOGOUT, NULL);
}


static void _ggzcore_net_handle_motd(struct _GGZNet *net)
{
	int status, lines;
	char **motd;
	
	status = _ggzcore_net_read_motd(net->fd, &lines, &motd);
	
	if (status < 0)
		_ggzcore_server_net_error(net->server, NULL);
	else
		/* FIXME: store somewhere */
		_ggzcore_server_event(net->server, GGZ_MOTD_LOADED, motd);
}


 void _ggzcore_net_handle_list_rooms(struct _GGZNet *net)
{
	int i, num, id, game, status;
	char *name;
	char *desc = NULL;
	struct _GGZRoom *room;

	/* Clear existing list (if any) */
	if (ggzcore_server_get_num_rooms(net->server) > 0)
		_ggzcore_server_free_roomlist(net->server);

	status = _ggzcore_net_read_num_rooms(net->fd, &num);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	if (num < 0) {
		ggzcore_debug(GGZ_DBG_SERVER, "Error loading rooms");
		_ggzcore_server_protocol_error(net->server, "Error loading rooms");
		return;
	}

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d rooms", num);
	_ggzcore_server_init_roomlist(net->server, num);

	for (i = 0; i < num; i++) {
		status = _ggzcore_net_read_room(net, &id, &name, &game, &desc);
						
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}

		ggzcore_debug(GGZ_DBG_ROOM, "Adding room %d to room list", id);
		
		room = _ggzcore_room_new();
		_ggzcore_room_init(room, net->server, id, name, game, desc);
		_ggzcore_server_add_room(net->server, room);
		
		/* Free allocated memory */
		if (desc)
			free(desc);
		if (name)
			free(name);
	}
	_ggzcore_server_event(net->server, GGZ_ROOM_LIST, NULL);
}


static void _ggzcore_net_handle_list_types(struct _GGZNet *net)
{
	int i, num, status;
	struct _GGZGameType *type;
	
	/* Free previous list of types*/
	if (ggzcore_server_get_num_gametypes(net->server) > 0)
		_ggzcore_server_free_typelist(net->server);

	status = _ggzcore_net_read_num_types(net->fd, &num);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	if (num < 0) {
		ggzcore_debug(GGZ_DBG_SERVER, "Error loading gametypes");
		_ggzcore_server_protocol_error(net->server, "Error loading gametypes");
		return;
	}

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d types", num);
	_ggzcore_server_init_typelist(net->server, num);

	for(i = 0; i < num; i++) {
		type = _ggzcore_gametype_new();
		status = _ggzcore_net_read_type(net->fd, type);

		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}
		
		_ggzcore_server_add_type(net->server, type);
	}

	_ggzcore_server_event(net->server, GGZ_TYPE_LIST, NULL);
}


static void _ggzcore_net_handle_room_join(struct _GGZNet *net)
{
	char ok;
	int status;
	struct _GGZRoom *room;

	status = _ggzcore_net_read_room_join(net->fd, &ok);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	ggzcore_debug(GGZ_DBG_SERVER, "Status of room join: %d", ok);

	room = ggzcore_server_get_cur_room(net->server);
	switch (ok) {
	case 0:
		/* Stop monitoring old room and start monitoring new one */
		if (room)
			_ggzcore_room_set_monitor(room, 0);
		_ggzcore_room_set_monitor(net->new_room, 1);
		_ggzcore_server_set_room(net->server, net->new_room);
		_ggzcore_server_change_state(net->server, GGZ_TRANS_ENTER_OK);
		_ggzcore_server_event(net->server, GGZ_ENTERED, NULL);
		break;
	case E_AT_TABLE:
		net->new_room = room;
		_ggzcore_server_change_state(net->server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(net->server, GGZ_ENTER_FAIL,
				      "Can't change rooms while at a table");
		break;
		
	case E_IN_TRANSIT:
		net->new_room = room;
		_ggzcore_server_change_state(net->server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(net->server, GGZ_ENTER_FAIL,
				      "Can't change rooms while joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		net->new_room = room;
		_ggzcore_server_change_state(net->server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(net->server, GGZ_ENTER_FAIL, 
				      "Bad room number");
				      
		break;
	}
}


static void _ggzcore_net_handle_chat(struct _GGZNet *net)
{
	GGZChatOp op;
	char *name = NULL, *msg = NULL;
	int status;
	struct _GGZRoom *room;
	
	status = _ggzcore_net_read_chat(net->fd, &op, &name, &msg);

	room = ggzcore_server_get_cur_room(net->server);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	_ggzcore_room_add_chat(room, op, name, msg);
	
	if (name)
		free(name);
	
	if (msg)
		free(msg);
}


static void _ggzcore_net_handle_list_players(struct _GGZNet *net)
{
	int i, num, table, status;
	char *name;
	struct _ggzcore_list *list;
	struct _GGZPlayer *player;
	struct _GGZRoom *room;

	room = ggzcore_server_get_cur_room(net->server);

	/* New list of players */
	list = _ggzcore_list_create(_ggzcore_player_compare, NULL,
				    _ggzcore_player_destroy, 0);
	
	status = _ggzcore_net_read_num_players(net->fd, &num);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	if (num < 0) {
		ggzcore_debug(GGZ_DBG_SERVER, "Error loading players");
		_ggzcore_server_protocol_error(net->server, "Error loading players");
		return;
	}

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d players", num);


	for (i = 0; i < num; i++) {
		status = _ggzcore_net_read_player(net->fd, &name, &table);
		
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}

		player = _ggzcore_player_new();
		_ggzcore_player_init(player, name, room, table);
		_ggzcore_list_insert(list, player);
		
		if (name)
			free(name);
	}

	_ggzcore_room_set_player_list(room, num, list);
}


static void _ggzcore_net_handle_update_players(struct _GGZNet *net)
{
	GGZUpdateOp op;
	char *name;
	int status;
	struct _GGZRoom *room;

	/* Use current room (until server tells which one update is for) */
	room = ggzcore_server_get_cur_room(net->server);

	status = _ggzcore_net_read_update_players(net->fd, &op, &name);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	switch (op) {
	case GGZ_UPDATE_DELETE:
		ggzcore_debug(GGZ_DBG_SERVER, "UPDATE_PLAYER: %s left", name);
		_ggzcore_room_remove_player(room, name);
		break;

	case GGZ_UPDATE_ADD:
		ggzcore_debug(GGZ_DBG_SERVER, "UPDATE_PLAYER: %s enter", name);
		_ggzcore_room_add_player(room, name);
		break;
	default:
		ggzcore_debug(GGZ_DBG_SERVER, "Bad update opcode");
		_ggzcore_server_protocol_error(net->server, "Bad update opcode");
		break;
	}
	
	if (name)
		free(name);
}


static void _ggzcore_net_handle_list_tables(struct _GGZNet *net)
{
	int i, num, status;
	struct _ggzcore_list *list;
	struct _GGZTable *table;
	struct _GGZRoom *room;

	room = ggzcore_server_get_cur_room(net->server);
	
	list = _ggzcore_list_create(_ggzcore_table_compare, NULL,
				    _ggzcore_table_destroy, 0);

	status = _ggzcore_net_read_num_tables(net->fd, &num);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	if (num < 0) {
		ggzcore_debug(GGZ_DBG_SERVER, "Error loading tables");
		_ggzcore_server_protocol_error(net->server, "Error loading tables");
		return;
	}

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d tables", num);
	
	for (i = 0; i < num; i++) {
		table = _ggzcore_net_handle_table(net);
		if (table)
			_ggzcore_list_insert(list, table);
	}
	
	_ggzcore_room_set_table_list(room, num, list);
}


static void _ggzcore_net_handle_update_tables(struct _GGZNet *net)
{
	GGZUpdateOp op;
	struct _GGZTable *table = NULL;
	struct _GGZRoom *room;
	int status, seat;
	char state, *player;
	int id;

	/* Use current room (until server tells which one update is for) */
	room = ggzcore_server_get_cur_room(net->server);

	status = _ggzcore_net_read_update_table_op(net->fd, &op);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	
	switch (op) {
	case GGZ_UPDATE_DELETE:
		status = _ggzcore_net_read_table_id(net->fd, &id);
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}
		_ggzcore_room_remove_table(room, id);
		break;
		
	case GGZ_UPDATE_STATE:
		status = _ggzcore_net_read_table_state(net->fd, &id, &state);
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}
		_ggzcore_room_new_table_state(room, id, state);
		break;

	case GGZ_UPDATE_JOIN:
		status = _ggzcore_net_read_table_seat(net->fd, &id, &seat,
						      &player);
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}
		_ggzcore_room_player_join_table(room, id, player, seat);
		break;

	case GGZ_UPDATE_LEAVE:
		status = _ggzcore_net_read_table_seat(net->fd, &id, &seat,
						      &player);
		if (status < 0) {
			_ggzcore_server_net_error(net->server, NULL);
			return;
		}
		_ggzcore_room_player_leave_table(room, id, player, seat);
		break;
		
	case GGZ_UPDATE_ADD:
		table = _ggzcore_net_handle_table(net);
		if (table)
			_ggzcore_room_add_table(room, table);
		
		break;
	}
}


static void _ggzcore_net_handle_rsp_chat(struct _GGZNet *net)
{
	int status;
	char ok;

	status = _ggzcore_net_read_rsp_chat(net->fd, &ok);

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}

	switch (ok) {
	case 0: /* Do nothing if successful */
		break;

	case E_NOT_IN_ROOM:
		_ggzcore_server_event(net->server, GGZ_CHAT_FAIL, "Not in a room");
		break;

	case E_BAD_OPTIONS:
		_ggzcore_server_event(net->server, GGZ_CHAT_FAIL, "Bad options");
		break;
	}
}


static void _ggzcore_net_handle_rsp_game(struct _GGZNet *net)
{
	unsigned int size;
	int status;
	char buffer[4096];
	char *buf_offset;
	struct _GGZRoom *room;

	room = ggzcore_server_get_cur_room(net->server);

	/* Leave room for storing 'size' in the first buf_offset bytes */
	buf_offset = buffer + sizeof(size);
	status = _ggzcore_net_read_game_data(net->fd, &size, buf_offset);
	*(int*)buffer = size;

	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	_ggzcore_room_recv_game_data(room, buffer);
}


static void _ggzcore_net_handle_table_launch(struct _GGZNet *net)
{
	int net_status;
	char op_status;

	net_status = _ggzcore_net_read_table_launch(net->fd, &op_status);

	if (net_status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	switch (op_status) {
	case 0: /* Do nothing if successful */
		break; 
	}
}


static void _ggzcore_net_handle_table_join(struct _GGZNet *net)
{
	int net_status;
	char op_status;

	net_status = _ggzcore_net_read_table_join(net->fd, &op_status);

	if (net_status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	switch (op_status) {
	case 0: /* Do nothing if successful */
		break; 
	}
}


static void _ggzcore_net_handle_table_leave(struct _GGZNet *net)
{
	int net_status;
	char op_status;

	net_status = _ggzcore_net_read_table_leave(net->fd, &op_status);

	if (net_status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return;
	}
	
	switch (op_status) {
	case 0: 
		_ggzcore_server_event(net->server, GGZ_TABLE_LEFT, NULL);
		break; 
	}
}


static struct _GGZTable* _ggzcore_net_handle_table(struct _GGZNet *net)
{
	int i, status, id, room_num, type, num_seats;
	char state, *desc, *player;
	GGZSeatType seat;
	struct _GGZGameType *gametype;
	struct _GGZTable *table;

	table = _ggzcore_table_new();
	status = _ggzcore_net_read_table(net->fd, &id, &room_num, 
					 &type, &desc, &state, &num_seats);
					 
	if (status < 0) {
		_ggzcore_server_net_error(net->server, NULL);
		return NULL;
	}
	
	/* Get gametype if we have the list */
	if (ggzcore_server_get_num_gametypes(net->server) > 0)
		gametype = _ggzcore_server_get_nth_gametype(net->server, type);
	else
		gametype = NULL;

	_ggzcore_table_init(table, gametype, desc, num_seats, state, id);
	
	for (i = 0; i < num_seats; i++) {
		status = _ggzcore_net_read_seat(net->fd, &seat, &player);
		
		if (status < 0) {
			_ggzcore_table_free(table);
			_ggzcore_server_net_error(net->server, NULL);
			return NULL;
		}
		
		switch (seat) {
		case GGZ_SEAT_PLAYER:
			_ggzcore_table_add_player(table, player, i);
			break;
		case GGZ_SEAT_RESERVED:
			_ggzcore_table_add_reserved(table, player, i);
			break;
		case GGZ_SEAT_BOT:
			_ggzcore_table_add_bot(table, NULL, i);
			break;
		default:
			break;
		}
	}

	return table;
}


