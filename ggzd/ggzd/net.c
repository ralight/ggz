/*
 * File: net.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 9/22/01
 * Desc: Functions for handling network IO
 * $Id: net.c 5938 2004-02-16 06:32:13Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <expat.h>
#include <ggz.h>

#include "client.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "hash.h"
#include "motd.h"
#include "net.h"
#include "protocols.h"
#include "seats.h"


/* For convenience */
#define BUFFSIZE 8192


/* GGZNet structure for handling the network connection to the server */
struct _GGZNetIO {

	/* The client this object serves */
	GGZClient *client;

	/* The actual socket */
	int fd;

	/* Flag to indicate we're in a parse call */
	bool parsing;

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
	bool tag_seen;
};


/* Table data structure */
typedef struct _GGZTableData {
	char *desc;
	GGZList *seats;
} GGZTableData;


/* Seat data structure */
typedef struct _GGZSeatData {
	int index;
	const char *type; 
	const char *name;
} GGZSeatData;


/* Authentication data structure */
typedef struct _GGZAuthData {
	char *name;
	char *password;
} GGZAuthData;


/* Callbacks for XML parser */
static void _net_parse_start_tag(void * data, const char *el,
				 const char **attr);
static void _net_parse_end_tag(void *data, const char *el);
static void _net_parse_text(void *data, const char *text, int len);
static GGZXMLElement* _net_new_element(const char *tag,
				       const char * const * attrs);

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
static void _net_handle_reseat(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_launch(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_table(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_seat(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_desc(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_motd(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_pong(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_ping(GGZNetIO *net, GGZXMLElement *element);
static void _net_handle_tls_start(GGZNetIO *net, GGZXMLElement *element);

/* Utility functions */
static int str_to_int(const char *str, int dflt);
static bool check_playerconn(GGZNetIO *net, const char *type);
static void _net_dump_data(struct _GGZNetIO *net, char *data, int size);
static GGZReturn _net_send_result(GGZNetIO *net, const char *action,
				  GGZClientReqError code);
static GGZReturn _net_send_login_normal_status(GGZNetIO *net,
					       GGZClientReqError status);
static GGZReturn _net_send_login_anon_status(GGZNetIO *net,
					     GGZClientReqError status);
static GGZReturn _net_send_login_new_status(GGZNetIO *net,
					    GGZClientReqError status,
					    const char *password);
static GGZReturn _net_send_table_status(GGZNetIO *net, GGZTable *table);
static GGZReturn _net_send_table_seat(GGZNetIO *net, GGZTable *table,
				      GGZTableSeat *seat);
static GGZReturn _net_send_table_spectator(GGZNetIO *net, GGZTable *table,
					   GGZTableSpectator *spectator);
static GGZReturn _net_send_table_desc(GGZNetIO *net, GGZTable *table);
static GGZReturn _net_send_seat(GGZNetIO *net, GGZTableSeat *seat);
static GGZReturn _net_send_spectator(GGZNetIO *net,
				     GGZTableSpectator *spectator);
static GGZReturn _net_send_pong(GGZNetIO *net, const char *id);

static GGZReturn _net_send_room_player_count(GGZNetIO *net,
					     int index, int player_count);
static GGZReturn _net_send_line(GGZNetIO *net, char *line, ...)
				ggz__attribute((format(printf, 2, 3)));

static GGZAuthData* _net_authdata_new(void);
static void _net_authdata_free(GGZAuthData *data);
static void _net_auth_set_name(GGZXMLElement *login, char *name);
static void _net_auth_set_password(GGZXMLElement *login, char *password);
static void _net_table_add_seat(GGZXMLElement*, GGZSeatData*);
static void _net_table_set_desc(GGZXMLElement*, const char*);
static GGZTableData* _net_tabledata_new(void);
static void _net_tabledata_free(GGZTableData*);
static void* _net_seat_copy(void *data);
static void _net_seat_free(GGZSeatData*);



/* Internal library functions (prototypes in net.h) */

GGZNetIO* net_new(int fd, GGZClient *client)
{
	GGZNetIO *net = NULL;

	net = ggz_malloc(sizeof(GGZNetIO));
	
	/* Set fd to invalid value */
	net->fd = fd;
	net->client = client;
	net->dump_file = -1;
	net->parsing = false;
	net->byte_count = 0;
	net->tag_seen = false;

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


GGZReturn net_set_dump_file(GGZNetIO *net, const char* filename)
{
	if (!filename) {
		/* Remove any existing dump file. */
		if (net->dump_file >= 0 && net->dump_file != STDERR_FILENO)
			close(net->dump_file);
		net->dump_file = -1;
		return GGZ_OK;
	}
	
	if (strcmp(filename, "stderr") == 0)
		net->dump_file = STDERR_FILENO;
	else
		net->dump_file = open(filename,
				      O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	
	if (net->dump_file < 0)
		return GGZ_ERROR;
	else
		return GGZ_OK;
}


/* Get filedescriptor for communication */
int net_get_fd(GGZNetIO *net)
{
	return (net ? net->fd : -1);
}


#if 0  /* For debugging purposes only! */
static void net_set_fd(GGZNetIO *net, int fd)
{
	net->fd = fd;
}
#endif

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
		
		ggz_free(net);
	}
}


/* net_send_XXX() functions for sending messages to the client */

GGZReturn net_send_serverid(GGZNetIO *net, const char *srv_name, bool use_tls)
{
	char *xml_srv_name = ggz_xml_escape(srv_name);
	GGZReturn status;

	_net_send_line(net, "<SESSION>");
	_net_send_line(net, "<SERVER ID='GGZ-%s' NAME='%s' VERSION='%d' "
		       "STATUS='ok' TLS_SUPPORT='%s'>",
		       VERSION, xml_srv_name, GGZ_CS_PROTO_VERSION,
		       ((use_tls && ggz_tls_support_query()) ? "yes" : "no"));
	_net_send_line(net, "<OPTIONS CHATLEN='%d'/>", MAX_CHAT_LEN);
	status = _net_send_line(net, "</SERVER>");

	ggz_free(xml_srv_name);

	return status;
}
 

GGZReturn net_send_server_full(GGZNetIO *net, const char *srv_name)
{
	char *xml_srv_name;
	GGZReturn status;

	xml_srv_name = ggz_xml_escape(srv_name);

	_net_send_line(net, "<SESSION>");
	status = _net_send_line(net, "<SERVER ID='GGZ-%s' NAME='%s' "
				"VERSION='%d' STATUS='full'/>",
				VERSION, xml_srv_name, GGZ_CS_PROTO_VERSION);

	ggz_free(xml_srv_name);

	return status;
}


GGZReturn net_send_login(GGZNetIO *net, GGZLoginType type,
			 GGZClientReqError status, const char *password)
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
	return GGZ_ERROR;
}


GGZReturn net_send_motd(GGZNetIO *net)
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

	return _net_send_line(net, "]]></MOTD>");
}


GGZReturn net_send_motd_error(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "motd", status);
}


GGZReturn net_send_room_list_error(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "list", status);
}


GGZReturn net_send_room_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='%s'>",
		       ggz_error_to_string(E_OK));
	return _net_send_line(net, "<LIST TYPE='room'>");
}


GGZReturn net_send_room(GGZNetIO *net, int index,
			RoomStruct *room, bool verbose)
{
	_net_send_line(net, "<ROOM ID='%d' NAME='%s' GAME='%d' PLAYERS='%d'>",
		       index, room->name, room->game_type,
		       room->player_count);
	if (verbose && room->description)
		_net_send_line(net, "<DESC>%s</DESC>", room->description);
	return _net_send_line(net, "</ROOM>");
}


GGZReturn net_send_room_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	return _net_send_line(net, "</RESULT>");
}


GGZReturn net_send_type_list_error(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "list", status);
}


GGZReturn net_send_type_list_count(GGZNetIO *net, int count)
{
	_net_send_line(net, "<RESULT ACTION='list' CODE='%s'>",
		       ggz_error_to_string(E_OK));
	return _net_send_line(net, "<LIST TYPE='game'>");
}


GGZReturn net_send_type(GGZNetIO *net, int index,
			GameInfo *type, bool verbose)
{
	char *players = ggz_numberlist_write(&type->player_allow_list);
	char *bots = ggz_numberlist_write(&type->bot_allow_list);
	char *spectators = bool_to_str(type->allow_spectators);

	_net_send_line(net, "<GAME ID='%d' NAME='%s' VERSION='%s'>",
		       index, type->name, type->version);
	_net_send_line(net, "<PROTOCOL ENGINE='%s' VERSION='%s'/>",
		       type->p_engine, type->p_version);
	_net_send_line(net, "<ALLOW PLAYERS='%s' BOTS='%s' SPECTATORS='%s'/>",
		       players, bots, spectators);

	ggz_free(players);
	ggz_free(bots);
	
	if (verbose) {
		_net_send_line(net, "<ABOUT AUTHOR='%s' URL='%s'/>",
			       type->author, type->homepage);
		_net_send_line(net, "<DESC>%s</DESC>", type->desc);
	}
	return _net_send_line(net, "</GAME>");
}


GGZReturn net_send_type_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	return _net_send_line(net, "</RESULT>");
}

GGZReturn net_send_player_list_error(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "list", status);
}


GGZReturn net_send_player_list_count(GGZNetIO *net, int count)
{
	int room;

	room = player_get_room(net->client->data);
	
	_net_send_line(net, "<RESULT ACTION='list' CODE='%s'>",
		       ggz_error_to_string(E_OK));
	return _net_send_line(net, "<LIST TYPE='player' ROOM='%d'>", room);
}


static void _net_get_player_stats_string(GGZPlayer *player,
					 char *buf, size_t bufsz)
{
	char record[256] = "", rating[64] = "";

	/* The caller should ensure that these values are safe to access... */
	/* FIXME: do this more elegantly */

	if (player->have_record) {
		snprintf(record, sizeof(record),
			 " WINS='%d' LOSSES='%d' TIES='%d' FORFEITS='%d'",
			 player->wins, player->losses, player->ties,
			 player->forfeits);
	}

	if (player->have_rating)
		snprintf(rating, sizeof(rating),
			 " RATING='%d'", player->rating);

	snprintf(buf, bufsz, "%s%s", record, rating);
}


GGZReturn net_send_player(GGZNetIO *net, GGZPlayer *player)
{
	GGZPlayerType type = player_get_type(player);
	const char *type_desc = ggz_playertype_to_string(type);
	char stats[512];

	_net_get_player_stats_string(player, stats, sizeof(stats));

	/* The caller should ensure that these values are safe to access... */
	return _net_send_line(net, 
			      "<PLAYER ID='%s' TYPE='%s' TABLE='%d' "
			      "LAG='%d'%s/>",
			      player->name, type_desc, player->table,
			      player->lag_class, stats);
}


static GGZReturn _net_send_player_lag(GGZNetIO *net, GGZPlayer *player)
{
	/* The caller should ensure that these values are safe to access... */
	return _net_send_line(net, 
			      "<PLAYER ID='%s' LAG='%d'/>",
			      player->name, player->lag_class);
}


static GGZReturn _net_send_player_stats(GGZNetIO *net, GGZPlayer *player)
{
	char stats[512];

	_net_get_player_stats_string(player, stats, sizeof(stats));

	return _net_send_line(net,
			      "<PLAYER ID='%s'%s/>",
			      player->name, stats);
}


GGZReturn net_send_player_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	return _net_send_line(net, "</RESULT>");
}


GGZReturn net_send_table_list_error(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "list", status);
}


GGZReturn net_send_table_list_count(GGZNetIO *net, int count)
{
	int room;

	room = player_get_room(net->client->data);

	_net_send_line(net, "<RESULT ACTION='list' CODE='%s'>",
		       ggz_error_to_string(E_OK));
	return _net_send_line(net, "<LIST TYPE='table' ROOM='%d'>", room);
}


GGZReturn net_send_table(GGZNetIO *net, GGZTable *table)
{
	int i;

	_net_send_line(net, "<TABLE ID='%d' GAME='%d' STATUS='%d' SEATS='%d'>",
		       table->index, table->type, table->state, 
		       seats_num(table));

	_net_send_line(net, "<DESC>%s</DESC>", table->desc);
	
	for (i = 0; i < seats_num(table); i++) {
		GGZTableSeat seat = {index: i,
				     type:  table->seat_types[i]};
		strncpy(seat.name, table->seat_names[i], MAX_USER_NAME_LEN+1);
		_net_send_seat(net, &seat);
	}

	for (i = 0; i < spectator_seats_num(table); i++) {
		GGZTableSpectator seat = {index: i};
		if (table->spectators[i][0] == '\0') continue;
		strcpy(seat.name, table->spectators[i]);
		_net_send_spectator(net, &seat);
	}
	
	return _net_send_line(net, "</TABLE>");
}


GGZReturn net_send_table_list_end(GGZNetIO *net)
{
	_net_send_line(net, "</LIST>");
	return _net_send_line(net, "</RESULT>");
}


GGZReturn net_send_room_join(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "enter", status);
}


GGZReturn net_send_chat(GGZNetIO *net, GGZChatType type,
			const char *sender, const char *msg)
{
	const char *type_str = ggz_chattype_to_string(type);

	if (type == GGZ_CHAT_BEEP) {
		/* A beep chat can't have a message. */
		if (msg) {
			err_msg("net_send_chat: invalid message.");
			msg = NULL;
		}
	} else {
		/* All other chats must have messages. */
		if (!msg) {
			err_msg("net_send_chat: missing message.");
			msg = "";
		}
	}

	if (msg) {
		return _net_send_line(net, "<CHAT TYPE='%s' FROM='%s'>"
				      "<![CDATA[%s]]></CHAT>", 
				      type_str, sender, msg);
	} else 
		return _net_send_line(net, "<CHAT TYPE='%s' FROM='%s'/>",
				      type_str, sender);
}


GGZReturn net_send_chat_result(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "chat", status);
}


GGZReturn net_send_table_launch(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "launch", status);
}


GGZReturn net_send_table_join(GGZNetIO *net,
			      bool is_spectator,
			      const unsigned int table_index)
{
	return _net_send_line(net,
			      "<JOIN TABLE='%d' SPECTATOR='%s'/>",
			      table_index, bool_to_str(is_spectator));
}


GGZReturn net_send_table_join_result(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "join", status);
}


GGZReturn net_send_table_leave(GGZNetIO *net, GGZLeaveType reason,
			       const char *player)
{
	const char *reason_str = ggz_leavetype_to_string(reason);

	if (player) {
		/* For a BOOT, the player is the person who booted them. */
		return _net_send_line(net,
				      "<LEAVE REASON='%s' PLAYER='%s'/>",
				      reason_str, player);
	} else
		return _net_send_line(net, "<LEAVE REASON='%s'/>",
				      reason_str);
}


GGZReturn net_send_table_leave_result(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "leave", status);
}


GGZReturn net_send_reseat_result(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "reseat", status);
}


GGZReturn net_send_player_update(GGZNetIO *net, GGZPlayerUpdateType opcode,
				 const char *name, int other_room)
{
	GGZPlayer *player;
	GGZPlayer p2;
	int room;

	room = player_get_room(net->client->data);
	
	switch (opcode) {
	case GGZ_PLAYER_UPDATE_DELETE:
	  _net_send_line(net, "<UPDATE TYPE='player' ACTION='delete' ROOM='%d' TOROOM='%d'>", room, other_room);
		_net_send_line(net, "<PLAYER ID='%s'/>", name);
		return _net_send_line(net, "</UPDATE>");

	case GGZ_PLAYER_UPDATE_ADD:
		/* This returns with player's write lock held, so drop it  */
		player = hash_player_lookup(name);
		if (!player) {
			err_msg("Player lookup failed!");
			return GGZ_OK;
		}
		pthread_rwlock_rdlock(&player->stats_lock);
		p2 = *player;
		pthread_rwlock_unlock(&player->stats_lock);
		pthread_rwlock_unlock(&player->lock);
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='add' ROOM='%d' FROMROOM='%d'>", room, other_room);
		net_send_player(net, &p2);
		return _net_send_line(net, "</UPDATE>");

	case GGZ_PLAYER_UPDATE_LAG:
		/* This returns with player's write lock held, so drop it  */
		player = hash_player_lookup(name);
		if (!player) {
			err_msg("Player lookup failed!");
			return GGZ_OK;
		}
		p2 = *player;
		pthread_rwlock_unlock(&player->lock);
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='lag' ROOM='%d'>", room);
		_net_send_player_lag(net, &p2);
		return _net_send_line(net, "</UPDATE>");
	case GGZ_PLAYER_UPDATE_STATS:
		/* This returns with player's write lock held, so drop it  */
		player = hash_player_lookup(name);
		if (!player) {
			err_msg("Player lookup failed!");
			return GGZ_OK;
		}
		pthread_rwlock_rdlock(&player->stats_lock);
		p2 = *player;
		pthread_rwlock_unlock(&player->stats_lock);
		pthread_rwlock_unlock(&player->lock);
		_net_send_line(net, "<UPDATE TYPE='player' ACTION='stats' ROOM='%d'>", room);
		_net_send_player_stats(net, player);
		return _net_send_line(net, "</UPDATE>");
	}

	err_msg("net_send_player_update: unknown opcode %d.", opcode);
	return GGZ_ERROR;
}


GGZReturn net_send_table_update(GGZNetIO *net, GGZTableUpdateType opcode,
				GGZTable *table, void* seat_data)
{
	char *action = NULL;
	int room;

	room = player_get_room(net->client->data);

	switch (opcode) {
	case GGZ_TABLE_UPDATE_DELETE:
		action = "delete";
		break;
	case GGZ_TABLE_UPDATE_ADD:
		action = "add";
		break;
	case GGZ_TABLE_UPDATE_LEAVE:
	case GGZ_TABLE_UPDATE_SPECTATOR_LEAVE:
		action = "leave";
		break;
	case GGZ_TABLE_UPDATE_JOIN:
	case GGZ_TABLE_UPDATE_SPECTATOR_JOIN:
		action = "join";
		break;
	case GGZ_TABLE_UPDATE_STATE:
		action = "status";
		break;
	case GGZ_TABLE_UPDATE_DESC:
		action = "desc";
		break;
	case GGZ_TABLE_UPDATE_SEAT:
		action = "seat";
		break;
	case GGZ_TABLE_UPDATE_RESIZE:
		action = "resize";
		break;
	}

	/* Always send opcode */
	_net_send_line(net, "<UPDATE TYPE='table' ACTION='%s' ROOM='%d'>",
		       action, room);

	switch (opcode) {
	case GGZ_TABLE_UPDATE_DELETE:
		_net_send_table_status(net, table);
		break;
	case GGZ_TABLE_UPDATE_ADD:
		net_send_table(net, table);
		break;
	case GGZ_TABLE_UPDATE_LEAVE:
	case GGZ_TABLE_UPDATE_JOIN:
	case GGZ_TABLE_UPDATE_SEAT:
		_net_send_table_seat(net, table, seat_data);
		break;
	case GGZ_TABLE_UPDATE_SPECTATOR_JOIN:
	case GGZ_TABLE_UPDATE_SPECTATOR_LEAVE:
		_net_send_table_spectator(net, table, seat_data);
		break;
	case GGZ_TABLE_UPDATE_STATE:
		_net_send_table_status(net, table);
		break;
	case GGZ_TABLE_UPDATE_DESC:
		_net_send_table_desc(net, table);
		break;
	case GGZ_TABLE_UPDATE_RESIZE:
		/* This sends unnecessary information, like the description
		   and spectator seats. */
		net_send_table(net, table);
		break;
	}
	
	return _net_send_line(net, "</UPDATE>");
}


GGZReturn _net_send_room_player_count(GGZNetIO *net,
				      int index, int player_count)
{
	return _net_send_line(net, "<ROOM ID='%d' PLAYERS='%d'/>",
			      index, player_count);
}


GGZReturn net_send_room_update(GGZNetIO *net, GGZRoomUpdateType opcode,
			       int index, int player_count)
{
	char *action = NULL;

	switch (opcode) {
	case GGZ_ROOM_UPDATE_PLAYER_COUNT:
		action = "players";
		break;
	}

	_net_send_line(net, "<UPDATE TYPE='room' ACTION='%s'>", action);

	switch (opcode) {
	case GGZ_ROOM_UPDATE_PLAYER_COUNT:
		_net_send_room_player_count(net, index, player_count);
		break;
	}

	return _net_send_line(net, "</UPDATE>");
}


GGZReturn net_send_update_result(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_result(net, "update", status);
}


GGZReturn net_send_logout(GGZNetIO *net, GGZClientReqError status)
{
	return _net_send_line(net, "</SESSION>");
}


GGZReturn net_send_ping(GGZNetIO *net)
{
	return _net_send_line(net, "<PING/>");
}


/* Check for incoming data */
#if 0
static bool net_data_is_pending(GGZNetIO *net)
{
	int result;
	struct pollfd fd[1] = {{net->fd, POLLIN, 0}};

	if (net && net->fd != -1) {
		dbg_msg(GGZ_DBG_CONNECTION, "Checking for net events");
		result = poll(fd, 1, 0);
		if (result < 0) {
			if (errno == EINTR) {
				/* Ignore interruptions */
				return false;
			} else {
				err_sys_exit("poll failed in ggzcore_server_data_is_pending");
			}

		} else if (result > 0) {
			return true;
		}
	}

	return false;
}
#endif


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
	net->parsing = true;

	/* Get a buffer to hold the data */
	if (!(buf = XML_GetBuffer(net->parser, BUFFSIZE)))
		err_sys_exit("Couldn't allocate buffer");

	/* Read in data from socket */
	if ( (len = ggz_tls_read(net->fd, buf, BUFFSIZE)) < 0) {
		
		/* If it's a non-blocking socket and there isn't data,
                   we get EAGAIN.  It's safe to just return */
		if (errno == EAGAIN) {
			net->parsing = false;
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
		net->tag_seen = false;
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
	net->parsing = false;

	return (done ? GGZ_REQ_DISCONNECT: GGZ_REQ_OK);
}


/********** Callbacks for XML parser **********/
static void _net_parse_start_tag(void *data, const char *el,
				 const char **attr)
{
	GGZNetIO *net = (GGZNetIO*)data;
	GGZXMLElement *element;

	dbg_msg(GGZ_DBG_XML, "New %s element", el);
	
	/* Create new element object */
	element = _net_new_element(el, attr);

	/* Put element on stack so we can process its children */
	ggz_stack_push(net->stack, element);

	/* Mark that we've seen a tag */
	net->tag_seen = true;
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
	net->tag_seen = true;
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

static GGZXMLElement* _net_new_element(const char *tag,
				       const char * const *attrs)
{
	void (*process_func)();

	if (strcasecmp(tag, "SESSION") == 0)
		process_func = _net_handle_session;
	else if (strcasecmp(tag, "LOGIN") == 0)
		process_func = _net_handle_login;
	else if (strcasecmp(tag, "CHANNEL") == 0)
		process_func = _net_handle_channel;
	else if (strcasecmp(tag, "NAME") == 0)
		process_func = _net_handle_name;
	else if (strcasecmp(tag, "PASSWORD") == 0)
		process_func = _net_handle_password;
	else if (strcasecmp(tag, "UPDATE") == 0)
		process_func = _net_handle_update;
	else if (strcasecmp(tag, "LIST") == 0)
		process_func = _net_handle_list;
	else if (strcasecmp(tag, "ENTER") == 0)
		process_func = _net_handle_enter;
	else if (strcasecmp(tag, "CHAT") == 0)
		process_func = _net_handle_chat;
	else if (strcasecmp(tag, "JOIN") == 0)
		process_func = _net_handle_join;
	else if (strcasecmp(tag, "LEAVE") == 0)
		process_func = _net_handle_leave;
	else if (strcasecmp(tag, "RESEAT") == 0)
		process_func = _net_handle_reseat;
	else if (strcasecmp(tag, "LAUNCH") == 0)
		process_func = _net_handle_launch;
	else if (strcasecmp(tag, "TABLE") == 0)
		process_func = _net_handle_table;
	else if (strcasecmp(tag, "SEAT") == 0)
		process_func = _net_handle_seat;
	else if (strcasecmp(tag, "DESC") == 0)
		process_func = _net_handle_desc;
	else if (strcasecmp(tag, "MOTD") == 0)
		process_func = _net_handle_motd;
	else if (strcasecmp(tag, "PONG") == 0)
		process_func = _net_handle_pong;
	else if (strcasecmp(tag, "PING") == 0)
		process_func = _net_handle_ping;
	else if (strcmp(tag, "TLS_START") == 0)
		process_func = _net_handle_tls_start;
	else
		process_func = NULL;
	
	return ggz_xmlelement_new(tag, attrs, process_func, NULL);
}


/* Functions for <SESSION> tag */
static void _net_handle_session(GGZNetIO *net, GGZXMLElement *element)
{
	client_end_session(net->client);
	net_send_logout(net, E_OK);
}


/* Functions for <CHANNEL> tag */
static void _net_handle_channel(GGZNetIO *net, GGZXMLElement *element)
{
	const char *id;

	if (!element) return;

	/* Grab table ID from tag */
	id = ggz_xmlelement_get_attr(element, "ID");

	/* It's an error if they didn't send an ID string */
	if (!id) {
		_net_send_result(net, "channel", E_BAD_OPTIONS);
		return;
	}

	/* FIXME: Do validity checking on the channel ID here */

	/* FIXME: perhaps lookup table and point net->client->data at it */

	client_set_type(net->client, GGZ_CLIENT_CHANNEL);
	net->client->data = ggz_strdup(id);
}


/* Functions for <LOGIN> tag */
static void _net_handle_login(GGZNetIO *net, GGZXMLElement *element)
{
	GGZLoginType login_type;
	GGZAuthData *auth;	
	const char *type;

	if (!element) return;

	auth = ggz_xmlelement_get_data(element);
	if (!auth) {
		/* It's an error if they didn't send the right data */
		_net_send_result(net, "login", E_BAD_OPTIONS);
		return;
	}

	type = ggz_xmlelement_get_attr(element, "TYPE");
	if (!type || strcasecmp(type, "guest") == 0) {
		/* If no type is sent, assume a guest login. */
		login_type = GGZ_LOGIN_GUEST;
	} else if (strcasecmp(type, "normal") == 0)
		login_type = GGZ_LOGIN;
	else if (strcasecmp(type, "first") == 0)
		login_type = GGZ_LOGIN_NEW;
	else {
		/* If they didn't send a valid TYPE, it's an error */
		_net_send_result(net, "login", E_BAD_OPTIONS);
		_net_authdata_free(auth);
		return;
	}

	if (!auth->name
	    || (!auth->password && login_type == GGZ_LOGIN)) {
		_net_send_result(net, "login", E_BAD_OPTIONS);
		_net_authdata_free(auth);
		return;
	}

	/* Can't login twice */
	if (net->client->data) {
		net_send_login(net, login_type, E_ALREADY_LOGGED_IN, NULL);
		_net_authdata_free(auth);
		return;
	}

	client_set_type(net->client, GGZ_CLIENT_PLAYER);
	login_player(login_type, net->client->data,
		     auth->name, auth->password);

	/* Free up any resources we allocated */
	_net_authdata_free(auth);
}


static GGZAuthData* _net_authdata_new(void)
{
	GGZAuthData *data = ggz_malloc(sizeof(GGZAuthData));
	
	data->name = NULL;
	data->password = NULL;
	
	return data;
}


static void _net_auth_set_name(GGZXMLElement *auth, char *name)
{
	GGZAuthData *data = ggz_xmlelement_get_data(auth);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_authdata_new();
		ggz_xmlelement_set_data(auth, data);
	}

	if (data->name) ggz_free(data->name); /* duplicate */
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

	if (data->password) ggz_free(data->password); /* duplicate */
	data->password = password;
}


static void _net_authdata_free(GGZAuthData *data)
{
	if (!data) return;

	if (data->name)
		ggz_free(data->name);
	if (data->password)
		ggz_free(data->password);
	ggz_free(data);
}


/* Functions for <NAME> tag */
static void _net_handle_name(GGZNetIO *net, GGZXMLElement *element)
{
	char *name;
	const char *parent_tag;
	GGZXMLElement *parent;

	if (!element) return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		/* If there are no elements above us, it's a protocol error */
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "LOGIN") != 0) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	name = ggz_strdup(ggz_xmlelement_get_text(element));
	_net_auth_set_name(parent, name);
}


/* Functions for <PASSWORD> tag */
static void _net_handle_password(GGZNetIO *net, GGZXMLElement *element)
{
	char *password;
	const char *parent_tag;
	GGZXMLElement *parent;

	if (!element) return;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		/* If there are no elements above us, it's a protocol error */
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "LOGIN") != 0) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	password = ggz_strdup(ggz_xmlelement_get_text(element));
	_net_auth_set_password(parent, password);
}


/* Functions for <UPDATE> tag */
static void _net_handle_update(GGZNetIO *net, GGZXMLElement *update)
{
	const char *type, *action, *room;

	if (!update) return;
	if (!check_playerconn(net, "update")) return;

	/* Grab update data from tag */
	type = ggz_xmlelement_get_attr(update, "TYPE");
	action = ggz_xmlelement_get_attr(update, "ACTION");
	room = ggz_xmlelement_get_attr(update, "ROOM");

	if (strcasecmp(type, "player") == 0) {
		GGZPlayer *player = ggz_xmlelement_get_data(update);
		if (!player) {
			_net_send_result(net, "update", E_BAD_OPTIONS);
			return;
		}

		/* FIXME: update password and other data */
		_net_send_result(net, "update", E_BAD_OPTIONS);
	} else if (strcasecmp(type, "table") == 0) {
		GGZTable *table = ggz_xmlelement_get_data(update);
		if (!table) {
			_net_send_result(net, "update", E_BAD_OPTIONS);
			return;
		}

		if (!type || !action) {
			_net_send_result(net, "update", E_BAD_OPTIONS);
			table_free(table);
			return;
		}

		/* If room was specified, override default current room */
		table->room = str_to_int(room, table->room);

		if (!strcasecmp(action, "desc")) {
			player_table_desc_update(net->client->data,
						 table->room,
						 table->index,
						 table->desc);
		} else if (!strcasecmp(action, "seat")) {
			int i;
			int handled = 0;
			for (i = 0; i < table->num_seats; i++) {
				GGZTableSeat seat;
				if (table->seat_types[i] == GGZ_SEAT_NONE)
					continue;
				seat.index = i;
				seat.type = table->seat_types[i];
				strcpy(seat.name, table->seat_names[i]);
				seat.fd = -1;
				player_table_seat_update(net->client->data,
							 table->room,
							 table->index,
							 &seat);
				handled = 1;
			}
			if (!handled) {
				_net_send_result(net, "update",
						 E_BAD_OPTIONS);
			}
		} else if (!strcasecmp(action, "boot")) {
			int i;
			int handled = 0;
			for (i = 0; i < table->num_seats; i++) {
				if (table->seat_types[i] == GGZ_SEAT_NONE
				    || !table->seat_names[i][0])
					continue;
				player_table_boot_update(net->client->data,
							 table->room,
							 table->index,
							 table->seat_names[i]);
				handled = 1;
			}
			if (!handled) {
				_net_send_result(net, "update",
						 E_BAD_OPTIONS);
			}
		} else {
			_net_send_result(net, "update", E_BAD_OPTIONS);
		}

		table_free(table);
	} else
		_net_send_result(net, "update", E_BAD_OPTIONS);
}


/* Functions for <LIST> tag */
static void _net_handle_list(GGZNetIO *net, GGZXMLElement *element)
{
	const char *type;
	int verbose;
	
	if (!element) return;
	if (!check_playerconn(net, "list")) return;

	type = ggz_xmlelement_get_attr(element, "TYPE");
	if (!type) {
		_net_send_result(net, "list", E_BAD_OPTIONS);
		return;
	}

	verbose = str_to_bool(ggz_xmlelement_get_attr(element, "FULL"), 0);

	if (strcasecmp(type, "game") == 0)
		player_list_types(net->client->data, verbose);
	else if (strcasecmp(type, "room") == 0)
		/* FIXME: Currently send all types */
		room_list_send(net->client->data, -1, verbose);
	else if (strcasecmp(type, "player") == 0)
		player_list_players(net->client->data);
	else if (strcasecmp(type, "table") == 0)
		/* FIXME: Currently send all local types */
		player_list_tables(net->client->data, GGZ_TYPE_ALL, 0);
	else
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
}


/* Functions for <ENTER> tag */
static void _net_handle_enter(GGZNetIO *net, GGZXMLElement *enter)
{
	int room;

	if (!enter) return;
	if (!check_playerconn(net, "enter")) return;

	room = str_to_int(ggz_xmlelement_get_attr(enter, "ROOM"), -1);
	room_handle_join(net->client->data, room);
}


/* Functions for <CHAT> tag */
static void _net_handle_chat(GGZNetIO *net, GGZXMLElement *element)
{
	const char *type_str, *to, *msg;
        GGZChatType type;
		
	if (!element) return;
	if (!check_playerconn(net, "chat")) return;

	/* Grab chat data from tag */
	type_str = ggz_xmlelement_get_attr(element, "TYPE");
	to = ggz_xmlelement_get_attr(element, "TO");
	msg = ggz_xmlelement_get_text(element);

	if (!type_str) {
		/* If no type is specified, assume a normal chat. */
		type = GGZ_CHAT_NORMAL;
	} else
		type = ggz_string_to_chattype(type_str);

	player_chat(net->client->data, type, to, msg);
}


/* Functions for <JOIN> tag */
static void _net_handle_join(GGZNetIO *net, GGZXMLElement *element)
{
	int table, spectator, seat_num;

	if (!element) return;
	if (!check_playerconn(net, "join")) return;

	table = str_to_int(ggz_xmlelement_get_attr(element, "TABLE"), -1);
	spectator = str_to_bool(ggz_xmlelement_get_attr(element,
							"SPECTATOR"), 0);
	seat_num = str_to_int(ggz_xmlelement_get_attr(element, "SEAT"), -1);

	if (spectator)
		player_table_join_spectator(net->client->data, table);
	else
		player_table_join(net->client->data, table, seat_num);
}


/* Functions for <LEAVE> tag */
static void _net_handle_leave(GGZNetIO *net, GGZXMLElement *element)
{
	int force, spectator;

	if (!element) return;
	if (!check_playerconn(net, "leave")) return;

	force = str_to_bool(ggz_xmlelement_get_attr(element, "FORCE"), 0);
	spectator = str_to_bool(ggz_xmlelement_get_attr(element,
							"SPECTATOR"), 0);

	player_table_leave(net->client->data, spectator, force);
}


/* Functions for <RESEAT> tag */
static void _net_handle_reseat(GGZNetIO *net, GGZXMLElement *element)
{
	const char *action;
	int seat_num;
	GGZReseatType type;

	if (!element) return;
	if (!check_playerconn(net, "reseat")) return;

	action = ggz_xmlelement_get_attr(element, "ACTION");
	if (!action) {
		_net_send_result(net, "reseat", E_BAD_OPTIONS);
		return;
	}
	if (!strcasecmp(action, "sit"))
		type = GGZ_RESEAT_SIT;
	else if (!strcasecmp(action, "stand"))
		type = GGZ_RESEAT_STAND;
	else if (!strcasecmp(action, "move"))
		type = GGZ_RESEAT_MOVE;
	else {
		_net_send_result(net, "reseat", E_BAD_OPTIONS);
		return;
	}

	seat_num = str_to_int(ggz_xmlelement_get_attr(element, "SEAT"), -1);

	player_table_reseat(net->client->data, type, seat_num);
}


/* Functions for <LAUNCH> tag */
static void _net_handle_launch(GGZNetIO *net, GGZXMLElement *element)
{
	GGZTable *table;

	if (!element) return;

	table = ggz_xmlelement_get_data(element);
	if (!table) {
		_net_send_result(net, "launch", E_BAD_OPTIONS);
		return;
	}

	if (!check_playerconn(net, "launch")) {
		table_free(table);
		return;
	}

	player_table_launch(net->client->data, table);

	/* Note: we don't have to free the table; the launch just uses it. */
}


/* Functions for <TABLE> tag */
static void _net_handle_table(GGZNetIO *net, GGZXMLElement *element)
{
	int num_seats;
	GGZTable *table, *old_data;
	GGZTableData *data;
	GGZSeatData *seat;
	const char *desc = NULL, *parent_tag;
	GGZList *seats = NULL;
	GGZListEntry *entry;
	GGZXMLElement *parent;

	if (!element) return;

	data = ggz_xmlelement_get_data(element);
	if (data) {
		desc = data->desc;
		seats = data->seats;
	}

	parent = ggz_stack_top(net->stack);
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		_net_tabledata_free(data);
		return;
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
 	if (strcasecmp(parent_tag, "LAUNCH") != 0
	    && strcasecmp(parent_tag, "UPDATE") != 0 ) {
		_net_tabledata_free(data);
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	/* Create and init new table */
	table = table_new();

	/* Get table data from tags */
	table->index = str_to_int(ggz_xmlelement_get_attr(element, "ID"), -1);
	table->type = str_to_int(ggz_xmlelement_get_attr(element, "GAME"), -1);
	num_seats = str_to_int(ggz_xmlelement_get_attr(element, "SEATS"), 0);

	if (num_seats > 0) {
		int i;
		/* FIXME: what if num_seats is really large? */
		table->num_seats = num_seats;
		table->seat_types = ggz_malloc(num_seats *
					       sizeof(*table->seat_types));
		table->seat_names = ggz_malloc(num_seats *
					       sizeof(*table->seat_names));
		for (i = 0; i < num_seats; i++) {
			table->seat_types[i] = GGZ_SEAT_NONE;
			table->seat_names[i][0] = '\0';
		}
	}

	/* If room was specified, use it, otherwise use players current room */
	table->room = player_get_room(net->client->data);

	if (desc)
		snprintf(table->desc, sizeof(table->desc), "%s", desc);

	/* Add seats */
	for (entry = ggz_list_head(seats);
	     entry;
	     entry = ggz_list_next(entry)) {
		GGZSeatType seat_type;
		ggzdbPlayerEntry player;

		seat = ggz_list_get_data(entry);
		seat_type = ggz_string_to_seattype(seat->type);

		if (seat->index < 0 || seat->index >= table->num_seats) {
			/* We treat this as an error -
			   we could just skip the seat instead */
			if (data) _net_tabledata_free(data);
			table_free(table);
			return;
		}

		switch (seat_type) {
		case GGZ_SEAT_OPEN:
		case GGZ_SEAT_BOT:
		case GGZ_SEAT_NONE:
			break;
		case GGZ_SEAT_RESERVED:
			/* We verify that this is a real,
			   registered player name. */
			/* FIXME: This should be done elsewhere. */
			snprintf(player.handle, MAX_USER_NAME_LEN+1,
				 "%s", seat->name ? seat->name : "");
			if (ggzdb_player_get(&player) != 0) {
				/* This is some kind of error...but for now we
				   just cover it up. */
				dbg_msg(GGZ_DBG_TABLE,
					"Invalid name '%s' sent for reserved "
					"seat.  Changing it to an open seat.",
					seat->name);
				seat_type = GGZ_SEAT_OPEN;
			}
			strcpy(table->seat_names[seat->index], seat->name);
			break;
		case GGZ_SEAT_PLAYER:
			/* This is valid for an UPDATE but not a LAUNCH.
			   We don't check it here but leave it up to
			   the code that uses it. */
			if (!seat->name)
				seat_type = GGZ_SEAT_NONE;
			else
				strcpy(table->seat_names[seat->index],
				       seat->name);
			break;
		}
		table->seat_types[seat->index] = seat_type;
	}

	old_data = ggz_xmlelement_get_data(parent);
	if (old_data)
		table_free(old_data);
	ggz_xmlelement_set_data(parent, table);	

	_net_tabledata_free(data);
}


static void _net_table_add_seat(GGZXMLElement *table, GGZSeatData *seat)
{
	struct _GGZTableData *data = ggz_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}

	ggz_list_insert(data->seats, seat);
}


static void _net_table_set_desc(GGZXMLElement *table, const char *desc)
{
	struct _GGZTableData *data = ggz_xmlelement_get_data(table);

	/* If data doesn't already exist, create it */
	if (!data) {
		data = _net_tabledata_new();
		ggz_xmlelement_set_data(table, data);
	}

	if (data->desc) ggz_free(data->desc);
	data->desc = ggz_strdup(desc);
}


static GGZTableData* _net_tabledata_new(void)
{
	struct _GGZTableData *data = ggz_malloc(sizeof(*data));

	data->desc = NULL;
	data->seats = ggz_list_create(NULL, 
				  _net_seat_copy, 
				  (ggzEntryDestroy)_net_seat_free, 
				  GGZ_LIST_ALLOW_DUPS);

	return data;
}


static void _net_tabledata_free(GGZTableData *data)
{
	if (!data) return;

	if (data->desc)
		ggz_free(data->desc);
	if (data->seats)
		ggz_list_free(data->seats);
	ggz_free(data);
}




/* Functions for <SEAT> tag */
static void _net_handle_seat(GGZNetIO *net, GGZXMLElement *element)
{
	struct _GGZSeatData seat;
	GGZXMLElement *parent;
	const char *parent_tag;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "TABLE") != 0) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (!element) return;

	/* Get seat information out of tag */
	seat.index = str_to_int(ggz_xmlelement_get_attr(element, "NUM"), -1);
	seat.type = ggz_xmlelement_get_attr(element, "TYPE");
	seat.name = ggz_xmlelement_get_text(element);

	_net_table_add_seat(parent, &seat);
}


static void* _net_seat_copy(void *data)
{
	struct _GGZSeatData *old_seat = data, *new_seat;

	new_seat = ggz_malloc(sizeof(*new_seat));

	new_seat->index = old_seat->index;
	new_seat->type = ggz_strdup(old_seat->type);
	new_seat->name = ggz_strdup(old_seat->name);

	return new_seat;
}


static void _net_seat_free(GGZSeatData *seat)
{
	if (!seat) return;

	if (seat->type)
		ggz_free(seat->type);
	if (seat->name)
		ggz_free(seat->name);
	ggz_free(seat);
}



/* Functions for <DESC> tag */
static void _net_handle_desc(GGZNetIO *net, GGZXMLElement *element)
{
	char *desc = NULL;
	const char *parent_tag;
	GGZXMLElement *parent;

	/* Get parent off top of stack */
	parent = ggz_stack_top(net->stack);
	if (!parent) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	parent_tag = ggz_xmlelement_get_tag(parent);
	if (strcasecmp(parent_tag, "TABLE") != 0) {
		_net_send_result(net, "protocol", E_BAD_OPTIONS);
		return;
	}

	if (!element) return;

	desc = ggz_xmlelement_get_text(element);

	_net_table_set_desc(parent, desc);
}


/* Functions for <MOTD> tag */
static void _net_handle_motd(GGZNetIO *net, GGZXMLElement *motd)
{
	if (!check_playerconn(net, "motd")) return;
	player_motd(net->client->data);
}


/* Function for <PONG> tag */
static void _net_handle_pong(GGZNetIO *net, GGZXMLElement *data)
{
	if (!check_playerconn(net, "pong")) return;
	player_handle_pong(net->client->data);
}


/* Function for <PING> tag */
static void _net_handle_ping(GGZNetIO *net, GGZXMLElement *element)
{
	const char *id = ggz_xmlelement_get_attr(element, "ID");
	_net_send_pong(net, id);
}

/* Function for <TLS_START> tag */
static void _net_handle_tls_start(GGZNetIO *net, GGZXMLElement *data)
{
	ggz_tls_enable_fd(net->fd, GGZ_TLS_SERVER, GGZ_TLS_VERIFY_NONE);
}

/************ Utility/Convenience functions *******************/

static int str_to_int(const char *str, int dflt)
{
	int val;

	if (!str || sscanf(str, "%d", &val) < 1)
		return dflt;

	return val;
}


static bool check_playerconn(GGZNetIO *net, const char *type)
{
	if (net->client->data)
		return true;

	/* This should only be caused by a malicious client. */
	dbg_msg(GGZ_DBG_CONNECTION,
		"Client requested %s before login.", type);

	_net_send_result(net, type, E_NOT_LOGGED_IN);

	return false;
}


static GGZReturn _net_send_table_seat(GGZNetIO *net, GGZTable *table, 
				      GGZTableSeat *seat)
{
	_net_send_line(net, "<TABLE ID='%d' SEATS='%d'>", table->index, 
		       seats_num(table));
	_net_send_seat(net, seat);
	return _net_send_line(net, "</TABLE>");
}


static GGZReturn _net_send_table_spectator(GGZNetIO *net, GGZTable *table, 
					   GGZTableSpectator *spectator)
{
	_net_send_line(net, "<TABLE ID='%d' SPECTATORS='%d'>",
		       table->index, spectator_seats_num(table));
	_net_send_spectator(net, spectator);
	return _net_send_line(net, "</TABLE>");
}


static GGZReturn _net_send_table_desc(GGZNetIO *net, GGZTable *table)
{
	_net_send_line(net, "<TABLE ID='%d'>", table->index);
	_net_send_line(net, "<DESC>%s</DESC>", table->desc);
	return _net_send_line(net, "</TABLE>");
}


static GGZReturn _net_send_table_status(GGZNetIO *net, GGZTable *table)
{
	return _net_send_line(net, "<TABLE ID='%d' STATUS='%d' SEATS='%d'/>", 
			      table->index, table->state, seats_num(table));
}


static GGZReturn _net_send_seat(GGZNetIO *net, GGZTableSeat *seat)
{
	const char *type_str = ggz_seattype_to_string(seat->type);
	char *name = NULL;

	switch (seat->type) {
	case GGZ_SEAT_RESERVED:
	case GGZ_SEAT_PLAYER:
		name = seat->name;
		break;
	case GGZ_SEAT_OPEN:
	case GGZ_SEAT_BOT:
	case GGZ_SEAT_NONE:
		name = NULL;
		break;
	}
	
	if (name)
		return _net_send_line(net,
				      "<SEAT NUM='%d' TYPE='%s'>%s</SEAT>",
				      seat->index, type_str, name);
	else
		return _net_send_line(net, "<SEAT NUM='%d' TYPE='%s'/>", 
				      seat->index, type_str);
}


static GGZReturn _net_send_spectator(GGZNetIO *net,
				     GGZTableSpectator *spectator)
{
	return _net_send_line(net, "<SPECTATOR NUM='%d'>%s</SPECTATOR>",
			      spectator->index, spectator->name);
}

static GGZReturn _net_send_result(GGZNetIO *net, const char *action,
				  GGZClientReqError code)
{
	return _net_send_line(net, "<RESULT ACTION='%s' CODE='%s'/>",
			      action, ggz_error_to_string(code));
}


static GGZReturn _net_send_login_normal_status(GGZNetIO *net,
					       GGZClientReqError status)
{
	return _net_send_result(net, "login", status);
}


static GGZReturn _net_send_login_anon_status(GGZNetIO *net,
					     GGZClientReqError status)
{
	return _net_send_result(net, "login", status);
}


static GGZReturn _net_send_login_new_status(GGZNetIO *net,
					    GGZClientReqError status,
					    const char *password)
{
	/* Try to send login status */
	_net_send_line(net, "<RESULT ACTION='login' CODE='%s'>",
		       ggz_error_to_string(status));

	/* Try to send checksum if successful */
	if (status == E_OK)
		_net_send_line(net, "<PASSWORD>%s</PASSWORD>", password);
	
	return _net_send_line(net, "</RESULT>");
}


/* Send <PONG> tag. */
static GGZReturn _net_send_pong(GGZNetIO *net, const char *id)
{
	if (id)
		return _net_send_line(net, "<PONG ID='%s'/>", id);
	else
		return _net_send_line(net, "<PONG/>");
}


static GGZReturn _net_send_line(GGZNetIO *net, char *line, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, line);
	vsnprintf(buf, sizeof(buf) - 1, line, ap);
	va_end(ap);
	strcat(buf, "\n");

	if (ggz_tls_write(net->fd, buf, strlen(buf)) < 0)
		return GGZ_ERROR;
	else
		return GGZ_OK;
}
