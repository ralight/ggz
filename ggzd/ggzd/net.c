/*
 * File: net.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 9/22/01
 * Desc: Functions for handling network IO
 * $Id: net.c 4141 2002-05-03 03:50:35Z bmh $
 * 
 * Code for parsing XML streamed from the server
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#include "err_func.h"
#include "ggzdb.h"
#include "hash.h"
#include "motd.h"
#include "net.h"
#include "protocols.h"
#include "seats.h"
#include "client.h"

#include <errno.h>
#include <expat.h>
#include <ggz.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/* For convenience */
#define BUFFSIZE 8192


/* GGZNet structure for handling the network connection to the server */
struct _GGZNetIO {

	/* The client this object serves */
	GGZClient *client;

	/* The actual socket */
	int fd;

	/* Flag to indicate we're in a parse call */
	char parsing;

	/* XML Parser */
	XML_Parser parser;

	/* Message parsing stack */
	GGZStack *stack;

	/* File to dump protocol session */
	int dump_file;
	
	/* Count the number of bytes we've parsed since the last tag*/
	int byte_count;

	/* Flag whether we've a tag on this round of parsing */
	/* This element, coupled with the byte counter helps us prevent DoS */
	char tag_seen;
};


/* Table data structure */
typedef struct _GGZTableData {
	char *desc;
	GGZList *seats;
} GGZTableData;


/* Seat data structure */
typedef struct _GGZSeatData {
	int index;
	char *type; 
	char *name;
} GGZSeatData;


/* Authentication data structure */
typedef struct _GGZAuthData {
	char *name;
	char *password;
} GGZAuthData;


/* Callbacks for XML parser */
static void _net_parse_start_tag(void *, const char*, const char **);
static void _net_parse_end_tag(void *data, const char *el);
static void _net_parse_text(void *data, const char *text, int len);
static GGZXMLElement* _net_new_element(char *tag, char **attrs);

/* Handler functions for various tags */
static void _net_handle_session(GGZNetIO *net, GGZXMLElement *session);
static void _net_handle_channel(GGZNetIO *net, GGZXMLElement *channel);
static void _net_handle_login(GGZNetIO *net, GGZXMLElement *login);
static void _net_handle_name(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_password(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_update(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_list(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_enter(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_chat(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_join(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_leave(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_launch(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_table(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_seat(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_desc(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_motd(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_data(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_pong(GGZNetIO *net, GGZXMLElement *element);

/* Utility functions */
static int safe_atoi(char *string);
static char* safe_strdup(char *str);
static void _net_dump_data(struct _GGZNetIO *net, char *data, int size);
static int _net_send_result(GGZNetIO *net, char *action, char code);
static int _net_send_login_normal_status(GGZNetIO *net, char status);
static int _net_send_login_anon_status(GGZNetIO *net, char status);
static int _net_send_login_new_status(GGZNetIO *net, char status, char *password);
static int _net_send_table_status(GGZNetIO *net, GGZTable *table);
static int _net_send_table_seat(GGZNetIO *net, GGZTable *table, int num);
static int _net_send_table_desc(GGZNetIO *net, GGZTable *table);
static int _net_send_seat(GGZNetIO *net, GGZTable *table, int num);
static int _net_send_line(GGZNetIO *net, char *line, ...)
			  ggz__attribute((format(printf, 2, 3)));
static int _net_send_string(GGZNetIO *net, char *fmt, ...)
			    ggz__attribute((format(printf, 2, 3)));

static GGZAuthData* _net_authdata_new(void);
static void _net_authdata_free(GGZAuthData *data);
static void _net_auth_set_name(GGZXMLElement *login, char *name);
static void _net_auth_set_password(GGZXMLElement *login, char *password);
static void _net_table_add_seat(GGZXMLElement*, GGZSeatData*);
static void _net_table_set_desc(GGZXMLElement*, char*);
static GGZTableData* _net_tabledata_new(void);
static void _net_tabledata_free(GGZTableData*);
static void* _net_seat_copy(void *data);
static void _net_seat_free(GGZSeatData*);



/* Internal library functions (prototypes in net.h) */

GGZNetIO* net_new(int fd, GGZClient *client)
{
	GGZNetIO *net = NULL;

	if ( (net = malloc(sizeof(GGZNetIO))) == NULL)
		err_sys_exit("malloc error in net_new()");
	
	/* Set fd to invalid value */
	net->fd = fd;
	net->client = client;
	net->dump_file = -1;
	net->parsing = 0;
	net->byte_count = 0;
	net->tag_seen = 0;

        /* Init parser */
        if (!(net->parser = XML_ParserCreate(NULL)))
                err_sys_exit("Couldn't allocate memory for XML parser");

        /* Setup handlers for tags */
        XML_SetElementHandler(net->parser, _net_parse_start_tag, 
                              _net_parse_end_tag);
        XML_SetCharacterDataHandler(net->parser, _net_parse_text);
        XML_SetUserData(net->parser, net);

        /* Initialize stack for messages */
        net->stack = ggz_stack_new();
	
	return net;
}


int net_set_dump_file(GGZNetIO *net, const char* filename)
{
	if (!filename)
		return 0;
	
	if (strcmp(filename, "stderr") == 0)
		net->dump_file = STDERR_FILENO;
	else
		net->dump_file = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	
	if (net->dump_file < 0)
		return -1;
	else
		return 0;
}


/* Get filedescriptor for communication */
int net_get_fd(GGZNetIO *net)
{
	return (net ? net->fd : -1);
}


/* For debugging purposes only! */
void net_set_fd(GGZNetIO *net, int fd)
{
	net->fd = fd;
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
void net_free(GGZNetIO *net)
{
	GGZXMLElement *element;

	if (net) {

		/* Clear elements off stack and free it */
		if (net->stack) {
			while ( (element = ggz_stack_pop(net->stack))) 
				ggz_xmlelement_free(element);
			ggz_stack_free(net->stack);
		}
		
		if (net->parser)
			XML_ParserFree(net->parser);
		
		free(net);
	}
}


/* net_send_XXX() functions for sending messages to the client */

int net_send_serverid(GGZNetIO *net, char *srv_name)
{
	char *xml_srv_name;

	xml_srv_name = ggz_xml_escape(srv_name);

	_net_send_line(net, "<SESSION>");
	_net_send_line(net, "\t<SERVER ID='GGZ-%s' NAME='%s' VERSION='%d' STATUS='%s'>", VERSION, xml_srv_name, GGZ_CS_PROTO_VERSION, "ok");
	_net_send_line(net, "\t\t<OPTIONS CHATLEN='%d'/>", MAX_CHAT_LEN);
	_net_send_line(net, "\t</SERVER>");

	ggz_free(xml_srv_name);

	return 0;
}
 

int net_send_server_full(GGZNetIO *net, char *srv_name)
{
	char *xml_srv_name;

	xml_srv_name = ggz_xml_escape(srv_name);

	_net_send_line(net, "<SESSION>");
	_net_send_line(net, "\t<SERVER ID='GGZ-%s' NAME='%s' VERSION='%d' STATUS='%s'/>", VERSION, xml_srv_name, GGZ_CS_PROTO_VERSION, "full");

	ggz_free(xml_srv_name);

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
	char *line;
	
	_net_send_line(net, "<MOTD PRIORITY='normal'><![CDATA[");

	num = motd_get_num_lines();
		
	for (i = 0; i < num; i++) {
		line = motd_get_line(i);
		_net_send_line(net, line);
		ggz_free(line);
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
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>");
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
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>");
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
	int room;

	room = player_get_room(net->client->data);
	
	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>");
	_net_send_line(net, "<LIST TYPE='player' ROOM='%d'>", room);
	return 0;
}


int net_send_player(GGZNetIO *net, GGZPlayer *p2)
{
	GGZPlayerType type;
	char *type_desc;
	
	type = player_get_type(p2);

	switch (type) {
	case GGZ_PLAYER_NORMAL:
		type_desc = "normal";
		break;
	case GGZ_PLAYER_GUEST:
		type_desc = "guest";
		break;
	case GGZ_PLAYER_ADMIN:
		type_desc = "admin";
		break;
	default:
		type_desc = "**none**";
		break;
	}

	/* FIXME: I coded lag_class the same way as table, but */
	/* shouldn't both of these values be locked/copied? */
	return _net_send_line(net, 
			      "<PLAYER ID='%s' TYPE='%s' TABLE='%d' LAG='%d'/>",
			      p2->name, type_desc, p2->table, p2->lag_class);
}


int _net_send_player_lag(GGZNetIO *net, GGZPlayer *p2)
{
	/* FIXME: I coded lag_class the same way as table, but */
	/* shouldn't both of these values be locked/copied? */
	return _net_send_line(net, 
			      "<PLAYER ID='%s' LAG='%d'/>",
			      p2->name, p2->lag_class);
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
	int room;

	room = player_get_room(net->client->data);

	_net_send_line(net, "<RESULT ACTION='list' CODE='0'>");
	_net_send_line(net, "<LIST TYPE='table' ROOM='%d'>", room);
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
	GGZPlayer *player;
	int room;

	room = player_get_room(net->client->data);
	
	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='delete' ROOM='%d'>", room);
		_net_send_line(net, "<PLAYER ID='%s'/>", name);
		_net_send_line(net, "</UPDATE>");
		break;
	case GGZ_UPDATE_ADD:
		/* This returns with player's write lock held, so drop it  */
		player = hash_player_lookup(name);
		if (!player) {
			err_msg("Player lookup failed!");
			return 0;
		}
		pthread_rwlock_unlock(&player->lock);
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='add' ROOM='%d'>", room);
		net_send_player(net, player);
		_net_send_line(net, "</UPDATE>");
		break;
	case GGZ_UPDATE_LAG:
		/* This returns with player's write lock held, so drop it  */
		player = hash_player_lookup(name);
		if (!player) {
			err_msg("Player lookup failed!");
			return 0;
		}
		pthread_rwlock_unlock(&player->lock);
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='lag' ROOM='%d'>", room);
		_net_send_player_lag(net, player);
		_net_send_line(net, "</UPDATE>");
		break;
	}
	
	return 0;
}


int net_send_table_update(GGZNetIO *net, GGZUpdateOpcode opcode, GGZTable *table, int seat)
{
	char *action = NULL;
	int room;

	room = player_get_room(net->client->data);

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
	case GGZ_UPDATE_DESC:
		action = "desc";
		break;
	case GGZ_UPDATE_SEAT:
		action = "seat";
		break;
	default:
		/* We should never get any other update types */
		return -1;
	}

	/* Always send opcode */
	_net_send_line(net, "<UPDATE TYPE='table' ACTION='%s' ROOM='%d'>",
		       action, room);

	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		_net_send_table_status(net, table);
		break;
	case GGZ_UPDATE_ADD:
		net_send_table(net, table);
		break;
	case GGZ_UPDATE_LEAVE:
	case GGZ_UPDATE_JOIN:
	case GGZ_UPDATE_SEAT:
		_net_send_table_seat(net, table, seat);
		break;
	case GGZ_UPDATE_STATE:
		_net_send_table_status(net, table);
		break;
	case GGZ_UPDATE_DESC:
		_net_send_table_desc(net, table);
		break;
	default:
		/* We should never get any other update types */
		return -1;
	}
	
	_net_send_line(net, "</UPDATE>");

	return 0;
}


int net_send_update_result(GGZNetIO *net, char status)
{
	return _net_send_result(net, "update", status);
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


int net_send_ping(GGZNetIO *net)
{
	_net_send_line(net, "<PING/>");
	return 0;
}


/* Check for incoming data */
int net_data_is_pending(GGZNetIO *net)
{
	int pending = 0;
	struct pollfd fd[1] = {{net->fd, POLLIN, 0}};

	if (net && net->fd != -1) {
	
	dbg_msg(GGZ_DBG_CONNECTION, "Checking for net events");	
	if ( (pending = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			pending = 0;
		else 
			err_sys_exit("poll failed in ggzcore_server_data_is_pending");
	}
	else if (pending)
		dbg_msg(GGZ_DBG_CONNECTION, "Found a net event!");
	}

	return pending;
}


/* Read in a bit more from the server and send it to the parser */
GGZPlayerHandlerStatus net_read_data(GGZNetIO *net)
{
	char *buf;
	int len, done;

	/* dbg_msg(GGZ_DBG_XML, "Parsing...");*/

	/* We're already in a parse call, and XML parsing is *not* reentrant */
	if (net->parsing) 
		return GGZ_REQ_OK;

	/* Set flag in case we get called recursively */
	net->parsing = 1;

	/* Get a buffer to hold the data */
	if (!(buf = XML_GetBuffer(net->parser, BUFFSIZE)))
		err_sys_exit("Couldn't allocate buffer");

	/* Read in data from socket */
	if ( (len = read(net->fd, buf, BUFFSIZE)) < 0) {
		
		/* If it's a non-blocking socket and there isn't data,
                   we get EAGAIN.  It's safe to just return */
		if (errno == EAGAIN) {
			net->parsing = 0;
			return GGZ_REQ_OK;
		}

		dbg_msg(GGZ_DBG_CONNECTION, "Network error reading data");
		return GGZ_REQ_DISCONNECT;
	}

	_net_dump_data(net, buf, len);
	
	/* If len == 0 then we've reached EOF */
	done = (len == 0);

	/* If client disconnected..*/
	if (done) {
		dbg_msg(GGZ_DBG_CONNECTION, "Client from %s disconnected", 
			net->client->addr);
	}
	else if (!XML_ParseBuffer(net->parser, len, done)) {
		dbg_msg(GGZ_DBG_XML, "Parse error at line %d, col %d:%s",
			XML_GetCurrentLineNumber(net->parser),
			XML_GetCurrentColumnNumber(net->parser),
			XML_ErrorString(XML_GetErrorCode(net->parser)));

		_net_send_result(net, "protocol", E_BAD_XML);
		done = 1;
	}
	
	/* If we saw any tags clear the byte counter, otherwise increment it*/
	if (net->tag_seen) {
		net->byte_count = 0;
		net->tag_seen = 0;
	}
	else {
		net->byte_count += len;
		/* If we haven't seen a tag in a while, it's an error */
		if (net->byte_count > MAX_CHAT_LEN) {
			dbg_msg(GGZ_DBG_XML, "Error: player overflowed XML buffer");
			
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
			done = 1;
		}
	}
	   

	/* Clear the flag now that we've completed this round of parsing */
	net->parsing = 0;

	return (done ? GGZ_REQ_DISCONNECT: GGZ_REQ_OK);
}


/********** Callbacks for XML parser **********/
static void _net_parse_start_tag(void *data, const char *el, const char **attr)
{
	GGZNetIO *net = (GGZNetIO*)data;
	GGZXMLElement *element;

	dbg_msg(GGZ_DBG_XML, "New %s element", el);
	
	/* Create new element object */
	element = _net_new_element((char*)el, (char**)attr);

	/* Put element on stack so we can process its children */
	ggz_stack_push(net->stack, element);

	/* Mark that we've seen a tag */
	net->tag_seen = 1;
}


static void _net_parse_end_tag(void *data, const char *el)
{
	GGZNetIO *net = (GGZNetIO*)data;
	GGZXMLElement *element;
	
	/* Pop element off stack */
	element = ggz_stack_pop(net->stack);

	/* Process tag */
	dbg_msg(GGZ_DBG_XML, "Handling %s element", element->tag);
	if (element->process)
		element->process(net, element);

	/* Free data structures */
	ggz_xmlelement_free(element);

	/* Mark that we've seen a tag */
	net->tag_seen = 1;
}


static void _net_parse_text(void *data, const char *text, int len) 
{
	GGZNetIO *net = (GGZNetIO*)data;
	GGZXMLElement *top;

	top = ggz_stack_top(net->stack);
	ggz_xmlelement_add_text(top, text, len);
}


static void _net_dump_data(GGZNetIO *net, char *data, int size)
{
	if (net->dump_file > 0)
		write(net->dump_file, data, size);
}

static GGZXMLElement* _net_new_element(char *tag, char **attrs)
{
	void (*process_func)();

	if (strcmp(tag, "SESSION") == 0)
		process_func = _net_handle_session;
	else if (strcmp(tag, "LOGIN") == 0)
		process_func = _net_handle_login;
	else if (strcmp(tag, "CHANNEL") == 0)
		process_func = _net_handle_channel;
	else if (strcmp(tag, "NAME") == 0)
		process_func = _net_handle_name;
	else if (strcmp(tag, "PASSWORD") == 0)
		process_func = _net_handle_password;
	else if (strcmp(tag, "UPDATE") == 0)
		process_func = _net_handle_update;
	else if (strcmp(tag, "LIST") == 0)
		process_func = _net_handle_list;
	else if (strcmp(tag, "ENTER") == 0)
		process_func = _net_handle_enter;
	else if (strcmp(tag, "CHAT") == 0)
		process_func = _net_handle_chat;
	else if (strcmp(tag, "JOIN") == 0)
		process_func = _net_handle_join;
	else if (strcmp(tag, "LEAVE") == 0)
		process_func = _net_handle_leave;
	else if (strcmp(tag, "LAUNCH") == 0)
		process_func = _net_handle_launch;
	else if (strcmp(tag, "TABLE") == 0)
		process_func = _net_handle_table;
	else if (strcmp(tag, "SEAT") == 0)
		process_func = _net_handle_seat;
	else if (strcmp(tag, "DESC") == 0)
		process_func = _net_handle_desc;
	else if (strcmp(tag, "MOTD") == 0)
		process_func = _net_handle_motd;
	else if (strcmp(tag, "DATA") == 0)
		process_func = _net_handle_data;
	else if (strcmp(tag, "PONG") == 0)
		process_func = _net_handle_pong;
	else
		process_func = NULL;
	
	return ggz_xmlelement_new(tag, attrs, process_func, NULL);
}


/* Functions for <SESSION> tag */
static void _net_handle_session(GGZNetIO *net, GGZXMLElement *session)
{
	if (net->client->type == GGZ_CLIENT_PLAYER)
		logout_player(net->client->data);
	client_end_session(net->client);
}


/* Functions for <CHANNEL> tag */
static void _net_handle_channel(GGZNetIO *net, GGZXMLElement *channel)
{
	char *id;

	if (channel) {
		/* Grab table ID from tag */
		id = ggz_xmlelement_get_attr(channel, "ID");

		/* It's an error if they didn't send an ID string */
		if (!id) {
			_net_send_result(net, "channel", E_BAD_OPTIONS);
			return;
		}

		/* FIXME: Do validity checking on the channel ID here */

		/* FIXME: perhaps lookup table and point net->client->data at it */
		
		client_set_type(net->client, GGZ_CLIENT_CHANNEL);
	}
}


/* Functions for <LOGIN> tag */
static void _net_handle_login(GGZNetIO *net, GGZXMLElement *login)
{
	GGZLoginType login_type;
	GGZAuthData *auth;	
	char *type;


	if (login) {
		type = ggz_xmlelement_get_attr(login, "TYPE");

		/* If they didn't send TYPE, it's an error */
		if (!type) {
			_net_send_result(net, "login", E_BAD_OPTIONS);
			return;
		}

		if (strcmp(type, "normal") == 0)
			login_type = GGZ_LOGIN;
		else if (strcmp(type, "guest") == 0)
			login_type = GGZ_LOGIN_GUEST;
		else if (strcmp(type, "first") == 0)
			login_type = GGZ_LOGIN_NEW;
		else {
			_net_send_result(net, "login", E_BAD_OPTIONS);
			return;
		}
		
		auth = ggz_xmlelement_get_data(login);
		
		/* It's an error if they didn't send the right data */
		if (!auth || !auth->name
		    || (!auth->password && login_type == GGZ_LOGIN)) {
			_net_send_result(net, "login", E_BAD_OPTIONS);
			return;
		}
		
		/* Can't login twice */
		if (net->client->data) {
			dbg_msg(GGZ_DBG_CONNECTION, "Client from %s attempted to log in again", 
				net->client->addr);
			net_send_login(net, login_type, E_ALREADY_LOGGED_IN, NULL);
		}
		else {
			client_set_type(net->client, GGZ_CLIENT_PLAYER);
			login_player(login_type, net->client->data, auth->name, auth->password);
			
			/* FIXME: this is a hack so that the player loop will start */
			client_end_session(net->client);
		}

		/* Free up any resources we allocated */
		_net_authdata_free(auth);
	}
}


static GGZAuthData* _net_authdata_new(void)
{
	GGZAuthData *data;

	if ( (data = malloc(sizeof(GGZAuthData))) == NULL)
		err_sys_exit("malloc error in net_authdata_new()");
	
	data->name = NULL;
	data->password = NULL;
	
	return data;
}


static void _net_auth_set_name(GGZXMLElement *auth, char *name)
{
	 GGZAuthData *data;
	
	data = ggz_xmlelement_get_data(auth);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_authdata_new();
		ggz_xmlelement_set_data(auth, data);
	}
	
	data->name = name;
}


static void _net_auth_set_password(GGZXMLElement *auth, char *password)
{
	GGZAuthData *data;
	
	data = ggz_xmlelement_get_data(auth);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_authdata_new();
		ggz_xmlelement_set_data(auth, data);
	}
	
	data->password = password;
}


static void _net_authdata_free(GGZAuthData *data)
{
	if (data) {
		if (data->name)
			free(data->name);
		if (data->password)
			free(data->password);
		free(data);
	}
}


/* Functions for <NAME> tag */
static void _net_handle_name(GGZNetIO *net, GGZXMLElement *element)
{
	char *name;
	char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	/* If there are no elements above us, it's protocol error */
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (element) {
		name = safe_strdup(ggz_xmlelement_get_text(element));
		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LOGIN") == 0)
			_net_auth_set_name(parent, name);
		else 
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
	}
}


/* Functions for <PASSWORD> tag */
static void _net_handle_password(GGZNetIO *net, GGZXMLElement *element)
{
	char *password;
	char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	/* If there are no elements above us, it's protocol error */
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (element) {
		password = safe_strdup(ggz_xmlelement_get_text(element));
		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LOGIN") == 0)
			_net_auth_set_password(parent, password);
		else 
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
	}
}


/* Functions for <UPDATE> tag */
static void _net_handle_update(GGZNetIO *net, GGZXMLElement *update)
{
	char *type, *room;
	GGZTable *table;
	GGZPlayer *player;

	if (update) {

		/* Grab update data from tag */
		type = ggz_xmlelement_get_attr(update, "TYPE");

		if (strcmp(type, "player") == 0) {
			if (!(player = ggz_xmlelement_get_data(update))) {
				_net_send_result(net, "protocol", E_BAD_OPTIONS);
				return;
			}

			/* FIXME: update password and other data */
		}
		else if (strcmp(type, "table") == 0) {
			if (!(table = ggz_xmlelement_get_data(update))) {
				_net_send_result(net, "protocol", E_BAD_OPTIONS);
				return;
			}

			/* If room was specified, override default current room */
			if ( (room = ggz_xmlelement_get_attr(update, "ROOM")))
				table->room = safe_atoi(room);
			
			player_table_update(net->client->data, table);
		}
	}
}


/* Functions for <LIST> tag */
static void _net_handle_list(GGZNetIO *net, GGZXMLElement *list)
{
	char *type, *full;
	char verbose = 0;
	
	if (list) {
		type = ggz_xmlelement_get_attr(list, "TYPE");
		
		if (!type) {
			_net_send_result(net, "list", E_BAD_OPTIONS);
			return;
		}
		
		full = ggz_xmlelement_get_attr(list, "FULL");
		if (full && strcmp(full, "true") == 0)
			verbose = 1;
		
		if (strcmp(type, "game") == 0)
			player_list_types(net->client->data, verbose);
		else if (strcmp(type, "room") == 0)
			/* FIXME: Currently send all types */
			room_list_send(net->client->data, -1, verbose);
		else if (strcmp(type, "player") == 0)
			player_list_players(net->client->data);
		else if (strcmp(type, "table") == 0)
			/* FIXME: Currently send all local types */
			player_list_tables(net->client->data, GGZ_TYPE_ALL, 0);
		else
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
	}
}


/* Functions for <ENTER> tag */
static void _net_handle_enter(GGZNetIO *net, GGZXMLElement *enter)
{
	int room;

	if (enter) {
		room = safe_atoi(ggz_xmlelement_get_attr(enter, "ROOM"));
		room_handle_join(net->client->data, room);
	}
}


/* Functions for <CHAT> tag */
static void _net_handle_chat(GGZNetIO *net, GGZXMLElement *chat)
{
	char *type, *to, *msg;
	unsigned char op = 0;
		
	if (chat) {

		/* Grab chat data from tag */
		type = ggz_xmlelement_get_attr(chat, "TYPE");
		to = ggz_xmlelement_get_attr(chat, "TO");
		msg = ggz_xmlelement_get_text(chat);

		/* TYPE is required */
		if (!type) {
			_net_send_result(net, "chat", E_BAD_OPTIONS);
			return;
		}
			
		/* FIXME: error checking on these? */
		
		if (strcmp(type, "normal") == 0)
			op = GGZ_CHAT_NORMAL;
		else if (strcmp(type, "private") == 0)
			op = GGZ_CHAT_PERSONAL;
		else if (strcmp(type, "announce") == 0)
			op = GGZ_CHAT_ANNOUNCE;
		else if (strcmp(type, "beep") == 0)
			op = GGZ_CHAT_BEEP;
		else {
			_net_send_result(net, "chat", E_BAD_OPTIONS);
		}

		player_chat(net->client->data, op, to, msg);
	}
}


/* Functions for <JOIN> tag */
static void _net_handle_join(GGZNetIO *net, GGZXMLElement *element)
{
	int table;

	if (element) {
		table = safe_atoi(ggz_xmlelement_get_attr(element, "TABLE"));
		player_table_join(net->client->data, table);
	}
}


/* Functions for <LEAVE> tag */
static void _net_handle_leave(GGZNetIO *net, GGZXMLElement *element)
{
	char *att;
	char force = 0;
	
	if (element) {
		att = ggz_xmlelement_get_attr(element, "FORCE");
		if (att && strcmp(att, "true") == 0)
			force = 1;
		player_table_leave(net->client->data, force);
	}
}


/* Functions for <LAUNCH> tag */
static void _net_handle_launch(GGZNetIO *net, GGZXMLElement *element)
{
	GGZTable *table;

	if (element) {
		table = ggz_xmlelement_get_data(element);
		if (!table) {
			_net_send_result(net, "launch", E_BAD_OPTIONS);
			return;
		}
		
		player_table_launch(net->client->data, table);
	}
}


/* Functions for <TABLE> tag */
static void _net_handle_table(GGZNetIO *net, GGZXMLElement *element)
{
	int type, index, db_status;
	GGZTable *table;
	GGZTableData *data;
	GGZSeatData *seat;
	char *desc = NULL;
	GGZList *seats = NULL;
	GGZListEntry *entry;
	char *parent_tag;
	GGZXMLElement *parent;
	ggzdbPlayerEntry player;

	parent = ggz_stack_top(net->stack);
	
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	/* Get table data from tag */
	type = safe_atoi(ggz_xmlelement_get_attr(element, "GAME"));

	/* For updates, they might have specified the ID and room */
	index = safe_atoi(ggz_xmlelement_get_attr(element, "ID"));
		
	data = ggz_xmlelement_get_data(element);
	if (data) {
		desc = data->desc;
		seats = data->seats;
	}
	
	/* Create and init new table */
	table = table_new();
	table->index = index;
	table->type = type;

	/* If room was specified, use it, otherwise use players current room */
	table->room = player_get_room(net->client->data);
	
	if (desc)
		snprintf(table->desc, sizeof(table->desc), "%s", desc);
	
	/* Add seats */
	entry = ggz_list_head(seats);
	while (entry) {
		GGZSeatType seat_type;
		
		seat = ggz_list_get_data(entry);
		seat_type = ggz_string_to_seattype(seat->type);

		switch (seat_type) {
		case GGZ_SEAT_OPEN:		
		case GGZ_SEAT_BOT:
		case GGZ_SEAT_NONE:
			break;
		case GGZ_SEAT_RESERVED:
			/* We verify that this is a real,
			   registered player name. */
			snprintf(player.handle, MAX_USER_NAME_LEN+1,
				 "%s", seat->name ? seat->name : "");
			db_status = ggzdb_player_get(&player);
			if (db_status == 0) {
				strcpy(table->seat_names[seat->index],
				       seat->name);
			} else {
				/* This is some kind of error...but for now we
				   just cover it up. */
				dbg_msg(GGZ_DBG_TABLE, "Invalid name '%s' sent for reserved seat.  Changing it to an open seat.", seat->name);
				seat_type = GGZ_SEAT_OPEN;
			}
			break;
		case GGZ_SEAT_PLAYER:
			/* A bad client might send this...how should
			   we deal with it? */
			err_msg("Client launched table with GGZ_SEAT_PLAYER seat.");
			seat_type = GGZ_SEAT_NONE;
			break;
		}
		table->seat_types[seat->index] = seat_type;
		
		entry = ggz_list_next(entry);
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
 	if (strcmp(parent_tag, "LAUNCH") == 0
	    || strcmp(parent_tag, "UPDATE") == 0 ) {
		ggz_xmlelement_set_data(parent, table);
	}
	else
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
	
	if (data)
		_net_tabledata_free(data);
}


static void _net_table_add_seat(GGZXMLElement *table, GGZSeatData *seat)
{
	struct _GGZTableData *data;
	
	data = ggz_xmlelement_get_data(table);
	
	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}

	ggz_list_insert(data->seats, seat);
}


static void _net_table_set_desc(GGZXMLElement *table, char *desc)
{
	struct _GGZTableData *data;
	
	data = ggz_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}
	
	data->desc = desc;
}


static GGZTableData* _net_tabledata_new(void)
{
	struct _GGZTableData *data;

	if ( (data = malloc(sizeof(struct _GGZTableData))) == NULL)
		err_sys_exit("malloc error in net_tabledata_new()");
	
	data->desc = NULL;
	data->seats = ggz_list_create(NULL, 
				  _net_seat_copy, 
				  (ggzEntryDestroy)_net_seat_free, 
				  GGZ_LIST_ALLOW_DUPS);
	
	return data;
}


static void _net_tabledata_free(GGZTableData *data)
{
	if (data) {
		if (data->desc)
			free(data->desc);
		if (data->seats)
			ggz_list_free(data->seats);
		free(data);
	}
}




/* Functions for <SEAT> tag */
static void _net_handle_seat(GGZNetIO *net, GGZXMLElement *element)
{
	struct _GGZSeatData seat;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (strcmp(parent->tag, "TABLE") != 0) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (element) {
		/* Get seat information out of tag */
		seat.index = safe_atoi(ggz_xmlelement_get_attr(element, "NUM"));
		seat.type = ggz_xmlelement_get_attr(element, "TYPE");
		seat.name = ggz_xmlelement_get_text(element);
		_net_table_add_seat(parent, &seat);
	}
}


static void* _net_seat_copy(void *data)
{
	struct _GGZSeatData *seat1, *seat2;

	seat1 = (GGZSeatData*)data;

	seat2 = calloc(1, sizeof(struct _GGZSeatData));

	seat2->index = seat1->index;
	seat2->type = safe_strdup(seat1->type);
	seat2->name = safe_strdup(seat1->name);

	return seat2;
}


static void _net_seat_free(GGZSeatData *seat)
{
	if (seat) {
		if (seat->type)
			free(seat->type);
		if (seat->name)
			free(seat->name);
		free(seat);
	}
}



/* Functions for <DESC> tag */
static void _net_handle_desc(GGZNetIO *net, GGZXMLElement *element)
{
	char *desc = NULL;
	char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (element) {
		if (ggz_xmlelement_get_text(element))
			desc = safe_strdup(ggz_xmlelement_get_text(element));
		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "TABLE") == 0)
			_net_table_set_desc(parent, desc);
		else
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
	}
}


/* Functions for <MOTD> tag */
static void _net_handle_motd(GGZNetIO *net, GGZXMLElement *motd)
{
	player_motd(net->client->data);
}


/* Functions for <DATA> tag */
static void _net_handle_data(GGZNetIO *net, GGZXMLElement *data)
{
	int i, size;
	char buffer[4096];
	char *msg;
	char *token;

	if (data) {

		/* Grab data from tag */
		size = safe_atoi(ggz_xmlelement_get_attr(data, "SIZE"));
		msg = ggz_xmlelement_get_text(data);

		if (!msg) {
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
			return;
		}
		
		/* check for possible buffer overflows */
		/* FIXME: this is a difficult issue.  Limiting buffer length to
		   4096 characters will break any games that send packets longer
		   than this (unless they are broken up at the client end).  It
		   is tempting just to malloc a buffer of appropriate size so
		   that the data will always fit...but that's not a reasonable
		   solution either, since it can be very easily abused.  In any
		   case, once direct connections are used this won't be an
		   issue.  --JDS */
		if (size >= sizeof(buffer) ) {
			dbg_msg(GGZ_DBG_XML, "Error: player overflowed XML buffer");
			_net_send_result(net, "protocol", E_BAD_OPTIONS);
			return;
		}
		
		token = strtok(msg, " ");
		for (i = 0; i < size; i++) {
			buffer[i] = safe_atoi(token);
			token = strtok(NULL, " ");
		}
		
		player_msg_from_sized(net->client->data, size, buffer);
	}
}


/* Function for <PONG> tag */
static void _net_handle_pong(GGZNetIO *net, GGZXMLElement *data)
{
	player_handle_pong(net->client->data);
}


/************ Utility/Convenience functions *******************/

static int safe_atoi(char *string)
{
	if (!string)
		return 0;
	else
		return atoi(string);
}


static char* safe_strdup(char *str)
{
	return str ? strdup(str) : NULL;
}


static int _net_send_table_seat(GGZNetIO *net, GGZTable *table, int seat)
{
	_net_send_line(net, "<TABLE ID='%d' SEATS='%d'>", table->index, 
		       seats_num(table));
	_net_send_seat(net, table, seat);
	_net_send_line(net, "</TABLE>");
	
	return 0;
}


static int _net_send_table_desc(GGZNetIO *net, GGZTable *table)
{
	_net_send_line(net, "<TABLE ID='%d'>", table->index);
	_net_send_line(net, "<DESC>%s</DESC>", table->desc);
	_net_send_line(net, "</TABLE>");
	
	return 0;
}


static int _net_send_table_status(GGZNetIO *net, GGZTable *table)
{
	return _net_send_line(net, "<TABLE ID='%d' STATUS='%d' SEATS='%d'/>", 
			      table->index, table->state, seats_num(table));
}


static int _net_send_seat(GGZNetIO *net, GGZTable *table, int num)
{
	GGZSeatType type = seats_type(table, num);
	char *type_str = ggz_seattype_to_string(type);
	char *name = NULL;

#if 0
	/* We should do it this way, since the ggzdmod field doesn't
	   get correctly initialized we can crash inside it. */
	name = ggzdmod_get_seat(table->ggzdmod, num).name;
#else /* #if 0 */
	switch (type) {
	case GGZ_SEAT_RESERVED:
	case GGZ_SEAT_PLAYER:
		name = table->seat_names[num];
		break;
	case GGZ_SEAT_OPEN:
	case GGZ_SEAT_BOT:
	case GGZ_SEAT_NONE:
		name = NULL;
		break;
	}
#endif /* #if 0 */
	
	if (name)
		_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'>%s</SEAT>",
			       num, type_str, name);
	else
		_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'/>", 
			       num, type_str);

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
