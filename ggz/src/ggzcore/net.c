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

/* Handlers for various server commands */
#if 0
static void _ggzcore_net_read_list_players(const unsigned int fd);
static void _ggzcore_net_read_list_tables(const unsigned int fd);
static void _ggzcore_net_read_list_types(const unsigned int fd);

/* Error function for Easysock */
static void _ggzcore_net_err_func(const char *, const EsOpType, 
				  const EsDataType);
			 
#endif


void _ggzcore_net_init(void)
{
#if 0
	/* Install socket error handler */
	es_err_func_set(_ggzcore_net_err_func);
	
#endif
}


/* FIXME: set a timeout for connecting */
int _ggzcore_net_connect(const char* server, const unsigned int port)
{
	ggzcore_debug(GGZ_DBG_NET, "Connecting to %s:%d", server, port);
	return es_make_socket(ES_CLIENT, port, server);
}


void _ggzcore_net_disconnect(const unsigned int fd)
{
	ggzcore_debug(GGZ_DBG_NET, "Disconnecting");
	close(fd);
}


int _ggzcore_net_send_login(const unsigned int fd, 
			     GGZLoginType type, 
			     const char* login, 
			     const char* pass)
{
	int status = 0;

	switch (type) {
	case GGZ_LOGIN:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN");
		if (es_write_int(fd, REQ_LOGIN) < 0
		    || es_write_string(fd, login) < 0
		    || es_write_string(fd, pass) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_GUEST:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_GUEST");
		if (es_write_int(fd, REQ_LOGIN_ANON) < 0
		    || es_write_string(fd, login) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_NEW:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_NEW");
		if (es_write_int(fd, REQ_LOGIN_NEW) < 0
		    || es_write_string(fd, login) < 0)
			status = -1;
		break;
	}

	return status;
}


int _ggzcore_net_send_logout(const unsigned int fd)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LOGOUT");	
	return es_write_int(fd, REQ_LOGOUT);
}


int _ggzcore_net_send_motd(const unsigned int fd)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_MOTD");	
	return es_write_int(fd, REQ_MOTD);
}


int _ggzcore_net_send_list_rooms(const unsigned fd,
				 const int type, 
				 const char verbose)
{	
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_ROOMS");	
	if (es_write_int(fd, REQ_LIST_ROOMS) < 0
	    || es_write_int(fd, type) < 0
	    || es_write_char(fd, verbose) < 0)
		status = -1;

	return status;
}


int _ggzcore_net_send_join_room(const unsigned int fd, const int room)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_ROOM_JOIN");	
	if (es_write_int(fd, REQ_ROOM_JOIN) < 0
	    || es_write_int(fd, room) < 0)
		status = -1;
	
	return status;
}


int _ggzcore_net_send_list_players(const unsigned int fd)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_PLAYERS");	
	return es_write_int(fd, REQ_LIST_PLAYERS);
}


int _ggzcore_net_send_chat(const unsigned int fd, 
			   const GGZChatOp op, 
			   const char* player, 
			   const char* msg)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_CHAT");	
	if (es_write_int(fd, REQ_CHAT) < 0
	    || es_write_char(fd, op) < 0)
		return -1;
	
	if (op & GGZ_CHAT_M_PLAYER)
		if (es_write_string(fd, player) < 0)
			return -1;
	
	if (op & GGZ_CHAT_M_MESSAGE)
		if (es_write_string(fd, msg) < 0)
			return -1;

	return 0;
}

#if 0
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
		fd = -1;
		break;
	default:
		break;
	}
}
#endif
			 

int _ggzcore_net_read_opcode(const unsigned int fd, GGZServerOp *op)
{
	return es_read_int(fd, (int*)op);
}
		
		
int _ggzcore_net_read_server_id(const unsigned int fd, int *protocol)
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


int _ggzcore_net_read_login(const unsigned int fd)
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


int _ggzcore_net_read_login_anon(const unsigned int fd, char *status)
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


int _ggzcore_net_read_motd(const unsigned int fd, int *lines, char ***buffer)
{
	int i;
	char **motd = *buffer;

	/* FIXME: check for errors */
	if (es_read_int(fd, lines) < 0)
		return -1;

	if (!(motd = calloc((*lines + 1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_read_motd");
	
	for (i = 0; i < *lines; i++)
		if (es_read_string_alloc(fd, &motd[i]) < 0)
			return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "MSG_MOTD from server : %d lines", *lines);

	return 0;
}


int _ggzcore_net_read_logout(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGOUT from server : %d", *status);
	
	return 0;
}


int _ggzcore_net_read_num_rooms(const unsigned int fd, int *num)
{
	return es_read_int(fd, num);
}


int _ggzcore_net_read_room(const unsigned int fd, 
			   const char verbose,
			   int *id,
			   char **name,
			   int *game,
			   char **desc)
{
	
	if (es_read_int(fd, id) < 0
	    || es_read_string_alloc(fd, name) < 0
	    || es_read_int(fd, game) < 0)
		return  -1;
	
	/* _ggzcore_state.room_verbose */
	if (verbose && es_read_string_alloc(fd, desc) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_NET, "Room: %d (%s) plays %d", *id, *name, 
		      *game);

	return 0;
}


int _ggzcore_net_read_room_join(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_ROOM_JOIN from server : %d", *status);
	
	return 0;
}
		
#if 0
static void _ggzcore_net_read_list_players(const unsigned int fd)
{
	int i, num, table;
	char name[256];
	
	if (es_read_int(fd, &num) < 0)
		return;

	_ggzcore_player_list_clear();

	for (i = 0; i < num; i++) {
		if (es_read_string(fd, name, sizeof(name)) < 0
		    || es_read_int(fd, &table) < 0)
			return;
		
		_ggzcore_player_list_add(name, table);
		ggzcore_debug(GGZ_DBG_NET, "%s at table %d", name, table);
	}
	
	/* FIXME: read in information and actually pass back to client */
	ggzcore_event_enqueue(GGZ_SERVER_LIST_PLAYERS, NULL, NULL);
}
#endif

int _ggzcore_net_read_rsp_chat(const unsigned int fd, char *status)
{
	if (es_read_char(fd, status) < 0)
		return -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_CHAT from server : %d",
		      *status);
	
	return 0;
}


int _ggzcore_net_read_chat(const unsigned int fd, GGZChatOp *op, char **name, 
			   char **msg)
{
	unsigned char opcode;

	if (es_read_char(fd, &opcode) < 0
	    || es_read_string_alloc(fd, name) < 0)
		return -1;
	
	ggzcore_debug(GGZ_DBG_SERVER, "opcode = %d", opcode);	

	*op = opcode;

	if (opcode & GGZ_CHAT_M_MESSAGE) 
		if (es_read_string_alloc(fd, msg) < 0)
			return -1;

	return 0;
}


int _ggzcore_net_read_update_players(const unsigned int fd)
{
	char subop;
	char *name;
	
	if (es_read_char(fd, &subop) < 0
	    || es_read_string_alloc(fd, &name) < 0)
		return -1;

	return 0;

#if 0
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
#endif 

}


int _ggzcore_net_read_update_tables(const unsigned int fd)
{
	char subop, state;
	int table, room, type, num, i, open;
	char *player, *desc;
	GGZSeatType seat;

	if (es_read_char(fd, &subop) < 0
	    || es_read_int(fd, &table) < 0)
		return -1;

	
	switch ((GGZUpdateOp)subop) {
	case GGZ_UPDATE_DELETE:
#if 0
		_ggzcore_table_list_remove(table);
#endif
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: %d done", table);
		break;
		
	case GGZ_UPDATE_STATE:
		if (es_read_char(fd, &state) < 0)
			return -1;
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: %d new state %d", 
			      table, state);
		break;

	case GGZ_UPDATE_JOIN:
		if (es_read_int(fd, &num) < 0
		    || es_read_string_alloc(fd, &player) < 0)
			return -1;
#if 0
		_ggzcore_table_list_join(table);
		_ggzcore_player_list_replace(player, table);
#endif
		break;

	case GGZ_UPDATE_LEAVE:
		if (es_read_int(fd, &seat) < 0
		    || es_read_string_alloc(fd, &player) < 0)
			return -1;
#if 0
		_ggzcore_table_list_leave(table);
		_ggzcore_player_list_replace(player, -1);
#endif
		break;
		
	case GGZ_UPDATE_ADD:
		if (es_read_int(fd, &room) < 0
		    || es_read_int(fd, &type) < 0
		    || es_read_string_alloc(fd, &desc) < 0
		    || es_read_char(fd, &state) < 0
		    || es_read_int(fd, &num) < 0)
			return -1;
		
		open = 0;
		for (i = 0; i < num; i++) {
			if (es_read_int(fd, &seat) < 0)
				return -1;
			if (seat == GGZ_SEAT_PLAYER
			    || seat == GGZ_SEAT_RESV) {
				if (es_read_string_alloc(fd, &player) < 0)
					return -1;
			}
			if (seat == GGZ_SEAT_OPEN)
				open++;
		}

#if 0
		_ggzcore_table_list_add(table, type, desc, state, num, open);
#endif
		ggzcore_debug(GGZ_DBG_NET, "UPDATE_TABLE: new table %d", 
			      table);
		break;
	}

	ggzcore_event_enqueue(GGZ_SERVER_LIST_PLAYERS, NULL, NULL);
	ggzcore_event_enqueue(GGZ_SERVER_TABLE_UPDATE, NULL, NULL);

	return 0;
}

#if 0
static void _ggzcore_net_read_list_tables(const unsigned int fd)
{
	int total, id, room, type, num, i, x, open;
	char *player = NULL, *desc = NULL;
	char state;
	int seat;

	_ggzcore_table_list_clear();
		
	if (es_read_int(fd, &total) < 0)
		return;

	for (i = 0; i < total; i++) {
		es_read_int(fd, &id);
		es_read_int(fd, &room);
		es_read_int(fd, &type);
		es_read_string_alloc(fd, &desc);
		es_read_char(fd, &state);
		es_read_int(fd, &num);

		open = 0;
		for (x = 0; x < num; x++) {
			es_read_int(fd, &seat);
			if (seat == GGZ_SEAT_PLAYER
			    || seat == GGZ_SEAT_RESV) {
				es_read_string_alloc(fd, &player);
				free(player);
			}
			if (seat == GGZ_SEAT_OPEN)
				open++;
		}
		_ggzcore_table_list_add(id, type, desc, state, num, open);
		free(desc);
	}

	ggzcore_event_enqueue(GGZ_SERVER_TABLE_UPDATE, NULL, NULL);
}


static void _ggzcore_net_read_list_types(const unsigned int fd)
{
	int count, i;
	int id;
	char players, bots;
	char *name, *version, *desc, *author, *url;


	es_read_int(fd, &count);

	for(i = 0; i < count; i++)
	{
		es_read_int(fd, &id);
		es_read_string_alloc(fd, &name);
		es_read_string_alloc(fd, &version);
		es_read_char(fd, &players);
		es_read_char(fd, &bots);
		es_read_string_alloc(fd, &desc);
		es_read_string_alloc(fd, &author);
		es_read_string_alloc(fd, &url);
		_ggzcore_gametype_list_add(id, name, version, players,
					bots, desc, author, url);
	}
}
#endif
