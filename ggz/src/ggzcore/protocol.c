/*
 * File: protocol.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for handling GGZ client-server protocol
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

#include <stdlib.h>
#include <string.h>

#include "net.h"
#include "protocol.h"
#include "xmlelement.h"
#include "lists.h"
#include "room.h"
#include "table.h"
#include "gametype.h"


/* Game data structure */
typedef struct _GGZGameData {
	char *prot_engine;
	char *prot_version;
	GGZAllowed allow_players; 
	GGZAllowed allow_bots;  
	char *desc;
	char *author; 
	char *url;
} GGZGameData;	

/* Table data structure */
typedef struct _GGZTableData {
	char *desc;
	_ggzcore_list *seats;
} GGZTableData;


/* Seat data structure */
typedef struct _GGZSeatData {
	int index;
	char *type; 
	char *name;
} GGZSeatData;


/* Handler functions for various tags */
static void _ggzcore_protocol_server_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_options_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_motd_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_result_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_password_handle(GGZXMLElement*, GGZNet *net);
static void _ggzcore_protocol_list_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_list_insert(GGZXMLElement*, void*);
static void _ggzcore_protocol_update_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_game_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_game_set_protocol(GGZXMLElement*, char*, char *);
static void _ggzcore_protocol_game_set_allowed(GGZXMLElement*, GGZAllowed, GGZAllowed);
static void _ggzcore_protocol_game_set_info(GGZXMLElement*, char*, char *);
static void _ggzcore_protocol_game_set_desc(GGZXMLElement*, char*);
static void _ggzcore_protocol_protocol_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_allow_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_about_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_desc_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_room_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_player_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_table_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_table_add_seat(GGZXMLElement*, GGZSeatData*);
static void _ggzcore_protocol_table_set_desc(GGZXMLElement*, char*);
static GGZTableData* _ggzcore_protocol_tabledata_new(void);
static void _ggzcore_protocol_tabledata_free(GGZTableData*);
static void _ggzcore_protocol_seat_handle(GGZXMLElement*, GGZNet*);
static void* _ggzcore_protocol_seat_copy(void *data);
static void _ggzcore_protocol_seat_free(GGZSeatData*);
static void _ggzcore_protocol_chat_handle(GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_data_handle(GGZXMLElement*, GGZNet*);

/* Utility functions */
static int safe_atoi(char *string);


GGZXMLElement* _ggzcore_protocol_new_element(char *tag, char **attrs)
{
	void (*process_func)();

	/* FIXME: Could we do this with a table lookup? */
	if (strcmp(tag, "SERVER") == 0)
		process_func = _ggzcore_protocol_server_handle;
	else if (strcmp(tag, "OPTIONS") == 0)
		process_func = _ggzcore_protocol_options_handle;
	else if (strcmp(tag, "MOTD") == 0)
		process_func = _ggzcore_protocol_motd_handle;
	else if (strcmp(tag, "RESULT") == 0)
		process_func = _ggzcore_protocol_result_handle;
	else if (strcmp(tag, "LIST") == 0)
		process_func = _ggzcore_protocol_list_handle;
	else if (strcmp(tag, "UPDATE") == 0)
		process_func = _ggzcore_protocol_update_handle;
	else if (strcmp(tag, "GAME") == 0)
		process_func = _ggzcore_protocol_game_handle;
	else if (strcmp(tag, "PROTOCOL") == 0)
		process_func = _ggzcore_protocol_protocol_handle;
	else if (strcmp(tag, "ALLOW") == 0)
		process_func = _ggzcore_protocol_allow_handle;
	else if (strcmp(tag, "ABOUT") == 0)
		process_func = _ggzcore_protocol_about_handle;
	else if (strcmp(tag, "ROOM") == 0)
		process_func = _ggzcore_protocol_room_handle;
	else if (strcmp(tag, "PLAYER") == 0)
		process_func = _ggzcore_protocol_player_handle;
	else if (strcmp(tag, "TABLE") == 0)
		process_func = _ggzcore_protocol_table_handle;
	else if (strcmp(tag, "SEAT") == 0)
		process_func = _ggzcore_protocol_seat_handle;
	else if (strcmp(tag, "CHAT") == 0)
		process_func = _ggzcore_protocol_chat_handle;
	else if (strcmp(tag, "DATA") == 0)
		process_func = _ggzcore_protocol_data_handle;
	else if (strcmp(tag, "DESC") == 0)
		process_func = _ggzcore_protocol_desc_handle;
	else if (strcmp(tag, "PASSWORD") == 0)
		process_func = _ggzcore_protocol_password_handle;
	else
		process_func = NULL;
	
	return _ggzcore_xmlelement_new(tag, attrs, process_func, NULL);
}


void _ggzcore_protocol_process_element(GGZXMLElement *element, GGZNet *net)
{
	char *tag;
	
	if (element) {
		tag = _ggzcore_xmlelement_get_tag(element);
		
		ggzcore_debug(GGZ_DBG_XML, "Handling %s element", tag);

		if (element->process)
			element->process(element, net);
	}
}


/* Functions for <SERVER> tag */
static void _ggzcore_protocol_server_handle(GGZXMLElement *server, GGZNet *net)
{
	char *name, *id, *status;
	int version, chatlen;

	if (server) {
		name = _ggzcore_xmlelement_get_attr(server, "NAME");
		id = _ggzcore_xmlelement_get_attr(server, "ID");
		status = _ggzcore_xmlelement_get_attr(server, "STATUS");
		version = safe_atoi(_ggzcore_xmlelement_get_attr(server, "VERSION"));
		chatlen = (int)_ggzcore_xmlelement_get_data(server);

		ggzcore_debug(GGZ_DBG_NET, 
			      "%s(%s) : status %s: protocol %d: chat size %d", 
			      name, id, status, version, chatlen);

		_ggzcore_net_set_chat_size(net, chatlen);
		/* FIXME: Do something with name, status */
		if (version == GGZ_CS_PROTO_VERSION)
			_ggzcore_server_set_negotiate_status(_ggzcore_net_get_server(net), 0);
		else
			_ggzcore_server_set_negotiate_status(_ggzcore_net_get_server(net), -1);
	}
}


/* Functions for <OPTIONS> tag */
static void _ggzcore_protocol_options_handle(GGZXMLElement *options, GGZNet *net)
{
	int len;
	GGZXMLElement *server;

	/* Get parent off top of stack */
	server = _net_get_head(net);

	if (options && server) {
		/* Grab chat length out of options and store on server */
		len = safe_atoi(_ggzcore_xmlelement_get_attr(options, "CHATLEN"));
		_ggzcore_xmlelement_set_data(server, (void*)len);
	}
}


/* Functions for <MOTD> tag */
static void _ggzcore_protocol_motd_handle(GGZXMLElement *motd, GGZNet *net)
{
	char **buffer;
	char *message, *priority;

	message = _ggzcore_xmlelement_get_text(motd);
	priority = _ggzcore_xmlelement_get_attr(motd, "PRIORITY");
	
	ggzcore_debug(GGZ_DBG_NET, "Motd of priority %s", priority);

	if (!(buffer = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_read_motd");

	buffer[0] = message;
	_ggzcore_server_event(_ggzcore_net_get_server(net), GGZ_MOTD_LOADED, buffer);
}


/* Functions for <RESULT> tag */
static void _ggzcore_protocol_result_handle(GGZXMLElement *result, GGZNet *net)
{
	GGZServer *server;
	GGZRoom *room;
	char *action;
	int code;
	void *data;

	if (result) {
		
		action = _ggzcore_xmlelement_get_attr(result, "ACTION");
		code = safe_atoi(_ggzcore_xmlelement_get_attr(result, "CODE"));
		data = _ggzcore_xmlelement_get_data(result);
		
		ggzcore_debug(GGZ_DBG_NET, "Result of %s was %d", action, 
			      code);
		
		server = _ggzcore_net_get_server(net);
		room = _ggzcore_server_get_cur_room(server);

		if (strcmp(action, "login") == 0) {
			/* FIXME: we should check if login type is 'new' */
			if (code == 0 && data) {
				_ggzcore_server_set_password(server, data);
				ggzcore_free(data);
			}
			_ggzcore_server_set_login_status(server, code);
		}
		else if (strcmp(action, "enter") == 0) {
			if (code == 0)
				_ggzcore_server_set_room(server, _ggzcore_net_get_new_room(net));
			_ggzcore_server_set_room_join_status(server, code);
		}
		else if  (strcmp(action, "launch") == 0)
			_ggzcore_room_set_table_launch_status(room, code);
		else if  (strcmp(action, "join") == 0)
			_ggzcore_room_set_table_join_status(room, code);
		else if  (strcmp(action, "leave") == 0)
			_ggzcore_room_set_table_leave_status(room, code);
		else if  (strcmp(action, "chat") == 0) {
			switch (code) {
			case 0: /* Do nothing if successful */
				break;
			case E_NOT_IN_ROOM:
				_ggzcore_server_event(server, GGZ_CHAT_FAIL, "Not in a room");
				break;
				
			case E_BAD_OPTIONS:
				_ggzcore_server_event(server, GGZ_CHAT_FAIL, "Bad options");
				break;
			}
		}
	}
}


/* Functions for <PASSWORD> tag */
static void _ggzcore_protocol_password_handle(GGZXMLElement *element, GGZNet *net)
{
	char *password;
	char *parent_tag;
	GGZXMLElement *parent;
	
	/* Get parent off top of stack */
	parent = _net_get_head(net);
	
	if (element && parent) {
		password = _ggzcore_xmlelement_get_text(element);
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		/* It had better be RESULT.... */
		if (strcmp(parent_tag, "RESULT") == 0)
			_ggzcore_xmlelement_set_data(parent, ggzcore_strdup(password));
	}
}


/* Functions for <LIST> tag */
static void _ggzcore_protocol_list_handle(GGZXMLElement *list_tag, GGZNet *net)
{
	_ggzcore_list *list;
	_ggzcore_list_entry *entry;
	GGZServer *server;
	GGZRoom *room;
	int count, room_num;
	char *type;
	
	if (list_tag) {

		/* Grab list data from tag */
		type = _ggzcore_xmlelement_get_attr(list_tag, "TYPE");
		list = _ggzcore_xmlelement_get_data(list_tag);
		if (!_ggzcore_xmlelement_get_attr(list_tag, "ROOM"))
			room_num = -1;
		else
			room_num = safe_atoi(_ggzcore_xmlelement_get_attr(list_tag, "ROOM"));
		
		entry = _ggzcore_list_head(list);
		server = _ggzcore_net_get_server(net);

		/* Get length of list */
		/* FIXME: it would be nice if we could get this from the list itself */
		count = 0;
		while (entry) {
			entry = _ggzcore_list_next(entry);
			count++;
		}


		if (strcmp(type, "room") == 0) {
			/* Clear existing list (if any) */
			if (_ggzcore_server_get_num_rooms(server) > 0)
				_ggzcore_server_free_roomlist(server);

			_ggzcore_server_init_roomlist(server, count);

			entry = _ggzcore_list_head(list);
			while (entry) {
				_ggzcore_server_add_room(server, _ggzcore_list_get_data(entry));
				entry = _ggzcore_list_next(entry);
			}
			_ggzcore_server_event(server, GGZ_ROOM_LIST, NULL);
		}
		else if (strcmp(type, "game") == 0) {
			/* Free previous list of types*/
			if (ggzcore_server_get_num_gametypes(server) > 0)
				_ggzcore_server_free_typelist(server);

			_ggzcore_server_init_typelist(server, count);
			entry = _ggzcore_list_head(list);
			while (entry) {
				_ggzcore_server_add_type(server, _ggzcore_list_get_data(entry));
				entry = _ggzcore_list_next(entry);
			}
			_ggzcore_server_event(server, GGZ_TYPE_LIST, NULL);
		}
		else if (strcmp(type, "player") == 0) {
			room = _ggzcore_server_get_room_by_id(server, room_num);
			_ggzcore_room_set_player_list(room, count, list);
			list = NULL;
		}
		else if (strcmp(type, "table") == 0) {
			room = _ggzcore_server_get_room_by_id(server, room_num);
			_ggzcore_room_set_table_list(room, count, list);
			list = NULL;
		}

		if (list)
			_ggzcore_list_destroy(list);
	}
}


static void _ggzcore_protocol_list_insert(GGZXMLElement *list_tag, void *data)
{
	char *type;
	_ggzcore_list *list;
	_ggzcoreEntryCompare compare_func = NULL;
	_ggzcoreEntryCreate  create_func = NULL;
	_ggzcoreEntryDestroy destroy_func = NULL;

	type = _ggzcore_xmlelement_get_attr(list_tag, "TYPE");
	list = _ggzcore_xmlelement_get_data(list_tag);
	
	/* If list doesn't already exist, create it */
	if (!list) {
		/* Setup actual list */
		if (strcmp(type, "game") == 0) {}
		else if (strcmp(type, "room") == 0) {}
		else if (strcmp(type, "player") == 0) {
			compare_func = _ggzcore_player_compare;
			destroy_func = _ggzcore_player_destroy;
		}		
		else if (strcmp(type, "table") == 0) {
			compare_func = _ggzcore_table_compare;
			destroy_func = _ggzcore_table_destroy;
		}		
		list = _ggzcore_list_create(compare_func, 
					    create_func, 
					    destroy_func,
					    _GGZCORE_LIST_ALLOW_DUPS);
		
		_ggzcore_xmlelement_set_data(list_tag, list);
	}

	_ggzcore_list_insert(list, data);
}


/* Functions for <UPDATE> tag */
static void _ggzcore_protocol_update_handle(GGZXMLElement *update, GGZNet *net)
{
	int i, seats, room_num;
	char *name, *action, *type;
	GGZServer *server;
	GGZRoom *room;
	GGZPlayer *player;
	GGZTable *table;

	if (update) {

		/* Grab update data from tag */
		type = _ggzcore_xmlelement_get_attr(update, "TYPE");
		action = _ggzcore_xmlelement_get_attr(update, "ACTION");
		if (!_ggzcore_xmlelement_get_attr(update, "ROOM"))
			room_num = -1;
		else
			room_num = safe_atoi(_ggzcore_xmlelement_get_attr(update, "ROOM"));

		server = _ggzcore_net_get_server(net);

		if (strcmp(type, "room") == 0) {
			/* FIXME: implement this */
		}
		else if (strcmp(type, "game") == 0) {
			/* FIXME: implement this */
		}
		else if (strcmp(type, "player") == 0) {
			player = _ggzcore_xmlelement_get_data(update);
			room = _ggzcore_server_get_room_by_id(server, room_num);
			if (strcmp(action, "add") == 0)
				_ggzcore_room_add_player(room, player->name, 
							 player->type);
			else if (strcmp(action, "delete") == 0)
				_ggzcore_room_remove_player(room, player->name);
			
			ggzcore_free(player);
		}
		else if (strcmp(type, "table") == 0) {
			table = _ggzcore_xmlelement_get_data(update);
			room = _ggzcore_server_get_room_by_id(server, room_num);
			if (strcmp(action, "add") == 0) {
				_ggzcore_room_add_table(room, table);
				table = NULL;
			}
			else if (strcmp(action, "delete") == 0)
				_ggzcore_room_remove_table(room, table->id);

			else if (strcmp(action, "join") == 0) {
				/* FIXME: this is a lousy way of getting this info */
				seats = _ggzcore_table_get_num_seats(table);
				for (i = 0; i < seats; i++) {
					name = _ggzcore_table_get_nth_player_name(table, i);
					if (name) {
						_ggzcore_room_player_join_table(room, table->id, name, i);
						break;
					}
				}
			}
			else if (strcmp(action, "leave") == 0) {
				/* FIXME: this is a lousy way of getting this info */
				seats = _ggzcore_table_get_num_seats(table);
				for (i = 0; i < seats; i++) {
					name = _ggzcore_table_get_nth_player_name(table, i);
					if (name) {
						_ggzcore_room_player_leave_table(room, table->id, name, i);
						break;
					}
				}
			}
			else if (strcmp(action, "status") == 0) {
				_ggzcore_room_new_table_state(room, table->id, table->state);
			}
			
			if (table)
				_ggzcore_table_free(table);
		}

	}
}


/* Functions for <GAME> tag */
static void _ggzcore_protocol_game_handle(GGZXMLElement *game, GGZNet *net)
{
	GGZGameType *type;
	GGZGameData *data;
	GGZXMLElement *parent;
	char *parent_tag;
	int id;
	char *name, *version;
	char *prot_engine = NULL;
	char *prot_version = NULL;
	GGZAllowed allow_players = 0;
	GGZAllowed allow_bots = 0;
	char *desc = NULL;
	char *author = NULL;
	char *url = NULL;

	/* Get parent off top of stack */
	parent = _net_get_head(net);
	
	if (game && parent) {
		
		/* Get game data from tag */
		id = safe_atoi(_ggzcore_xmlelement_get_attr(game, "ID"));
		name = _ggzcore_xmlelement_get_attr(game, "NAME");
		version = _ggzcore_xmlelement_get_attr(game, "VERSION");
		data = _ggzcore_xmlelement_get_data(game);

		if (data) {
			prot_engine = data->prot_engine;
			prot_version = data->prot_version;
			allow_players = data->allow_players;
			allow_bots = data->allow_bots;
			desc = data->desc;
			author = data->author;
			url = data->url;
		}

		type = _ggzcore_gametype_new();
		_ggzcore_gametype_init(type, id, name, version, prot_engine,
				       prot_version, allow_players,
				       allow_bots,  desc, author, url);

		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_protocol_list_insert(parent, type);

		if (data) {
			if (data->prot_engine)
				ggzcore_free(data->prot_engine);
			if (data->prot_version)
				ggzcore_free(data->prot_version);
			if (data->author)
				ggzcore_free(data->author);
			if (data->url)
				ggzcore_free(data->url);
			if (data->desc)
				ggzcore_free(data->desc);
			
			ggzcore_free(data);
		}

	}
}


static void _ggzcore_protocol_game_set_protocol(GGZXMLElement *game, char *engine, char *version)
{
	struct _GGZGameData *data;
	
	data = _ggzcore_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggzcore_malloc(sizeof(struct _GGZGameData));
		_ggzcore_xmlelement_set_data(game, data);
	}
	
	data->prot_engine = engine;
	data->prot_version = version;
}


static void _ggzcore_protocol_game_set_allowed(GGZXMLElement *game, GGZAllowed players, GGZAllowed bots)
{
	struct _GGZGameData *data;
	
	data = _ggzcore_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggzcore_malloc(sizeof(struct _GGZGameData));
		_ggzcore_xmlelement_set_data(game, data);
	}
	
	data->allow_players = players;
	data->allow_bots = bots;
}


static void _ggzcore_protocol_game_set_info(GGZXMLElement *game, char *author, char *url)
{
	struct _GGZGameData *data;
	
	data = _ggzcore_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggzcore_malloc(sizeof(struct _GGZGameData));
		_ggzcore_xmlelement_set_data(game, data);
	}
	
	data->author = author;
	data->url = url;
}


static void _ggzcore_protocol_game_set_desc(GGZXMLElement *game, char *desc)
{
	struct _GGZGameData *data;
	
	data = _ggzcore_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggzcore_malloc(sizeof(struct _GGZGameData));
		_ggzcore_xmlelement_set_data(game, data);
	}
	
	data->desc = desc;
}


/* Functions for <PROTOCOL> tag */
static void _ggzcore_protocol_protocol_handle(GGZXMLElement *protocol, GGZNet *net)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (protocol && parent) {
		_ggzcore_protocol_game_set_protocol(parent,
						    ggzcore_strdup(_ggzcore_xmlelement_get_attr(protocol, "ENGINE")),
						    ggzcore_strdup(_ggzcore_xmlelement_get_attr(protocol, "VERSION")));
	}
}


/* Functions for <ALLOW> tag */
static void _ggzcore_protocol_allow_handle(GGZXMLElement *allow, GGZNet *net)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (allow && parent) {
		_ggzcore_protocol_game_set_allowed(parent, 
						   safe_atoi(_ggzcore_xmlelement_get_attr(allow, "PLAYERS")),
						   safe_atoi(_ggzcore_xmlelement_get_attr(allow, "BOTS")));

	}
}


/* Functions for <ABOUT> tag */
static void _ggzcore_protocol_about_handle(GGZXMLElement *about, GGZNet *net)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (about && parent) {
		_ggzcore_protocol_game_set_info(parent, 
						ggzcore_strdup(_ggzcore_xmlelement_get_attr(about, "AUTHOR")),
						ggzcore_strdup(_ggzcore_xmlelement_get_attr(about, "URL")));
	}
}


/* Functions for <DESC> tag */
static void _ggzcore_protocol_desc_handle(GGZXMLElement *element, GGZNet *net)
{
	char *desc;
	char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (element && parent) {
		desc = ggzcore_strdup(_ggzcore_xmlelement_get_text(element));
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "GAME") == 0)
			_ggzcore_protocol_game_set_desc(parent, desc);
		else if (strcmp(parent_tag, "ROOM") == 0)
			_ggzcore_xmlelement_set_data(parent, desc);
		else if (strcmp(parent_tag, "TABLE") == 0)
			_ggzcore_protocol_table_set_desc(parent, desc);
	}
}


/* Functions for <ROOM> tag */
static void _ggzcore_protocol_room_handle(GGZXMLElement *room, GGZNet *net)
{
	GGZRoom *ggz_room;
	char *parent_tag;
	int id, game;
	char *name, *desc;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (room && parent) {

		/* Grab data from tag */
		id = safe_atoi(_ggzcore_xmlelement_get_attr(room, "ID"));
		name = _ggzcore_xmlelement_get_attr(room, "NAME");
		game = safe_atoi(_ggzcore_xmlelement_get_attr(room, "GAME"));
		desc = _ggzcore_xmlelement_get_data(room);

		/* Set up GGZRoom object */
		ggz_room = _ggzcore_room_new();
		_ggzcore_room_init(ggz_room, _ggzcore_net_get_server(net), 
				   id, name, game, desc);

		/* Free description if present */
		if (desc)
			ggzcore_free(desc);

		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_protocol_list_insert(parent, ggz_room);
	}
}


/* Functions for <PLAYER> tag */
static void _ggzcore_protocol_player_handle(GGZXMLElement *player, GGZNet *net)
{
	GGZPlayer *ggz_player;
	GGZPlayerType type;
	char *parent_tag;
	GGZServer *server;
	GGZRoom *room;
	char *name, *str_type;
	int table;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (player && parent) {
		server = _ggzcore_net_get_server(net);
		room = ggzcore_server_get_cur_room(server);

		/* Grab player data from tag */
		str_type = _ggzcore_xmlelement_get_attr(player, "TYPE");
		name = _ggzcore_xmlelement_get_attr(player, "ID");
		table = safe_atoi(_ggzcore_xmlelement_get_attr(player, "TABLE"));

		if (strcmp(str_type, "normal") == 0)
			type = GGZ_PLAYER_NORMAL;
		else if (strcmp(str_type, "guest") == 0)
			type = GGZ_PLAYER_GUEST;
		else if (strcmp(str_type, "admin") == 0)
			type = GGZ_PLAYER_ADMIN;
		else
			type = GGZ_PLAYER_NONE;
		
		/* Set up GGZPlayer object */
		ggz_player = _ggzcore_player_new();
		_ggzcore_player_init(ggz_player,  name, room, table, type);

		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_protocol_list_insert(parent, ggz_player);
		else if (strcmp(parent_tag, "UPDATE") == 0)
			_ggzcore_xmlelement_set_data(parent, ggz_player);
	}
}


/* Functions for <TABLE> tag */
static void _ggzcore_protocol_table_handle(GGZXMLElement *table, GGZNet *net)
{
	char *parent_tag;
	GGZGameType *type;
	GGZServer *server;
	GGZSeatData *seat;
	GGZTableData *data;
	GGZTable *table_obj;
	_ggzcore_list *seats = NULL;
	_ggzcore_list_entry *entry;
	int id, game, status, num_seats;
	char *desc = NULL;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (table && parent) {
		parent_tag = _ggzcore_xmlelement_get_tag(parent);

		/* Grab table data from tag */

		id = safe_atoi(_ggzcore_xmlelement_get_attr(table, "ID"));
		game = safe_atoi(_ggzcore_xmlelement_get_attr(table, "GAME"));
		status = safe_atoi(_ggzcore_xmlelement_get_attr(table, "STATUS"));
		num_seats = safe_atoi(_ggzcore_xmlelement_get_attr(table, "SEATS"));
		data = _ggzcore_xmlelement_get_data(table);
		if (data) {
			desc = data->desc;
			seats = data->seats;
		}

		/* Create new table structure */
		table_obj = _ggzcore_table_new();
		server = _ggzcore_net_get_server(net);
		type = _ggzcore_server_get_type_by_id(server, game);
		_ggzcore_table_init(table_obj, type, desc, num_seats, status, 
				    id);
				    
		/* Add seats */
		entry = _ggzcore_list_head(seats);
		while (entry) {
			seat = _ggzcore_list_get_data(entry);
			if (strcmp(seat->type, "open") == 0) {
				/* Nothing to do: seats default to open */
			}
			else if (strcmp(seat->type, "bot") == 0) {
				_ggzcore_table_add_bot(table_obj, seat->name, seat->index);
			}
			else if (strcmp(seat->type, "player") == 0) {
				_ggzcore_table_add_player(table_obj, seat->name, seat->index);
			}
			else if (strcmp(seat->type, "reserved") == 0) {
				_ggzcore_table_add_reserved(table_obj, seat->name, seat->index);
			}
			entry = _ggzcore_list_next(entry);
		}
		
		if (strcmp(parent_tag, "LIST") == 0) {
			_ggzcore_protocol_list_insert(parent, table_obj);
		}
		else if (strcmp(parent_tag, "UPDATE") == 0) {
			_ggzcore_xmlelement_set_data(parent, table_obj);
		}

		if (data)
			_ggzcore_protocol_tabledata_free(data);
	}
}

static void _ggzcore_protocol_table_add_seat(GGZXMLElement *table, GGZSeatData *seat)
{
	struct _GGZTableData *data;
	
	data = _ggzcore_xmlelement_get_data(table);
	
	/* If data doesn't already exist, create it */
	if (!data) {
		data = _ggzcore_protocol_tabledata_new();
		_ggzcore_xmlelement_set_data(table, data);
	}

	_ggzcore_list_insert(data->seats, seat);
}


static void _ggzcore_protocol_table_set_desc(GGZXMLElement *table, char *desc)
{
	struct _GGZTableData *data;
	
	data = _ggzcore_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _ggzcore_protocol_tabledata_new();
		_ggzcore_xmlelement_set_data(table, data);
	}
	
	data->desc = desc;
}


static GGZTableData* _ggzcore_protocol_tabledata_new(void)
{
	struct _GGZTableData *data;

	data = ggzcore_malloc(sizeof(struct _GGZTableData));

	data->seats = _ggzcore_list_create(NULL, 
					   _ggzcore_protocol_seat_copy, 
					   (_ggzcoreEntryDestroy)_ggzcore_protocol_seat_free, 
					   _GGZCORE_LIST_ALLOW_DUPS);

	return data;
}


static void _ggzcore_protocol_tabledata_free(GGZTableData *data)
{
	if (data) {
		if (data->desc)
			ggzcore_free(data->desc);
		if (data->seats)
			_ggzcore_list_destroy(data->seats);
		ggzcore_free(data);
	}
}


/* Functions for <SEAT> tag */
static void _ggzcore_protocol_seat_handle(GGZXMLElement *seat, GGZNet *net)
{
	struct _GGZSeatData seat_obj;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = _net_get_head(net);

	if (seat && parent) {

		/* Get seat information out of tag */
		seat_obj.index = safe_atoi(_ggzcore_xmlelement_get_attr(seat, "NUM"));
		seat_obj.type = _ggzcore_xmlelement_get_attr(seat, "TYPE");
		seat_obj.name = _ggzcore_xmlelement_get_text(seat);
		_ggzcore_protocol_table_add_seat(parent, &seat_obj);
	}
}


static void* _ggzcore_protocol_seat_copy(void *data)
{
	struct _GGZSeatData *seat1, *seat2;

	seat1 = (GGZSeatData*)data;

	seat2 = ggzcore_malloc(sizeof(struct _GGZSeatData));

	seat2->index = seat1->index;
	seat2->type = ggzcore_strdup(seat1->type);
	seat2->name = ggzcore_strdup(seat1->name);

	return seat2;
}


static void _ggzcore_protocol_seat_free(GGZSeatData *seat)
{
	if (seat) {
		if (seat->type)
			ggzcore_free(seat->type);
		if (seat->name)
			ggzcore_free(seat->name);
		ggzcore_free(seat);
	}
}



/* Functions for <CHAT> tag */
static void _ggzcore_protocol_chat_handle(GGZXMLElement *chat, GGZNet *net)
{
	char *msg, *type, *from;
	GGZRoom *room;
	GGZChatOp op = 0;

	if (chat) {
		
		/* Grab chat data from tag */
		type = _ggzcore_xmlelement_get_attr(chat, "TYPE");
		from = _ggzcore_xmlelement_get_attr(chat, "FROM");
		msg = _ggzcore_xmlelement_get_text(chat);

		ggzcore_debug(GGZ_DBG_NET, "%s message from %s: '%s'", 
			      type, from, msg);	

		if (strcmp(type, "normal") == 0)
			op = GGZ_CHAT_NORMAL;
		else if (strcmp(type, "private") == 0)
			op = GGZ_CHAT_PERSONAL;
		else if (strcmp(type, "announce") == 0)
			op = GGZ_CHAT_ANNOUNCE;
		else if (strcmp(type, "beep") == 0)
			op = GGZ_CHAT_BEEP;
		
		room = ggzcore_server_get_cur_room(_ggzcore_net_get_server(net));
		_ggzcore_room_add_chat(room, op, from, msg);
	}
}


/* Functions for <DATA> tag */
static void _ggzcore_protocol_data_handle(GGZXMLElement *data, GGZNet *net)
{
	GGZServer *server;
	GGZRoom *room;
	int i, size;
	char buffer[4096 + sizeof(size)];
	char *buf_offset;
	char *msg;
	char *token;

	if (data) {

		/* Grab data from tag */
		size = safe_atoi(_ggzcore_xmlelement_get_attr(data, "SIZE"));
		msg = _ggzcore_xmlelement_get_text(data);
		
		server = _ggzcore_net_get_server(net);
		room = ggzcore_server_get_cur_room(server);
		
		/* Leave room for storing 'size' in the first buf_offset bytes */
		*(int*)buffer = size;
		buf_offset = buffer + sizeof(size);

		token = strtok(msg, " ");
		for (i = 0; i < size; i++) {
			buf_offset[i] = safe_atoi(token);
			token = strtok(NULL, " ");
		}
		/*memcpy(buf_offset, msg, data->size);*/

		_ggzcore_room_recv_game_data(room, buffer);
	}
}


static int safe_atoi(char *string)
{
	if (!string)
		return 0;
	else
		return atoi(string);
}
