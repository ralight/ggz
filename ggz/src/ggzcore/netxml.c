/*
 * File: netxml.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 * $Id: netxml.c 4940 2002-10-17 21:23:47Z jdorje $
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
#include "protocol.h"
#include "player.h"
#include "room.h"
#include "state.h"
#include "table.h"
#include "gametype.h"

#include <ggz.h>
#include <ggz_common.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <expat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* For convenience */
#define XML_BUFFSIZE 8192


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

	/* Flag to indicate we're in a parse call */
	char parsing;

	/* XML Parser */
	XML_Parser parser;

	/* Message parsing stack */
	GGZStack *stack;

	/* File to dump protocol session */
	int dump_file;

};


/* Game data structure */
typedef struct _GGZGameData {
	char *prot_engine;
	char *prot_version;
	GGZNumberList player_allow_list;
	GGZNumberList bot_allow_list;
	int spectators_allow;
	char *desc;
	char *author;
	char *url;
} GGZGameData;	

/* Table data structure */
typedef struct _GGZTableData {
	char *desc;
	GGZList *seats;
} GGZTableData;


/* Callbacks for XML parser */
static void _ggzcore_net_parse_start_tag(void *, const char*, const char **);
static void _ggzcore_net_parse_end_tag(void *data, const char *el);
static void _ggzcore_net_parse_text(void *data, const char *text, int len);
static GGZXMLElement* _ggzcore_net_new_element(char *tag, char **attrs);

/* Handler functions for various tags */
static void _ggzcore_net_handle_server(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_options(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_motd(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_result(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_password(GGZNet *net, GGZXMLElement*);
static void _ggzcore_net_handle_list(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_update(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_game(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_protocol(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_allow(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_about(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_desc(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_room(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_player(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_table(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_seat(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_chat(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_ping(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_session(GGZNet*, GGZXMLElement*);

/* Extra functions fot handling data associated with specific tags */
static void _ggzcore_net_list_insert(GGZXMLElement*, void*);
static void _ggzcore_net_game_set_protocol(GGZXMLElement*, char*, char *);
static void _ggzcore_net_game_set_allowed(GGZXMLElement*,
					  GGZNumberList, GGZNumberList, int);
static void _ggzcore_net_game_set_info(GGZXMLElement*, char*, char *);
static void _ggzcore_net_game_set_desc(GGZXMLElement*, char*);
static void _ggzcore_net_table_add_seat(GGZXMLElement*, struct _GGZSeat*);
static void _ggzcore_net_player_update(GGZNet *net, GGZXMLElement *update, char *action);
static void _ggzcore_net_table_update(GGZNet *net, GGZXMLElement *update, char *action);
static void _ggzcore_net_table_set_desc(GGZXMLElement*, char*);
static GGZTableData* _ggzcore_net_tabledata_new(void);
static void _ggzcore_net_tabledata_free(GGZTableData*);
static struct _GGZSeat* _ggzcore_net_seat_copy(struct _GGZSeat *orig);
static void _ggzcore_net_seat_free(struct _GGZSeat*);

/* Trigger network error event */
static void _ggzcore_net_error(struct _GGZNet *net, char* message);

/* Dump network data to debugging file */
static void _ggzcore_net_dump_data(struct _GGZNet *net, char *data, int size);

/* Utility functions */
static int _ggzcore_net_send_table_seat(struct _GGZNet *net, struct _GGZSeat *seat);
static void _ggzcore_net_send_header(GGZNet *net);
static int _ggzcore_net_send_line(GGZNet *net, char *line, ...);
static int safe_atoi(char *string);



/* Internal library functions (prototypes in net.h) */

struct _GGZNet* _ggzcore_net_new(void)
{
	struct _GGZNet *net;

	net = ggz_malloc(sizeof(struct _GGZNet));
	
	/* Set fd to invalid value */
	net->fd = -1;
	net->dump_file = -1;
	
	return net;
}


void _ggzcore_net_init(struct _GGZNet *net, struct _GGZServer *server, const char *host, unsigned int port)
{
	net->server = server;
	net->host = ggz_strdup(host);
	net->port = port;
	net->fd = -1;

	/* Init parser */
	if (!(net->parser = XML_ParserCreate("UTF-8")))
		ggz_error_sys_exit("Couldn't allocate memory for XML parser");

	/* Setup handlers for tags */
	XML_SetElementHandler(net->parser, 
			      _ggzcore_net_parse_start_tag, 
			      _ggzcore_net_parse_end_tag);
	XML_SetCharacterDataHandler(net->parser, _ggzcore_net_parse_text);
	XML_SetUserData(net->parser, net);

	/* Initialize stack for messages */
	net->stack = ggz_stack_new();
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


/* For debugging purposes only! */
void _ggzcore_net_set_fd(struct _GGZNet *net, int fd)
{
	net->fd = fd;
}

void _ggzcore_net_free(struct _GGZNet *net)
{
	GGZXMLElement *element;

	if (net->fd >= 0)
		_ggzcore_net_disconnect(net);
	
	if (net->host)
		ggz_free(net->host);

	/* Clear elements off stack and free it */
	if (net->stack) {
		while ( (element = ggz_stack_pop(net->stack))) 
			ggz_xmlelement_free(element);
		ggz_stack_free(net->stack);
	}

	if (net->parser)
		XML_ParserFree(net->parser);

	ggz_free(net);
}


/* FIXME: set a timeout for connecting */
int _ggzcore_net_connect(struct _GGZNet *net)
{
	ggz_debug(GGZCORE_DBG_NET, "Connecting to %s:%d",
		  net->host, net->port);
	net->fd = ggz_make_socket(GGZ_SOCK_CLIENT, net->port, net->host);
	
	if (net->fd >= 0)
		return 0;  /* success */
	else
		return -1; /* error */
}


void _ggzcore_net_disconnect(struct _GGZNet *net)
{
	ggz_debug(GGZCORE_DBG_NET, "Disconnecting");
	close(net->fd);
	net->fd = -1;
}


/* ggzcore_net_send_XXX() functions for sending messages to the server */

int _ggzcore_net_send_login(struct _GGZNet *net)
{
	GGZLoginType login_type;
	char *type, *handle, *password;
	int status = 0;

	login_type = _ggzcore_server_get_type(net->server);
	handle = _ggzcore_server_get_handle(net->server);
	password = _ggzcore_server_get_password(net->server);
	
	switch (login_type) {
	case GGZ_LOGIN:
		type = "normal";
		break;
	case GGZ_LOGIN_NEW:
		type = "first";
		break;
	case GGZ_LOGIN_GUEST:
	default:
		type = "guest";
	}
	
	_ggzcore_net_send_line(net, "<LOGIN TYPE='%s'>", type);
	_ggzcore_net_send_line(net, "<NAME>%s</NAME>", handle);

	if (login_type == GGZ_LOGIN)
		_ggzcore_net_send_line(net, "<PASSWORD>%s</PASSWORD>", password);
	_ggzcore_net_send_line(net, "</LOGIN>");

	if (status < 0)
		_ggzcore_net_error(net, "Sending login");

	return status;
}


int _ggzcore_net_send_channel(struct _GGZNet *net)
{
	char *id;
	int status = 0;

	id = _ggzcore_server_get_handle(net->server);
	
	status = _ggzcore_net_send_line(net, "<CHANNEL ID='%s' />", id);
	if (status < 0)
		_ggzcore_net_error(net, "Sending channel");

	return status;
}


int _ggzcore_net_send_motd(struct _GGZNet *net)
{
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending MOTD request");	
	_ggzcore_net_send_line(net, "<MOTD/>");

	return status;
}


int _ggzcore_net_send_list_types(struct _GGZNet *net, const char verbose)
{
	int status = 0;
	char *full;

	net->gametype_verbose = verbose;

	ggz_debug(GGZCORE_DBG_NET, "Sending gametype list request");	
	full = bool_to_str(verbose);
	
	_ggzcore_net_send_line(net, "<LIST TYPE='game' FULL='%s'/>", full);

	return status;
}


int _ggzcore_net_send_list_rooms(struct _GGZNet *net, const int type, const char verbose)
{	
	int status = 0;
	char *full;
	
	net->room_verbose = verbose;
	ggz_debug(GGZCORE_DBG_NET, "Sending room list request");	
	full = bool_to_str(verbose);
	
	_ggzcore_net_send_line(net, "<LIST TYPE='room' FULL='%s'/>", full);

	return status;
}


int _ggzcore_net_send_join_room(struct _GGZNet *net, const unsigned int id)
{
	int status = 0;
	struct _GGZRoom *room;

	room = _ggzcore_server_get_room_by_id(net->server, id);
	net->new_room = room;
	
	ggz_debug(GGZCORE_DBG_NET, "Sending room join request");	

	_ggzcore_net_send_line(net, "<ENTER ROOM='%d'/>", id);
	
	return status;
}


int _ggzcore_net_send_list_players(struct _GGZNet *net)
{	
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending player list request");
	_ggzcore_net_send_line(net, "<LIST TYPE='player'/>");

	return status;
}


int _ggzcore_net_send_list_tables(struct _GGZNet *net, const int type, const char global)
{	
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending table list request");
	_ggzcore_net_send_line(net, "<LIST TYPE='table'/>");
	
	return status;
}


int _ggzcore_net_send_chat(struct _GGZNet *net, const GGZChatType type,
			   const char* player, const char* msg)
{
	int status = -1;
	const char *type_str;

	ggz_debug(GGZCORE_DBG_NET, "Sending chat");	


	/* FIXME: We need to handle the case where the chat is longer than
	   the server's allowed chat size */

	type_str = ggz_chattype_to_string(type);

	switch (type) {
	case GGZ_CHAT_NORMAL:
	case GGZ_CHAT_ANNOUNCE:
	case GGZ_CHAT_TABLE:
		status = _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s'><![CDATA[%s]]></CHAT>",
			type_str, msg);
		break;
	case GGZ_CHAT_BEEP:
		status = _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s' TO='%s'/>",
			type_str, player);
		break;
	case GGZ_CHAT_PERSONAL:
		status = _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s' TO='%s'><![CDATA[%s]]></CHAT>",
			type_str, player, msg);
		break;
	case GGZ_CHAT_NONE:
		break;
	}

	return status;
}


int _ggzcore_net_send_table_launch(struct _GGZNet *net, struct _GGZTable *table)
{
	int i, type, num_seats, status = 0;
	char *desc;
	
	ggz_debug(GGZCORE_DBG_NET, "Sending table launch request");
		
	type = _ggzcore_gametype_get_id(_ggzcore_table_get_type(table));
	desc = _ggzcore_table_get_desc(table);
	num_seats = _ggzcore_table_get_num_seats(table);

	_ggzcore_net_send_line(net, "<LAUNCH>");
	_ggzcore_net_send_line(net, "<TABLE GAME='%d' SEATS='%d'>", type, num_seats);
	if (desc)
		_ggzcore_net_send_line(net, "<DESC>%s</DESC>", desc);
	
	for (i = 0; i < num_seats; i++)
		_ggzcore_net_send_table_seat(net, _ggzcore_table_get_nth_seat(table, i));
	
	_ggzcore_net_send_line(net, "</TABLE>");
	_ggzcore_net_send_line(net, "</LAUNCH>");

	return status;
}


static int _ggzcore_net_send_table_seat(struct _GGZNet *net, struct _GGZSeat *seat)
{
	int status = 0;
	const char *type;

	ggz_debug(GGZCORE_DBG_NET, "Sending seat info");
	
	type = ggz_seattype_to_string(seat->type);
	
	if (seat->name)
		_ggzcore_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'>%s</SEAT>", 
				       seat->index,type, seat->name);
	else
		_ggzcore_net_send_line(net, "<SEAT NUM='%d' TYPE='%s'/>", 
				       seat->index, type);
	
	return status;
}


int _ggzcore_net_send_table_join(struct _GGZNet *net,
				 const unsigned int num,
				 int spectator)
{
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending table join request");
	_ggzcore_net_send_line(net, "<JOIN TABLE='%d' SPECTATOR='%s'/>",
			       num, bool_to_str(spectator));

	return status;
}

int _ggzcore_net_send_table_leave(struct _GGZNet *net,
				  int force,
				  int spectator)
{
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending table leave request");
	_ggzcore_net_send_line(net, "<LEAVE FORCE='%s' SPECTATOR='%s'/>",
			       bool_to_str(force),
			       bool_to_str(spectator));

	return status;
}


int _ggzcore_net_send_table_seat_update(struct _GGZNet *net, struct _GGZTable *table, struct _GGZSeat *seat)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table seat update request");
	_ggzcore_net_send_line(net, "<UPDATE TYPE='table' ROOM='%d'>", _ggzcore_room_get_id(table->room));
	_ggzcore_net_send_line(net, "<TABLE ID='%d'>", table->id);
	_ggzcore_net_send_table_seat(net, seat);
	_ggzcore_net_send_line(net, "</TABLE>");
	_ggzcore_net_send_line(net, "</UPDATE>");	

	return 0;
}



int _ggzcore_net_send_table_desc_update(struct _GGZNet *net, struct _GGZTable *table, const char *desc)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table description update request");
	_ggzcore_net_send_line(net, "<UPDATE TYPE='table' ROOM='%d'>", _ggzcore_room_get_id(table->room));
	_ggzcore_net_send_line(net, "<TABLE ID='%d'>", table->id);
	_ggzcore_net_send_line(net, "<DESC>%s</DESC>", desc);
	_ggzcore_net_send_line(net, "</TABLE>");
	_ggzcore_net_send_line(net, "</UPDATE>");	

	return 0;
}


int _ggzcore_net_send_logout(struct _GGZNet *net)
{
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending LOGOUT");	
	_ggzcore_net_send_line(net, "</SESSION>");

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
	
		ggz_debug(GGZCORE_DBG_POLL, "Checking for net events");	
		if ( (pending = poll(fd, 1, 0)) < 0) {
			if (errno == EINTR) 
				/* Ignore interruptions */
				pending = 0;
			else 
				ggz_error_sys_exit("poll failed in ggzcore_server_data_is_pending");
		}
		else if (pending)
			ggz_debug(GGZCORE_DBG_POLL, "Found a net event!");
	}

	return pending;
}


/* Read in a bit more from the server and send it to the parser */
int _ggzcore_net_read_data(struct _GGZNet *net)
{
	char *buf;
	int len, done;

	/* We're already in a parse call, and XML parsing is *not* reentrant */
	if (net->parsing) 
		return 0;

	/* Set flag in case we get called recursively */
	net->parsing = 1;

	/* Get a buffer to hold the data */
	if (!(buf = XML_GetBuffer(net->parser, XML_BUFFSIZE)))
		ggz_error_sys_exit("Couldn't allocate buffer");

	/* Read in data from socket */
	if ( (len = read(net->fd, buf, XML_BUFFSIZE)) < 0) {
		
		/* If it's a non-blocking socket and there isn't data,
                   we get EAGAIN.  It's safe to just return */
		if (errno == EAGAIN) {
			net->parsing = 0;
			return 0;
		}
		_ggzcore_net_error(net, "Reading data from server");
	}

	_ggzcore_net_dump_data(net, buf, len);
	
	/* If len == 0 then we've reached EOF */
	done = (len == 0);
	if (done) {
		_ggzcore_server_protocol_error(net->server, "Server disconnected");
		_ggzcore_net_disconnect(net);
		_ggzcore_server_session_over(net->server, net);
	}
	else if (!XML_ParseBuffer(net->parser, len, done)) {
		ggz_debug(GGZCORE_DBG_XML, "Parse error at line %d, col %d:%s",
			  XML_GetCurrentLineNumber(net->parser),
			  XML_GetCurrentColumnNumber(net->parser),
			  XML_ErrorString(XML_GetErrorCode(net->parser)));
		_ggzcore_server_protocol_error(net->server, "Bad XML from server");
	}
	
	/* Clear the flag now that we're done */
	net->parsing = 0;
	return done;
}


/********** Callbacks for XML parser **********/
static void _ggzcore_net_parse_start_tag(void *data, const char *el, const char **attr)
{
	struct _GGZNet *net = (struct _GGZNet*)data;
	GGZStack *stack = net->stack;
	GGZXMLElement *element;

	ggz_debug(GGZCORE_DBG_XML, "New %s element", el);
	
	/* Create new element object */
	element = _ggzcore_net_new_element((char*)el, (char**)attr);

	/* Put element on stack so we can process its children */
	ggz_stack_push(stack, element);
}


static void _ggzcore_net_parse_end_tag(void *data, const char *el)
{
	GGZXMLElement *element;
	struct _GGZNet *net = (struct _GGZNet*)data;
	
	/* Pop element off stack */
	element = ggz_stack_pop(net->stack);

	/* Process tag */
	ggz_debug(GGZCORE_DBG_XML, "Handling %s element", 
		  ggz_xmlelement_get_tag(element));
		  
	
	if (element->process)
		element->process(net, element);

	/* Free data structures */
	ggz_xmlelement_free(element);
}


static void _ggzcore_net_parse_text(void *data, const char *text, int len) 
{
	struct _GGZNet *net = (struct _GGZNet*)data;
	GGZStack *stack = net->stack;
	GGZXMLElement *top;

	top = ggz_stack_top(stack);
	ggz_xmlelement_add_text(top, text, len);
}


static void _ggzcore_net_error(struct _GGZNet *net, char* message)
{
	ggz_debug(GGZCORE_DBG_NET, "Network error: %s", message);
	_ggzcore_net_disconnect(net);
	_ggzcore_server_net_error(net->server, message);
}


static void _ggzcore_net_dump_data(struct _GGZNet *net, char *data, int size)
{
	if (net->dump_file > 0)
		write(net->dump_file, data, size);
}

static GGZXMLElement* _ggzcore_net_new_element(char *tag, char **attrs)
{
	void (*process_func)();

	/* FIXME: Could we do this with a table lookup? */
	if (strcmp(tag, "SERVER") == 0)
		process_func = _ggzcore_net_handle_server;
	else if (strcmp(tag, "OPTIONS") == 0)
		process_func = _ggzcore_net_handle_options;
	else if (strcmp(tag, "MOTD") == 0)
		process_func = _ggzcore_net_handle_motd;
	else if (strcmp(tag, "RESULT") == 0)
		process_func = _ggzcore_net_handle_result;
	else if (strcmp(tag, "LIST") == 0)
		process_func = _ggzcore_net_handle_list;
	else if (strcmp(tag, "UPDATE") == 0)
		process_func = _ggzcore_net_handle_update;
	else if (strcmp(tag, "GAME") == 0)
		process_func = _ggzcore_net_handle_game;
	else if (strcmp(tag, "PROTOCOL") == 0)
		process_func = _ggzcore_net_handle_protocol;
	else if (strcmp(tag, "ALLOW") == 0)
		process_func = _ggzcore_net_handle_allow;
	else if (strcmp(tag, "ABOUT") == 0)
		process_func = _ggzcore_net_handle_about;
	else if (strcmp(tag, "ROOM") == 0)
		process_func = _ggzcore_net_handle_room;
	else if (strcmp(tag, "PLAYER") == 0)
		process_func = _ggzcore_net_handle_player;
	else if (strcmp(tag, "TABLE") == 0)
		process_func = _ggzcore_net_handle_table;
	else if (strcmp(tag, "SEAT") == 0)
		process_func = _ggzcore_net_handle_seat;
	else if (strcmp(tag, "SPECTATOR") == 0)
		process_func = _ggzcore_net_handle_seat;
	else if (strcmp(tag, "CHAT") == 0)
		process_func = _ggzcore_net_handle_chat;
	else if (strcmp(tag, "DESC") == 0)
		process_func = _ggzcore_net_handle_desc;
	else if (strcmp(tag, "PASSWORD") == 0)
		process_func = _ggzcore_net_handle_password;
	else if (strcmp(tag, "PING") == 0)
		process_func = _ggzcore_net_handle_ping;
	else if (strcmp(tag, "SESSION") == 0)
		process_func = _ggzcore_net_handle_session;
	else
		process_func = NULL;
	
	return ggz_xmlelement_new(tag, attrs, process_func, NULL);
}


/* Functions for <SERVER> tag */
void _ggzcore_net_handle_server(GGZNet *net, GGZXMLElement *server)
{
	char *name, *id, *status;
	int version, chatlen;

	if (server) {
		name = ggz_xmlelement_get_attr(server, "NAME");
		id = ggz_xmlelement_get_attr(server, "ID");
		status = ggz_xmlelement_get_attr(server, "STATUS");
		version = safe_atoi(ggz_xmlelement_get_attr(server, "VERSION"));
		chatlen = (int)ggz_xmlelement_get_data(server);

		ggz_debug(GGZCORE_DBG_NET, 
			      "%s(%s) : status %s: protocol %d: chat size %d", 
			      name, id, status, version, chatlen);

		net->chat_size = chatlen;
		/* FIXME: Do something with name, status */
		if (version == GGZ_CS_PROTO_VERSION) {
			/* Everything checked out so start session */
			_ggzcore_net_send_header(net);

			_ggzcore_server_set_negotiate_status(net->server, net, 0);
		}
		else
			_ggzcore_server_set_negotiate_status(net->server, net, -1);
	}
}


/* Functions for <OPTIONS> tag */
static void _ggzcore_net_handle_options(GGZNet *net, GGZXMLElement *options)
{
	int len;
	GGZXMLElement *server;

	/* Get parent off top of stack */
	server = ggz_stack_top(net->stack);

	if (options && server) {
		/* Grab chat length out of options and store on server */
		len = safe_atoi(ggz_xmlelement_get_attr(options, "CHATLEN"));
		ggz_xmlelement_set_data(server, (void*)len);
	}
}


/* Functions for <MOTD> tag */
static void _ggzcore_net_handle_motd(GGZNet *net, GGZXMLElement *motd)
{
	char **buffer;
	char *message, *priority;

	message = ggz_xmlelement_get_text(motd);
	priority = ggz_xmlelement_get_attr(motd, "PRIORITY");
	
	ggz_debug(GGZCORE_DBG_NET, "Motd of priority %s", priority);

	if (!(buffer = calloc(2, sizeof(char*))))
		ggz_error_sys_exit("calloc() failed in net_read_motd");

	buffer[0] = message;
	_ggzcore_server_event(net->server, GGZ_MOTD_LOADED, buffer);
}


/* Functions for <RESULT> tag */
static void _ggzcore_net_handle_result(GGZNet *net, GGZXMLElement *result)
{
	GGZRoom *room;
	char *action;
	int code;
	void *data;
	char *message;

	if (result) {
		
		action = ggz_xmlelement_get_attr(result, "ACTION");
		code = safe_atoi(ggz_xmlelement_get_attr(result, "CODE"));
		data = ggz_xmlelement_get_data(result);
		
		ggz_debug(GGZCORE_DBG_NET, "Result of %s was %d", action, 
			      code);
		
		room = _ggzcore_server_get_cur_room(net->server);

		if (strcmp(action, "login") == 0) {
			/* FIXME: we should check if login type is 'new' */
			if (code == 0 && data) {
				_ggzcore_server_set_password(net->server, data);
				ggz_free(data);
			}
			_ggzcore_server_set_login_status(net->server, code);
		}
		else if (strcmp(action, "enter") == 0) {
			if (code == 0)
				_ggzcore_server_set_room(net->server, net->new_room);
			_ggzcore_server_set_room_join_status(net->server, code);
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
				_ggzcore_server_event(net->server, GGZ_CHAT_FAIL, "Not in a room");
				break;
				
			case E_BAD_OPTIONS:
				_ggzcore_server_event(net->server, GGZ_CHAT_FAIL, "Bad options");
				break;
			case E_NO_PERMISSION:
				_ggzcore_server_event(net->server, GGZ_CHAT_FAIL, "Prohibited");
				break;
			}
		}
		else if (strcmp(action, "protocol") == 0) {
			/* These are always errors */
			switch (code) {
			case E_BAD_OPTIONS:
				message = "Server didn't recognize one of our commands";
				break;
			case E_BAD_XML:
				message = "Server didn't like our XML";
				break;
			default:
				message = "Unknown protocol error";
			}

			_ggzcore_server_protocol_error(net->server, message);
		}
	}
}


/* Functions for <PASSWORD> tag */
static void _ggzcore_net_handle_password(GGZNet *net, GGZXMLElement *element)
{
	char *password;
	char *parent_tag;
	GGZXMLElement *parent;
	
	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	
	if (element && parent) {
		password = ggz_xmlelement_get_text(element);
		parent_tag = ggz_xmlelement_get_tag(parent);
		
		/* It had better be RESULT.... */
		if (strcmp(parent_tag, "RESULT") == 0)
			ggz_xmlelement_set_data(parent, ggz_strdup(password));
	}
}


/* Functions for <LIST> tag */
static void _ggzcore_net_handle_list(GGZNet *net, GGZXMLElement *list_tag)
{
	GGZList *list;
	GGZListEntry *entry;
	GGZRoom *room;
	int count, room_num;
	char *type;
	
	if (list_tag) {

		/* Grab list data from tag */
		type = ggz_xmlelement_get_attr(list_tag, "TYPE");
		list = ggz_xmlelement_get_data(list_tag);
		if (!ggz_xmlelement_get_attr(list_tag, "ROOM"))
			room_num = -1;
		else
			room_num = safe_atoi(ggz_xmlelement_get_attr(list_tag, "ROOM"));
		
		entry = ggz_list_head(list);

		/* Get length of list */
		/* FIXME: it would be nice if we could get this from the list itself */
		count = 0;
		while (entry) {
			entry = ggz_list_next(entry);
			count++;
		}


		if (strcmp(type, "room") == 0) {
			/* Clear existing list (if any) */
			if (_ggzcore_server_get_num_rooms(net->server) > 0)
				_ggzcore_server_free_roomlist(net->server);

			_ggzcore_server_init_roomlist(net->server, count);

			entry = ggz_list_head(list);
			while (entry) {
				_ggzcore_server_add_room(net->server, ggz_list_get_data(entry));
				entry = ggz_list_next(entry);
			}
			_ggzcore_server_event(net->server, GGZ_ROOM_LIST, NULL);
		}
		else if (strcmp(type, "game") == 0) {
			/* Free previous list of types*/
			if (ggzcore_server_get_num_gametypes(net->server) > 0)
				_ggzcore_server_free_typelist(net->server);

			_ggzcore_server_init_typelist(net->server, count);
			entry = ggz_list_head(list);
			while (entry) {
				_ggzcore_server_add_type(net->server, ggz_list_get_data(entry));
				entry = ggz_list_next(entry);
			}
			_ggzcore_server_event(net->server, GGZ_TYPE_LIST, NULL);
		}
		else if (strcmp(type, "player") == 0) {
			room = _ggzcore_server_get_room_by_id(net->server, room_num);
			_ggzcore_room_set_player_list(room, count, list);
			list = NULL;
		}
		else if (strcmp(type, "table") == 0) {
			room = _ggzcore_server_get_room_by_id(net->server, room_num);
			_ggzcore_room_set_table_list(room, count, list);
			list = NULL;
		}

		if (list)
			ggz_list_free(list);
	}
}


static void _ggzcore_net_list_insert(GGZXMLElement *list_tag, void *data)
{
	char *type;
	GGZList *list;
	ggzEntryCompare compare_func = NULL;
	ggzEntryCreate  create_func = NULL;
	ggzEntryDestroy destroy_func = NULL;

	type = ggz_xmlelement_get_attr(list_tag, "TYPE");
	list = ggz_xmlelement_get_data(list_tag);
	
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
		list = ggz_list_create(compare_func, 
					    create_func, 
					    destroy_func,
					    GGZ_LIST_ALLOW_DUPS);
		
		ggz_xmlelement_set_data(list_tag, list);
	}

	ggz_list_insert(list, data);
}


/* Functions for <UPDATE> tag */
static void _ggzcore_net_handle_update(GGZNet *net, GGZXMLElement *update)
{
	char *action, *type;

	/* Return if there's no tag */
	if (!update)
		return;
	
	/* Grab update data from tag */
	type = ggz_xmlelement_get_attr(update, "TYPE");
	action = ggz_xmlelement_get_attr(update, "ACTION");

	if (strcmp(type, "room") == 0) {
		/* FIXME: implement this */
	}
	else if (strcmp(type, "game") == 0) {
		/* FIXME: implement this */
	}
	else if (strcmp(type, "player") == 0)
		_ggzcore_net_player_update(net, update, action);
	else if (strcmp(type, "table") == 0)
		_ggzcore_net_table_update(net, update, action);
}


/* Handle Player update */
static void _ggzcore_net_player_update(GGZNet *net, GGZXMLElement *update, char *action)
{
	int room_num;
	GGZPlayer *player;
	GGZRoom *room;

	if (!ggz_xmlelement_get_attr(update, "ROOM"))
		room_num = -1;
	else
		room_num = safe_atoi(ggz_xmlelement_get_attr(update, "ROOM"));


	player = ggz_xmlelement_get_data(update);
	room = _ggzcore_server_get_room_by_id(net->server, room_num);
	
	if (strcmp(action, "add") == 0)
		_ggzcore_room_add_player(room, player->name, player->type, player->lag);
	
	else if (strcmp(action, "delete") == 0)
		_ggzcore_room_remove_player(room, player->name);
	else if (strcmp(action, "lag") == 0)
		/* FIXME: Should be a player "class-based" event */
		_ggzcore_room_set_player_lag(room, player->name, player->lag);
	
	ggz_free(player);
}


/* Handle table update */
static void _ggzcore_net_table_update(GGZNet *net, GGZXMLElement *update, char *action)
{
	int i, room_num;
	GGZTable *table, *table_data;
	GGZRoom *room;

	/* Sanity check: we can't proceed without a room number */
	if (!ggz_xmlelement_get_attr(update, "ROOM")) {
		room_num = -1;
		_ggzcore_server_protocol_error(net->server, "Server didn't send room number");
		return;
	}
	
	room_num = safe_atoi(ggz_xmlelement_get_attr(update, "ROOM"));
	room = _ggzcore_server_get_room_by_id(net->server, room_num);

	if (!room) {
		_ggzcore_server_protocol_error(net->server, "Server specified non-existent room");
		return;
	}
	
	table_data = ggz_xmlelement_get_data(update);
	table = _ggzcore_room_get_table_by_id(room, table_data->id);

	/* Table can only be NULL if we're adding it */
	if (!table && strcmp(action, "add") != 0) {
		_ggzcore_server_protocol_error(net->server, "Server specified non-existent table");
		return;
	}

	if (strcmp(action, "add") == 0) {
		_ggzcore_room_add_table(room, table_data);
		/* Set table_data to NULL so it doesn't get freed at
                   the end of this function */
		table_data = NULL;
	}
	else if (strcmp(action, "delete") == 0)
		_ggzcore_room_remove_table(room, table_data->id);

	else if (strcmp(action, "join") == 0) {
		for (i = 0; i < table_data->num_seats; i++)
			if (table_data->seats[i].type != GGZ_SEAT_NONE)
				_ggzcore_table_set_seat(table, &table_data->seats[i]);
	} else if (strcmp(action, "joinspectator") == 0) {
		for (i = 0; i < table_data->num_spectator_seats; i++) {
			if (table_data->spectator_seats[i].name) {
				_ggzcore_table_set_spectator_seat(table,
					&table_data->spectator_seats[i]);
			}
		}
	}
	else if (strcmp(action, "leave") == 0) {
		for (i = 0; i < table_data->num_seats; i++) {
			if (table_data->seats[i].type != GGZ_SEAT_NONE) {
				/* Player is vacating seat */
				struct _GGZSeat seat;
				seat.index = i;
				seat.type = GGZ_SEAT_OPEN;
				seat.name = NULL;
				_ggzcore_table_set_seat(table, &seat);
			}
		}
	}
	else if (strcmp(action, "leavespectator") == 0) {
		for (i = 0; i < table_data->num_spectator_seats; i++) {
			if (table_data->spectator_seats[i].name) {
				/* Player is vacating seat */
				struct _GGZSeat seat;
				seat.index = i;
				seat.name = NULL;
				_ggzcore_table_set_spectator_seat(table,
								  &seat);
			}
		}
	}
	else if (strcmp(action, "status") == 0) {
		_ggzcore_table_set_state(table, table_data->state);
	}
	else if (strcmp(action, "desc") == 0) {
		_ggzcore_table_set_desc(table, table_data->desc);
	}
	else if (strcmp(action, "seat") == 0) {
		for (i = 0; i < table_data->num_seats; i++) 
			if (table_data->seats[i].type != GGZ_SEAT_NONE)
				_ggzcore_table_set_seat(table, &table_data->seats[i]);
	}
			
	if (table_data)
		_ggzcore_table_free(table_data);

}



/* Functions for <GAME> tag */
static void _ggzcore_net_handle_game(GGZNet *net, GGZXMLElement *game)
{
	GGZGameType *type;
	GGZGameData *data;
	GGZXMLElement *parent;
	char *parent_tag;
	int id;
	char *name, *version;
	char *prot_engine = NULL;
	char *prot_version = NULL;
	GGZNumberList player_allow_list = ggz_numberlist_new();
	GGZNumberList bot_allow_list = ggz_numberlist_new();
	int spectators_allow = 0;
	char *desc = NULL;
	char *author = NULL;
	char *url = NULL;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	
	if (game && parent) {
		
		/* Get game data from tag */
		id = safe_atoi(ggz_xmlelement_get_attr(game, "ID"));
		name = ggz_xmlelement_get_attr(game, "NAME");
		version = ggz_xmlelement_get_attr(game, "VERSION");
		data = ggz_xmlelement_get_data(game);

		if (data) {
			prot_engine = data->prot_engine;
			prot_version = data->prot_version;
			player_allow_list = data->player_allow_list;
			bot_allow_list = data->bot_allow_list;
			spectators_allow = data->spectators_allow;
			desc = data->desc;
			author = data->author;
			url = data->url;
		}

		type = _ggzcore_gametype_new();
		_ggzcore_gametype_init(type, id, name, version, prot_engine,
				       prot_version, 
				       player_allow_list, bot_allow_list,
				       spectators_allow, desc, author, url);

		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_net_list_insert(parent, type);

		if (data) {
			if (data->prot_engine)
				ggz_free(data->prot_engine);
			if (data->prot_version)
				ggz_free(data->prot_version);
			if (data->author)
				ggz_free(data->author);
			if (data->url)
				ggz_free(data->url);
			if (data->desc)
				ggz_free(data->desc);
			
			ggz_free(data);
		}

	}
}


static void _ggzcore_net_game_set_protocol(GGZXMLElement *game, char *engine, char *version)
{
	struct _GGZGameData *data;
	
	data = ggz_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggz_malloc(sizeof(struct _GGZGameData));
		ggz_xmlelement_set_data(game, data);
	}
	
	data->prot_engine = engine;
	data->prot_version = version;
}


static void _ggzcore_net_game_set_allowed(GGZXMLElement *game,
					  GGZNumberList players, 
					  GGZNumberList bots,
					  int spectators)
{
	struct _GGZGameData *data;
	
	data = ggz_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggz_malloc(sizeof(struct _GGZGameData));
		ggz_xmlelement_set_data(game, data);
	}
	
	data->player_allow_list = players;
	data->bot_allow_list = bots;
	data->spectators_allow = spectators;
}


static void _ggzcore_net_game_set_info(GGZXMLElement *game, char *author, char *url)
{
	struct _GGZGameData *data;
	
	data = ggz_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggz_malloc(sizeof(struct _GGZGameData));
		ggz_xmlelement_set_data(game, data);
	}
	
	data->author = author;
	data->url = url;
}


static void _ggzcore_net_game_set_desc(GGZXMLElement *game, char *desc)
{
	struct _GGZGameData *data;
	
	data = ggz_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggz_malloc(sizeof(struct _GGZGameData));
		ggz_xmlelement_set_data(game, data);
	}
	
	data->desc = desc;
}


/* Functions for <PROTOCOL> tag */
static void _ggzcore_net_handle_protocol(GGZNet *net, GGZXMLElement *protocol)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (protocol && parent) {
		_ggzcore_net_game_set_protocol(parent,
						    ggz_strdup(ggz_xmlelement_get_attr(protocol, "ENGINE")),
						    ggz_strdup(ggz_xmlelement_get_attr(protocol, "VERSION")));
	}
}


/* Functions for <ALLOW> tag */
static void _ggzcore_net_handle_allow(GGZNet *net, GGZXMLElement *allow)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (allow && parent) {
		GGZNumberList players = ggz_numberlist_read(ggz_xmlelement_get_attr(allow, "PLAYERS"));
		GGZNumberList bots = ggz_numberlist_read(ggz_xmlelement_get_attr(allow, "BOTS"));
		int spectators = str_to_bool(ggz_xmlelement_get_attr(allow, "SPECTATORS"), 0);

		_ggzcore_net_game_set_allowed(parent,
					      players, bots, spectators);
	}
}


/* Functions for <ABOUT> tag */
static void _ggzcore_net_handle_about(GGZNet *net, GGZXMLElement *about)
{
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (about && parent) {
		_ggzcore_net_game_set_info(parent, 
						ggz_strdup(ggz_xmlelement_get_attr(about, "AUTHOR")),
						ggz_strdup(ggz_xmlelement_get_attr(about, "URL")));
	}
}


/* Functions for <DESC> tag */
static void _ggzcore_net_handle_desc(GGZNet *net, GGZXMLElement *element)
{
	char *desc;
	char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (element && parent) {
		desc = ggz_strdup(ggz_xmlelement_get_text(element));
		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "GAME") == 0)
			_ggzcore_net_game_set_desc(parent, desc);
		else if (strcmp(parent_tag, "ROOM") == 0)
			ggz_xmlelement_set_data(parent, desc);
		else if (strcmp(parent_tag, "TABLE") == 0)
			_ggzcore_net_table_set_desc(parent, desc);
	}
}


/* Functions for <ROOM> tag */
static void _ggzcore_net_handle_room(GGZNet *net, GGZXMLElement *room)
{
	GGZRoom *ggz_room;
	char *parent_tag;
	int id, game;
	char *name, *desc;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (room && parent) {

		/* Grab data from tag */
		id = safe_atoi(ggz_xmlelement_get_attr(room, "ID"));
		name = ggz_xmlelement_get_attr(room, "NAME");
		game = safe_atoi(ggz_xmlelement_get_attr(room, "GAME"));
		desc = ggz_xmlelement_get_data(room);

		/* Set up GGZRoom object */
		ggz_room = _ggzcore_room_new();
		_ggzcore_room_init(ggz_room, net->server, id, name, game, 
				   desc);

		/* Free description if present */
		if (desc)
			ggz_free(desc);

		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_net_list_insert(parent, ggz_room);
	}
}


/* Functions for <PLAYER> tag */
static void _ggzcore_net_handle_player(GGZNet *net, GGZXMLElement *player)
{
	GGZPlayer *ggz_player;
	GGZPlayerType type;
	char *parent_tag;
	GGZRoom *room;
	char *name, *str_type;
	int table, lag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (player && parent) {
		room = ggzcore_server_get_cur_room(net->server);

		/* Grab player data from tag */
		str_type = ggz_xmlelement_get_attr(player, "TYPE");
		name = ggz_xmlelement_get_attr(player, "ID");
		table = safe_atoi(ggz_xmlelement_get_attr(player, "TABLE"));
		lag = safe_atoi(ggz_xmlelement_get_attr(player, "LAG"));

		/* Set player's type */
		type = GGZ_PLAYER_NONE;
		if (str_type) {
			if (strcmp(str_type, "normal") == 0)
				type = GGZ_PLAYER_NORMAL;
			else if (strcmp(str_type, "guest") == 0)
				type = GGZ_PLAYER_GUEST;
			else if (strcmp(str_type, "admin") == 0)
				type = GGZ_PLAYER_ADMIN;
		}
			
		
		/* Set up GGZPlayer object */
		ggz_player = _ggzcore_player_new();
		_ggzcore_player_init(ggz_player,  name, room, table, type, lag);

		parent_tag = ggz_xmlelement_get_tag(parent);
		
		if (strcmp(parent_tag, "LIST") == 0)
			_ggzcore_net_list_insert(parent, ggz_player);
		else if (strcmp(parent_tag, "UPDATE") == 0)
			ggz_xmlelement_set_data(parent, ggz_player);
	}
}


/* Functions for <TABLE> tag */
static void _ggzcore_net_handle_table(GGZNet *net, GGZXMLElement *table)
{
	char *parent_tag;
	GGZGameType *type;
	GGZTableData *data;
	GGZTable *table_obj;
	GGZList *seats = NULL;
	GGZListEntry *entry;
	int id, game, status, num_seats, num_spectators, i;
	char *desc = NULL;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (table && parent) {
		parent_tag = ggz_xmlelement_get_tag(parent);

		/* Grab table data from tag */

		id = safe_atoi(ggz_xmlelement_get_attr(table, "ID"));
		game = safe_atoi(ggz_xmlelement_get_attr(table, "GAME"));
		status = safe_atoi(ggz_xmlelement_get_attr(table, "STATUS"));
		num_seats = safe_atoi(ggz_xmlelement_get_attr(table, "SEATS"));
		num_spectators = safe_atoi(ggz_xmlelement_get_attr(table,
					   "SPECTATORS"));
		data = ggz_xmlelement_get_data(table);
		if (data) {
			desc = data->desc;
			seats = data->seats;
		}

		/* Create new table structure */
		table_obj = _ggzcore_table_new();
		type = _ggzcore_server_get_type_by_id(net->server, game);
		_ggzcore_table_init(table_obj, type, desc, num_seats,
				    status, id);

		/* Initialize seats to none */
		/* FIXME: perhaps tables should come this way? */
		for (i = 0; i < num_seats; i++)
			table_obj->seats[i].type = GGZ_SEAT_NONE;
		
		/* Add seats */
		entry = ggz_list_head(seats);
		while (entry) {
			void* seat = ggz_list_get_data(entry);
			if (num_seats > 0)
				_ggzcore_table_set_seat(table_obj, seat);
			else if (num_spectators > 0)
				_ggzcore_table_set_spectator_seat(table_obj,
								  seat);
			entry = ggz_list_next(entry);
		}
		
		if (strcmp(parent_tag, "LIST") == 0) {
			_ggzcore_net_list_insert(parent, table_obj);
		}
		else if (strcmp(parent_tag, "UPDATE") == 0) {
			ggz_xmlelement_set_data(parent, table_obj);
		}

		if (data)
			_ggzcore_net_tabledata_free(data);
	}
}

static void _ggzcore_net_table_add_seat(GGZXMLElement *table, struct _GGZSeat *seat)
{
	struct _GGZTableData *data;
	
	data = ggz_xmlelement_get_data(table);
	
	/* If data doesn't already exist, create it */
	if (!data) {
		data = _ggzcore_net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}

	ggz_list_insert(data->seats, seat);
}


static void _ggzcore_net_table_set_desc(GGZXMLElement *table, char *desc)
{
	struct _GGZTableData *data;
	
	data = ggz_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _ggzcore_net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}
	
	data->desc = desc;
}


static GGZTableData* _ggzcore_net_tabledata_new(void)
{
	struct _GGZTableData *data;

	data = ggz_malloc(sizeof(struct _GGZTableData));

	data->seats = ggz_list_create(NULL, 
				      (ggzEntryCreate)_ggzcore_net_seat_copy, 
				      (ggzEntryDestroy)_ggzcore_net_seat_free, 
				      GGZ_LIST_ALLOW_DUPS);

	return data;
}


static void _ggzcore_net_tabledata_free(GGZTableData *data)
{
	if (data) {
		if (data->desc)
			ggz_free(data->desc);
		if (data->seats)
			ggz_list_free(data->seats);
		ggz_free(data);
	}
}


/* Functions for <SEAT> and <SPECTATOR> tags */
static void _ggzcore_net_handle_seat(GGZNet *net, GGZXMLElement *seat)
{
	struct _GGZSeat seat_obj;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);

	if (seat && parent) {

		/* Get seat information out of tag */
		seat_obj.index = safe_atoi(ggz_xmlelement_get_attr(seat, "NUM"));
		seat_obj.type = ggz_string_to_seattype(ggz_xmlelement_get_attr(seat, "TYPE"));
		seat_obj.name = ggz_xmlelement_get_text(seat);
		_ggzcore_net_table_add_seat(parent, &seat_obj);
	}
}

static struct _GGZSeat* _ggzcore_net_seat_copy(struct _GGZSeat *orig)
{
	struct _GGZSeat *copy;

	copy = ggz_malloc(sizeof(struct _GGZSeat));

	copy->index = orig->index;
	copy->type = orig->type;
	copy->name = ggz_strdup(orig->name);

	return copy;
}


static void _ggzcore_net_seat_free(struct _GGZSeat *seat)
{
	if (seat) {
		if (seat->name)
			ggz_free(seat->name);
		ggz_free(seat);
	}
}



/* Functions for <CHAT> tag */
static void _ggzcore_net_handle_chat(GGZNet *net, GGZXMLElement *chat)
{
	if (chat) {
		char *msg, *type_str, *from;
		GGZRoom *room;
		GGZChatType type;

		/* Grab chat data from tag */
		type_str = ggz_xmlelement_get_attr(chat, "TYPE");
		from = ggz_xmlelement_get_attr(chat, "FROM");
		msg = ggz_xmlelement_get_text(chat);

		ggz_debug(GGZCORE_DBG_NET, "%s message from %s: '%s'",
			  type_str, from, msg);	

		type = ggz_string_to_chattype(type_str);

		if (type == GGZ_CHAT_NONE) {
			/* Hopefully this will handle future chat ops
			   gracefully. */
			type = GGZ_CHAT_NORMAL;
		}

		if (!from) {
			/* Ignore any message that has no sender. */
			return;
		}

		if (!msg && type != GGZ_CHAT_BEEP) {
			/* Ignore an empty message, except for the
			   appropriate chat types. */
			return;
		}

		room = ggzcore_server_get_cur_room(net->server);
		_ggzcore_room_add_chat(room, type, from, msg);
	}
}


/* Function for <PING> tag */
static void _ggzcore_net_handle_ping(GGZNet *net, GGZXMLElement *data)
{
	/* No need to bother the client or anything, just send pong */
	_ggzcore_net_send_line(net, "<PONG/>");
}


/* Function for <SESSION> tag */
static void _ggzcore_net_handle_session(GGZNet *net, GGZXMLElement *data)
{
	_ggzcore_server_session_over(net->server, net);
}

/* Send the session header */
void _ggzcore_net_send_header(GGZNet *net)
{
	_ggzcore_net_send_line(net, "<?xml version='1.0' encoding='ISO-8859-1'?>");
	_ggzcore_net_send_line(net, "<SESSION>");
}


static int _ggzcore_net_send_line(GGZNet *net, char *line, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, line);
	vsprintf(buf, line, ap);
	va_end(ap);
	strcat(buf, "\n");
	return write(net->fd, buf, strlen(buf));
}


static int safe_atoi(char *string)
{
	if (!string)
		return 0;
	else
		return atoi(string);
}
