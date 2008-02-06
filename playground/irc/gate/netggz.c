/*******************************************************************
*
* ggz-ircgate - Gateway between GGZ room chat and IRC channel
* Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
* Based on Grubby chatbot sources
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include <ggz.h>
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

/* Globals */
static int status = NET_NOOP;
static GGZServer *server = NULL;
static char *inputstr = NULL;
static char *save_room;

/* Prototypes */
static GGZHookReturn net_hook_connect(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_login(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_roomlist(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_enter(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_fail(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_roomenter(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_roomleave(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_chat(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_chatfail(unsigned int id, const void *event_data, const void *user_data);

void net_ggz_cleanup()
{
	ggz_free(save_room);
	ggz_free(inputstr);

	ggzcore_server_logout(server);
	ggzcore_server_disconnect(server);
	ggzcore_server_free(server);
	ggzcore_destroy();
}

/* Transparently connect to any host */
void net_ggz_connect(const char *ggzuri)
{
	GGZOptions opt;
	int ret;
	ggz_uri_t uri;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	ret = ggzcore_init(opt);

	server = ggzcore_server_new();

	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, net_hook_connect);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_CHAT_FAIL, net_hook_chatfail);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);

	uri = ggz_uri_from_string(ggzuri);

	ggzcore_server_set_hostinfo(server, uri.host, uri.port, 0);
	ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, uri.user, uri.password, NULL);
	ggzcore_server_connect(server);

	save_room = ggz_strdup(uri.path);

	ggz_uri_free(uri);
}

static void net_join(const char *room)
{
	int roomid, i;
	GGZRoom *ggzroom;

	if(room)
	{
		roomid = atoi(room);
		for(i = 0; i < ggzcore_server_get_num_rooms(server); i++)
		{
			ggzroom = ggzcore_server_get_nth_room(server, i);
			if(!strcasecmp(ggzcore_room_get_name(ggzroom), room)) roomid = i;
		}
	}

	if(room)
	{
		ggzroom = ggzcore_server_get_nth_room(server, roomid);
		ggzcore_server_join_room(server, ggzroom);
	}
	else ggzcore_server_join_room(server, NULL);
}

/* Loop function */
int net_ggz_status(void)
{
	int ret;
	int fd;
	fd_set set;
	struct timeval to;

	to.tv_sec = 1;
	to.tv_usec = 0;

	if(ggzcore_server_get_fd(server) < 0)
		return NET_NOOP;

	fd = ggzcore_server_get_fd(server);
	FD_ZERO(&set);
	FD_SET(fd, &set);
	ret = select(fd + 1, &set, NULL, NULL, &to);
	if(ret == 1)
		ggzcore_server_read_data(server, fd);

	return NET_NOOP;
}

/* Get an entry from the queue */
char *net_ggz_input(void)
{
	return inputstr;
}

/* Let the gate speak */
void net_ggz_output(const char *output)
{
	if(!ggzcore_server_get_cur_room(server))
		return;

	ggzcore_room_chat(ggzcore_server_get_cur_room(server), GGZ_CHAT_NORMAL, NULL, output);
}

/* Callback for successful connection */
GGZHookReturn net_hook_connect(unsigned int id, const void *event_data, const void *user_data)
{
	/*nasty ggzcore bug?*/
	while((!ggzcore_server_is_online(server)) && (status == NET_NOOP))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));

	if(status == NET_NOOP)
	{
		ggzcore_server_login(server);
	}

	return GGZ_HOOK_OK;
}

/* Callback for login */
GGZHookReturn net_hook_login(unsigned int id, const void *event_data, const void *user_data)
{
	ggzcore_server_list_rooms(server, 0);

	return GGZ_HOOK_OK;
}

/* Callback for rooms list */
GGZHookReturn net_hook_roomlist(unsigned int id, const void *event_data, const void *user_data)
{
	status = NET_LOGIN;

	net_join(save_room);

	return GGZ_HOOK_OK;
}

/* Callback for joining a room */
GGZHookReturn net_hook_enter(unsigned int id, const void *event_data, const void *user_data)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
	ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, net_hook_chat);

	status = NET_GOTREADY;

	return GGZ_HOOK_OK;
}

/* Common error callback */
GGZHookReturn net_hook_fail(unsigned int id, const void *event_data, const void *user_data)
{
	const char *msg = event_data;

	fprintf(stderr, "ERROR (ggz): %s\n", msg);

	if((id != GGZ_ENTER_FAIL) || (!ggzcore_server_get_cur_room(server)))
		status = NET_ERROR;

	return GGZ_HOOK_OK;
}

/* Callback for new players coming in */
GGZHookReturn net_hook_roomenter(unsigned int id, const void *event_data, const void *user_data)
{
	const GGZRoomChangeEventData *data = event_data;

	if(inputstr)
		ggz_free(inputstr);

	inputstr = ggz_strbuild("%s has joined the room", data->player_name);
	status = NET_INPUT;

	return GGZ_HOOK_OK;
}

/* Callback for players leaving the room */
GGZHookReturn net_hook_roomleave(unsigned int id, const void *event_data, const void *user_data)
{
	const GGZRoomChangeEventData *data = event_data;

	if(inputstr)
		ggz_free(inputstr);

	inputstr = ggz_strbuild("%s has left the room", data->player_name);
	status = NET_INPUT;

	return GGZ_HOOK_OK;
}

/* Chat callback which passes message to grubby */
GGZHookReturn net_hook_chat(unsigned int id, const void *event_data, const void *user_data)
{
	/*int type;
	const char *roomname;
	time_t t;
	char *ts;
	int playertype;*/
	const GGZChatEventData *chat = event_data;

	if(inputstr)
		ggz_free(inputstr);

	inputstr = ggz_strbuild("[%s] %s", chat->sender, chat->message);
	status = NET_INPUT;

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_chatfail(unsigned int id, const void *event_data, const void *user_data)
{
	fprintf(stderr, "Chat failed!\n");

	return GGZ_HOOK_OK;
}

