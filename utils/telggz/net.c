#include "net.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

GGZServer *server = NULL;
GGZRoom *room = NULL;
char *m_username = NULL;
char *m_password = NULL;
char *buffer = NULL;
int m_allow = 1;
char *host = NULL;

void net_init()
{
	server = ggzcore_server_new();
	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, net_hook_connect);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, net_hook_failure);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, net_hook_failure);
}

void net_login(char *username, char *password)
{
	if(room) return;

	if(username) m_username = strdup(username);
	if(password) m_password = strdup(password);

	if(!host) host = "localhost";
	ggzcore_server_set_hostinfo(server, host, 5688);
	ggzcore_server_connect(server);
}

void net_process()
{
	if(server)
	{
		if(ggzcore_server_data_is_pending(server))
			ggzcore_server_read_data(server);
	}
}

void net_send(char *buffer)
{
	if(!room) return;

	ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, buffer);
}

void net_allow(int allow)
{
	m_allow = allow;
}

void net_host(char *hostname)
{
	if(room) return;

	if(host) free(host);
	host = strdup(hostname);
	printf("TelGGZ: Host is now %s\n", hostname);
	fflush(NULL);
}

void net_join(int roomnum)
{
	if(room) ggzcore_server_join_room(server, roomnum);
}

GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data)
{
	/*nasty ggzcore bug?*/
	while(!ggzcore_server_is_online(server)) ggzcore_server_read_data(server); 

	ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, m_username, m_password);
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_failure(unsigned int id, void *event_data, void *user_data)
{
	printf("TelGGZ: Error! Hook error\n");
	fflush(NULL);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data)
{
	printf("TelGGZ: Logged in.\n");
	fflush(NULL);

	ggzcore_server_join_room(server, 0);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data)
{
	room = ggzcore_server_get_cur_room(server);

	printf("TelGGZ: Joined room %s.\n", ggzcore_room_get_name(room));
	fflush(NULL);

	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
	ggzcore_room_add_event_hook(room, GGZ_CHAT, net_hook_chat);
	ggzcore_room_add_event_hook(room, GGZ_PRVMSG, net_hook_prvmsg);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data)
{
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data)
{
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data)
{
	char *player, *message;
	int oldsize;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	if(m_allow)
	{
		if(buffer)
		{
			printf(buffer);
			free(buffer);
			buffer = NULL;
		}
		printf("[%s]: %s\n", player, message);
		fflush(NULL);
	}
	else
	{
		oldsize = (buffer ? strlen(buffer) : 0);
		buffer = (char*)realloc(buffer, oldsize + strlen(message) + strlen(player) + 6);
		if(oldsize) strcat(buffer, "[");
		else strcpy(buffer, "[");
		strcat(buffer, player);
		strcat(buffer, "]: ");
		strcat(buffer, message);
		strcat(buffer, "\n");
	}

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_prvmsg(unsigned int id, void *event_data, void *user_data)
{
	return GGZ_HOOK_OK;
}

