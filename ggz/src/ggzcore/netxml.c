/*
 * File: netxml.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for parsing XML streamed from the server
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
#include "player.h"
#include "room.h"
#include "state.h"
#include "table.h"
#include "gametype.h"
#include "stack.h"
#include "xmlelement.h"

#include <easysock.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <expat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

/* For convenience */
#define BUFFSIZE 8192



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

	/* Maximum chat size allowed */
	unsigned int chat_size;

	/* Room to which we are transitioning */
	struct _GGZRoom *new_room;

	/* Room verbosity (need to save) */
	char room_verbose;

	/* Gametype verbosity (need to save) */
	char gametype_verbose;

	/* XML Parser */
	XML_Parser parser;

	/* Message parsing stack */
	GGZStack *stack;

	/* File to dump protocol session */
	int dump_file;

};


/* Callbacks for XML parser */
static void _net_parse_start_tag(void *, const char*, const char **);
static void _net_parse_end_tag(void *data, const char *el);
static void _net_parse_text(void *data, const char *text, int len);

/* Trigger network error event */
static void _ggzcore_net_error(struct _GGZNet *net, char* message);

/* Dump network data to debugging file */
static void _net_dump_data(struct _GGZNet *net, char *data, int size);


/* Internal library functions (prototypes in net.h) */

struct _GGZNet* _ggzcore_net_new(void)
{
	struct _GGZNet *net;

	net = ggzcore_malloc(sizeof(struct _GGZNet));
	
	/* Set fd to invalid value */
	net->fd = -1;
	net->dump_file = -1;
	
	return net;
}


void _ggzcore_net_init(struct _GGZNet *net, struct _GGZServer *server, const char *host, unsigned int port)
{
	net->server = server;
	net->host = ggzcore_strdup(host);
	net->port = port;
	net->fd = -1;

	/* Init parser */
	if (!(net->parser = XML_ParserCreate("UTF-8")))
		ggzcore_error_sys_exit("Couldn't allocate memory for XML parser");

	/* Setup handlers for tags */
	XML_SetElementHandler(net->parser, _net_parse_start_tag, 
			      _net_parse_end_tag);
	XML_SetCharacterDataHandler(net->parser, _net_parse_text);
	XML_SetUserData(net->parser, net);

	/* Initialize stack for messages */
	net->stack = _ggzcore_stack_new();
}


int _ggzcore_net_set_dump_file(struct _GGZNet *net, const char* filename)
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


GGZServer *  _ggzcore_net_get_server(struct _GGZNet *net)
{
	return net->server;
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


GGZRoom* _ggzcore_net_get_new_room(struct _GGZNet *net)
{
	return net->new_room;
}

/* For debugging purposes only! */
void _ggzcore_net_set_fd(struct _GGZNet *net, int fd)
{
	net->fd = fd;
}

/* FIXME: should we do this here or as part of a protocol object? */
void _ggzcore_net_set_chat_size(struct _GGZNet *net, int size)
{
	net->chat_size = size;
}

void _ggzcore_net_free(struct _GGZNet *net)
{
	if (net->host)
		ggzcore_free(net->host);

	if (net->stack)
		_ggzcore_stack_free(net->stack);

	if (net->parser)
		XML_ParserFree(net->parser);

	ggzcore_free(net);
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


void _ggzcore_net_disconnect(struct _GGZNet *net)
{
	ggzcore_debug(GGZ_DBG_NET, "Disconnecting");
	close(net->fd);
	net->fd = -1;
}


/* ggzcore_net_send_XXX() functions for sending messages to the server */

int _ggzcore_net_send_login(struct _GGZNet *net)
{
	GGZLoginType type;
	char *handle, *password;
	int status = 0;

	type = _ggzcore_server_get_type(net->server);
	handle = _ggzcore_server_get_handle(net->server);
	password = _ggzcore_server_get_password(net->server);
	
	switch (type) {
	case GGZ_LOGIN:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN");
		if (es_write_int(net->fd, REQ_LOGIN) < 0
		    || es_write_string(net->fd, handle) < 0
		    || es_write_string(net->fd, password) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_GUEST:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_GUEST");
		if (es_write_int(net->fd, REQ_LOGIN_ANON) < 0
		    || es_write_string(net->fd, handle) < 0)
			status = -1;
		break;
	case GGZ_LOGIN_NEW:
		ggzcore_debug(GGZ_DBG_NET, "Executing net login: GGZ_LOGIN_NEW");
		if (es_write_int(net->fd, REQ_LOGIN_NEW) < 0
		    || es_write_string(net->fd, handle) < 0)
			status = -1;
		break;
	}
	
	if (status < 0)
		_ggzcore_net_error(net, "Sending login");

	return status;
}


int _ggzcore_net_send_motd(struct _GGZNet *net)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_MOTD");	
	status = es_write_int(net->fd, REQ_MOTD);
	if (status < 0)
		_ggzcore_net_error(net, "Sending motd request");

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

	if (status < 0)
		_ggzcore_net_error(net, "Sending typelist request");

	return status;
}


int _ggzcore_net_send_list_rooms(struct _GGZNet *net, const int type, const char verbose)
{	
	int status = 0;
	
	net->room_verbose = verbose;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_ROOMS");	
	if (es_write_int(net->fd, REQ_LIST_ROOMS) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_char(net->fd, verbose) < 0)
		status = -1;

	if (status < 0)
		_ggzcore_net_error(net, "Sending roomlist request");

	return status;
}


int _ggzcore_net_send_join_room(struct _GGZNet *net, const unsigned int id)
{
	int status = 0;
	struct _GGZRoom *room;

	room = _ggzcore_server_get_room_by_id(net->server, id);

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_ROOM_JOIN");	
	if (es_write_int(net->fd, REQ_ROOM_JOIN) < 0
	    || es_write_int(net->fd, id) < 0)
		status = -1;
	
	if (status < 0)
		_ggzcore_net_error(net, "Sending room join request");
	else 
		net->new_room = room;

	return status;
}


int _ggzcore_net_send_list_players(struct _GGZNet *net)
{	
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_PLAYERS");	
	status = es_write_int(net->fd, REQ_LIST_PLAYERS);
	if (status < 0)
		_ggzcore_net_error(net, "Sending playerlist request");

	return status;
}


int _ggzcore_net_send_list_tables(struct _GGZNet *net, const int type, const char global)
{	
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_TABLES");	
	if (es_write_int(net->fd, REQ_LIST_TABLES) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_char(net->fd, global) < 0)
		status = -1;

	if (status < 0)
		_ggzcore_net_error(net, "Sending tablelist request");

	return status;
}


int _ggzcore_net_send_chat(struct _GGZNet *net, const GGZChatOp op, const char* player, const char* msg)
{
	int status = 0;
	char msg_buf[net->chat_size];

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_CHAT");	
	if (es_write_int(net->fd, REQ_CHAT) < 0
	    || es_write_char(net->fd, op) < 0)
		status = -1;
	
	if (status == 0 && (op & GGZ_CHAT_M_PLAYER))
		status = es_write_string(net->fd, player);

	/* We don't want to send messages longer than the server will accept.
	 * TODO: We may want to split it up into segments to send?  -- JDS */
	snprintf(msg_buf, sizeof(msg_buf), "%s", msg);
	
	if (status == 0 && (op & GGZ_CHAT_M_MESSAGE))
		status = es_write_string(net->fd, msg_buf);
	
	if (status < 0)
		_ggzcore_net_error(net, "Sending chat");

	return status;
}


int _ggzcore_net_send_table_launch(struct _GGZNet *net, const int type, char *desc, const int num_seats)
{
	int status = 0;
	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_LAUNCH");
	if (es_write_int(net->fd, REQ_TABLE_LAUNCH) < 0
	    || es_write_int(net->fd, type) < 0
	    || es_write_string(net->fd, desc) < 0
	    || es_write_int(net->fd, num_seats) < 0)
		status = -1;

	if (status < 0)
		_ggzcore_net_error(net, "Sending table launch");

	return status;
}


int _ggzcore_net_send_seat(struct _GGZNet *net, GGZSeatType seat, char *name)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending seat info");
	status = es_write_int(net->fd, seat);
	
	if (status > 0) {
		switch (seat) {
		case GGZ_SEAT_PLAYER:
		case GGZ_SEAT_RESERVED:
			status = es_write_string(net->fd, name);
			break;
		case GGZ_SEAT_BOT:
			break;
		default:
			break;
		}
	}
	
	if (status < 0)
		_ggzcore_net_error(net, "Sending seat info");

	return status;
}


int _ggzcore_net_send_table_join(struct _GGZNet *net, const unsigned int num)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_JOIN");
	if (es_write_int(net->fd, REQ_TABLE_JOIN) < 0
	    || es_write_int(net->fd, num) < 0)
		status = -1;

	if (status < 0)
		_ggzcore_net_error(net, "Sending table join");

	return status;
}


int _ggzcore_net_send_table_leave(struct _GGZNet *net)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_TABLE_LEAVE");
	status = es_write_int(net->fd, REQ_TABLE_LEAVE);
	if (status < 0)
		_ggzcore_net_error(net, "Sending table leave");

	return status;
}


int _ggzcore_net_send_game_data(struct _GGZNet *net, int size, char *buffer)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_GAME: %d bytes from game", size);
	if (es_write_int(net->fd, REQ_GAME) < 0
	    || es_write_int(net->fd, size) < 0
	    || es_writen(net->fd, buffer, size) < 0)
		status = -1;

	if (status < 0)
		_ggzcore_net_error(net, "Sending game data");

	return status;
}


int _ggzcore_net_send_logout(struct _GGZNet *net)
{
	int status = 0;

	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LOGOUT");	
	status = es_write_int(net->fd, REQ_LOGOUT);

	if (status < 0)
		_ggzcore_net_error(net, "Sending logout request");
	
	return status;
}


/* Check for incoming data */
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


/* Read in a bit more from the server and send it to the parser */
int _ggzcore_net_read_data(struct _GGZNet *net)
{
	char *buf;
	int len, done;
	/* Get a buffer to hold the data */
	if (!(buf = XML_GetBuffer(net->parser, BUFFSIZE)))
		ggzcore_error_sys_exit("Couldn't allocate buffer");

	/* Read in data from socket */
	if ( (len = read(net->fd, buf, BUFFSIZE)) < 0) {
		
		/* If it's a non-blocking socket and there isn't data,
                   we get EAGAIN.  It's safe to just return */
		if (errno == EAGAIN)
			return 0;

		_ggzcore_net_error(net, "Reading data from server");
	}

	_net_dump_data(net, buf, len);
	
	/* If len == 0 then we've reached EOF */
	done = (len == 0);
	if (done) {
		_ggzcore_net_disconnect(net);
		net->fd = -1;
		_ggzcore_server_set_logout_status(net->server, 1);
	}
	if (!XML_ParseBuffer(net->parser, len, done)) {
		fprintf(stderr, "Parse error at line %d:\n%s\n",
			XML_GetCurrentLineNumber(net->parser),
			XML_ErrorString(XML_GetErrorCode(net->parser)));
		_ggzcore_server_protocol_error(net->server, "Bad XML from server");
	}
	
	return done;
}


/********** Callback for XML parser **********/
static void _net_parse_start_tag(void *data, const char *el, const char **attr)
{
	struct _GGZNet *net = (struct _GGZNet*)data;
	GGZStack *stack = net->stack;
	GGZXMLElement *element;

	ggzcore_debug(GGZ_DBG_XML, "New %s element", el);
	
	/* Create new element object */
	element = _ggzcore_protocol_new_element((char*)el, (char**)attr);

	/* Put element on stack so we can process its children */
	_ggzcore_stack_push(stack, element);
}


static void _net_parse_end_tag(void *data, const char *el)
{
	struct _GGZNet *net = (struct _GGZNet*)data;
	GGZStack *stack = net->stack;
	GGZXMLElement *element, *head;
	
	/* Pop element off stack */
	element = _ggzcore_stack_pop(stack);

	/* Check top of stack */
	head = _ggzcore_stack_top(stack);
	
	/* Process tag */
	_ggzcore_protocol_process_element(element, head, net);

	/* Free data structures */
	_ggzcore_xmlelement_free(element);
}


static void _net_parse_text(void *data, const char *text, int len) 
{
	struct _GGZNet *net = (struct _GGZNet*)data;
	GGZStack *stack = net->stack;
	GGZXMLElement *top;

	top = _ggzcore_stack_top(stack);
	_ggzcore_xmlelement_add_text(top, text, len);
}


static void _ggzcore_net_error(struct _GGZNet *net, char* message)
{
	ggzcore_debug(GGZ_DBG_NET, "Network error: %s", message);
	_ggzcore_net_disconnect(net);
	_ggzcore_server_net_error(net->server, message);
}


static void _net_dump_data(struct _GGZNet *net, char *data, int size)
{
	if (net->dump_file > 0)
		write(net->dump_file, data, size);
}
