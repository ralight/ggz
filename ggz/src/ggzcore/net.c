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


#include <config.h>
#include <ggzcore.h>
#include <net.h>
#include <protocol.h>
#include <easysock.h>
#include <player.h>
#include <room.h>

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>


/* Handlers for various server commands */
static void _ggzcore_net_handle_server_id(void);
static void _ggzcore_net_handle_login_anon(void);
static void _ggzcore_net_handle_motd(void);
static void _ggzcore_net_handle_logout(void);
static void _ggzcore_net_handle_list_rooms(void);
static void _ggzcore_net_handle_room_join(void);
static void _ggzcore_net_handle_list_players(void);
static void _ggzcore_net_handle_rsp_chat(void);
static void _ggzcore_net_handle_chat(void);
static void _ggzcore_net_handle_update_players(void);
static void _ggzcore_net_handle_update_tables(void);

/* Error function for Easysock */
static void _ggzcore_net_err_func(const char *, const EsOpType, 
				  const EsDataType);
			 
/* Handler for GGZ_NET_ERROR event */
static void _ggzcore_net_handle_error(GGZEventID id, void* event_data, 
				      void* user_data);


/* Datatype for a message from the server */
struct _GGZServerMsg {

	/* Opcode from server */
	GGZServerOp opcode;

	/* Name of opcode */
	char* name;

	/* Handler */
	void (*handler)(void); 
};

/* Array of all server messages */
static struct _GGZServerMsg ggz_server_msgs[] = {
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
	{RSP_LOGIN,          "rsp_login", NULL},
	{RSP_LOGIN_ANON,     "rsp_login_anon", _ggzcore_net_handle_login_anon},
	{RSP_LOGOUT,         "rsp_logout", _ggzcore_net_handle_logout},
	{RSP_PREF_CHANGE,    "rsp_pref_change", NULL},
	{RSP_REMOVE_USER,    "rsp_remove_user", NULL},
	{RSP_LIST_PLAYERS,   "rsp_list_players", _ggzcore_net_handle_list_players},
	{RSP_LIST_TYPES,     "rsp_list_types", NULL},
	{RSP_LIST_TABLES,    "rsp_list_tables", NULL},
	{RSP_LIST_ROOMS,     "rsp_list_rooms", _ggzcore_net_handle_list_rooms},
	{RSP_TABLE_OPTIONS,  "rsp_table_options", NULL},
	{RSP_USER_STAT,      "rsp_user_stat", NULL},
	{RSP_TABLE_LAUNCH,   "rsp_table_launch", NULL},
	{RSP_TABLE_JOIN,     "rsp_table_join", NULL},
	{RSP_TABLE_LEAVE,    "rsp_table_leave", NULL},
	{RSP_GAME,           "rsp_game", NULL},
	{RSP_CHAT,           "rsp_chat", _ggzcore_net_handle_rsp_chat},
        {RSP_MOTD,           "rsp_motd", _ggzcore_net_handle_motd},
	{RSP_ROOM_JOIN,      "rsp_room_join", _ggzcore_net_handle_room_join}
};

/* Total number of recognized messages */
static unsigned int num_messages;

/* Socket for communication with server */
static int ggz_server_sock = -1;


/* FIXME: Handle threaded I/O */
void _ggzcore_net_init(void)
{
	/* Obtain accurate message count */
	num_messages = sizeof(ggz_server_msgs)/sizeof(struct _GGZServerMsg);

	/* Install socket error handler */
	es_err_func_set(_ggzcore_net_err_func);

	/* Register callback for network error event */
	ggzcore_event_add_callback(GGZ_NET_ERROR, _ggzcore_net_handle_error);
}


/* FIXME: set a timeout for connecting */
void _ggzcore_net_connect(const char* server, const unsigned int port)
{
	int sock;
	
	ggzcore_debug(GGZ_DBG_NET, "Connecting to %s:%d", server, port);
	if ( (sock = es_make_socket(ES_CLIENT, port, server)) >= 0)
		ggz_server_sock = sock;
	
	/* We don't need to handle failure here because it gets done
	   by the error func. registered with easysock */

	/* FIXME: somehow notify FE that we have a fd now */
}


void _ggzcore_net_disconnect(void)
{
	if (ggzcore_state_get_id() == GGZ_STATE_OFFLINE)
		return;

	ggzcore_debug(GGZ_DBG_NET, "Disconnecting");
	close(ggz_server_sock);
	ggz_server_sock = -1;
}


/* ggzcore_net_get_fd() - Get a copy of the network socket
 * Receives:
 *
 * Returns:
 * int : network socket fd
 *
 * Note: this is for detecting network data arrival only.  Do *NOT* attempt
 * to write to this fd.
 */
int ggzcore_net_get_fd(void)
{
	return ggz_server_sock;
}


int _ggzcore_net_ispending(void)
{
	int status = 0;
	struct pollfd fd[1] = {{ggz_server_sock, POLLIN, 0}};

	if (ggz_server_sock == -1)
		return 0;

	ggzcore_debug(GGZ_DBG_POLL, "Checking for net events");	
	if ( (status = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			status = 0;
		else 
			ggzcore_error_sys_exit("poll failed in _ggzcore_net_pending");
	}
	else if (status)
		ggzcore_debug(GGZ_DBG_POLL, "Found a net event!");

	return status;
}


int _ggzcore_net_process(void)
{
	GGZServerOp opcode;
	void (*handler)(void);

	ggzcore_debug(GGZ_DBG_NET, "Processing net events");
	if (es_read_int(ggz_server_sock, (int*)&opcode) < 0)
		return -1;
		
	if (opcode < 0 || opcode >= num_messages) {
		ggzcore_debug(GGZ_DBG_NET, "Bad opcode from server %d/%d", opcode, num_messages);
		ggzcore_event_enqueue(GGZ_SERVER_ERROR, NULL, NULL);
		return -1;
	}
	
	/* Call handler function */
	if (!(handler = ggz_server_msgs[opcode].handler))
		ggzcore_debug(GGZ_DBG_NET, "Server msg: %s not handled yet", 
			      ggz_server_msgs[opcode].name);
	else {
		ggzcore_debug(GGZ_DBG_NET, "Handling %s", 
			      ggz_server_msgs[opcode].name);
		handler();
	}
	
	return 0;
}


void _ggzcore_net_send_login(GGZLoginType type, const char* login, 
			     const char* pass)
{
	ggzcore_debug(GGZ_DBG_NET, "Executing net login");
	switch (type) {
	case GGZ_LOGIN:
		if (es_write_int(ggz_server_sock, REQ_LOGIN) == 0
		    && es_write_string(ggz_server_sock, login) == 0)
			es_write_string(ggz_server_sock, pass);
		break;
	case GGZ_LOGIN_GUEST:
		if (es_write_int(ggz_server_sock, REQ_LOGIN_ANON) == 0)
			es_write_string(ggz_server_sock, login);
		break;
	case GGZ_LOGIN_NEW:
		if (es_write_int(ggz_server_sock, REQ_LOGIN_NEW) == 0)
			es_write_string(ggz_server_sock, login);
		break;
	}
}


void _ggzcore_net_send_logout(void)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LOGOUT");	
	es_write_int(ggz_server_sock, REQ_LOGOUT);
}


void _ggzcore_net_send_motd(void)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_MOTD");	
	es_write_int(ggz_server_sock, REQ_MOTD);
}


void _ggzcore_net_send_list_rooms(const int type, const char verbose)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_ROOMS");	
	if (es_write_int(ggz_server_sock, REQ_LIST_ROOMS) == 0
	    && es_write_int(ggz_server_sock, type) == 0)
		es_write_char(ggz_server_sock, verbose);
}


void _ggzcore_net_send_join_room(const int room)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_ROOM_JOIN");	
	if (es_write_int(ggz_server_sock, REQ_ROOM_JOIN) == 0)
		es_write_int(ggz_server_sock, room);
}


void _ggzcore_net_send_list_players(void)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_PLAYERS");	
	es_write_int(ggz_server_sock, REQ_LIST_PLAYERS);
}


void _ggzcore_net_send_chat(const GGZChatOp op, const char* player, 
			    const char* msg)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_CHAT");	
	if (es_write_int(ggz_server_sock, REQ_CHAT) < 0
	    || es_write_char(ggz_server_sock, op) < 0)
		return;
	
	if (op & GGZ_CHAT_M_PLAYER)
		if (es_write_string(ggz_server_sock, player) < 0)
			return;
	
	if (op & GGZ_CHAT_M_MESSAGE)
		if (es_write_string(ggz_server_sock, msg) < 0)
			return;
}


static void _ggzcore_net_err_func(const char * msg, const EsOpType op, 
				  const EsDataType data)
{
	switch(op) {
	case ES_CREATE:
		ggzcore_event_enqueue(GGZ_SERVER_CONNECT_FAIL, (void*)msg, 
				      NULL);
		break;
	case ES_WRITE:
	case ES_READ:
		ggzcore_event_enqueue(GGZ_NET_ERROR, (void*)msg, NULL);
		ggz_server_sock = -1;
		break;
	default:
		break;
	}
}
			 

static void _ggzcore_net_handle_error(GGZEventID id, void* event_data, 
				      void* user_data)
{
	ggzcore_debug(GGZ_DBG_NET, "Network error: %s", (char*)event_data);
	_ggzcore_net_disconnect();
}


static void _ggzcore_net_handle_server_id(void)
{
	unsigned int size;
	unsigned int version;
	char* status;
	char msg[4096];

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_string(ggz_server_sock, (char*)&msg, 4096) == 0
	    && es_read_int(ggz_server_sock, &version) == 0
	    && es_read_int(ggz_server_sock, &size) == 0) {
		
		ggzcore_debug(GGZ_DBG_NET, "%s : proto %d: chat %d", msg, 
			      version, size);

		if (version == GGZ_CS_PROTO_VERSION)
			ggzcore_event_enqueue(GGZ_SERVER_CONNECT, NULL, NULL);
		else
			ggzcore_event_enqueue(GGZ_SERVER_CONNECT_FAIL, 
					      "Protocol mismatch", NULL);
	}
}


static void _ggzcore_net_handle_login_anon(void)
{
	char status;
	int checksum;

	if (es_read_char(ggz_server_sock, &status) < 0)
		return;
	
	if (status == 0 && es_read_int(ggz_server_sock, &checksum) < 0)
		return;
	
	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGIN_ANON from server : %d, %d", 
		      status, checksum);
	
	switch (status) {
	case 0:
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN, NULL, NULL);
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
}


static void _ggzcore_net_handle_motd(void)
{
	int i, lines;
	char **motd;

	/* FIXME: check for errors */
	if (es_read_int(ggz_server_sock, &lines) < 0)
		return;

	if (!(motd = calloc((lines+1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_handle_motd");
	
	for (i = 0; i < lines; i++)
		if (es_read_string_alloc(ggz_server_sock, &motd[i]) < 0)
			return;
	
	ggzcore_debug(GGZ_DBG_NET, "MSG_MOTD from server : %d lines", lines);

	/* FIXME: use argv free function */
	ggzcore_event_enqueue(GGZ_SERVER_MOTD, motd, NULL);
}


static void _ggzcore_net_handle_logout(void)
{
	char* status;

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_char(ggz_server_sock, status) < 0)
		return;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGOUT from server : %d", *status);
	ggzcore_event_enqueue(GGZ_SERVER_LOGOUT, status, free);
}


static void _ggzcore_net_handle_list_rooms(void)
{
	int i, num, id, game;
	char name[256];
	
	if (es_read_int(ggz_server_sock, &num) < 0)
		return;

	_ggzcore_room_list_clear();

	for (i = 0; i < num; i++) {
		if (es_read_int(ggz_server_sock, &id) < 0
		    || es_read_string(ggz_server_sock, name, sizeof(name)) < 0
		    || es_read_int(ggz_server_sock, &game) < 0)
			return;

		_ggzcore_room_list_add(id, name, game, NULL);
		ggzcore_debug(GGZ_DBG_NET, "Room: %d plays %d %s", id, game, 
			      name);
	}
	
	/* FIXME: read in information and actually pass back to client */
	ggzcore_event_enqueue(GGZ_SERVER_LIST_ROOMS, NULL, NULL);
}


static void _ggzcore_net_handle_room_join(void)
{
	char status;
	
	if (es_read_char(ggz_server_sock, &status) < 0)
		return;

	ggzcore_debug(GGZ_DBG_NET, "RSP_ROOM_JOIN from server : %d",
		      status);

	switch (status) {
	case 0:
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_JOIN, NULL, NULL);
		break;
	case E_AT_TABLE:
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_JOIN_FAIL, 
				      "Can't change rooms while at a table", 
				      NULL);
		break;
		
	case E_IN_TRANSIT:
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_JOIN_FAIL, 
				      "Can't change rooms while joining/leaving a table", 
				      NULL);
		break;
		
	case E_BAD_OPTIONS:
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_JOIN_FAIL, 
				      "Bad room number", NULL);
		break;
	}
}
		

static void _ggzcore_net_handle_list_players(void)
{
	int i, num, table;
	char name[256];
	
	if (es_read_int(ggz_server_sock, &num) < 0)
		return;

	_ggzcore_player_list_clear();

	for (i = 0; i < num; i++) {
		if (es_read_string(ggz_server_sock, name, sizeof(name)) < 0
		    || es_read_int(ggz_server_sock, &table) < 0)
			return;
		
		_ggzcore_player_list_add(name, table);
		ggzcore_debug(GGZ_DBG_NET, "%s at table %d", name, table);
	}
	
	/* FIXME: read in information and actually pass back to client */
	ggzcore_event_enqueue(GGZ_SERVER_LIST_PLAYERS, NULL, NULL);
}


static void _ggzcore_net_handle_rsp_chat(void)
{
	char status;
	
	if (es_read_char(ggz_server_sock, &status) < 0)
		return;

	ggzcore_debug(GGZ_DBG_NET, "RSP_CHAT from server : %d",
		      status);
	
	switch (status) {
	case 0:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT, NULL, NULL);
		break;

	case E_NOT_IN_ROOM:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_FAIL, "Not in a room",
				      NULL);
		break;

	case E_BAD_OPTIONS:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_FAIL, "Bad options",
				      NULL);
		break;
	}
}


static void _ggzcore_net_handle_chat(void)
{
	unsigned char subop;
	char** data;

	if (!(data = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_handle_chat");

	if (es_read_char(ggz_server_sock, &subop) < 0
	    || es_read_string_alloc(ggz_server_sock, &(data[0])) < 0)
		return;

	if (subop & GGZ_CHAT_M_MESSAGE) 
		if (es_read_string_alloc(ggz_server_sock, &(data[1])) < 0)
			return;

	switch((GGZChatOp)subop) {
	case GGZ_CHAT_NORMAL:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_MSG, data, NULL);
		break;
	case GGZ_CHAT_ANNOUNCE:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_ANNOUNCE, data, NULL);
		break;
	case GGZ_CHAT_PERSONAL:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_PRVMSG, data, NULL);
		break;
	case GGZ_CHAT_BEEP:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_BEEP, data, NULL);
		break;
	}

	/* FIXME: come up with a function to free data */
}


static void _ggzcore_net_handle_update_players(void)
{
	char subop;
	char *name;
	
	if (es_read_char(ggz_server_sock, &subop) < 0
	    || es_read_string_alloc(ggz_server_sock, &name) < 0)
		return;

	switch ((GGZUpdateOp)subop) {
	case GGZ_UPDATE_DELETE:
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_PLAYER: %s left", name);
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_LEAVE, name, free);
		_ggzcore_player_list_remove(name);
		break;

	case GGZ_UPDATE_ADD:
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_PLAYER: %s enter", name);
		ggzcore_event_enqueue(GGZ_SERVER_ROOM_ENTER, name, free);
		_ggzcore_player_list_add(name, -1);
		break;
	default:
		/* FIXME: handle invalid opcode? */
	}
}


static void _ggzcore_net_handle_update_tables(void)
{
	char subop, state;
	int table, room, type, num, i;
	char *player, *desc;
	GGZSeatType seat;

	if (es_read_char(ggz_server_sock, &subop) < 0
	    || es_read_int(ggz_server_sock, &table) < 0)
		return;

	/* FIXME: Do something with data */
	
	switch ((GGZUpdateOp)subop) {
	case GGZ_UPDATE_DELETE:
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: %d done", table);
		break;
		
	case GGZ_UPDATE_STATE:
		if (es_read_char(ggz_server_sock, &state) < 0)
			return;
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: %d new state %d", 
			      table, state);
		break;

	case GGZ_UPDATE_JOIN:
		if (es_read_int(ggz_server_sock, &num) < 0
		    || es_read_string_alloc(ggz_server_sock, &player) < 0)
			return;
		ggzcore_debug(GGZ_DBG_NET, 
			      "UPDATE_TABLE: %s joined seat %d at table %d", 
			      player, num, table);
		_ggzcore_player_list_replace(player, table);
		break;

	case GGZ_UPDATE_LEAVE:
		if (es_read_int(ggz_server_sock, &seat) < 0
		    || es_read_string_alloc(ggz_server_sock, &player) < 0)
			return;
		ggzcore_debug(GGZ_DBG_NET, 
			      "UPDATE_TABLE: %s left seat %d at table %d", 
			      player, seat, table);
		_ggzcore_player_list_replace(player, -1);
		break;
		
	case GGZ_UPDATE_ADD:
		es_read_int(ggz_server_sock, &room);
		es_read_int(ggz_server_sock, &type);
		es_read_string_alloc(ggz_server_sock, &desc);
		es_read_char(ggz_server_sock, &state);
		es_read_int(ggz_server_sock, &num);
		
		for (i = 0; i < num; i++) {
			es_read_int(ggz_server_sock, &seat);
			if (seat == GGZ_SEAT_PLAYER
			    || seat == GGZ_SEAT_RESV) {
				es_read_string_alloc(ggz_server_sock, &player);
			}
		}

		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: new table %d", 
			      table);
		break;
	}
}
