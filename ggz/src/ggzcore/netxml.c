/*
 * File: netxml.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 * $Id: netxml.c 5433 2003-02-22 05:40:33Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <expat.h>
#include <ggz.h>
#include <ggz_common.h>

#include "ggzcore.h"
#include "net.h"
#include "protocol.h"
#include "player.h"
#include "room.h"
#include "state.h"
#include "table.h"
#include "gametype.h"

/* For convenience */
#define XML_BUFFSIZE 8192

#define ATTR ggz_xmlelement_get_attr


/* GGZNet structure for handling the network connection to the server */
struct _GGZNet {
	
	/* Server structure handling this session */
	GGZServer *server;
	
	/* Host name of server */
	const char *host;

	/* Port on which GGZ server in running */
	unsigned int port;

	/* File descriptor for communication with server */
	int fd;

	/* Maximum chat size allowed */
	unsigned int chat_size;

	/* Room to which we are transitioning */
	GGZRoom *new_room;

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

	/* Whether to use TLS or not */
	int use_tls;
};


/* Game data structure */
typedef struct {
	const char *prot_engine;
	const char *prot_version;
	GGZNumberList player_allow_list;
	GGZNumberList bot_allow_list;
	int spectators_allow;
	const char *desc;
	const char *author;
	const char *url;
} GGZGameData;	

/* Table data structure */
typedef struct {
	const char *desc;
	GGZList *seats;
	GGZList *spectatorseats;
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
static void _ggzcore_net_handle_spectator_seat(GGZNet *net,
					       GGZXMLElement *seat);
static void _ggzcore_net_handle_chat(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_leave(GGZNet *net, GGZXMLElement *element);
static void _ggzcore_net_handle_join(GGZNet *net, GGZXMLElement *element);
static void _ggzcore_net_handle_ping(GGZNet*, GGZXMLElement*);
static void _ggzcore_net_handle_session(GGZNet*, GGZXMLElement*);

/* Extra functions fot handling data associated with specific tags */
static void _ggzcore_net_list_insert(GGZXMLElement*, void*);
static GGZGameData *_ggzcore_net_game_get_data(GGZXMLElement *game);
static void _ggzcore_net_game_set_protocol(GGZXMLElement*, char*, char *);
static void _ggzcore_net_game_set_allowed(GGZXMLElement*,
					  GGZNumberList, GGZNumberList, int);
static void _ggzcore_net_game_set_info(GGZXMLElement*, char*, char *);
static void _ggzcore_net_game_set_desc(GGZXMLElement*, char*);
static void _ggzcore_net_table_add_seat(GGZXMLElement*, struct _GGZSeat*,
					int spectator);
static void _ggzcore_net_player_update(GGZNet *net, GGZXMLElement *update, char *action);
static void _ggzcore_net_table_update(GGZNet *net, GGZXMLElement *update, char *action);
static GGZTableData *_ggzcore_net_table_get_data(GGZXMLElement *table);
static void _ggzcore_net_table_set_desc(GGZXMLElement*, char*);
static GGZTableData* _ggzcore_net_tabledata_new(void);
static void _ggzcore_net_tabledata_free(GGZTableData*);
static struct _GGZSeat* _ggzcore_net_seat_copy(struct _GGZSeat *orig);
static void _ggzcore_net_seat_free(struct _GGZSeat*);

/* Trigger network error event */
static void _ggzcore_net_error(GGZNet *net, char* message);

/* Dump network data to debugging file */
static void _ggzcore_net_dump_data(GGZNet *net, char *data, int size);
static void _ggzcore_net_negotiate_tls(GGZNet *net);

/* Utility functions */
static int _ggzcore_net_send_table_seat(GGZNet *net, struct _GGZSeat *seat);
static void _ggzcore_net_send_header(GGZNet *net);
static int _ggzcore_net_send_pong(GGZNet *net, const char *id);
static int _ggzcore_net_send_line(GGZNet *net, char *line, ...)
	ggz__attribute((format(printf, 2, 3)));
static int str_to_int(const char *str, int dflt);



/* Internal library functions (prototypes in net.h) */

GGZNet* _ggzcore_net_new(void)
{
	GGZNet *net;

	net = ggz_malloc(sizeof(GGZNet));
	
	/* Set fd to invalid value */
	net->fd = -1;
	net->dump_file = -1;
	net->use_tls = -1;
	
	return net;
}


void _ggzcore_net_init(GGZNet *net, GGZServer *server,
		       const char *host, unsigned int port, unsigned int use_tls)
{
	net->server = server;
	net->host = ggz_strdup(host);
	net->port = port;
	net->fd = -1;
	net->use_tls = use_tls;

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


int _ggzcore_net_set_dump_file(GGZNet *net, const char* filename)
{
	if (!filename)
		return 0;
	
	if (strcasecmp(filename, "stderr") == 0)
		net->dump_file = STDERR_FILENO;
	else
		net->dump_file = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	
	if (net->dump_file < 0)
		return -1;
	else
		return 0;
}


const char* _ggzcore_net_get_host(GGZNet *net)
{
	return net->host;
}


unsigned int _ggzcore_net_get_port(GGZNet *net)
{
	return net->port;
}


int _ggzcore_net_get_fd(GGZNet *net)
{
	return net->fd;
}


int _ggzcore_net_get_tls(struct _GGZNet *net)
{
	return net->use_tls;
}


/* For debugging purposes only! */
void _ggzcore_net_set_fd(GGZNet *net, int fd)
{
	net->fd = fd;
}

void _ggzcore_net_free(GGZNet *net)
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
int _ggzcore_net_connect(GGZNet *net)
{
	ggz_debug(GGZCORE_DBG_NET, "Connecting to %s:%d",
		  net->host, net->port);
	net->fd = ggz_make_socket(GGZ_SOCK_CLIENT, net->port, net->host);
	
	if (net->fd >= 0)
		return 0;  /* success */
	else
		return -1; /* error */
}


void _ggzcore_net_disconnect(GGZNet *net)
{
	ggz_debug(GGZCORE_DBG_NET, "Disconnecting");
	close(net->fd);
	net->fd = -1;
}


/* ggzcore_net_send_XXX() functions for sending messages to the server */

int _ggzcore_net_send_login(GGZNet *net)
{
	GGZLoginType login_type;
	char *type, *handle, *password, *language;
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

	language = getenv("LANG");
	_ggzcore_net_send_line(net, "<LANGUAGE>%s</LANGUAGE>", language);
	
	_ggzcore_net_send_line(net, "<LOGIN TYPE='%s'>", type);
	_ggzcore_net_send_line(net, "<NAME>%s</NAME>", handle);

	if (login_type == GGZ_LOGIN)
		_ggzcore_net_send_line(net, "<PASSWORD>%s</PASSWORD>", password);
	_ggzcore_net_send_line(net, "</LOGIN>");

	if (status < 0)
		_ggzcore_net_error(net, "Sending login");

	return status;
}


int _ggzcore_net_send_channel(GGZNet *net)
{
	char *id;
	int status = 0;

	id = _ggzcore_server_get_handle(net->server);
	
	status = _ggzcore_net_send_line(net, "<CHANNEL ID='%s' />", id);
	if (status < 0)
		_ggzcore_net_error(net, "Sending channel");

	return status;
}


int _ggzcore_net_send_motd(GGZNet *net)
{
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending MOTD request");	
	_ggzcore_net_send_line(net, "<MOTD/>");

	return status;
}


int _ggzcore_net_send_list_types(GGZNet *net, const char verbose)
{
	int status = 0;
	char *full;

	net->gametype_verbose = verbose;

	ggz_debug(GGZCORE_DBG_NET, "Sending gametype list request");	
	full = bool_to_str(verbose);
	
	_ggzcore_net_send_line(net, "<LIST TYPE='game' FULL='%s'/>", full);

	return status;
}


int _ggzcore_net_send_list_rooms(GGZNet *net, const int type, const char verbose)
{	
	int status = 0;
	char *full;
	
	net->room_verbose = verbose;
	ggz_debug(GGZCORE_DBG_NET, "Sending room list request");	
	full = bool_to_str(verbose);
	
	_ggzcore_net_send_line(net, "<LIST TYPE='room' FULL='%s'/>", full);

	return status;
}


int _ggzcore_net_send_join_room(GGZNet *net, const unsigned int id)
{
	int status = 0;
	GGZRoom *room;

	room = _ggzcore_server_get_room_by_id(net->server, id);
	net->new_room = room;
	
	ggz_debug(GGZCORE_DBG_NET, "Sending room join request");	

	_ggzcore_net_send_line(net, "<ENTER ROOM='%d'/>", id);
	
	return status;
}


int _ggzcore_net_send_list_players(GGZNet *net)
{	
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending player list request");
	_ggzcore_net_send_line(net, "<LIST TYPE='player'/>");

	return status;
}


int _ggzcore_net_send_list_tables(GGZNet *net, const int type, const char global)
{	
	int status = 0;

	ggz_debug(GGZCORE_DBG_NET, "Sending table list request");
	_ggzcore_net_send_line(net, "<LIST TYPE='table'/>");
	
	return status;
}


int _ggzcore_net_send_chat(GGZNet *net, const GGZChatType type,
			   const char* player, const char* msg)
{
	const char *type_str;

	ggz_debug(GGZCORE_DBG_NET, "Sending chat");	

	/* FIXME: We need to handle the case where the chat is longer than
	   the server's allowed chat size */

	type_str = ggz_chattype_to_string(type);

	switch (type) {
	case GGZ_CHAT_NORMAL:
	case GGZ_CHAT_ANNOUNCE:
	case GGZ_CHAT_TABLE:
		return _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s'><![CDATA[%s]]></CHAT>",
			type_str, msg);
	case GGZ_CHAT_BEEP:
		return _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s' TO='%s'/>",
			type_str, player);
	case GGZ_CHAT_PERSONAL:
		return _ggzcore_net_send_line(net,
			"<CHAT TYPE='%s' TO='%s'><![CDATA[%s]]></CHAT>",
			type_str, player, msg);
	case GGZ_CHAT_UNKNOWN:
		break;
	}

	ggz_error_msg("ggzcore_net_send_chat: "
		      "unknown chat type given.");

	/* Returning an error would mean a *network* error,
	   which isn't the case. */
	return 0;
}


int _ggzcore_net_send_table_launch(GGZNet *net, GGZTable *table)
{
	int i, type, num_seats, status = 0;
	const char * desc;
	
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


static int _ggzcore_net_send_table_seat(GGZNet *net,
					struct _GGZSeat *seat)
{
	const char *type;

	ggz_debug(GGZCORE_DBG_NET, "Sending seat info");
	
	type = ggz_seattype_to_string(seat->type);
	
	if (!seat->name)
		return _ggzcore_net_send_line(net,
					      "<SEAT NUM='%d' TYPE='%s'/>", 
					      seat->index, type);
	return _ggzcore_net_send_line(net, 
				      "<SEAT NUM='%d' TYPE='%s'>%s</SEAT>", 
				      seat->index,type, seat->name);
}


int _ggzcore_net_send_table_join(GGZNet *net,
				 const unsigned int num,
				 int spectator)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table join request");
	return _ggzcore_net_send_line(net, "<JOIN TABLE='%d' SPECTATOR='%s'/>",
				      num, bool_to_str(spectator));
}

int _ggzcore_net_send_table_leave(GGZNet *net,
				  int force,
				  int spectator)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table leave request");
	return _ggzcore_net_send_line(net,
				      "<LEAVE FORCE='%s' SPECTATOR='%s'/>",
				      bool_to_str(force),
				      bool_to_str(spectator));
}


int _ggzcore_net_send_table_reseat(GGZNet *net,
				   GGZReseatType opcode,
				   int seat_num)
{
	const char *action = NULL;

	switch (opcode) {
	case GGZ_RESEAT_SIT:
		action = "sit";
		break;
	case GGZ_RESEAT_STAND:
		action = "stand";
		seat_num = -1;
		break;
	case GGZ_RESEAT_MOVE:
		action = "move";
		if (seat_num < 0)
			return -1;
		break;
	}

	if (!action) return -1;

	if (seat_num < 0)
		return _ggzcore_net_send_line(net,
					      "<RESEAT ACTION='%s'/>",
					      action);

	return _ggzcore_net_send_line(net,
				      "<RESEAT ACTION='%s' SEAT='%d'/>",
				      action, seat_num);
}


int _ggzcore_net_send_table_seat_update(GGZNet *net, GGZTable *table,
					struct _GGZSeat *seat)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table seat update request");
	_ggzcore_net_send_line(net,
			       "<UPDATE TYPE='table' ACTION='seat' ROOM='%d'>",
			       _ggzcore_room_get_id(table->room));
	_ggzcore_net_send_line(net, "<TABLE ID='%d' SEATS='%d'>",
			       table->id, table->num_seats);
	_ggzcore_net_send_table_seat(net, seat);
	_ggzcore_net_send_line(net, "</TABLE>");
	return _ggzcore_net_send_line(net, "</UPDATE>");
}



int _ggzcore_net_send_table_desc_update(GGZNet *net, GGZTable *table,
					const char *desc)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending table description update request");
	_ggzcore_net_send_line(net,
			       "<UPDATE TYPE='table' ACTION='desc' ROOM='%d'>",
			       _ggzcore_room_get_id(table->room));
	_ggzcore_net_send_line(net, "<TABLE ID='%d'>", table->id);
	_ggzcore_net_send_line(net, "<DESC>%s</DESC>", desc);
	_ggzcore_net_send_line(net, "</TABLE>");
	return _ggzcore_net_send_line(net, "</UPDATE>");	
}


int _ggzcore_net_send_table_boot_update(GGZNet *net, GGZTable *table,
					struct _GGZSeat *seat)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending boot of player %s.", seat->name);

	if (!seat->name)
		return -1;
	seat->type = GGZ_SEAT_PLAYER;
	seat->index = 0;

	_ggzcore_net_send_line(net,
			       "<UPDATE TYPE='table' ACTION='boot' ROOM='%d'>",
			       _ggzcore_room_get_id(table->room));

	_ggzcore_net_send_line(net, "<TABLE ID='%d' SEATS='1'>", table->id);
	_ggzcore_net_send_table_seat(net, seat);
	_ggzcore_net_send_line(net, "</TABLE>");

	return _ggzcore_net_send_line(net, "</UPDATE>");
}


int _ggzcore_net_send_logout(GGZNet *net)
{
	ggz_debug(GGZCORE_DBG_NET, "Sending LOGOUT");	
	return _ggzcore_net_send_line(net, "</SESSION>");
}


/* Check for incoming data */
int _ggzcore_net_data_is_pending(GGZNet *net)
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
int _ggzcore_net_read_data(GGZNet *net)
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
	if ( (len = ggz_tls_read(net->fd, buf, XML_BUFFSIZE)) < 0) {
		
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
	GGZNet *net = data;
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
	GGZNet *net = data;

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
	GGZNet *net = data;
	GGZStack *stack = net->stack;
	GGZXMLElement *top;

	top = ggz_stack_top(stack);
	ggz_xmlelement_add_text(top, text, len);
}


static void _ggzcore_net_error(GGZNet *net, char* message)
{
	ggz_debug(GGZCORE_DBG_NET, "Network error: %s", message);
	_ggzcore_net_disconnect(net);
	_ggzcore_server_net_error(net->server, message);
}


static void _ggzcore_net_dump_data(GGZNet *net, char *data, int size)
{
	if (net->dump_file > 0)
		write(net->dump_file, data, size);
}

static GGZXMLElement* _ggzcore_net_new_element(char *tag, char **attrs)
{
	void (*process_func)();

	/* FIXME: Could we do this with a table lookup? */
	if (strcasecmp(tag, "SERVER") == 0)
		process_func = _ggzcore_net_handle_server;
	else if (strcasecmp(tag, "OPTIONS") == 0)
		process_func = _ggzcore_net_handle_options;
	else if (strcasecmp(tag, "MOTD") == 0)
		process_func = _ggzcore_net_handle_motd;
	else if (strcasecmp(tag, "RESULT") == 0)
		process_func = _ggzcore_net_handle_result;
	else if (strcasecmp(tag, "LIST") == 0)
		process_func = _ggzcore_net_handle_list;
	else if (strcasecmp(tag, "UPDATE") == 0)
		process_func = _ggzcore_net_handle_update;
	else if (strcasecmp(tag, "GAME") == 0)
		process_func = _ggzcore_net_handle_game;
	else if (strcasecmp(tag, "PROTOCOL") == 0)
		process_func = _ggzcore_net_handle_protocol;
	else if (strcasecmp(tag, "ALLOW") == 0)
		process_func = _ggzcore_net_handle_allow;
	else if (strcasecmp(tag, "ABOUT") == 0)
		process_func = _ggzcore_net_handle_about;
	else if (strcasecmp(tag, "ROOM") == 0)
		process_func = _ggzcore_net_handle_room;
	else if (strcasecmp(tag, "PLAYER") == 0)
		process_func = _ggzcore_net_handle_player;
	else if (strcasecmp(tag, "TABLE") == 0)
		process_func = _ggzcore_net_handle_table;
	else if (strcasecmp(tag, "SEAT") == 0)
		process_func = _ggzcore_net_handle_seat;
	else if (strcasecmp(tag, "SPECTATOR") == 0)
		process_func = _ggzcore_net_handle_spectator_seat;
	else if (strcasecmp(tag, "LEAVE") == 0)
		process_func = _ggzcore_net_handle_leave;
	else if (strcasecmp(tag, "JOIN") == 0)
		process_func = _ggzcore_net_handle_join;
	else if (strcasecmp(tag, "CHAT") == 0)
		process_func = _ggzcore_net_handle_chat;
	else if (strcasecmp(tag, "DESC") == 0)
		process_func = _ggzcore_net_handle_desc;
	else if (strcasecmp(tag, "PASSWORD") == 0)
		process_func = _ggzcore_net_handle_password;
	else if (strcasecmp(tag, "PING") == 0)
		process_func = _ggzcore_net_handle_ping;
	else if (strcasecmp(tag, "SESSION") == 0)
		process_func = _ggzcore_net_handle_session;
	else
		process_func = NULL;
	
	return ggz_xmlelement_new(tag, attrs, process_func, NULL);
}


/* Functions for <SERVER> tag */
void _ggzcore_net_handle_server(GGZNet *net, GGZXMLElement *element)
{
	char *name, *id, *status, *tls;
	int version;
	int *chatlen;

	if (!element) return;

	name = ATTR(element, "NAME");
	id = ATTR(element, "ID");
	status = ATTR(element, "STATUS");
	version = str_to_int(ATTR(element, "VERSION"), -1);
	tls = ATTR(element, "TLS_SUPPORT");

	chatlen = ggz_xmlelement_get_data(element);
	if (chatlen) {
		net->chat_size = *chatlen;
		ggz_free(chatlen);
	} else {
		/* If no chat length is specified, assume an unlimited
		   (i.e. really large) length. */
		net->chat_size = chatlen ? *chatlen : UINT_MAX;
	}

	ggz_debug(GGZCORE_DBG_NET, 
		  "%s(%s) : status %s: protocol %d: chat size %u tls: %s",
		  name, id, status, version, net->chat_size, tls);

	/* FIXME: Do something with name, status */
	if (version == GGZ_CS_PROTO_VERSION) {
		/* Everything checked out so start session */
		_ggzcore_net_send_header(net);

		/* If TLS is enabled set it up */
		if(tls && !strcmp(tls, "yes") && _ggzcore_net_get_tls(net) == 1 && ggz_tls_support_query())
			_ggzcore_net_negotiate_tls(net);

		_ggzcore_server_set_negotiate_status(net->server, net,
						     E_OK);
	} else
		_ggzcore_server_set_negotiate_status(net->server, net, -1);
}


/* Functions for <OPTIONS> tag */
static void _ggzcore_net_handle_options(GGZNet *net, GGZXMLElement *element)
{
	int *len, chatlen;
	GGZXMLElement *parent;
	const char *parent_tag;

	if (!element) return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return; /* should this be an error? */
	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "SERVER") != 0)
		return; /* should this be an error? */

	/* Read the server's maximum chat length. */
	chatlen = str_to_int(ATTR(element, "CHATLEN"), -1);
	if (chatlen < 0) return;

	len = ggz_malloc(sizeof(*len));
	*len = chatlen;
	ggz_xmlelement_set_data(parent, len);
}


/* Functions for <MOTD> tag */
static void _ggzcore_net_handle_motd(GGZNet *net, GGZXMLElement *element)
{
	char *message, *priority;
	char **buffer;

	message = ggz_xmlelement_get_text(element);
	priority = ATTR(element, "PRIORITY");
	
	ggz_debug(GGZCORE_DBG_NET, "Motd of priority %s", priority);

	/* In the old interface the MOTD was sent a line at a time,
	   NULL-terminated.  Now it's just sent all at once. */
	buffer = ggz_malloc(2 * sizeof(char*));
	buffer[0] = message;
	buffer[1] = NULL;

	/* FIXME: do something with the priority */
	_ggzcore_server_event(net->server, GGZ_MOTD_LOADED, buffer);

	ggz_free(buffer);
}


/* Functions for <RESULT> tag */
static void _ggzcore_net_handle_result(GGZNet *net, GGZXMLElement *element)
{
	GGZRoom *room;
	char *action;
	GGZClientReqError code;
	void *data;
	char *message;

	if (!element) return;

	action = ATTR(element, "ACTION");
	code = ggz_string_to_error(ATTR(element, "CODE"));
	data = ggz_xmlelement_get_data(element);

	ggz_debug(GGZCORE_DBG_NET, "Result of %s was %d", action, 
		  code);

	room = _ggzcore_server_get_cur_room(net->server);

	if (strcasecmp(action, "login") == 0) {
		/* Password may have already been updated. */
		_ggzcore_server_set_login_status(net->server, code);
	} else if (strcasecmp(action, "enter") == 0) {
		if (code == E_OK)
			_ggzcore_server_set_room(net->server, net->new_room);
		_ggzcore_server_set_room_join_status(net->server, code);
	} else if  (strcasecmp(action, "launch") == 0)
		_ggzcore_room_set_table_launch_status(room, code);
	else if  (strcasecmp(action, "join") == 0)
		_ggzcore_room_set_table_join_status(room, code);
	else if  (strcasecmp(action, "leave") == 0)
		_ggzcore_room_set_table_leave_status(room, code);
	else if  (strcasecmp(action, "chat") == 0) {
		if (code != E_OK) {
			GGZErrorEventData error = {status: code};

			switch (code) {
			case E_NOT_IN_ROOM:
				snprintf(error.message, sizeof(error.message),
					 "Not in a room");
				break;
			case E_BAD_OPTIONS:
				snprintf(error.message, sizeof(error.message),
					 "Bad options");
				break;
			case E_NO_PERMISSION:
				snprintf(error.message, sizeof(error.message),
					 "Prohibited");
				break;
			case E_USR_LOOKUP:
				snprintf(error.message, sizeof(error.message),
					 "No such player");
				break;
			case E_AT_TABLE:
				snprintf(error.message, sizeof(error.message),
					 "Can't chat at table");
				break;
			default:
				snprintf(error.message, sizeof(error.message),
					 "Unknown error");
				break;
			}
			_ggzcore_server_event(net->server, GGZ_CHAT_FAIL,
					      &error);
		}
	} else if (strcasecmp(action, "protocol") == 0) {
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

	/* FIXME: memory leak on tag data */
}


/* Functions for <PASSWORD> tag */
static void _ggzcore_net_handle_password(GGZNet *net, GGZXMLElement *element)
{
	char *password;
	GGZXMLElement *parent;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	password = ggz_xmlelement_get_text(element);
	if (!password)
		return;

	/* This means the server can change our password at any time just
	   by sending us a <PASSWORD> tag.  This could be bad, but fixing
	   it so that we only accept new passwords when we asked for them
	   would be very difficult.  Plus, the password system needs to
	   be changed anyway... */
	_ggzcore_server_set_password(net->server, password);
}


/* Functions for <LIST> tag */
static void _ggzcore_net_handle_list(GGZNet *net, GGZXMLElement *element)
{
	GGZList *list;
	GGZListEntry *entry;
	GGZRoom *room;
	int count, room_num;
	char *type;

	if (!element)
		return;

	/* Grab list data from tag */
	type = ATTR(element, "TYPE");
	list = ggz_xmlelement_get_data(element);
	room_num = str_to_int(ATTR(element, "ROOM"), -1);

	/* Get length of list */
	/* FIXME: we should be able to get this from the list itself */
	count = 0;
	for (entry = ggz_list_head(list);
	     entry;
	     entry = ggz_list_next(entry))
		count++;

	if (strcasecmp(type, "room") == 0) {
		/* Clear existing list (if any) */
		if (_ggzcore_server_get_num_rooms(net->server) > 0)
			_ggzcore_server_free_roomlist(net->server);

		_ggzcore_server_init_roomlist(net->server, count);

		for (entry = ggz_list_head(list);
		     entry;
		     entry = ggz_list_next(entry)) {
			_ggzcore_server_add_room(net->server,
						 ggz_list_get_data(entry));
		}
		_ggzcore_server_event(net->server, GGZ_ROOM_LIST, NULL);
	} else if (strcasecmp(type, "game") == 0) {
		/* Free previous list of types*/
		if (ggzcore_server_get_num_gametypes(net->server) > 0)
			_ggzcore_server_free_typelist(net->server);

		_ggzcore_server_init_typelist(net->server, count);
		for (entry = ggz_list_head(list);
		     entry;
		     entry = ggz_list_next(entry)) {
			_ggzcore_server_add_type(net->server,
						 ggz_list_get_data(entry));
		}
		_ggzcore_server_event(net->server, GGZ_TYPE_LIST, NULL);
	} else if (strcasecmp(type, "player") == 0) {
		room = _ggzcore_server_get_room_by_id(net->server, room_num);
		_ggzcore_room_set_player_list(room, count, list);
		list = NULL; /* avoid freeing list */
	} else if (strcasecmp(type, "table") == 0) {
		room = _ggzcore_server_get_room_by_id(net->server, room_num);
		_ggzcore_room_set_table_list(room, count, list);
		list = NULL; /* avoid freeing list */
	}

	if (list)
		ggz_list_free(list);
}


static void _ggzcore_net_list_insert(GGZXMLElement *list_tag, void *data)
{
	GGZList *list = ggz_xmlelement_get_data(list_tag);
	
	/* If list doesn't already exist, create it */
	if (!list) {
		/* Setup actual list */	
		char *type = ATTR(list_tag, "TYPE");
		ggzEntryCompare compare_func = NULL;
		ggzEntryCreate  create_func = NULL;
		ggzEntryDestroy destroy_func = NULL;

		if (strcasecmp(type, "game") == 0) {}
		else if (strcasecmp(type, "room") == 0) {}
		else if (strcasecmp(type, "player") == 0) {
			compare_func = _ggzcore_player_compare;
			destroy_func = _ggzcore_player_destroy;
		}		
		else if (strcasecmp(type, "table") == 0) {
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
static void _ggzcore_net_handle_update(GGZNet *net, GGZXMLElement *element)
{
	char *action, *type;

	/* Return if there's no tag */
	if (!element)
		return;
	
	/* Grab update data from tag */
	type = ATTR(element, "TYPE");
	action = ATTR(element, "ACTION");

	if (strcasecmp(type, "room") == 0) {
		/* FIXME: implement this */
	} else if (strcasecmp(type, "game") == 0) {
		/* FIXME: implement this */
	} else if (strcasecmp(type, "player") == 0)
		_ggzcore_net_player_update(net, element, action);
	else if (strcasecmp(type, "table") == 0)
		_ggzcore_net_table_update(net, element, action);
}


/* Handle Player update */
static void _ggzcore_net_player_update(GGZNet *net, GGZXMLElement *update,
				       char *action)
{
	int room_num;
	GGZPlayer *player;
	GGZRoom *room;

	room_num = str_to_int(ATTR(update, "ROOM"), -1);

	player = ggz_xmlelement_get_data(update);
	room = _ggzcore_server_get_room_by_id(net->server, room_num);
	
	if (strcasecmp(action, "add") == 0)
		_ggzcore_room_add_player(room, player);
	else if (strcasecmp(action, "delete") == 0)
		_ggzcore_room_remove_player(room, player->name);
	else if (strcasecmp(action, "lag") == 0) {
		/* FIXME: Should be a player "class-based" event */
		_ggzcore_room_set_player_lag(room, player->name, player->lag);
	} else if (strcasecmp(action, "stats") == 0) {
		/* FIXME: Should be a player "class-based" event */
		_ggzcore_room_set_player_stats(room, player);
	}

	_ggzcore_player_free(player);
}


/* Handle table update */
static void _ggzcore_net_table_update(GGZNet *net, GGZXMLElement *update,
				      char *action)
{
	int i, room_num;
	GGZTable *table, *table_data;
	GGZRoom *room;
	char *room_str;

	/* Sanity check: we can't proceed without a room number */
	room_str = ATTR(update, "ROOM");
	if (!room_str) {
		/* Assume we're talking about the current room.  This is
		   no doubt preferable to simply dropping the connection. */
		room = ggzcore_server_get_cur_room(net->server);
		room_num = _ggzcore_room_get_id(room);
	} else
		room_num = str_to_int(room_str, -1);


	room = _ggzcore_server_get_room_by_id(net->server, room_num);
	if (!room) {
		/* We could use the current room in this case too, but
		   that would be more dangerous. */
		char msg[256];
		snprintf(msg, sizeof(msg),
			 "Server specified non-existent room '%s'",
			 room_str);
		_ggzcore_server_protocol_error(net->server, msg);
		return;
	}
	
	table_data = ggz_xmlelement_get_data(update);
	table = _ggzcore_room_get_table_by_id(room, table_data->id);

	/* Table can only be NULL if we're adding it */
	if (!table && strcasecmp(action, "add") != 0) {
		char msg[256];
		snprintf(msg, sizeof(msg),
			 "Server specified non-existent table %d",
			 table_data->id);
		_ggzcore_server_protocol_error(net->server, msg);
		return;
	}

	if (strcasecmp(action, "add") == 0) {
		_ggzcore_room_add_table(room, table_data);
		/* Set table_data to NULL so it doesn't get freed at
                   the end of this function.  You would think this wouldn't
		   be necessary (since the table is inserted into a list,
		   which should copy it), but it appears as though it is. */
		table_data = NULL;
	} else if (strcasecmp(action, "delete") == 0)
		_ggzcore_room_remove_table(room, table_data->id);
	else if (strcasecmp(action, "join") == 0) {
		/* Loop over both seats and spectators. */
		for (i = 0; i < table_data->num_seats; i++)
			if (table_data->seats[i].type != GGZ_SEAT_NONE)
				_ggzcore_table_set_seat(table,
					&table_data->seats[i]);
		for (i = 0; i < table_data->num_spectator_seats; i++) {
			if (table_data->spectator_seats[i].name) {
				_ggzcore_table_set_spectator_seat(table,
					&table_data->spectator_seats[i]);
			}
		}
	} else if (strcasecmp(action, "leave") == 0) {
		/* The server sends us the player that is leaving - that
		   is, a GGZ_SEAT_PLAYER not a GGZ_SEAT_OPEN.  It may
		   be either a regular or a spectator seat. */
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
	} else if (strcasecmp(action, "status") == 0) {
		_ggzcore_table_set_state(table, table_data->state);
	} else if (strcasecmp(action, "desc") == 0) {
		_ggzcore_table_set_desc(table, table_data->desc);
	} else if (strcasecmp(action, "seat") == 0) {
		for (i = 0; i < table_data->num_seats; i++) 
			if (table_data->seats[i].type != GGZ_SEAT_NONE)
				_ggzcore_table_set_seat(table, &table_data->seats[i]);
	}
			
	if (table_data)
		_ggzcore_table_free(table_data);

}


/* Functions for <GAME> tag */
static void _ggzcore_net_handle_game(GGZNet *net, GGZXMLElement *element)
{
	GGZGameType *type;
	GGZGameData *data;
	GGZXMLElement *parent;
	const char *parent_tag, *parent_type;
	int id;
	char *name, *version;
	const char *prot_engine = NULL;
	const char *prot_version = NULL;
	GGZNumberList player_allow_list = ggz_numberlist_new();
	GGZNumberList bot_allow_list = ggz_numberlist_new();
	int spectators_allow = 0;
	const char *desc = NULL;
	const char *author = NULL;
	const char *url = NULL;

	if (!element)
		return;

	/* Get game data from tag */
	id = str_to_int(ATTR(element, "ID"), -1);
	name = ATTR(element, "NAME");
	version = ATTR(element, "VERSION");
	data = ggz_xmlelement_get_data(element);

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

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	parent_tag = ggz_xmlelement_get_tag(parent);
	parent_type = ATTR(parent, "TYPE");

	if (parent
	    && strcasecmp(parent_tag, "LIST") == 0
	    && strcasecmp(parent_type, "game") == 0)
		_ggzcore_net_list_insert(parent, type);
	else
		_ggzcore_gametype_free(type);

	/* Free game data */
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


/* This should not be called by the parent tag, but only by the
   child tags to set data for the parent. */
static GGZGameData *_ggzcore_net_game_get_data(GGZXMLElement *game)
{
	GGZGameData *data = ggz_xmlelement_get_data(game);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = ggz_malloc(sizeof(GGZGameData));
		ggz_xmlelement_set_data(game, data);
	}

	return data;
}


static void _ggzcore_net_game_set_protocol(GGZXMLElement *game,
					   char *engine, char *version)
{
	GGZGameData *data = _ggzcore_net_game_get_data(game);

	if (!data->prot_engine)
		data->prot_engine = ggz_strdup(engine);
	if (!data->prot_version)
		data->prot_version = ggz_strdup(version);
}


static void _ggzcore_net_game_set_allowed(GGZXMLElement *game,
					  GGZNumberList players, 
					  GGZNumberList bots,
					  int spectators)
{
	GGZGameData *data = _ggzcore_net_game_get_data(game);

	data->player_allow_list = players;
	data->bot_allow_list = bots;
	data->spectators_allow = spectators;
}


static void _ggzcore_net_game_set_info(GGZXMLElement *game,
				       char *author, char *url)
{
	GGZGameData *data = _ggzcore_net_game_get_data(game);

	if (!data->author)
		data->author = ggz_strdup(author);
	if (!data->url)
		data->url = ggz_strdup(url);
}


static void _ggzcore_net_game_set_desc(GGZXMLElement *game, char *desc)
{
	GGZGameData *data = _ggzcore_net_game_get_data(game);

	if (!data->desc)
		data->desc = ggz_strdup(desc);
}


/* Functions for <PROTOCOL> tag */
static void _ggzcore_net_handle_protocol(GGZNet *net, GGZXMLElement *element)
{
	GGZXMLElement *parent;
	const char *parent_tag;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "GAME") != 0)
		return;

	_ggzcore_net_game_set_protocol(parent,
				       ATTR(element, "ENGINE"),
				       ATTR(element, "VERSION"));
}


/* Functions for <ALLOW> tag */
static void _ggzcore_net_handle_allow(GGZNet *net, GGZXMLElement *element)
{
	GGZXMLElement *parent;
	const char *parent_tag;
	GGZNumberList players, bots;
	int spectators;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "GAME") != 0)
		return;

	players = ggz_numberlist_read(ATTR(element, "PLAYERS"));
	bots = ggz_numberlist_read(ATTR(element, "BOTS"));
	spectators = str_to_bool(ATTR(element, "SPECTATORS"), 0);

	_ggzcore_net_game_set_allowed(parent, players, bots, spectators);
}


/* Functions for <ABOUT> tag */
static void _ggzcore_net_handle_about(GGZNet *net, GGZXMLElement *element)
{
	GGZXMLElement *parent;
	const char *parent_tag;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "GAME") != 0)
		return;

	_ggzcore_net_game_set_info(parent, 
				   ATTR(element, "AUTHOR"),
				   ATTR(element, "URL"));
}


/* Functions for <DESC> tag */
static void _ggzcore_net_handle_desc(GGZNet *net, GGZXMLElement *element)
{
	char *desc;
	const char *parent_tag;
	GGZXMLElement *parent;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	desc = ggz_xmlelement_get_text(element);

	parent_tag = ggz_xmlelement_get_tag(parent);

	/* This tag can be a child of <GAME>, <ROOM>, or <TABLE>. */
	if (strcasecmp(parent_tag, "GAME") == 0)
		_ggzcore_net_game_set_desc(parent, desc);
	else if (strcasecmp(parent_tag, "ROOM") == 0) {
		if (!ggz_xmlelement_get_data(parent))
			ggz_xmlelement_set_data(parent, ggz_strdup(desc));
	} else if (strcasecmp(parent_tag, "TABLE") == 0)
		_ggzcore_net_table_set_desc(parent, desc);
}


/* Functions for <ROOM> tag */
static void _ggzcore_net_handle_room(GGZNet *net, GGZXMLElement *element)
{
	GGZRoom *ggz_room;
	int id, game;
	char *name, *desc;
	GGZXMLElement *parent;
	const char *parent_tag, *parent_type;

	if (!element)
		return;

	/* Grab data from tag */
	id = str_to_int(ATTR(element, "ID"), -1);
	name = ATTR(element, "NAME");
	game = str_to_int(ATTR(element, "GAME"), -1);
	desc = ggz_xmlelement_get_data(element);

	/* Set up GGZRoom object */
	ggz_room = _ggzcore_room_new();
	_ggzcore_room_init(ggz_room, net->server, id, name, game, desc);

	/* Free description if present */
	if (desc)
		ggz_free(desc);

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	parent_tag = ggz_xmlelement_get_tag(parent);
	parent_type = ATTR(parent, "TYPE");

	if (parent
	    && strcasecmp(parent_tag, "LIST") == 0
	    && strcasecmp(parent_type, "room") == 0)
		_ggzcore_net_list_insert(parent, ggz_room);
	else
		_ggzcore_room_free(ggz_room);
}


/* Functions for <PLAYER> tag */
static void _ggzcore_net_handle_player(GGZNet *net, GGZXMLElement *element)
{
	GGZPlayer *ggz_player;
	GGZPlayerType type;
	GGZRoom *room;
	char *name, *str_type;
	int table, lag;
	GGZXMLElement *parent;
	const char *parent_tag, *parent_type;

	if (!element)
		return;

	room = ggzcore_server_get_cur_room(net->server);

	/* Grab player data from tag */
	str_type = ATTR(element, "TYPE");
	name = ATTR(element, "ID");
	table = str_to_int(ATTR(element, "TABLE"), -1);
	lag = str_to_int(ATTR(element, "LAG"), 0);

	/* Set player's type */
	if (!str_type || strcasecmp(str_type, "guest") == 0)
		type = GGZ_PLAYER_GUEST;
	else if (strcasecmp(str_type, "normal") == 0)
		type = GGZ_PLAYER_NORMAL;
	else if (strcasecmp(str_type, "admin") == 0)
		type = GGZ_PLAYER_ADMIN;
	else
		type = GGZ_PLAYER_GUEST;

	/* Set up GGZPlayer object */
	ggz_player = _ggzcore_player_new();
	_ggzcore_player_init(ggz_player,  name, room, table, type, lag);

	/* FIXME: should these be initialized through an accessor function? */
	ggz_player->wins = str_to_int(ATTR(element, "WINS"), NO_RECORD);
	ggz_player->ties = str_to_int(ATTR(element, "TIES"), NO_RECORD);
	ggz_player->losses = str_to_int(ATTR(element, "LOSSES"), NO_RECORD);
	ggz_player->forfeits = str_to_int(ATTR(element, "FORFEITS"),
					  NO_RECORD);
	ggz_player->rating = str_to_int(ATTR(element, "RATING"), NO_RATING);
	ggz_player->ranking = str_to_int(ATTR(element, "RANKING"), NO_RANKING);

	/* FIXME: highscore is a long... */
	ggz_player->highscore = str_to_int(ATTR(element, "HIGHSCORE"),
					   NO_HIGHSCORE);

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	parent_tag = ggz_xmlelement_get_tag(parent);
	parent_type = ATTR(parent, "TYPE");

	if (parent
	    && strcasecmp(parent_tag, "LIST") == 0
	    && strcasecmp(parent_type, "player") == 0)
		_ggzcore_net_list_insert(parent, ggz_player);
	else if (parent
		 && strcasecmp(parent_tag, "UPDATE") == 0
		 && strcasecmp(parent_type, "player") == 0)
		ggz_xmlelement_set_data(parent, ggz_player);
	else
		_ggzcore_player_free(ggz_player);
}


/* Functions for <TABLE> tag */
static void _ggzcore_net_handle_table(GGZNet *net, GGZXMLElement *element)
{
	GGZGameType *type;
	GGZTableData *data;
	GGZTable *table_obj;
	GGZList *seats = NULL;
	GGZList *spectatorseats = NULL;
	GGZListEntry *entry;
	int id, game, status, num_seats, num_spectators, i;
	const char *desc = NULL;
	GGZXMLElement *parent;
	const char *parent_tag, *parent_type;

	if (!element)
		return;

	/* Grab table data from tag */
	id = str_to_int(ATTR(element, "ID"), -1);
	game = str_to_int(ATTR(element, "GAME"), -1);
	status = str_to_int(ATTR(element, "STATUS"), 0);
	num_seats = str_to_int(ATTR(element, "SEATS"), 0);
	num_spectators = str_to_int(ATTR(element, "SPECTATORS"), -1);
	data = ggz_xmlelement_get_data(element);
	if (data) {
		desc = data->desc;
		seats = data->seats;
		spectatorseats = data->spectatorseats;
	}

	/* Create new table structure */
	table_obj = _ggzcore_table_new();
	type = _ggzcore_server_get_type_by_id(net->server, game);
	_ggzcore_table_init(table_obj, type, desc, num_seats, status, id);

	/* Initialize seats to none */
	/* FIXME: perhaps tables should come this way? */
	for (i = 0; i < num_seats; i++)
		table_obj->seats[i].type = GGZ_SEAT_NONE;

	/* Add seats */
	entry = ggz_list_head(seats);
	while (entry) {
		struct _GGZSeat* seat = ggz_list_get_data(entry);
		_ggzcore_table_set_seat(table_obj, seat);
		entry = ggz_list_next(entry);
	}

	/* Add spectator seats */
	entry = ggz_list_head(spectatorseats);
	while (entry) {
		struct _GGZSeat* seat = ggz_list_get_data(entry);
		_ggzcore_table_set_spectator_seat(table_obj, seat);
		entry = ggz_list_next(entry);
	}


	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	parent_tag = ggz_xmlelement_get_tag(parent);
	parent_type = ATTR(parent, "TYPE");

	if (parent
	    && strcasecmp(parent_tag, "LIST") == 0
	    && strcasecmp(parent_type, "table") == 0) {
		_ggzcore_net_list_insert(parent, table_obj);
	} else if (parent
		   && strcasecmp(parent_tag, "UPDATE") == 0
		   && strcasecmp(parent_type, "table") == 0) {
		ggz_xmlelement_set_data(parent, table_obj);
	} else {
		_ggzcore_table_free(table_obj);
	}

	if (data)
		_ggzcore_net_tabledata_free(data);
}

static GGZTableData *_ggzcore_net_table_get_data(GGZXMLElement *table)
{
	GGZTableData *data = ggz_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _ggzcore_net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}

	return data;
}

static void _ggzcore_net_table_add_seat(GGZXMLElement *table,
					struct _GGZSeat *seat,
					int spectator)
{
	GGZTableData *data = _ggzcore_net_table_get_data(table);

	if (spectator) {
		ggz_list_insert(data->spectatorseats, seat);
	} else {
		ggz_list_insert(data->seats, seat);
	}
}

static void _ggzcore_net_table_set_desc(GGZXMLElement *table, char *desc)
{
	GGZTableData *data = _ggzcore_net_table_get_data(table);

	if (!data->desc)
		data->desc = ggz_strdup(desc);
}


static GGZTableData* _ggzcore_net_tabledata_new(void)
{
	GGZTableData *data = ggz_malloc(sizeof(GGZTableData));

	data->seats = ggz_list_create(NULL, 
				      (ggzEntryCreate)_ggzcore_net_seat_copy, 
				      (ggzEntryDestroy)_ggzcore_net_seat_free, 
				      GGZ_LIST_ALLOW_DUPS);

	data->spectatorseats = ggz_list_create(NULL, 
				(ggzEntryCreate)_ggzcore_net_seat_copy, 
				(ggzEntryDestroy)_ggzcore_net_seat_free, 
				GGZ_LIST_ALLOW_DUPS);

	return data;
}


static void _ggzcore_net_tabledata_free(GGZTableData *data)
{
	if (!data)
		return;

	if (data->desc)
		ggz_free(data->desc);
	if (data->seats)
		ggz_list_free(data->seats);
	if (data->spectatorseats)
		ggz_list_free(data->spectatorseats);
	ggz_free(data);
}


/* Functions for <SEAT> tag */
static void _ggzcore_net_handle_seat(GGZNet *net, GGZXMLElement *element)
{
	struct _GGZSeat seat_obj;
	GGZXMLElement *parent;
	const char *parent_tag;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	/* Make sure parent is <TABLE> */
	parent_tag = ggz_xmlelement_get_tag(parent);
	if (!parent_tag
	    || strcasecmp(parent_tag, "TABLE"))
		return;

	/* Get seat information out of tag */
	seat_obj.index = str_to_int(ATTR(element, "NUM"), -1);
	seat_obj.type = ggz_string_to_seattype(ATTR(element, "TYPE"));
	seat_obj.name = ggz_xmlelement_get_text(element);
	_ggzcore_net_table_add_seat(parent, &seat_obj, 0);
}

/* Functions for <SPECTATOR> tag */
static void _ggzcore_net_handle_spectator_seat(GGZNet *net,
					       GGZXMLElement *element)
{
	struct _GGZSeat seat_obj;
	GGZXMLElement *parent;
	const char *parent_tag;

	if (!element)
		return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent)
		return;

	/* Make sure parent is <TABLE> */
	parent_tag = ggz_xmlelement_get_tag(parent);
	if (!parent_tag
	    || strcasecmp(parent_tag, "TABLE"))
		return;

	/* Get seat information out of tag */
	seat_obj.index = str_to_int(ATTR(element, "NUM"), -1);
	seat_obj.name = ggz_xmlelement_get_text(element);
	_ggzcore_net_table_add_seat(parent, &seat_obj, 1);
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
	if (!seat)
		return;

	if (seat->name)
		ggz_free(seat->name);
	ggz_free(seat);
}


static void _ggzcore_net_handle_leave(GGZNet *net, GGZXMLElement *element)
{
	GGZRoom *room;
	GGZLeaveType reason;
	const char *player;

	if (!element) return;

	room = _ggzcore_server_get_cur_room(net->server);
	reason = ggz_string_to_leavetype(ATTR(element, "REASON"));
	player = ATTR(element, "PLAYER");

	_ggzcore_room_set_table_leave(room, reason, player);
}


static void _ggzcore_net_handle_join(GGZNet *net, GGZXMLElement *element)
{
	GGZRoom *room;
	int table;

	if (!element) return;

	room = _ggzcore_server_get_cur_room(net->server);
	table = str_to_int(ATTR(element, "TABLE"), -1);

	_ggzcore_room_set_table_join(room, table);
}


/* Functions for <CHAT> tag */
static void _ggzcore_net_handle_chat(GGZNet *net, GGZXMLElement *element)
{
	char *msg, *type_str, *from;
	GGZRoom *room;
	GGZChatType type;

	if (!element) return;

	/* Grab chat data from tag */
	type_str = ATTR(element, "TYPE");
	from = ATTR(element, "FROM");
	msg = ggz_xmlelement_get_text(element);

	ggz_debug(GGZCORE_DBG_NET, "%s message from %s: '%s'",
		  type_str, from, msg);	

	type = ggz_string_to_chattype(type_str);

	if (!from && type != GGZ_CHAT_UNKNOWN) {
		/* Ignore any message that has no sender. */
		return;
	}

	if (!msg
	    && type != GGZ_CHAT_BEEP
	    && type != GGZ_CHAT_UNKNOWN) {
		/* Ignore an empty message, except for the
		   appropriate chat types. */
		return;
	}

	room = ggzcore_server_get_cur_room(net->server);
	_ggzcore_room_add_chat(room, type, from, msg);
}


/* Function for <PING> tag */
static void _ggzcore_net_handle_ping(GGZNet *net, GGZXMLElement *element)
{
	/* No need to bother the client or anything, just send pong */
	const char *id = ATTR(element, "ID");
	_ggzcore_net_send_pong(net, id);
}


/* Function for <SESSION> tag */
static void _ggzcore_net_handle_session(GGZNet *net, GGZXMLElement *element)
{
	/* Note we only get this after </SESSION> is sent. */
	_ggzcore_server_session_over(net->server, net);
}

/* Send the session header */
void _ggzcore_net_send_header(GGZNet *net)
{
	_ggzcore_net_send_line(net,
			       "<?xml version='1.0' encoding='ISO-8859-1'?>");
	_ggzcore_net_send_line(net, "<SESSION>");
}


int _ggzcore_net_send_pong(GGZNet *net, const char *id)
{
	if (id)
		return _ggzcore_net_send_line(net, "<PONG ID='%s'/>", id);
	else
		return _ggzcore_net_send_line(net, "<PONG/>");
}


/* Send a TLS_START notice and negotiate the handshake */
static void _ggzcore_net_negotiate_tls(GGZNet *net)
{
	int ret;

	_ggzcore_net_send_line(net, "<TLS_START/>");
	/* This should return a status one day to tell client if */
	/* the handshake failed for some reason */
	ret = ggz_tls_enable_fd(net->fd, GGZ_TLS_CLIENT, GGZ_TLS_VERIFY_NONE);
	if(!ret) net->use_tls = 0;
}


static int _ggzcore_net_send_line(GGZNet *net, char *line, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, line);
	vsprintf(buf, line, ap);
	va_end(ap);
	strcat(buf, "\n");

	/* Some of our callers assume we return 0 on success.  So don't
	   return a positive value... */
	if (ggz_tls_write(net->fd, buf, strlen(buf)) < 0)
		return -1;
	return 0;
}


static int str_to_int(const char *str, int dflt)
{
	int val;

	if (!str || sscanf(str, "%d", &val) < 1)
		return dflt;

	return val;
}
