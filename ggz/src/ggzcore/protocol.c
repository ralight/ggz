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


/* Structure for <SERVER> tag */
typedef struct _GGZServerTag {

	/* The GGZXMLElement structure needs to be the first element
	 *  in the structure in order for the object mechanism to work
	 *  correctly. This allows a message pointer to be cast to a
	 *  GGZXMLElement pointer.  */
	GGZXMLElement element;

	char *id;
	char *name;
	int version;
	char *status;
	int chatlen;
} GGZServerTag;

/* Structure for <OPTIONS> tag. Strictly speaking, we don't need a
   full structure for this, but I'm planning for future expansion and
   more options --Brent */
typedef struct _GGZOptionsTag {
	GGZXMLElement element;
	int chatlen;
} GGZOptionsTag;

/* Structure for <MOTD> tag */
typedef struct _GGZMotdTag {
	GGZXMLElement element;
	char *priority;
} GGZMotdTag;

/* Structure for <RESULT> tag.  FIXME: Perhaps we should put a
   reference to its contents here? */
typedef struct _GGZResultTag {
	GGZXMLElement element;
	char *action;
	int code;
	void *data;
} GGZResultTag;

/* Structure for <LIST> tag */
typedef struct _GGZListTag {
	GGZXMLElement element;
	char *type;
	int room;
	_ggzcore_list *list;
} GGZListTag;

/* Structure for <UPDATE> tag */
typedef struct _GGZUpdateTag {
	GGZXMLElement element;
	char *type;
	char *action;
	int room;
	void *data;
} GGZUpdateTag;

/* Structure for <GAME> tag */
typedef struct _GGZGameTag {
	GGZXMLElement element;
	unsigned int id;
	char *name; 
	char *version;
	char *prot_engine;
	char *prot_version;
	GGZAllowed allow_players; 
	GGZAllowed allow_bots;  
	char *desc;
	char *author; 
	char *url;
} GGZGameTag;

/* Structure for <PROTOCOL> tag */
typedef struct _GGZProtocolTag {
	GGZXMLElement element;
	char *engine;
	char *version;
} GGZProtocolTag;

/* Structure for <ALLOW> tag */
typedef struct _GGZAllowTag {
	GGZXMLElement element;
	unsigned char players;
	unsigned char bots;
} GGZAllowTag;

/* Structure for <ABOUT> tag */
typedef struct _GGZAboutTag {
	GGZXMLElement element;
	char *author;
	char *url;
} GGZAboutTag;

/* Structure for <ROOM> tag */
typedef struct _GGZRoomTag {
	GGZXMLElement element;
	unsigned int id; 
	char *name; 
	unsigned int game; 
	char *desc;
} GGZRoomTag;

/* Structure for <PLAYER> tag */
typedef struct _GGZPlayerTag {
	GGZXMLElement element;
	char *id;
	char *type; 
	int table; 
} GGZPlayerTag;

/* Structure for <TABLE> tag */
typedef struct _GGZTableTag {
	GGZXMLElement element;
	int id;
	int game;
	int status;
	int num_seats;
	char *desc;
	_ggzcore_list *seats;
} GGZTableTag;

/* Structure for <SEAT> tag */
typedef struct _GGZSeatTag {
	GGZXMLElement element;
	int index;
	char *type; 
	char *name;
} GGZSeatTag;

/* Structure for <CHAT> tag */
typedef struct _GGZChatTag {
	GGZXMLElement element;
	char *type;
	char *from;
	char *msg;
} GGZChatTag;

/* Structure for <DATA> tag */
typedef struct _GGZDataTag {
	GGZXMLElement element;
	int size;
} GGZDataTag;



/* Functions for <SERVER> tag */
static GGZServerTag* _ggzcore_protocol_server_new(char **attrs);
static void _ggzcore_protocol_server_process(GGZServerTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_server_free(GGZServerTag*);

/* Functions for <OPTIONS> tag */
static GGZOptionsTag* _ggzcore_protocol_options_new(char **attrs);
static void _ggzcore_protocol_options_process(GGZOptionsTag*, GGZServerTag*, GGZNet*);
static void _ggzcore_protocol_options_free(GGZOptionsTag *);

/* Functions for <MOTD> tag */
static GGZMotdTag* _ggzcore_protocol_motd_new(char **attrs);
static void _ggzcore_protocol_motd_process(GGZMotdTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_motd_free(GGZMotdTag *);

/* Functions for <RESULT> tag */
static GGZResultTag* _ggzcore_protocol_result_new(char **attrs);
static void _ggzcore_protocol_result_process(GGZResultTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_result_free(GGZResultTag *result);

/* Functions for <PASSWORD> tag */
static void _ggzcore_protocol_password_process(GGZXMLElement*, GGZXMLElement*, GGZNet *net);

/* Functions for <LIST> tag */
static GGZListTag* _ggzcore_protocol_list_new(char **attrs);
static void _ggzcore_protocol_list_process(GGZListTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_list_free(GGZListTag*);

/* Functions for <UPDATE> tag */
static GGZUpdateTag* _ggzcore_protocol_update_new(char **attrs);
static void _ggzcore_protocol_update_process(GGZUpdateTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_update_free(GGZUpdateTag*);

/* Functions for <GAME> tag */
static GGZGameTag* _ggzcore_protocol_game_new(char **attrs);
static void _ggzcore_protocol_game_process(GGZGameTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_game_free(GGZGameTag*);

/* Functions for <PROTOCOL> tag */
static GGZProtocolTag* _ggzcore_protocol_protocol_new(char **attrs);
static void _ggzcore_protocol_protocol_process(GGZProtocolTag *protocol, GGZGameTag *game, GGZNet *net);
static void _ggzcore_protocol_protocol_free(GGZProtocolTag*);

/* Functions for <ALLOW> tag */
static GGZAllowTag* _ggzcore_protocol_allow_new(char **attrs);
static void _ggzcore_protocol_allow_process(GGZAllowTag *allow, GGZGameTag *game, GGZNet *net);
static void _ggzcore_protocol_allow_free(GGZAllowTag*);

/* Functions for <ABOUT> tag */
static GGZAboutTag* _ggzcore_protocol_about_new(char **attrs);
static void _ggzcore_protocol_about_process(GGZAboutTag *about, GGZGameTag *game, GGZNet *net);
static void _ggzcore_protocol_about_free(GGZAboutTag*);

/* Functions for <DESC> tag */
static void _ggzcore_protocol_desc_process(GGZXMLElement*, GGZXMLElement*, GGZNet *net);

/* Functions for <ROOM> tag */
static GGZRoomTag* _ggzcore_protocol_room_new(char **attrs);
static void _ggzcore_protocol_room_process(GGZRoomTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_room_free(GGZRoomTag*);

/* Functions for <PLAYER> tag */
static GGZPlayerTag* _ggzcore_protocol_player_new(char **attrs);
static void _ggzcore_protocol_player_process(GGZPlayerTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_player_free(GGZPlayerTag*);

/* Functions for <TABLE> tag */
static GGZTableTag* _ggzcore_protocol_table_new(char **attrs);
static void _ggzcore_protocol_table_process(GGZTableTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_table_free(GGZTableTag*);

/* Functions for <SEAT> tag */
static GGZSeatTag* _ggzcore_protocol_seat_new(char **attrs);
static void _ggzcore_protocol_seat_process(GGZSeatTag*, GGZTableTag*, GGZNet*);
static void _ggzcore_protocol_seat_free(GGZSeatTag*);
static void* _ggzcore_protocol_seat_copy(void *data);

/* Functions for <CHAT> tag */
static GGZChatTag* _ggzcore_protocol_chat_new(char **attrs);
static void _ggzcore_protocol_chat_process(GGZChatTag*, GGZXMLElement*, GGZNet*);
static void _ggzcore_protocol_chat_free(GGZChatTag*);

/* Functions for <DATA> tag */
static GGZDataTag* _ggzcore_protocol_data_new(char **attrs);
static void _ggzcore_protocol_data_process(GGZDataTag *data, GGZXMLElement *parent, GGZNet *net);
static void _ggzcore_protocol_data_free(GGZDataTag *);

static void dbg_room(GGZRoom *room);
static void dbg_game(GGZGameType *type);


GGZXMLElement* _ggzcore_protocol_new_element(char *tag, char **attrs)
{
	GGZXMLElement *element;

	/* FIXME: Could we do this with a table lookup? */
	if (strcmp(tag, "SERVER") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_server_new(attrs);
	else if (strcmp(tag, "OPTIONS") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_options_new(attrs);
	else if (strcmp(tag, "MOTD") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_motd_new(attrs);
	else if (strcmp(tag, "RESULT") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_result_new(attrs);
	else if (strcmp(tag, "LIST") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_list_new(attrs);
	else if (strcmp(tag, "UPDATE") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_update_new(attrs);
	else if (strcmp(tag, "GAME") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_game_new(attrs);
	else if (strcmp(tag, "PROTOCOL") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_protocol_new(attrs);
	else if (strcmp(tag, "ALLOW") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_allow_new(attrs);
	else if (strcmp(tag, "ABOUT") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_about_new(attrs);
	else if (strcmp(tag, "ROOM") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_room_new(attrs);
	else if (strcmp(tag, "PLAYER") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_player_new(attrs);
	else if (strcmp(tag, "TABLE") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_table_new(attrs);
	else if (strcmp(tag, "SEAT") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_seat_new(attrs);
	else if (strcmp(tag, "CHAT") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_chat_new(attrs);
	else if (strcmp(tag, "DATA") == 0)
		element = (GGZXMLElement*)_ggzcore_protocol_data_new(attrs);
	else if (strcmp(tag, "DESC") == 0)
		element = _ggzcore_xmlelement_new(tag, _ggzcore_protocol_desc_process, NULL);
	else if (strcmp(tag, "PASSWORD") == 0)
		element = _ggzcore_xmlelement_new(tag, _ggzcore_protocol_password_process, NULL);
	else
		element = _ggzcore_xmlelement_new(tag, NULL, NULL);
	
	return element;
}


void _ggzcore_protocol_process_element(GGZXMLElement *element, GGZXMLElement *parent, GGZNet *net)
{
	char *tag;
	
	if (element) {
		tag = _ggzcore_xmlelement_get_tag(element);
		
		ggzcore_debug(GGZ_DBG_XML, "Processing %s element", tag);

		if (element->process)
			element->process(element, parent, net);
	}
}


/* Functions for <SERVER> tag */
static struct _GGZServerTag* _ggzcore_protocol_server_new(char **attrs)
{
	int i;
	struct _GGZServerTag *server;

	server = ggzcore_malloc(sizeof(struct _GGZServerTag));

	/* Setup GGZXMLElement potion of server */
	_ggzcore_xmlelement_init((GGZXMLElement*)server, "SERVER", 
				 _ggzcore_protocol_server_process, 
				 _ggzcore_protocol_server_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ID") == 0)
			server->id = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "NAME") == 0)
			server->name = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "VERSION") == 0)
			server->version = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "STATUS") == 0)
			server->status = ggzcore_strdup(attrs[i+1]);
		
		i += 2;
	}

	return server;
}


static void _ggzcore_protocol_server_process(GGZServerTag *server, GGZXMLElement *parent, GGZNet *net)
{
	if (server) {
		ggzcore_debug(GGZ_DBG_NET, 
			      "%s(%s) : status %s: protocol %d: chat size %d", 
			      server->name, server->id, server->status,
			      server->version, server->chatlen);

		_ggzcore_net_set_chat_size(net, server->chatlen);
		/* FIXME: Do something with name, status */
		if (server->version == GGZ_CS_PROTO_VERSION)
			_ggzcore_server_set_negotiate_status(_ggzcore_net_get_server(net), 0);
		else
			_ggzcore_server_set_negotiate_status(_ggzcore_net_get_server(net), -1);
	}
}


static void _ggzcore_protocol_server_free(GGZServerTag *server)
{
	if (server) {
		if (server->id)
			ggzcore_free(server->id);
		if (server->name)
			ggzcore_free(server->name);
		if (server->status)
			ggzcore_free(server->status);
		ggzcore_free(server);
	}
}


/* Functions for <OPTIONS> tag */
static struct _GGZOptionsTag* _ggzcore_protocol_options_new(char **attrs)
{
	int i;
	struct _GGZOptionsTag *options;

	options = ggzcore_malloc(sizeof(struct _GGZOptionsTag));

	/* Setup GGZXMLElement potion of options */
	_ggzcore_xmlelement_init((GGZXMLElement*)options, "OPTIONS", 
				 _ggzcore_protocol_options_process, 
				 _ggzcore_protocol_options_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "CHATLEN") == 0)
			options->chatlen = atoi(attrs[i+1]);

		i += 2;
	}
	
	return options;
}


static void _ggzcore_protocol_options_process(GGZOptionsTag *options, GGZServerTag *server, GGZNet *net)
{
	if (options && server)
		/* Grab chat length out of options and store on server */
		server->chatlen = options->chatlen;
}


static void _ggzcore_protocol_options_free(GGZOptionsTag *options)
{
	if (options)
		ggzcore_free(options);
}


/* Functions for <MOTD> tag */
static struct _GGZMotdTag* _ggzcore_protocol_motd_new(char **attrs)
{
	int i;
	struct _GGZMotdTag *motd;

	motd = ggzcore_malloc(sizeof(struct _GGZMotdTag));

	/* Setup GGZXMLElement potion of motd */
	_ggzcore_xmlelement_init((GGZXMLElement*)motd, "MOTD", 
				 _ggzcore_protocol_motd_process, 
				 _ggzcore_protocol_motd_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "PRIORITY") == 0)
			motd->priority = ggzcore_strdup(attrs[i+1]);

		i += 2;
	}
	
	return motd;
}


static void _ggzcore_protocol_motd_process(GGZMotdTag *motd, GGZXMLElement *parent, GGZNet *net)
{
	char **buffer;
	char *message;

	message = _ggzcore_xmlelement_get_text((GGZXMLElement*)motd);
	
	ggzcore_debug(GGZ_DBG_NET, "Motd of priority %s: '%s'", motd->priority,
		      message);

	if (!(buffer = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_read_motd");

	buffer[0] = message;
	_ggzcore_server_event(_ggzcore_net_get_server(net), GGZ_MOTD_LOADED, buffer);
}


static void _ggzcore_protocol_motd_free(GGZMotdTag *motd)
{
	if (motd) {
		if (motd->priority)
			ggzcore_free(motd->priority);
		ggzcore_free(motd);
	}
}


/* Functions for <RESULT> tag */
static struct _GGZResultTag* _ggzcore_protocol_result_new(char **attrs)
{
	int i;
	struct _GGZResultTag *result;

	result = ggzcore_malloc(sizeof(struct _GGZResultTag));

	/* Setup GGZXMLElement potion of result */
	_ggzcore_xmlelement_init((GGZXMLElement*)result, "RESULT", 
				 _ggzcore_protocol_result_process, 
				 _ggzcore_protocol_result_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ACTION") == 0)
			result->action = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "CODE") == 0)
			result->code = atoi(attrs[i+1]);
		
		i += 2;
	}
	
	return result;
}


static void _ggzcore_protocol_result_process(GGZResultTag *result, GGZXMLElement *parent, GGZNet *net)
{
	GGZServer *server;
	GGZRoom *room;

	if (result) {
		ggzcore_debug(GGZ_DBG_NET, "Result of %s was %d", result->action, 
			      result->code);

		server = _ggzcore_net_get_server(net);
		if (strcmp(result->action, "login") == 0) {
			/* FIXME: we should check if login type is 'new' */
			if (result->code == 0 && result->data) {
				_ggzcore_server_set_password(server, result->data);
			}
			_ggzcore_server_set_login_status(server, result->code);
		}
		else if (strcmp(result->action, "enter") == 0) {
			if (result->code == 0)
				_ggzcore_server_set_room(server, _ggzcore_net_get_new_room(net));
			_ggzcore_server_set_room_join_status(server, result->code);
		}
		else if  (strcmp(result->action, "launch") == 0) {
			room = _ggzcore_server_get_cur_room(server);
			_ggzcore_room_set_table_launch_status(room, result->code);
		}
		else if  (strcmp(result->action, "join") == 0) {
			room = _ggzcore_server_get_cur_room(server);
			_ggzcore_room_set_table_join_status(room, result->code);
		}
		else if  (strcmp(result->action, "leave") == 0) {
			room = _ggzcore_server_get_cur_room(server);
			_ggzcore_room_set_table_leave_status(room, result->code);
		}
		else if  (strcmp(result->action, "chat") == 0) {
			switch (result->code) {
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


static void _ggzcore_protocol_result_free(GGZResultTag *result)
{
	if (result) {
		if (result->action)
			ggzcore_free(result->action);
		ggzcore_free(result);
	}
}


/* Functions for <PASSWORD> tag */
static void _ggzcore_protocol_password_process(GGZXMLElement *element, GGZXMLElement *parent, GGZNet *net)
{
	char *password;
	char *parent_tag;
	
	if (element && parent) {
		password = _ggzcore_xmlelement_get_text(element);
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		/* It had better be RESULT.... */
		if (strcmp(parent_tag, "RESULT") == 0) {
			((GGZResultTag*)parent)->data = ggzcore_strdup(password);
		}
	}
}


/* Functions for <LIST> tag */
static struct _GGZListTag* _ggzcore_protocol_list_new(char **attrs)
{
	int i;
	struct _GGZListTag *list;
	_ggzcoreEntryCompare compare_func = NULL;
	_ggzcoreEntryCreate  create_func = NULL;
	_ggzcoreEntryDestroy destroy_func = NULL;

	list = ggzcore_malloc(sizeof(struct _GGZListTag));
	list->room = -1;

	/* Setup GGZXMLElement potion of list */
	_ggzcore_xmlelement_init((GGZXMLElement*)list, "LIST", 
				 _ggzcore_protocol_list_process, 
				 _ggzcore_protocol_list_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "TYPE") == 0)
			list->type = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "ROOM") == 0)
			list->room = atoi(attrs[i+1]);

		i += 2;
	}

	/* Setup actual list */
	if (strcmp(list->type, "game") == 0) {}
	else if (strcmp(list->type, "room") == 0) {}
	else if (strcmp(list->type, "player") == 0) {
		compare_func = _ggzcore_player_compare;
		destroy_func = _ggzcore_player_destroy;
	}		
	else if (strcmp(list->type, "table") == 0) {
		compare_func = _ggzcore_table_compare;
		destroy_func = _ggzcore_table_destroy;
	}		
	list->list = _ggzcore_list_create(compare_func, 
					  create_func, 
					  destroy_func,
					  _GGZCORE_LIST_ALLOW_DUPS);
	
	return list;
}


static void _ggzcore_protocol_list_process(GGZListTag *list, GGZXMLElement *parent, GGZNet *net)
{
	_ggzcore_list_entry *entry;
	GGZServer *server;
	GGZRoom *room;
	int count;
	
	if (list) {
		entry = _ggzcore_list_head(list->list);
		server = _ggzcore_net_get_server(net);

		/* Get length of list */
		/* FIXME: it would be nice if we could get this from the list itself */
		count = 0;
		while (entry) {
			entry = _ggzcore_list_next(entry);
			count++;
		}


		if (strcmp(list->type, "room") == 0) {
			/* Clear existing list (if any) */
			if (_ggzcore_server_get_num_rooms(server) > 0)
				_ggzcore_server_free_roomlist(server);

			_ggzcore_server_init_roomlist(server, count);

			entry = _ggzcore_list_head(list->list);
			while (entry) {
				_ggzcore_server_add_room(server, _ggzcore_list_get_data(entry));
				entry = _ggzcore_list_next(entry);
			}
			_ggzcore_server_event(server, GGZ_ROOM_LIST, NULL);
		}
		else if (strcmp(list->type, "game") == 0) {
			/* Free previous list of types*/
			if (ggzcore_server_get_num_gametypes(server) > 0)
				_ggzcore_server_free_typelist(server);

			_ggzcore_server_init_typelist(server, count);
			entry = _ggzcore_list_head(list->list);
			while (entry) {
				_ggzcore_server_add_type(server, _ggzcore_list_get_data(entry));
				entry = _ggzcore_list_next(entry);
			}
			_ggzcore_server_event(server, GGZ_TYPE_LIST, NULL);
		}
		else if (strcmp(list->type, "player") == 0) {
			room = ggzcore_server_get_cur_room(server);
			_ggzcore_room_set_player_list(room, count, list->list);
			list->list = NULL;
		}
		else if (strcmp(list->type, "table") == 0) {
			room = ggzcore_server_get_cur_room(server);
			_ggzcore_room_set_table_list(room, count, list->list);
			list->list = NULL;
		}

	}
}


static void _ggzcore_protocol_list_free(GGZListTag *list)
{
	if (list) {
		if (list->list)
			_ggzcore_list_destroy(list->list);
		if (list->type)
			ggzcore_free(list->type);
		ggzcore_free(list);
	}
}


/* Functions for <UPDATE> tag */
static struct _GGZUpdateTag* _ggzcore_protocol_update_new(char **attrs)
{
	int i;
	struct _GGZUpdateTag *update;

	update = ggzcore_malloc(sizeof(struct _GGZUpdateTag));
	update->room = -1;

	/* Setup GGZXMLElement potion of update */
	_ggzcore_xmlelement_init((GGZXMLElement*)update, "UPDATE", 
				 _ggzcore_protocol_update_process, 
				 _ggzcore_protocol_update_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "TYPE") == 0)
			update->type = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "ACTION") == 0)
			update->action = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "ROOM") == 0)
			update->room = atoi(attrs[i+1]);

		i += 2;
	}

	return update;
}


static void _ggzcore_protocol_update_process(GGZUpdateTag *update, GGZXMLElement *parent, GGZNet *net)
{
	int i, seats;
	char *name;
	GGZServer *server;
	GGZRoom *room;
	
	if (update) {
		server = _ggzcore_net_get_server(net);
		room = ggzcore_server_get_cur_room(server);

		if (strcmp(update->type, "room") == 0) {
			/* FIXME: implement this */
		}
		else if (strcmp(update->type, "game") == 0) {
			/* FIXME: implement this */
		}
		else if (strcmp(update->type, "player") == 0) {
			GGZPlayer *player = update->data;
			
			if (strcmp(update->action, "add") == 0) {
				_ggzcore_room_add_player(room, player->name);
			}
			else if (strcmp(update->action, "delete") == 0) {
				_ggzcore_room_remove_player(room, player->name);
			}
		}
		else if (strcmp(update->type, "table") == 0) {
			GGZTable *table = update->data;
			if (strcmp(update->action, "add") == 0) {
				_ggzcore_room_add_table(room, table);
				update->data = NULL;
			}
			if (strcmp(update->action, "delete") == 0) {
				_ggzcore_room_remove_table(room, table->id);
			}
			if (strcmp(update->action, "join") == 0) {
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
			if (strcmp(update->action, "leave") == 0) {
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
			if (strcmp(update->action, "status") == 0) {
				_ggzcore_room_new_table_state(room, table->id, table->state);
			}
		}

	}
}


static void _ggzcore_protocol_update_free(GGZUpdateTag *update)
{
	if (update) {
		if (update->action)
			ggzcore_free(update->action);
		if (update->data) {
			if (strcmp(update->type, "table") == 0)
				_ggzcore_table_free(update->data);
			else if (strcmp(update->type, "player") == 0)
				_ggzcore_player_free(update->data);
		}
		if (update->type)
			ggzcore_free(update->type);
		ggzcore_free(update);
	}
}


/* Functions for <GAME> tag */
static GGZGameTag* _ggzcore_protocol_game_new(char **attrs) 
{
	int i;
	struct _GGZGameTag *game;

	game = ggzcore_malloc(sizeof(struct _GGZGameTag));

	/* Setup GGZXMLElement potion of game */
	_ggzcore_xmlelement_init((GGZXMLElement*)game, "GAME", 
				 _ggzcore_protocol_game_process, 
				 _ggzcore_protocol_game_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ID") == 0)
			game->id = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "NAME") == 0)
			game->name = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "VERSION") == 0)
			game->version = ggzcore_strdup(attrs[i+1]);
		
		i += 2;
	}
	
	return game;
}


static void _ggzcore_protocol_game_process(GGZGameTag *game, GGZXMLElement *parent, GGZNet *net)
{
	GGZGameType *type;
	char *parent_tag;
	
	if (game && parent) {
		
		type = _ggzcore_gametype_new();
		_ggzcore_gametype_init(type, game->id, game->name, 
				       game->version,
				       game->prot_engine,
				       game->prot_version,
				       game->allow_players, 
				       game->allow_bots,  
				       game->desc,
				       game->author, 
				       game->url);

		parent_tag = _ggzcore_xmlelement_get_tag(parent);

		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_list_insert(((GGZListTag*)parent)->list, type);
	}
}


static void _ggzcore_protocol_game_free(GGZGameTag *game)
{
	if (game) {
		if (game->name)
			ggzcore_free(game->name);
		if (game->version)
			ggzcore_free(game->version);
		if (game->prot_engine)
			ggzcore_free(game->prot_engine);
		if (game->prot_version)
			ggzcore_free(game->prot_version);
		if (game->author)
			ggzcore_free(game->author);
		if (game->url)
			ggzcore_free(game->url);
		if (game->desc)
			ggzcore_free(game->desc);

		ggzcore_free(game);
	}
}


/* Functions for <PROTOCOL> tag */
static GGZProtocolTag* _ggzcore_protocol_protocol_new(char **attrs) 
{
	int i;
	struct _GGZProtocolTag *protocol;

	protocol = ggzcore_malloc(sizeof(struct _GGZProtocolTag));

	/* Setup GGZXMLElement potion of protocol */
	_ggzcore_xmlelement_init((GGZXMLElement*)protocol, "PROTOCOL", 
				 _ggzcore_protocol_protocol_process, 
				 _ggzcore_protocol_protocol_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ENGINE") == 0)
			protocol->engine = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "VERSION") == 0)
			protocol->version = ggzcore_strdup(attrs[i+1]);
		
		i += 2;
	}

	return protocol;
}


static void _ggzcore_protocol_protocol_process(GGZProtocolTag *protocol, GGZGameTag *game, GGZNet *net)
{
	if (protocol && game) {
		game->prot_engine = protocol->engine;
		protocol->engine = NULL;
		game->prot_version = protocol->version;
		protocol->version = NULL;
	}
}


static void _ggzcore_protocol_protocol_free(GGZProtocolTag *protocol)
{
	if (protocol) {
		if (protocol->engine)
			ggzcore_free(protocol->engine);
		if (protocol->version)
			ggzcore_free(protocol->version);
		ggzcore_free(protocol);
	}
}


/* Functions for <ALLOW> tag */
static GGZAllowTag* _ggzcore_protocol_allow_new(char **attrs) 
{
	int i;
	struct _GGZAllowTag *allow;

	allow = ggzcore_malloc(sizeof(struct _GGZAllowTag));

	/* Setup GGZXMLElement potion of allow */
	_ggzcore_xmlelement_init((GGZXMLElement*)allow, "ALLOW", 
				 _ggzcore_protocol_allow_process, 
				 _ggzcore_protocol_allow_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "PLAYERS") == 0)
			allow->players = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "BOTS") == 0)
			allow->bots = atoi(attrs[i+1]);
		
		i += 2;
	}


	return allow;
}


static void _ggzcore_protocol_allow_process(GGZAllowTag *allow, GGZGameTag *game, GGZNet *net)
{
	if (allow && game) {
		game->allow_players = allow->players;
		game->allow_bots = allow->bots;
	}
}


static void _ggzcore_protocol_allow_free(GGZAllowTag *allow)
{
	if (allow) {
		ggzcore_free(allow);
	}
}


/* Functions for <ABOUT> tag */
static GGZAboutTag* _ggzcore_protocol_about_new(char **attrs) 
{
	int i;
	struct _GGZAboutTag *about;

	about = ggzcore_malloc(sizeof(struct _GGZAboutTag));

	/* Setup GGZXMLElement potion of about */
	_ggzcore_xmlelement_init((GGZXMLElement*)about, "ABOUT", 
				 _ggzcore_protocol_about_process, 
				 _ggzcore_protocol_about_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "AUTHOR") == 0)
			about->author = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "URL") == 0)
			about->url = ggzcore_strdup(attrs[i+1]);
		
		i += 2;
	}

	return about;
}


static void _ggzcore_protocol_about_process(GGZAboutTag *about, GGZGameTag *game, GGZNet *net)
{
	if (about && game) {
		game->author = about->author;
		about->author = NULL;
		game->url = about->url;
		about->url = NULL;
	}
}


static void _ggzcore_protocol_about_free(GGZAboutTag *about)
{
	if (about) {
		if (about->author)
			ggzcore_free(about->author);
		if (about->url)
			ggzcore_free(about->url);
		ggzcore_free(about);
	}
}


/* Functions for <DESC> tag */
static void _ggzcore_protocol_desc_process(GGZXMLElement *element, GGZXMLElement *parent, GGZNet *net)
{
	char *desc;
	char *parent_tag;
	
	if (element && parent) {
		desc = _ggzcore_xmlelement_get_text(element);
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "GAME") == 0) {
			((GGZGameTag*)parent)->desc = ggzcore_strdup(desc);
		}
		else if (strcmp(parent_tag, "ROOM") == 0) {
			((GGZRoomTag*)parent)->desc = ggzcore_strdup(desc);
		}
		else if (strcmp(parent_tag, "TABLE") == 0) {
			((GGZTableTag*)parent)->desc = ggzcore_strdup(desc);
		}

	}
}


/* Functions for <ROOM> tag */
static GGZRoomTag* _ggzcore_protocol_room_new(char **attrs)
{
	int i;
	struct _GGZRoomTag *room;

	room = ggzcore_malloc(sizeof(struct _GGZRoomTag));

	/* Setup GGZXMLElement potion of room */
	_ggzcore_xmlelement_init((GGZXMLElement*)room, "ROOM", 
				 _ggzcore_protocol_room_process, 
				 _ggzcore_protocol_room_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ID") == 0)
			room->id = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "NAME") == 0)
			room->name = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "GAME") == 0)
			room->game = atoi(attrs[i+1]);
		
		i += 2;
	}

	return room;
}


static void _ggzcore_protocol_room_process(GGZRoomTag *room, GGZXMLElement *parent, GGZNet *net)
{
	GGZRoom *ggz_room;
	char *parent_tag;

	if (room && parent) {
		ggz_room = _ggzcore_room_new();
		_ggzcore_room_init(ggz_room, _ggzcore_net_get_server(net), 
				   room->id, room->name, room->game, 
				   room->desc);
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_list_insert(((GGZListTag*)parent)->list, ggz_room);
	}
}


static void _ggzcore_protocol_room_free(GGZRoomTag *room)
{
	if (room) {
		if (room->name)
			ggzcore_free(room->name);
		if (room->desc)
			ggzcore_free(room->desc);
		ggzcore_free(room);
	}
}


/* Functions for <PLAYER> tag */
static GGZPlayerTag* _ggzcore_protocol_player_new(char **attrs)
{
	int i;
	struct _GGZPlayerTag *player;

	player = ggzcore_malloc(sizeof(struct _GGZPlayerTag));

	/* Setup GGZXMLElement potion of player */
	_ggzcore_xmlelement_init((GGZXMLElement*)player, "PLAYER", 
				 _ggzcore_protocol_player_process, 
				 _ggzcore_protocol_player_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ID") == 0)
			player->id = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "TYPE") == 0)
			player->type = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "TABLE") == 0)
			player->table = atoi(attrs[i+1]);
		
		i += 2;
	}

	return player;
}


static void _ggzcore_protocol_player_process(GGZPlayerTag *player, GGZXMLElement *parent, GGZNet *net)
{
	GGZPlayer *ggz_player;
	char *parent_tag;
	GGZServer *server;
	GGZRoom *room;

	if (player && parent) {
		server = _ggzcore_net_get_server(net);
		room = ggzcore_server_get_cur_room(server);

		ggz_player = _ggzcore_player_new();
		_ggzcore_player_init(ggz_player,  player->id, room,
				     player->table);
		parent_tag = _ggzcore_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_list_insert(((GGZListTag*)parent)->list, ggz_player);
		else if (strcmp(parent_tag, "UPDATE") == 0)
			((GGZUpdateTag*)parent)->data = ggz_player;
	}
}


static void _ggzcore_protocol_player_free(GGZPlayerTag *player)
{
	if (player) {
		if (player->id)
			ggzcore_free(player->id);
		if (player->type)
			ggzcore_free(player->type);
		ggzcore_free(player);
	}
}


/* Functions for <TABLE> tag */
static GGZTableTag* _ggzcore_protocol_table_new(char **attrs)
{
	int i;
	struct _GGZTableTag *table;

	table = ggzcore_malloc(sizeof(struct _GGZTableTag));

	/* Setup GGZXMLElement potion of table */
	_ggzcore_xmlelement_init((GGZXMLElement*)table, "TABLE", 
				 _ggzcore_protocol_table_process, 
				 _ggzcore_protocol_table_free);
	
	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "ID") == 0)
			table->id = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "GAME") == 0)
			table->game = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "STATUS") == 0)
			table->status = atoi(attrs[i+1]);
		else if (strcmp(attrs[i], "SEATS") == 0)
			table->num_seats = atoi(attrs[i+1]);
		
		i += 2;
	}
	
	table->seats = _ggzcore_list_create(NULL, 
					    _ggzcore_protocol_seat_copy, 
					    (_ggzcoreEntryDestroy)_ggzcore_protocol_seat_free, 
					    _GGZCORE_LIST_ALLOW_DUPS);

	return table;
}


static void _ggzcore_protocol_table_process(GGZTableTag *table, GGZXMLElement *parent, GGZNet *net)
{
	char *parent_tag;
	GGZGameType *type;
	GGZServer *server;
	GGZSeatTag *seat;
	GGZTable *ggz_table;
	_ggzcore_list_entry *entry;

	if (table && parent) {
		parent_tag = _ggzcore_xmlelement_get_tag(parent);

		/* Create new table structure */
		ggz_table = _ggzcore_table_new();
		server = _ggzcore_net_get_server(net);
		type = _ggzcore_server_get_type_by_id(server, table->game);
		_ggzcore_table_init(ggz_table, type, table->desc, table->num_seats, table->status, table->id);
		
		/* Add seats */
		entry = _ggzcore_list_head(table->seats);
		while (entry) {
			seat = _ggzcore_list_get_data(entry);
			if (strcmp(seat->type, "open") == 0) {
				/* Nothing to do: seats default to open */
			}
			else if (strcmp(seat->type, "bot") == 0) {
				_ggzcore_table_add_bot(ggz_table, seat->name, seat->index);
			}
			else if (strcmp(seat->type, "player") == 0) {
				_ggzcore_table_add_player(ggz_table, seat->name, seat->index);
			}
			else if (strcmp(seat->type, "reserved") == 0) {
				_ggzcore_table_add_reserved(ggz_table, seat->name, seat->index);
			}
			entry = _ggzcore_list_next(entry);
		}
		
		if (strcmp(parent_tag, "LIST") == 0) {
			_ggzcore_list_insert(((GGZListTag*)parent)->list, ggz_table);
		}
		else if (strcmp(parent_tag, "UPDATE") == 0) {
			((GGZUpdateTag*)parent)->data = ggz_table;
		}
	}
}


static void _ggzcore_protocol_table_free(GGZTableTag *table)
{
	if (table) {
		if (table->desc)
			ggzcore_free(table->desc);
		if (table->seats)
			_ggzcore_list_destroy(table->seats);
		ggzcore_free(table);
	}
}


/* Functions for <SEAT> tag */
static GGZSeatTag* _ggzcore_protocol_seat_new(char **attrs)
{
	int i;
	struct _GGZSeatTag *seat;

	seat = ggzcore_malloc(sizeof(struct _GGZSeatTag));

	
	/* Setup GGZXMLElement potion of seat */
	_ggzcore_xmlelement_init((GGZXMLElement*)seat, "SEAT", 
				 _ggzcore_protocol_seat_process, 
				 _ggzcore_protocol_seat_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "TYPE") == 0)
			seat->type = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "NUM") == 0)
			seat->index = atoi(attrs[i+1]);
		
		i += 2;
	}

	return seat;
}


static void* _ggzcore_protocol_seat_copy(void *data)
{
	struct _GGZSeatTag *seat1, *seat2;

	seat1 = (GGZSeatTag*)data;

	seat2 = ggzcore_malloc(sizeof(struct _GGZSeatTag));

	seat2->index = seat1->index;
	seat2->type = ggzcore_strdup(seat1->type);
	seat2->name = ggzcore_strdup(seat1->name);

	return seat2;
}


static void _ggzcore_protocol_seat_process(GGZSeatTag *seat, GGZTableTag *parent, GGZNet *net)
{
	if (seat && parent) {
		seat->name = ggzcore_strdup(_ggzcore_xmlelement_get_text((GGZXMLElement*)seat));
		_ggzcore_list_insert(parent->seats, seat);
	}
}


static void _ggzcore_protocol_seat_free(GGZSeatTag *seat)
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
static GGZChatTag* _ggzcore_protocol_chat_new(char **attrs)
{
	int i;
	struct _GGZChatTag *chat;

	chat = ggzcore_malloc(sizeof(struct _GGZChatTag));

	/* Setup GGZXMLElement potion of chat */
	_ggzcore_xmlelement_init((GGZXMLElement*)chat, "CHAT", 
				 _ggzcore_protocol_chat_process, 
				 _ggzcore_protocol_chat_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "TYPE") == 0)
			chat->type = ggzcore_strdup(attrs[i+1]);
		else if (strcmp(attrs[i], "FROM") == 0)
			chat->from = ggzcore_strdup(attrs[i+1]);
		
		i += 2;
	}


	return chat;
}


static void _ggzcore_protocol_chat_process(GGZChatTag *chat, GGZXMLElement *parent, GGZNet *net)
{
	char *msg;
	GGZRoom *room;
	GGZChatOp op = 0;

	if (chat) {
		msg = _ggzcore_xmlelement_get_text((GGZXMLElement*)chat);
		ggzcore_debug(GGZ_DBG_NET, "%s message from %s: '%s'", 
			      chat->type, chat->from, msg);	

		if (strcmp(chat->type, "normal") == 0)
			op = GGZ_CHAT_NORMAL;
		else if (strcmp(chat->type, "private") == 0)
			op = GGZ_CHAT_PERSONAL;
		else if (strcmp(chat->type, "announce") == 0)
			op = GGZ_CHAT_ANNOUNCE;
		else if (strcmp(chat->type, "beep") == 0)
			op = GGZ_CHAT_BEEP;
		
		room = ggzcore_server_get_cur_room(_ggzcore_net_get_server(net));
		_ggzcore_room_add_chat(room, op, chat->from, msg);
	}
}


static void _ggzcore_protocol_chat_free(GGZChatTag *chat)
{
	if (chat) {
		if (chat->type)
			ggzcore_free(chat->type);
		if (chat->from)
			ggzcore_free(chat->from);
		ggzcore_free(chat);
	}
}


/* Functions for <DATA> tag */
static struct _GGZDataTag* _ggzcore_protocol_data_new(char **attrs)
{
	int i;
	struct _GGZDataTag *data;

	data = ggzcore_malloc(sizeof(struct _GGZDataTag));

	/* Setup GGZXMLElement potion of data */
	_ggzcore_xmlelement_init((GGZXMLElement*)data, "DATA", 
				 _ggzcore_protocol_data_process, 
				 _ggzcore_protocol_data_free);

	i = 0;
	while (attrs[i]) {
		if (strcmp(attrs[i], "SIZE") == 0)
			data->size = atoi(attrs[i+1]);

		i += 2;
	}
	
	return data;
}


static void _ggzcore_protocol_data_process(GGZDataTag *data, GGZXMLElement *parent, GGZNet *net)
{
	GGZServer *server;
	GGZRoom *room;
	char buffer[4096 + sizeof(data->size)];
	char *buf_offset;
	char *msg;
	char *token;
	int i;

	if (data) {
		msg = _ggzcore_xmlelement_get_text((GGZXMLElement*)data);
		
		server = _ggzcore_net_get_server(net);
		room = ggzcore_server_get_cur_room(server);
		
		/* Leave room for storing 'size' in the first buf_offset bytes */
		*(int*)buffer = data->size;
		buf_offset = buffer + sizeof(data->size);

		token = strtok(msg, " ");
		for (i = 0; i < data->size; i++) {
			buf_offset[i] = atoi(token);
			token = strtok(NULL, " ");
		}
		/*memcpy(buf_offset, msg, data->size);*/

		_ggzcore_room_recv_game_data(room, buffer);
	}
}


static void _ggzcore_protocol_data_free(GGZDataTag *data)
{
	if (data)
		ggzcore_free(data);
}



static void dbg_room(GGZRoom *room)
{
	ggzcore_debug(GGZ_DBG_XML, "ROOM:");
	ggzcore_debug(GGZ_DBG_XML, "\tID = %d", _ggzcore_room_get_id(room));
	ggzcore_debug(GGZ_DBG_XML, "\tNAME = %s", _ggzcore_room_get_name(room));
	ggzcore_debug(GGZ_DBG_XML, "\tDESC = %s", _ggzcore_room_get_desc(room));
	/*ggzcore_debug(GGZ_DBG_XML, "\tGAME = %d", room->game);*/
}

static void dbg_game(GGZGameType *type)
{

	ggzcore_debug(GGZ_DBG_XML, "GAME:");
	ggzcore_debug(GGZ_DBG_XML, "\tID = %d", _ggzcore_gametype_get_id(type));
	ggzcore_debug(GGZ_DBG_XML, "\tNAME = %s", _ggzcore_gametype_get_name(type));
	ggzcore_debug(GGZ_DBG_XML, "\tVERSION = %s", _ggzcore_gametype_get_version(type));
	ggzcore_debug(GGZ_DBG_XML, "\tPROTO = %s", _ggzcore_gametype_get_prot_engine(type));	
	ggzcore_debug(GGZ_DBG_XML, "\tVER = %s", _ggzcore_gametype_get_prot_version(type));
	ggzcore_debug(GGZ_DBG_XML, "\tAUTHOR = %s", _ggzcore_gametype_get_author(type));	
	ggzcore_debug(GGZ_DBG_XML, "\tURL = %s", _ggzcore_gametype_get_url(type));
	ggzcore_debug(GGZ_DBG_XML, "\tMAX PLAY = %d", _ggzcore_gametype_get_max_players(type));
	ggzcore_debug(GGZ_DBG_XML, "\tMAX BOTS = %d", _ggzcore_gametype_get_max_bots(type));

	ggzcore_debug(GGZ_DBG_XML, "\tDESC = %s", _ggzcore_gametype_get_desc(type));
}



