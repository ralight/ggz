/*
 * File: room.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: room.c 6444 2004-12-11 19:06:32Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "game.h"
#include "ggzcore.h"
#include "hook.h"
#include "net.h"
#include "player.h"
#include "protocol.h"
#include "room.h"
#include "server.h"
#include "table.h"

/* Local functions */
static int _ggzcore_room_event_is_valid(GGZRoomEvent event);
static GGZHookReturn _ggzcore_room_event(struct _GGZRoom *room, 
					 GGZRoomEvent id, void *data);

/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_room_events)/sizeof(_ggzcore_room_events[0]);


/* Publicly exported functions */
GGZRoom* ggzcore_room_new(void)
{
	return _ggzcore_room_new();
}


/* Initialize room object */
int ggzcore_room_init(GGZRoom *room, const GGZServer *server,
		      const unsigned int id, const char *name,
		      const unsigned int game, const char *desc)
{
	if (room) {
		_ggzcore_room_init(room, server, id, name, game, desc, 0);
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


GGZServer *ggzcore_room_get_server(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_server(room);
	else
		return NULL;
}


int ggzcore_room_get_id(GGZRoom *room)
{
	if (room)
		return _ggzcore_room_get_id(room);
	else
		return -1;
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



GGZTable* ggzcore_room_get_table_by_id(GGZRoom *room, const unsigned int id)
{
	if (!room)
		return NULL;

	return _ggzcore_room_get_table_by_id(room, id);
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


int ggzcore_room_chat(struct _GGZRoom *room, const GGZChatType type,
		      const char *player, const char *msg)
{
	if (room && room->server) {
		/* FIXME: check validty of args */
		return _ggzcore_room_chat(room, type, player, msg);
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


int ggzcore_room_join_table(GGZRoom *room, const unsigned int num,
			    int spectator)
{
	if (!room
	    || !room->server
	    || !_ggzcore_server_get_cur_game(room->server))
		return -1;

	return _ggzcore_room_join_table(room, num, spectator);
}


int ggzcore_room_leave_table(GGZRoom *room, int force)
{
	if (room && room->server)
		return _ggzcore_room_leave_table(room, force);
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

	room = ggz_malloc(sizeof(struct _GGZRoom));

	return room;
}


void _ggzcore_room_init(struct _GGZRoom *room,
			const struct _GGZServer *server,
			const unsigned int id, 
			const char* name, 
			const unsigned int game, 
			const char* desc,
			const int player_count)
{
	int i;
	
	room->server = (struct _GGZServer*)server;
	room->id = id;
	room->game = game;
	room->name = ggz_strdup(name);
	room->desc = ggz_strdup(desc);
	room->player_count = player_count;
	
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
		ggz_free(room->name);

	if (room->desc)
		ggz_free(room->desc);

	if (room->players)
		ggz_list_free(room->players);

	if (room->tables)
		ggz_list_free(room->tables);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(room->event_hooks[i]);
	
	ggz_free(room);
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
	if (ggzcore_server_get_cur_room(room->server) == room) {
		return room->num_players;
	} else {
		return room->player_count;
	}
}


struct _GGZPlayer* _ggzcore_room_get_nth_player(struct _GGZRoom *room, 
						const unsigned int num)
{
	int i;
	GGZListEntry *cur;
	
	cur = ggz_list_head(room->players);
	for (i = 0; i < num; i++)
		cur = ggz_list_next(cur);
	
	return ggz_list_get_data(cur);
}


struct _GGZPlayer* _ggzcore_room_get_player_by_name(struct _GGZRoom *room, 
						    const char *name)
{
        GGZListEntry *entry;
	struct _GGZPlayer player, *found = NULL;

	if (room->players) {
		player.name = (char*)name;
		entry = ggz_list_search(room->players, &player);

		if (entry)
			found = ggz_list_get_data(entry);
	}
	
	return found;
}


unsigned int _ggzcore_room_get_num_tables(struct _GGZRoom *room)
{
	/* FIXME: we should let the list track this instead of doing it ourselves */
	return room->num_tables;
}


struct _GGZTable* _ggzcore_room_get_nth_table(struct _GGZRoom *room, 
					       const unsigned int num)
{
	int i;
	GGZListEntry *cur;
	
	cur = ggz_list_head(room->tables);
	for (i = 0; i < num; i++)
		cur = ggz_list_next(cur);
	
	return ggz_list_get_data(cur);
}


struct _GGZTable* _ggzcore_room_get_table_by_id(struct _GGZRoom *room, 
						 const unsigned int id)
{
	GGZListEntry *entry;
	struct _GGZTable table, *found = NULL;
	
	if (room->tables) {
		_ggzcore_table_set_id(&table, id);
		entry = ggz_list_search(room->tables, &table);
		
		if (entry)
			found =  ggz_list_get_data(entry);
	}

	return found;
}


void _ggzcore_room_set_player_list(struct _GGZRoom *room,
				   unsigned int count,
				   GGZList *list)
{
	/* Get rid of old list */
	ggz_list_free(room->players);

	room->num_players = count;
	room->player_count = count;
	room->players = list;

	_ggzcore_room_event(room, GGZ_PLAYER_LIST, &room->id);
}


void _ggzcore_room_set_players(struct _GGZRoom *room,
			       int players)
{
	room->player_count = players;
	if (room->player_count < 0) {
		/* Sanity check. */
		room->player_count = 0;
	}
	_ggzcore_room_event(room, GGZ_PLAYER_COUNT, &room->id);
}


void _ggzcore_room_set_table_list(struct _GGZRoom *room,
				  unsigned int count,
				  GGZList *list)
{
	GGZListEntry *cur;
	GGZTable *table;

	/* Get rid of old list */
	ggz_list_free(room->tables);

	room->num_tables = count;
	room->tables = list;

	/* Sanity check: make sure these tables point to us */
	for (cur = ggz_list_head(list); cur; cur = ggz_list_next(cur)) {
		table = ggz_list_get_data(cur);
		_ggzcore_table_set_room(table, room);
	}
	
	_ggzcore_room_event(room, GGZ_TABLE_LIST, NULL);
}


void _ggzcore_room_set_monitor(struct _GGZRoom *room, char monitor)
{
	room->monitor = monitor;

	/* If turning off monitoring, clear lists */
	if (!monitor) {
		room->num_players = 0;
		ggz_list_free(room->players);
		room->players = NULL;

		room->num_tables = 0;
		ggz_list_free(room->tables);
		room->tables = NULL;
	}
}


void _ggzcore_room_add_player(struct _GGZRoom *room, GGZPlayer *pdata,
			      int from_room)
{
	struct _GGZPlayer *player;
	GGZRoomChangeEventData data;

	ggz_debug(GGZCORE_DBG_ROOM, "Adding player %s", pdata->name);

	/* Create the list if it doesn't exist yet */
	if (!room->players)
		room->players = ggz_list_create(_ggzcore_player_compare, 
						    NULL,
						    _ggzcore_player_destroy, 0);

	/* Default new people in room to no table (-1) */
	player = _ggzcore_player_new();
	_ggzcore_player_init(player, pdata->name, pdata->room,
			     -1, pdata->type, pdata->lag);
	_ggzcore_player_init_stats(player,
				   pdata->wins,
				   pdata->losses,
				   pdata->ties,
				   pdata->forfeits,
				   pdata->rating,
				   pdata->ranking,
				   pdata->highscore);

	ggz_list_insert(room->players, player);
	room->num_players++;
	room->player_count = room->num_players;

	data.player_name = pdata->name;
	data.from_room = from_room;
	data.to_room = room->id;
	_ggzcore_room_event(room, GGZ_ROOM_ENTER, &data);

	if ((room = _ggzcore_server_get_room_by_id(room->server, from_room))) {
		_ggzcore_room_set_players(room, room->player_count - 1);
	}
}
			      

void _ggzcore_room_remove_player(struct _GGZRoom *room, char *name,
				 int to_room)
{
	struct _GGZPlayer player;
	GGZListEntry *entry;
	GGZRoomChangeEventData data;

	ggz_debug(GGZCORE_DBG_ROOM, "Removing player %s", name);

	/* Only try to delete if the list exists */
	if (room->players) {	
		player.name = name;
		entry = ggz_list_search(room->players, &player);
		if (entry) {
			ggz_list_delete_entry(room->players, entry);
			room->num_players--;
			room->player_count = room->num_players;

			data.player_name = name;
			data.from_room = room->id;
			data.to_room = to_room;
			
			_ggzcore_room_event(room, GGZ_ROOM_LEAVE, &data);
		}
	}

	if ((room = _ggzcore_server_get_room_by_id(room->server, to_room))) {
		_ggzcore_room_set_players(room, room->player_count + 1);
	}
}


void _ggzcore_room_set_player_lag(struct _GGZRoom *room, char *name, int lag)
{
	/* FIXME: This should be sending a player "class-based" event */
	struct _GGZPlayer *player;

	ggz_debug(GGZCORE_DBG_ROOM, "Setting lag to %d for %s", lag, name);

	if (room->players) {
		player = _ggzcore_room_get_player_by_name(room, name);
		if (player) /* make sure they're still in room */
		{
			_ggzcore_player_set_lag(player, lag);
			_ggzcore_room_event(room, GGZ_PLAYER_LAG, name);
		}
	}
}


void _ggzcore_room_set_player_stats(GGZRoom *room, GGZPlayer *pdata)
{
	/* FIXME: This should be sending a player "class-based" event */
	GGZPlayer *player;

	ggz_debug(GGZCORE_DBG_ROOM, "Setting stats for %s: %d-%d-%d",
		  pdata->name, pdata->wins, pdata->losses, pdata->ties);

	if (!room->players)
		return;

	player = _ggzcore_room_get_player_by_name(room, pdata->name);

	/* make sure they're still in room */
	if (!player)
		return;

	_ggzcore_player_init_stats(player,
				   pdata->wins,
				   pdata->losses,
				   pdata->ties,
				   pdata->forfeits,
				   pdata->rating,
				   pdata->ranking,
				   pdata->highscore);
	_ggzcore_room_event(room, GGZ_PLAYER_STATS, player->name);
}


void _ggzcore_room_add_table(struct _GGZRoom *room, struct _GGZTable *table)
{
	ggz_debug(GGZCORE_DBG_ROOM, "Adding table %d", 
		      _ggzcore_table_get_id(table));
	
	/* Set table to point to this room */
	_ggzcore_table_set_room(table, room);

	/* Create the list if it doesn't exist yet */
	if (!room->tables)
		room->tables = ggz_list_create(_ggzcore_table_compare, NULL,
					       _ggzcore_table_destroy, 0);
	
	ggz_list_insert(room->tables, table);
	room->num_tables++;
	_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
}


void _ggzcore_room_remove_table(struct _GGZRoom *room, const unsigned int id) 
{
	GGZListEntry *entry;
	struct _GGZTable table;

	ggz_debug(GGZCORE_DBG_ROOM, "Deleting table: %d", id);

	/* Only try to delete if the list exists */
	if (room->tables) {
		_ggzcore_table_set_id(&table, id);
		entry = ggz_list_search(room->tables, &table);
		if (entry) {
			ggz_list_delete_entry(room->tables, entry);
			room->num_tables--;
			_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
		}
	}
}


void _ggzcore_room_player_set_table(struct _GGZRoom *room,
				    const char * name, int table)
{
	struct _GGZPlayer *player;

	ggz_debug(GGZCORE_DBG_ROOM, "%s table is now %d", name, table);

	if (room->players) {
		/* make sure they're still in room */
		if ( (player = _ggzcore_room_get_player_by_name(room, name)))
			_ggzcore_player_set_table(player, table);
	}
	
	_ggzcore_room_event(room, GGZ_TABLE_UPDATE, NULL);
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
		       const GGZChatType type,
		       const char *player,
		       const char *msg)
{
	struct _GGZNet *net;

	net = _ggzcore_server_get_net(room->server);

	if (msg && strchr(msg, '\n')) {
		/* If the chat includes multiple lines, then we send each
		 * line as a separate chat item.  The implementation of
		 * this is a little inefficient, but I can't see a better way
		 * to do it (cleanly).  Also it is possible that we should
		 * treat \r and \r\n as breaks instead of just \n.
		 *
		 * Note that if you send a string like "\n\n", this code will
		 * simply send the empty string twice...and the server will
		 * ignore both of them.  So we might also want to check for
		 * empty strings and just not send them.
		 *
		 * As far as I can tell, this implementation is functionally
		 * equivalent to common IRC behavior.
		 */
		size_t len = strlen(msg);
		char text[len + 1];
		char *this = text, *newline;

		strncpy(text, msg, len);
		text[len] = '\0';

		while ((newline = strchr(this, '\n'))) {
			*newline = '\0';
			if (_ggzcore_net_send_chat(net, type,
						   player, this) < 0) {
				return -1;
			}
			this = newline + 1;
		}
		return _ggzcore_net_send_chat(net, type, player, this);
	} else {
		return _ggzcore_net_send_chat(net, type, player, msg);
	}
}


void _ggzcore_room_add_chat(struct _GGZRoom *room, GGZChatType type,
			    const char *name, const char *msg)
{
	GGZChatEventData data = { type : type,
				  sender : name,
				  message : msg };

	ggz_debug(GGZCORE_DBG_ROOM, "Chat (%s) from %s",
		  ggz_chattype_to_string(type), name);

	_ggzcore_room_event(room, GGZ_CHAT_EVENT, &data);
	if (type == GGZ_CHAT_TABLE) {
		GGZGame *game = _ggzcore_server_get_cur_game(room->server);

		_ggzcore_game_inform_chat(game, name, msg);
	}
}


void _ggzcore_room_set_table_launch_status(struct _GGZRoom *room, int status)
{
	_ggzcore_server_set_table_launch_status(room->server, status);

	if (status == E_OK) {
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCHED, NULL);
	} else {
		GGZErrorEventData error = {status: status};

		switch (status) {
		case E_BAD_OPTIONS:
			snprintf(error.message, sizeof(error.message),
				 "Bad option");
			break;
		case E_ROOM_FULL:
			snprintf(error.message, sizeof(error.message),
				 "The room has reached its table limit.");
			break;
		case E_LAUNCH_FAIL:
			snprintf(error.message, sizeof(error.message),
				 "Launch failed on server");
			break;
		case E_NOT_IN_ROOM:
			snprintf(error.message, sizeof(error.message),
				 "Not in a room");
			break;
		case E_AT_TABLE:
			snprintf(error.message, sizeof(error.message),
				 "Already at a table");
			break;
		case E_IN_TRANSIT:
			snprintf(error.message, sizeof(error.message),
				 "Already joining/leaving a table");
			break;
		case E_NO_PERMISSION:
			snprintf(error.message, sizeof(error.message),
				 "Insufficient permissions");
			break;
		default:
			snprintf(error.message, sizeof(error.message),
				 "Unknown launch failure");
			break;
		}
		_ggzcore_room_event(room, GGZ_TABLE_LAUNCH_FAIL, &error);
	}
}


/* OK, we've joined a table.  Deal with it. */
void _ggzcore_room_set_table_join(GGZRoom *room, int table_index)
{
	ggz_debug(GGZCORE_DBG_ROOM, "Player joined table %d.", table_index);
	_ggzcore_server_set_table_join_status(room->server, E_OK);
	_ggzcore_room_event(room, GGZ_TABLE_JOINED, &table_index);

	if (_ggzcore_server_get_cur_game(room->server) == NULL) {
		/* GGZd thinks we're at a table but we know there's no game
		 * client running.  Probably the game client exited during the
		 * connection process.  So, tell ggzd that we've left. */
		if (_ggzcore_room_leave_table(room, 1) < 0) {
			/* Uh-oh; this shouldn't happen. */
		}
	}
}
					   

void _ggzcore_room_set_table_join_status(struct _GGZRoom *room,
					 GGZClientReqError status)
{
	char buf[128];

	if (status != E_OK)
		_ggzcore_server_set_table_join_status(room->server, status);
	
	switch (status) {
	case E_OK:
		/* Do nothing if successful.  The join itself will
		   be handled separately.  See
		   _ggzcore_room_set_table_join. */
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
		
	case E_TABLE_FULL:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "The table is full "
				    "(or has reserved seats)");
		break;

	case E_NO_PERMISSION:
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL,
				    "You don't have enough "
				    "permissions to join this table.");
		break;

	default:
		snprintf(buf, sizeof(buf),
		         "Unknown join failure (status %d)",
		         status);
		_ggzcore_room_event(room, GGZ_TABLE_JOIN_FAIL, buf);
		break;

	}
}

void _ggzcore_room_set_table_leave(GGZRoom *room,
				   GGZLeaveType reason, const char *player)
{
	GGZTableLeaveEventData event_data = {reason: reason,
					     player: player};
	ggz_debug(GGZCORE_DBG_ROOM, "Player left table: %s (%s).",
		  ggz_leavetype_to_string(reason), player);
	_ggzcore_server_set_table_leave_status(room->server, E_OK);
	_ggzcore_room_event(room, GGZ_TABLE_LEFT, &event_data);
}

void _ggzcore_room_set_table_leave_status(struct _GGZRoom *room,
					  GGZClientReqError status)
{
	char buf[128];

	if (status != E_OK)
		_ggzcore_server_set_table_leave_status(room->server, status);

	switch (status) {
	case E_OK:
		/* Do nothing if successful.  The join itself will
		   be handled separately.  See
		   _ggzcore_room_set_table_leave. */
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
		snprintf(buf, sizeof(buf),
		         "Unknown leave failure (status %d)",
		         status);
		_ggzcore_room_event(room, GGZ_TABLE_LEAVE_FAIL, buf);
		break;
	}
}


void _ggzcore_room_table_event(struct _GGZRoom *room, GGZRoomEvent event, void *data)
{
	_ggzcore_room_event(room, event, data);
}


int _ggzcore_room_launch_table(struct _GGZRoom *room, struct _GGZTable *table)
{
	struct _GGZNet *net;
	int status;
	GGZGame *game = ggzcore_server_get_cur_game(room->server);

	/* Make sure we're actually in a room (FIXME: should probably
           make sure we're in *this* room) and not already playing a
           game */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_IN_ROOM
	    || !game)
		return -1;

	net = _ggzcore_server_get_net(room->server);
	status = _ggzcore_net_send_table_launch(net, table);
	
	if (status == 0) {
		_ggzcore_game_set_player(game, 0, -1);

		_ggzcore_server_set_table_launching(room->server);
	}
	
	return status;
}


int _ggzcore_room_join_table(struct _GGZRoom *room,
			     const unsigned int num,
			     int spectator)
{
	int status;
	struct _GGZNet *net;
	GGZGame *game = ggzcore_server_get_cur_game(room->server);
	GGZRoom *cur_room = _ggzcore_server_get_cur_room(room->server);
	GGZTable *table;

	/* Make sure we're actually in this room, and a game object
	   has been created, and that we know the table exists.
	   FIXME: make sure the game isn't already launched... */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_IN_ROOM
	    || !cur_room || room->id != cur_room->id
	    || !game
	    || !(table = _ggzcore_room_get_table_by_id(room, num)))
		return -1;

	net = _ggzcore_server_get_net(room->server);
	status = _ggzcore_net_send_table_join(net, num, spectator);

	if (status == 0) {
		_ggzcore_game_set_table(game, room->id, num);
		_ggzcore_game_set_player(game, spectator, -1);

		_ggzcore_server_set_table_joining(room->server);
	}

	return status;
}


int _ggzcore_room_leave_table(struct _GGZRoom *room, int force)
{
	int status;
	struct _GGZNet *net;
	GGZGame *game = ggzcore_server_get_cur_game(room->server);
	int spectating;

	/* Game may be NULL if the game client has already exited. */

	/* Make sure we're at a table. (FIXME: should probably make
           sure we're in *this* room) */
	if (_ggzcore_server_get_state(room->server) != GGZ_STATE_AT_TABLE) {
		return -1;
	}

	net = _ggzcore_server_get_net(room->server);
	if (game) {
		spectating = _ggzcore_game_is_spectator(game);
	} else {
		spectating = 0;
	}
	status = _ggzcore_net_send_table_leave(net, force, spectating);

	if (status == 0)
		_ggzcore_server_set_table_leaving(room->server);
	
	return status;
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
			   src->desc, src->player_count);
	
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
