/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Globals */
int status = NET_NOOP;
GGZServer *server = NULL;
GGZRoom *room = NULL;
Guru **queue = NULL;
int queuelen = 1;
char *guruname = NULL;
char *guruguestname = NULL;
FILE *logfile;

/* Prototypes */
GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data);

void net_internal_init()
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	opt.debug_file = "/tmp/doobadoo"; /* In ggzcore 0.0.5 this can be NULL, but we want to be save */
	opt.debug_levels = 0;
	ret = ggzcore_init(opt);
	/* Improve this! (configurable log file) */
	logfile = fopen("/tmp/guruchatlog", "a");
}

void net_internal_queueadd(const char *player, const char *message, int type)
{
	Guru *guru;
	char *listtoken;
	char *token;
	int i;
	char realmessage[1024];

	/* add dummy field */
	if((message) && (type == GURU_PRIVMSG))
	{
		sprintf(realmessage, "grubbydummy %s", message);
		message = realmessage;
	}

	guru = (Guru*)malloc(sizeof(Guru));
	guru->type = type;
	if(player) guru->player = strdup(player);
	else guru->player = NULL;
	if(message)
	{
		guru->message = strdup(message);
		guru->list = NULL;
		listtoken = strdup(message);
		token = strtok(listtoken, " ,./:-");
		i = 0;
		while(token)
		{
			guru->list = (char**)realloc(guru->list, (i + 2) * sizeof(char*));
			guru->list[i] = (char*)malloc(strlen(token) + 1);
			strcpy(guru->list[i], token);
			guru->list[i + 1] = NULL;
			i++;
			token = strtok(NULL, " ,./:-");
		}
		free(listtoken);
	}
	else
	{
		guru->message = NULL;
		guru->list = NULL;
	}

	queuelen++;
	queue = (Guru**)realloc(queue, sizeof(Guru*) * queuelen);
	queue[queuelen - 2] = guru;
	queue[queuelen - 1] = NULL;
}

void net_connect(const char *host, int port, const char *name, const char *guestname)
{
	net_internal_init();

	guruname = (char*)name;
	guruguestname = (char*)guestname;

	server = ggzcore_server_new();

	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, net_hook_connect);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);

	ggzcore_server_set_hostinfo(server, host, port);
	ggzcore_server_connect(server);
}

void net_join(int room)
{
	ggzcore_server_join_room(server, room);
}

int net_status()
{
	int ret;

	if(ggzcore_server_data_is_pending(server))
		ggzcore_server_read_data(server);

	ret = status;
	if(status == NET_GOTREADY) status = NET_NOOP;
	if(status == NET_LOGIN) status = NET_NOOP;
	if(status == NET_INPUT) status = NET_NOOP;
	return ret;
}

Guru *net_input()
{
	if(queue)
	{
		return queue[queuelen - 2];
	}
	return NULL;
}

void net_output(Guru *output)
{
	if(!room) return;
	switch(output->type)
	{
		case GURU_CHAT:
			ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, output->message);
			break;
		case GURU_PRIVMSG:
			ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, output->player, output->message);
			break;
	}
}

GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data)
{
	/*nasty ggzcore bug?*/
	while((!ggzcore_server_is_online(server)) && (status == NET_NOOP)) ggzcore_server_read_data(server);

	if(status == NET_NOOP)
	{
		ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, guruname, "");
		ggzcore_server_login(server);
	}
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data)
{
	status = NET_LOGIN;
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data)
{
	room = ggzcore_server_get_cur_room(server);

	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
	ggzcore_room_add_event_hook(room, GGZ_CHAT, net_hook_chat);
	ggzcore_room_add_event_hook(room, GGZ_PRVMSG, net_hook_chat);

	status = NET_GOTREADY;
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data)
{
	status = NET_ERROR;
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
	int type;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];

	/* Ignore all self-generates messages */
	if((strcmp(player, guruname)) && (strcmp(player, guruguestname)))
	{
		if(id == GGZ_PRVMSG) type = GURU_PRIVMSG;
		else type = GURU_CHAT;
		net_internal_queueadd(player, message, type);
		status = NET_INPUT;
	}

	/* Logging here? */
	if(logfile)
	{
		fprintf(logfile, "[%s]: %s\n", player, message);
		fflush(logfile);
	}
	
	return GGZ_HOOK_OK;
}

