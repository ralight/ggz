/*******************************************************************
*
* Agrue - A GGZ Real User Emulator
* Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
* Idea based on the original Agrue, code based on Grubby
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"

#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define AGRUE_DEBUG 1

static GGZHookReturn net_hook_connect(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_negotiate(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_login(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_roomlist(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_enter(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_fail(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_roomenter(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_roomleave(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_chat(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_chatfail(unsigned int id, const void *ed, const void *ud);
static GGZHookReturn net_hook_typelist(unsigned int id, const void *ed, const void *ud);

static void net_internal_init(void)
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	ret = ggzcore_init(opt);
}

void net_connect(const char *host, int port, Agrue *agrue)
{
	GGZServer *server;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] connect to %s:%i...\n", agrue->name, host, port);
#endif

	net_internal_init();

	server = ggzcore_server_new();

	ggzcore_server_add_event_hook_full(server, GGZ_CONNECTED, net_hook_connect, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_CONNECT_FAIL, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATED, net_hook_negotiate, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATE_FAIL, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGGED_IN, net_hook_login, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGIN_FAIL, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTERED, net_hook_enter, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTER_FAIL, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_ROOM_LIST, net_hook_roomlist, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_TYPE_LIST, net_hook_typelist, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_PROTOCOL_ERROR, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_NET_ERROR, net_hook_fail, agrue);
	ggzcore_server_add_event_hook_full(server, GGZ_CHAT_FAIL, net_hook_chatfail, agrue);

	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_connect(server);

	agrue->server = server;
}

void net_work(Agrue *agrue)
{
	int ret;
	int fd;
	fd_set set;
	struct timeval to;
	GGZRoom *room;
	int random;
	int roomnumber;

	fd = ggzcore_server_get_fd(agrue->server);

	if (fd < 0) {
		fprintf(stderr, "(net) [%s] ERROR: Could not connect to server.\n", agrue->name);
		return;
	}

	while(!agrue->finished)
	{
		FD_ZERO(&set);
		FD_SET(fd, &set);

		to.tv_sec = 2;
		to.tv_usec = 0;

		ret = select(fd + 1, &set, NULL, NULL, &to);
		if(FD_ISSET(fd, &set))
			ggzcore_server_read_data(agrue->server, fd);

		random = rand() % 100;
		if(random < agrue->mobility)
		{
			roomnumber = rand() % ggzcore_server_get_num_rooms(agrue->server);
			room = ggzcore_server_get_nth_room(agrue->server, roomnumber);
			ggzcore_server_join_room(agrue->server, room);
		}
		else
		{
			if(random < agrue->activity)
			{
				room = ggzcore_server_get_cur_room(agrue->server);
				ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, "Here be some text.");
			}
		}
	}
}

GGZHookReturn net_hook_connect(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] connected; wait for negotiation\n", agrue->name);
#endif

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_negotiate(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] negotiated; now login\n", agrue->name);
#endif

	/* FIXME: nasty ggzcore bug? */
	/*while(!ggzcore_server_is_online(agrue->server))
	{
		fprintf(stderr, "!not online!\n");

		ggzcore_server_read_data(agrue->server,
			ggzcore_server_get_fd(agrue->server));
	}*/

	if(agrue->password)
		ggzcore_server_set_logininfo(agrue->server,
			GGZ_LOGIN, agrue->name, agrue->password, NULL);
	else
		ggzcore_server_set_logininfo(agrue->server,
			GGZ_LOGIN_GUEST, agrue->name, NULL, NULL);

	ggzcore_server_login(agrue->server);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_login(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] logged in; now list rooms and games\n", agrue->name);
#endif

	ggzcore_server_list_rooms(agrue->server, -1, 0);
	ggzcore_server_list_gametypes(agrue->server, 1);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomlist(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;
	GGZRoom *room;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] got rooms; now join room\n", agrue->name);
#endif

	room = ggzcore_server_get_nth_room(agrue->server, 0);
	ggzcore_server_join_room(agrue->server, room);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_typelist(unsigned int id, const void *ed, const void *ud)
{
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_enter(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;
	GGZRoom *room;

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] room joined; now list players\n", agrue->name);
#endif

	room = ggzcore_server_get_cur_room(agrue->server);

	ggzcore_room_list_players(room);

	ggzcore_room_add_event_hook_full(room, GGZ_ROOM_ENTER, net_hook_roomenter, ud);
	ggzcore_room_add_event_hook_full(room, GGZ_ROOM_LEAVE, net_hook_roomleave, ud);
	ggzcore_room_add_event_hook_full(room, GGZ_CHAT_EVENT, net_hook_chat, ud);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_fail(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;
	const char *msg = ed;

	fprintf(stderr, "(net) [%s] ERROR: %s\n", agrue->name, msg);

	agrue->finished = 1;

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomenter(unsigned int id, const void *ed, const void *ud)
{
	const char *player;
	const GGZRoomChangeEventData *data = ed;

	player = data->player_name;
	/*printf(">> ENTER: %s\n", player);*/

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomleave(unsigned int id, const void *ed, const void *ud)
{
	const char *player;
	const GGZRoomChangeEventData *data = ed;

	player = data->player_name;
	/*printf("<< LEAVE: %s\n", player);*/

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_chat(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;
	/*const GGZChatEventData *chat = ed;*/

#ifdef AGRUE_DEBUG
	printf("(net-debug) [%s] received chat\n", agrue->name);
#endif

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_chatfail(unsigned int id, const void *ed, const void *ud)
{
	Agrue *agrue = (Agrue*)ud;

	fprintf(stderr, "(net) [%s] ERROR: Chat failed!\n", agrue->name);

	agrue->finished = 1;

	return GGZ_HOOK_OK;
}

