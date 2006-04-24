/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 - 2003 Josef Spillner, dr_maux@users.sourceforge.net

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Header files */
#include "net.h"

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Global variables */
GGZServer *server = NULL;
GGZRoom *room = NULL;
char *m_username = NULL;
char *m_password = NULL;
char *buffer = NULL;
int m_allow = 1;
char *host = NULL;
int port = 0;

/* Prototypes */
static void flush_buffer(void);

/* Public Functions */

void net_login(const char *username, const char *password)
{
	if(room) return;

	if(username) m_username = strdup(username);
	if(password) m_password = strdup(password);

	if(!host) host = "localhost";
	if(!port) port = 5688;
	ggzcore_server_set_hostinfo(server, host, port, 0);
	printf("Logging in as %s...\n", m_username);
	ggzcore_server_connect(server);
}

void net_process(void)
{
	if(server)
		if(ggzcore_server_data_is_pending(server))
			ggzcore_server_read_data(server, ggzcore_server_get_fd(server));
}

void net_send(char *buffer)
{
	if(!room) return;

	ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, buffer);
}

void net_allow(int allow)
{
	if(!allow)
	{
		printf("* Buffered input\n");
	}
	m_allow = allow;
}

void net_host(const char *hostname, int portnumber)
{
	if(room) return;

	if(host) free(host);
	host = strdup(hostname);
	port = portnumber;
	printf("TelGGZ: Host is now %s:%i.\n", hostname, port);
}

void net_join(int roomnum)
{
	if(room) ggzcore_server_join_room(server, roomnum);
}

void net_list(void)
{
	int i;
	GGZRoom *room;

	for(i = 0; i < ggzcore_server_get_num_rooms(server); i++)
	{
		room = ggzcore_server_get_nth_room(server, i);
		printf("%3i: %s\n", i, ggzcore_room_get_name(room));
	}
}

void net_who(void)
{
	if(!room)
	{
		printf("Not in a room yet.\n");
		return;
	}

	ggzcore_room_list_players(room);
}

static GGZHookReturn net_hook_players(unsigned int id,
				      const void *event_data,
				      const void *user_data)
{
	GGZPlayer *player;
	int i;

	for(i = 0; i < ggzcore_room_get_num_players(room); i++)
	{
		player = ggzcore_room_get_nth_player(room, i);
		printf(" * %s\n", ggzcore_player_get_name(player));
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_connect(unsigned int id, const void *event_data,
				      const void *user_data)
{
	while(!ggzcore_server_is_online(server))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));

	ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST,
				     m_username, m_password, NULL);
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_negotiated(unsigned int id,
					 const void *event_data,
					 const void *user_data)
{
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_failure(unsigned int id, const void *event_data,
				      const void *user_data)
{
	GGZErrorEventData error;

	error = *(GGZErrorEventData*)event_data;
	printf("TelGGZ: Error %i: %s\n", id, error.message);
	exit(-1);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_login(unsigned int id, const void *event_data,
				    const void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_motd(unsigned int id, const void *event_data,
				   const void *user_data)
{
	const GGZMotdEventData *motd;

	motd = event_data;
	printf("MOTD:\n");
	printf("%s\n", motd->motd);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_roomlist(unsigned int id,
				       const void *event_data,
				       const void *user_data)
{
	printf("TelGGZ: Logged in.\n");

	ggzcore_server_join_room(server, 0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_roomenter(unsigned int id,
					const void *event_data,
					const void *user_data)
{
	int oldsize;
	char *player;

	player = (char*)event_data;

	if(m_allow)
	{
		flush_buffer();
		printf("TelGGZ: %s has joined the room.\n", player);
	}
	else
	{
		oldsize = (buffer ? strlen(buffer) : 0);
		buffer = (char*)realloc(buffer, oldsize + strlen(player) + 33);
		if(oldsize) strcat(buffer, "");
		else(strcpy(buffer, ""));
		strcat(buffer, "TelGGZ: ");
		strcat(buffer, player);
		strcat(buffer, " has joined the room.\n");
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_roomleave(unsigned int id,
					const void *event_data,
					const void *user_data)
{
	int oldsize;
	char *player;

	player = (char*)event_data;

	if(m_allow)
	{
		flush_buffer();
		printf("TelGGZ: %s has left the room.\n", player);
	}
	else
	{
		oldsize = (buffer ? strlen(buffer) : 0);
		buffer = (char*)realloc(buffer, oldsize + strlen(player) + 31);
		if(oldsize) strcat(buffer, "");
		else(strcpy(buffer, ""));
		strcat(buffer, "TelGGZ: ");
		strcat(buffer, player);
		strcat(buffer, " has left the room.\n");
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_hook_chat(unsigned int id, const void *event_data,
				   const void *user_data)
{
	const GGZChatEventData *chat = event_data;

	if (!chat->sender || !chat->message) {
		/* TelGGZ is not set up to handle this kind of message */
		return GGZ_HOOK_OK;
	}

	if(m_allow)
	{
		flush_buffer();
		printf("[%s]: %s\n", chat->sender, chat->message);
	}
	else
	{
		int oldsize = buffer ? strlen(buffer) : 0;
		buffer = realloc(buffer, oldsize + strlen(chat->message)
				 + strlen(chat->sender) + 6);
		if(oldsize) strcat(buffer, "[");
		else strcpy(buffer, "[");
		strcat(buffer, chat->sender);
		strcat(buffer, "]: ");
		strcat(buffer, chat->message);
		strcat(buffer, "\n");
	}

	return GGZ_HOOK_OK;
}

#if 0 /* This code is unused -- but what's it supposed to do? */
static GGZHookReturn net_hook_prvmsg(unsigned int id, const void *event_data,
				     const void *user_data)
{
	char *player, *message;
	int oldsize;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	if(m_allow)
	{
		flush_buffer();
		printf("[[%s]]: %s\n", player, message);
	}
	else
	{
		oldsize = (buffer ? strlen(buffer) : 0);
		buffer = (char*)realloc(buffer, oldsize + strlen(message) + strlen(player) + 8);
		if(oldsize) strcat(buffer, "[[");
		else strcpy(buffer, "[[");
		strcat(buffer, player);
		strcat(buffer, "]]: ");
		strcat(buffer, message);
		strcat(buffer, "\n");
	}

	return GGZ_HOOK_OK;
}
#endif

static GGZHookReturn net_hook_enter(unsigned int id, const void *event_data,
				    const void *user_data)
{
	room = ggzcore_server_get_cur_room(server);

	printf("TelGGZ: Joined room %s.\n", ggzcore_room_get_name(room));

	if(room)
	{
		ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
		ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
		ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, net_hook_chat);
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST, net_hook_players);
	}

	return GGZ_HOOK_OK;
}

void net_init(void)
{
	server = ggzcore_server_new();
	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, net_hook_connect);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATED, net_hook_negotiated);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_MOTD_LOADED, net_hook_motd);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_CHAT_FAIL, net_hook_failure);
}

/* Private Functions */

static void flush_buffer(void)
{
	if(buffer)
	{
		printf(buffer);
		free(buffer);
		buffer = NULL;
	}
}

