
/*
 * File: room.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling rooms
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include "ggzcore.h"
#include "room.h"
#include "server.h"
#include "table.h"
#include "player.h"
#include "lists.h"
#include "hook.h"
#include "net.h"
#include "protocol.h"

#include <stdlib.h>
#include <string.h>

/* Local functions */
static int _ggzcore_room_event_is_valid(GGZRoomEvent event);
static GGZHookReturn _ggzcore_room_event(struct _GGZRoom *room, 
					 GGZRoomEvent id, void *data);

/* Array of GGZRoom messages */
static char* _ggzcore_room_events[] = {
	"GGZ_PLAYER_LIST",
	"GGZ_TABLE_LIST",
	"GGZ_CHAT",
	"GGZ_ANNOUNCE",
	"GGZ_PRVMSG",
	"GGZ_BEEP",
	"GGZ_ROOM_ENTER",
	"GGZ_ROOM_LEAVE",
	"GGZ_TABLE_UPDATE",
	"GGZ_TABLE_LAUNCHED",
	"GGZ_TABLE_LAUNCH_FAIL",
	"GGZ_TABLE_JOINED",
	"GGZ_TABLE_JOIN_FAIL",
	"GGZ_TABLE_LEFT",
	"GGZ_TABLE_LEAVE_FAIL",
	"GGZ_TABLE_DATA",
	"GGZ_PLAYER_LAG"
};

/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_room_events)/sizeof(_ggzcore_room_events[0]);


/*
 * The GGZRoom struct manages information about a particular 
 */
struct _GGZRoom {

	/* Server which this room is on */
	struct _GGZServer *server;

	/* Monitoring flag */
	char monitor;

	/* Room ID on the server */
	unsigned int id;

	/* Name of room */
	char *name;	
	
	/* Supported game type (ID on server) */
	unsigned int game;

	/* Room description */
	char *desc;

	/* Number of player */
	unsigned int num_players;

	/* List of players in the room */
	struct _ggzcore_list *players;

	/* Number of tables */
	unsigned int num_tables;

	/* List of tables in the room */
	struct _ggzcore_list *tables;
	
	/* Room events */
	GGZHookList *event_hooks[sizeof(_ggzcore_room_events)/sizeof(_ggzcore_room_events[0])];

};


/* Publicly exported functions */
GGZRoom* ggzcore_room_new(void)
{
	return _ggzcore_room_new();
}


/* Initialize room object */
int ggzcore_room_init(GGZRoom *room, const GGZServer *server, const unsigned int id, const char *name, const unsigned int game, const char *desc)
{
	if (room) {
		_ggzcore_room_init(room, server, id, name, game, desc);
		return 0;
	}
	else
		return -1;
}
		      

/* De-allocate room object and its children */
void ggzcore_room_free(GGZRoom *room)
{
	if (room)
		_ggzcore_room_free(room);
}


char* ggzcore_room_get_name(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_name(room);
	else
		return NULL;
}


char* ggzcore_room_get_desc(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_desc(room);
	else
		return NULL;
}


GGZGameType* ggzcore_room_get_gametype(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_game(room);
	else
		return NULL;
}


int ggzcore_room_get_num_players(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_num_players(room);
	else
		return -1;
}


int ggzcore_room_get_num_tables(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_num_tables(room);
	else
		return -1;
}


GGZPlayer* ggzcore_room_get_nth_player(GGZRoom *room, const unsigned int num)
{
	if (room && num < room->num_players)
		return _ggzcore_room_get_nth_player(room, num);
	else
		return NULL;
}


GGZTable* ggzcore_room_get_nth_table(GGZRoom *room, const unsigned int num)
{
	if (room && num < room->num_tables)
		return _ggzcore_room_get_nth_table(room, num);
	else
		return NULL;
}


/* Functions for manipulating hooks to GGZRoom events */
int ggzcore_room_add_event_hook(GGZRoom *room, const GGZRoomEvent event, const GGZHookFunc func)
{
	if (room && func && _ggzcore_room_event_is_valid(event))
		return _ggzcore_room_add_event_hook_full(room, event, func, NULL);
	else
		return -1;
}


int ggzcore_room_add_event_hook_full(GGZRoom *room, const GGZRoomEvent event, const GGZHookFunc func, void *data)
{
	if (room && func  && _ggzcore_room_event_is_valid(event))
		return _ggzcore_room_add_event_hook_full(room, event, func, data);
	else
		return -1;
}


int ggzcore_room_remove_event_hook(GGZRoom *room, const GGZRoomEvent event, const GGZHookFunc func)
{
	if (room && func && _ggzcore_room_event_is_valid(event))
		return _ggzcore_room_remove_event_hook(room, event, func);
	else
		return -1;
}


int ggzcore_room_remove_event_hook_id(GGZRoom *room, const GGZRoomEvent event, const unsigned int hook_id)
{
	if (room && _ggzcore_room_event_is_valid(event))
		return _ggzcore_room_remove_event_hook_id(room, event, hook_id);
	else
		return -1;
}


int ggzcore_room_list_players(GGZRoom *room)
{
	if (room && room->server)
		return _ggzcore_room_load_playerlist(room);
	else
		return -1;
}


int ggzcore_room_list_tables(GGZRoom *room, const int type, const char global)
{
	if (room && room->server)
		return _ggzcore_room_load_tablelist(room, type, global);
	else
		return -1;
}


int ggzcore_room_chat(struct _GGZRoom *room, const GGZChatOp opcode, const char *player, const char *msg)
{
	if (room && room->server) {
		/* FIXME: check validty of args */
		return _ggzcore_room_chat(room, opcode, player, msg);
	}
	else
		return -1;
}


int ggzcore_room_launch_table(GGZRoom *room, GGZTable *table)
{
	if (room && room->server && table)
		return _ggzcore_room_launch_table(room, table);
	
	else
		return -1;
}


int ggzcore_room_join_table(GGZRoom *room, const unsigned int num)
{
	if (room && room->server)
		return _ggzcore_room_join_table(room, num);
	else
		return -1;
}


int ggzcore_room_leave_table(GGZRoom *room)
{
	if (room && room->server)
		return _ggzcore_room_leave_table(room);
	else
		return -1;
}


int ggzcore_room_send_game_data(GGZRoom *room, char *buffer)
{
	if (room && buffer)
		return _ggzcore_room_send_game_data(room, buffer);
	else
		return -1;
}


/* 
 * Internal library functions (prototypes in room.h) 
 * NOTE:All of these functions assume valid inputs!
 */


/* Create a new room object for a given server*/
struct _GGZRoom* _ggzcore_room_new(void)
{
	struct _GGZRoom *room;

	room = ggzcore_malloc(sizeof(struct _GGZRoom));

	return room;
}


void _ggzcore_room_init(struct _GGZRoom *room,
			const struct _GGZServer *server,
			const unsigned int id, 
			const char* name, 
			const unsigned int game, 
			const char* desc)
{
	int i;
	
	room->server = (struct _GGZServer*)server;
	room->id = id;
	room->game = game;
	room->name = ggzcore_strdup(name);
	if(desc)
		room->desc = ggzcore_strdup(desc);
	
	/* FIXME: create player list? */
	/* FIXME: create table list? */

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		room->event_hooks[i] = _ggzcore_hook_list_init(i);
}


void _ggzcore_room_free(struct _GGZRoom *room)
{
	int i;
	
	if (room->name)
		ggzcore_free(room->name);

	if (room->desc)
		ggzcore_free(room->desc);

	if (room->players)
		_ggzcore_list_destroy(room->players);

	if (room->tables)
		_ggzcore_list_destroy(room->tables);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(room->event_hooks[i]);
	
	ggzcore_free(room);
}


/* Functions to retrieve room information */
struct _GGZServer* _ggzcore_room_get_server(struct _GGZRoom *room)
{
	return room->server;
}


unsigned int _ggzcore_room_get_id(struct _GGZRoom *room)
{
	return room->id;
}


char* _ggzcore_room_get_name(struct _GGZRoom *room)
{
	return room->name;
}


struct _GGZGameType* _ggzcore_room_get_game(struct _GGZRoom *room)
{
	return _ggzcore_server_get_type_by_id(room->server, room->game);
}


char* _ggzcore_room_get_desc(struct _GGZRoom *room)
{
	return room->desc;
}


unsigned int _ggzcore_room_get_num_players(struct _GGZRoom *room)
{
	return room->num_players;
}


struct _GGZPlayer* _ggzcore_room_get_nth_player(struct _GGZRoom *room, 
						const unsigned int num)
{
	int i;
	_ggzcore_list_entry *cur;
	
	cur = _ggzcore_list_head(room->players);
	for (i = 0; i < num; i++)
		cur = _ggzcore_list_next(cur);
	
	return _ggzcore_list_get_data(cur);
}


struct _GGZPlayer* _ggzcore_room_get_player_by_name(struct _GGZRoom *room, 
						    const char *name)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZPlayer player, *found = NULL;

	if (room->players) {
		_ggzcore_player_init(&player, name, room, -1, GGZ_PLAYER_NONE, 0);
		entry = _ggzcore_list_search(room->players, &player);

		if (entry)
			found = _ggzcore_list_get_data(entry);
	}
	
	return found;
}


unsigned int _ggzcore_room_get_num_tables(struct _GGZRoom *room)
{
	return room->num_tables;
}


struct _GGZTable* _ggzcore_room_get_nth_table(struct _GGZRoom *room, 
					       const unsigned int num)
{
	int i;
	_ggzcore_list_entry *cur;
	
	cur = _ggzcore_list_head(room->tables);
	for (i = 0; i < num; i++)
		cur = _ggzcore_list_next(cur);
	
	return _ggzcore_list_get_data(cur);
}


struct _GGZTable* _ggzcore_room_get_table_by_id(struct _GGZRoom *room, 
						 const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZTable table, *found = NULL;
	
	if (room->tables) {
		_ggzcore_table_set_id(&table, id);
		entry = _ggzcore_list_search(room->tables, &table);
		
		if (entry)
			found =  _ggzcore_list_get_data(entry);
	}

	return found;
}


void _ggzcore_room_set_player_list(struct _GGZRoom *room,
				   unsigned int count,
				   struct _ggzcore_list *list)
{
	/* Get rid of old list */
	_ggzcore_list_destroy(room->players);

	room->num_players = count;
	room->players = list;

	_ggzcore_room_event(room, GGZ_PLAYER_LIST, NULL);
}


void _ggzcore_room_set_table_list(struct _GGZRoom *room,
				  unsigned int count,
				  struct _ggzcore_list *list)
{
	/* Get rid of old list */
	_ggzcore_list_destroy(room->tables);

	room->num_tables = count;
	room->tables = list;

	_ggzcore_room_event(room, GGZ_TABLE_LIST, NULL);
}


void _ggzcore_room_set_monitor(struct _GGZRoom *room, char monitor)
{
	room->monitor = monitor;

	/* If turning off monitoring, clear lists */
	if (!monitor) {
		room->num_players = 0;
		_ggzcore_list_destroy(room->players);
		room->players = NULL;

		room->num_tables = 0;
		_ggzcore_list_destroy(room->tables);
		room->tables = NULL;
	}
}


void _ggzcore_room_add_player(struct _GGZRoom *room, char *name, GGZPlayerType type, int lag)
{
	struct _GGZPlayer *player;

	ggzcore_debug(GGZ_DBG_ROOM, "Adding player %s", name);

	/* Create the list if it doesn't exist yet */
	if (!room->players)
		room->players = _ggzcore_list_create(_ggzcore_player_compare, 
						    NULL,
						    _ggzcore_player_destroy, 0);

	/* Default new people in room to no table (-1) */
	player = _ggzcore_player_new();
	_ggzcore_player_init(player, name, room, -1, type, lag);
	_ggzcore_list_insert(room->players, player);
	room->num_players++;
	_ggzcore_room_event(room, GGZ_ROOM_ENTER, name);
}
			      

void _ggzcore_room_remove_player(struct _GGZRoom *room, char *name)
{
	struct _GGZPlayer player;
	struct _ggzcore_list_entry *entry;

	ggzcore_debug(GGZ_DBG_ROOM, "Removing player %s", name);

	/* Only try to delete if the list exists */
	if (room->players) {	
		/* Default to no table (-1) or type */
		_ggzcore_player_init(&player, name, room, -1, GGZ_PLAYER_NONE, 0);
		entry = _ggzcore_list_search(room->players, &player);
		if (entry) {
			_ggzcore_list_delete_entry(room->players, entry);
			room->num_players--;
			_ggzcore_room_event(room, GGZ_ROOM_LEAVE, name);
		}
	}
}


void _ggzcore_room_set_player_lag(struct _GGZRoom *room, char *name, int lag)
{
	/* FIXME: This should be sending a player "class-based" event */
	struct _GGZPlayer *player;

	ggzcore_debug(GGZ_DBG_ROOM, "Setting lag to %d for %s", lag, name);

	if (room->players) {
		player = _ggzcore_room_get_player_by_name(room, name);
		if (player) /* make sure they're still in room */
		{
			_ggzcore_player_set_lag(player, lag);
			_ggzcore_room_event(room, GGZ_PLAYER_LAG, name);
		}
	}
}


void _ggzcore_room_add_table(struct _GGZRoom *room, struct _GGZTable *table)
{
	ggzcore_debug(GGZ_DBG_ROOM, "Adding table %d", 
		      _ggzcore_table_get_id(table));

	/* Create the list if it doesn't exist yet */
	if (!room->tables)
		room->tables = _ggzcore_list_create(_ggzcore_table_compare, 
						    NULL,
						    _ggzcore_table_destroy, 0);

	_ggzcore_list_insert(room->tables, table);
	room->num_tables++;
	_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
}


void _ggzcore_room_remove_table(struct _GGZRoom *room, const unsigned int id) 
{
	struct _ggzcore_list_entry *entry;
	struct _GGZTable table;

	ggzcore_debug(GGZ_DBG_ROOM, "Deleting table: %d", id);

	/* Only try to delete if the list exists */
	if (room->tables) {
		_ggzcore_table_set_id(&table, id);
		entry = _ggzcore_list_search(room->tables, &table);
		if (entry) {
			_ggzcore_list_delete_entry(room->tables, entry);
			room->num_tables--;
			_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
		}
	}
}


void _ggzcore_room_player_join_table(struct _GGZRoom *room, 
				     const unsigned int id,
				     char *name,
				     const unsigned int seat)
{
	struct _GGZTable *table;
	struct _GGZPlayer *player;

	ggzcore_debug(GGZ_DBG_ROOM, "%s joining seat %d at table %d", name, 
		      seat, id);

	if (room->tables) {
		table = _ggzcore_room_get_table_by_id(room, id);
		_ggzcore_table_add_player(table, name, seat);
	
		if (room->players) {
			player = _ggzcore_room_get_player_by_name(room, name);
			if (player) /* make sure they're still in room */
				_ggzcore_player_set_table(player, id);
		}

		_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
	}
}


void _ggzcore_room_player_leave_table(struct _GGZRoom *room, 
				      const unsigned int id,
				      char *name,
				      const unsigned int seat)
{
	struct _GGZTable *table;
	struct _GGZPlayer *player;
		
	ggzcore_debug(GGZ_DBG_ROOM, "%s leaving seat %d at table %d", name, 
		      seat, id);

	if (room->tables) {
		table = _ggzcore_room_get_table_by_id(room, id);
		_ggzcore_table_remove_player(table, name);

		if (room->players) {
			player = _ggzcore_room_get_player_by_name(room, name);
			if (player) /* make sure they're still in room */
				_ggzcore_player_set_table(player, -1);
		}

		_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
	}
}


void _ggzcore_room_new_table_state(struct _GGZRoom *room, const unsigned int id, char state)
{
	struct _GGZTable *table;
			
	ggzcore_debug(GGZ_DBG_ROOM, "Table %d new state %d", id, state);
		      
	if (room->tables) {		      
		table = _ggzcore_room_get_table_by_id(room, id);
		_ggzcore_table_set_state(table, state);
		_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
	}
}


int _ggzcore_room_load_playerlist(struct _GGZRoom *room)
{
	struct _GGZNet *net;

	net = _ggzcore_server_get_net(room->server);
	return _ggzcore_net_send_list_players(net);
}


int _ggzcore_room_load_tablelist(struct _GGZRoom *room, const int type, const char global)
{
	struct _GGZNet *net;

	net = _ggzcore_server_get_net(room->server);
	return _ggzcore_net_send_list_tables(net, type, global);
}


int _ggzcore_room_chat(struct _GGZRoom *room,
		       const GGZChatOp opcode,
		       const char *player,
		       const char *msg)
{
	struct _GGZNet *net;

	net = _ggzcore_server_get_net(room->server);
	return _ggzcore_net_send_chat(net, opcode, player, msg);
}


void _ggzcore_room_add_chat(struct _GGZRoom *room, GGZChatOp op, char *name,
			    char *msg)
{
	char *data[2];

	data[0] = name;
	data[1] = msg;

	ggzcore_debug(GGZ_DBG_ROOM, "op = %d", op);

	switch(op) {
	case GGZ_CHAT_NORMAL:
		_ggzcore_room_event(room, GGZ_CHAT, data);
		break;
	case GGZ_CHAT_ANNOUNCE:
		_ggzcore_room_event(room, GGZ_ANNOUNCE, data);		
		break;
	case GGZ_CHAT_PERSONAL:
		_ggzcore_room_event(room, GGZ_PRVMSG, data);
		break;
	case GGZ_CHAT_BEEP:
		_ggzcore_room_event(room, GGZ_BEEP, data);
		break;
	}
}


void _ggzcore_room_set_table_launch_status(struct _GGZRoom *room, int status)
{
	_ggzcore_server_set_table_launch_status(room->server, status);
	
	switch (status) {
	case 0:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCHED, NULL);
		break;

	case E_NOT_IN_ROOM:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Not in a room");
		break;

	case E_LAUNCH_FAIL:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Launch failed on server");
		break;

	case E_AT_TABLE:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Already at a table");
		break;
		
	case E_IN_TRANSIT:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Already joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Bad option");
		break;

	default:
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL,
				    "Unknown launch failure");
		break;
	}
}
					   

void _ggzcore_room_set_table_join_status(struct _GGZRoom *room, int status)
{
	_ggzcore_server_set_table_join_status(room->server, status);
	
	switch (status) {
	case 0:
		_ggzcore_room_event(room, GGZ_TABLE_JOINED, NULL);
		break;

	case E_NOT_IN_ROOM:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "Not in a room");
		break;

	case E_AT_TABLE:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "Already at a table");
		break;
		
	case E_IN_TRANSIT:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "Already joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "Bad option");
		break;

	case E_NO_TABLE:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "No such table");
		break;

	default:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "Unknown join failure");
		break;

	}
}

					 
void _ggzcore_room_set_table_leave_status(struct _GGZRoom *room, int status)
{
	_ggzcore_server_set_table_leave_status(room->server, status);

	switch (status) {
	case 0:
		_ggzcore_room_event(room, GGZ_TABLE_LEFT, NULL);
		break;

	case E_NOT_IN_ROOM:
		_ggzcore_room_event(room, GGZ_TABLE_LEAVE_FAIL,
				    "Not at a table");
		break;

	case E_NO_TABLE:
		_ggzcore_room_event(room, GGZ_TABLE_LEAVE_FAIL,
				    "No such table");
		break;
		
	case E_LEAVE_FORBIDDEN:
		_ggzcore_room_event(room, GGZ_TABLE_LEAVE_FAIL,
				    "Cannot leave games of this type");
		break;

	default:
		_ggzcore_room_event(room, GGZ_TABLE_LEAVE_FAIL,
				    "Unknown leave failure");
		break;
	}
}
					  

int _ggzcore_room_launch_table(struct _GGZRoom *room, struct _GGZTable *table)
{
	struct _GGZNet *net;
	int status;

	/* Make sure we're actually in a room (FIXME: should probably
           make sure we're in *this* room) and not already playing a
           game */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_IN_ROOM)
		return -1;

	net = _ggzcore_server_get_net(room->server);
	status = _ggzcore_net_send_table_launch(net, table);
	
	if (status == 0)
		_ggzcore_server_set_table_launching(room->server);
	
	return status;
}


int _ggzcore_room_join_table(struct _GGZRoom *room, const unsigned int num)
{
	int status;
	struct _GGZNet *net;

	/* Make sure we're actually in a room (FIXME: should probably
           make sure we're in *this* room) and not already playing a
           game */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_IN_ROOM)
		return -1;


	net = _ggzcore_server_get_net(room->server);
	status = _ggzcore_net_send_table_join(net, num);

	if (status == 0)
		_ggzcore_server_set_table_joining(room->server);
	
	return status;
}


int _ggzcore_room_leave_table(struct _GGZRoom *room)
{
	int status;
	struct _GGZNet *net;

	/* Make sure we're at a table (FIXME: should probably make
           sure we're in *this* room) */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_AT_TABLE)
		return -1;

	net = _ggzcore_server_get_net(room->server);
	status = _ggzcore_net_send_table_leave(net);

	if (status == 0)
		_ggzcore_server_set_table_leaving(room->server);
	
	return status;
}


int _ggzcore_room_send_game_data(struct _GGZRoom *room, char *buffer)
{
	int size;
	char *buf_offset;
	struct _GGZNet *net;

	/* Extract size from first bytes of buffer */
	size = *(int*)buffer;
	buf_offset = buffer + sizeof(size);

	net = _ggzcore_server_get_net(room->server);
	return _ggzcore_net_send_game_data(net, size, buf_offset);
}


void _ggzcore_room_recv_game_data(struct _GGZRoom *room, char *buffer)
{
	_ggzcore_room_event(room, GGZ_TABLE_DATA, buffer);
}




/* Functions for attaching hooks to GGZRoom events */
int _ggzcore_room_add_event_hook_full(GGZRoom *room,
				      const GGZRoomEvent event, 
				      const GGZHookFunc func,
				      void *data)
{
	return _ggzcore_hook_add_full(room->event_hooks[event], func, data);
}


/* Functions for removing hooks from GGZRoom events */
int _ggzcore_room_remove_event_hook(GGZRoom *room,
				    const GGZRoomEvent event, 
				    const GGZHookFunc func) 
{
	return _ggzcore_hook_remove(room->event_hooks[event], func);
}


int _ggzcore_room_remove_event_hook_id(GGZRoom *room,
				       const GGZRoomEvent event, 
				       const unsigned int hook_id) 
{
	return _ggzcore_hook_remove_id(room->event_hooks[event], hook_id);
}


/* Return 0 if equal and -1 otherwise */
int _ggzcore_room_compare(void* p, void* q)
{
	if (((struct _GGZRoom*)p)->id == ((struct _GGZRoom*)q)->id)
		return 0;

	return -1;
}


/* Create a copy of a room object */
void* _ggzcore_room_copy(void* p)
{
	struct _GGZRoom *new, *src = p;

	new = _ggzcore_room_new();
	_ggzcore_room_init(new, src->server, src->id, src->name, src->game,
			   src->desc);
	
	return (void*)new;
}


void  _ggzcore_room_destroy(void* p)
{
	_ggzcore_room_free(p);
}




/* Static functions internal to this file */

static int _ggzcore_room_event_is_valid(GGZRoomEvent event)
{
	return (event >= 0 && event < _ggzcore_num_events);
}


static GGZHookReturn _ggzcore_room_event(GGZRoom *room, GGZRoomEvent id, 
					 void *data)
{
	return _ggzcore_hook_list_invoke(room->event_hooks[id], data);
}
