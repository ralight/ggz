/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include "i18n.h"
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

/* Globals */
int status = NET_NOOP;
GGZServer *server = NULL;
GGZRoom *room = NULL;
Guru **queue = NULL;
int queuelen = 1;
char *guruname = NULL;
char *guruguestname = NULL;
FILE *logstream = NULL;

/* Prototypes */
GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_chatfail(unsigned int id, void *event_data, void *user_data);

#if 0
/* Initialize the net functions */
static void net_internal_init(const char *logfile)
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	ret = ggzcore_init(opt);
}
#endif

/* Set up the logfile or close it again */
void net_logfile(const char *logfile)
{
	if(logfile)
	{
		if(!logstream) logstream = fopen(logfile, "a");
	}
	else
	{
		if(logstream)
		{
			fclose(logstream);
			logstream = NULL;
		}
	}
}

/* Add a message to the incoming queue */
/* FIXME: Provide real queue */
static void net_internal_queueadd(const char *player, const char *message, int type)
{
	Guru *guru;
	char *listtoken;
	char *token;
	int i;
	char realmessage[1024];

	/* Add dummy field on dummy messages */
	if((message) && (type == GURU_PRIVMSG))
	{
		sprintf(realmessage, "%s %s", guruname, message);
		message = realmessage;
	}

	/* Insert new grubby structure */
	guru = (Guru*)malloc(sizeof(Guru));
	guru->type = type;
	if(player) guru->player = strdup(player);
	else guru->player = NULL;
	if(message)
	{
		guru->message = strdup(message);
		guru->list = NULL;
		listtoken = strdup(message);
		token = strtok(listtoken, " ,./:-?!\'");
		i = 0;
		while(token)
		{
			guru->list = (char**)realloc(guru->list, (i + 2) * sizeof(char*));
			guru->list[i] = (char*)malloc(strlen(token) + 1);
			strcpy(guru->list[i], token);
			guru->list[i + 1] = NULL;
			i++;
			token = strtok(NULL, " ,./:-?!\'");
		}
		free(listtoken);
	}
	else
	{
		guru->message = NULL;
		guru->list = NULL;
	}

	/* Insert structure into queue */
	queuelen++;
	queue = (Guru**)realloc(queue, sizeof(Guru*) * queuelen);
	queue[queuelen - 2] = guru;
	queue[queuelen - 1] = NULL;
}

/* Transparently connect to any host */
void net_connect(const char *host, int port, const char *name, const char *guestname)
{
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
	ggzcore_server_add_event_hook(server, GGZ_CHAT_FAIL, net_hook_chatfail);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);

	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_connect(server);
}

/* Change the current room */
void net_join(const char *room)
{
	if(room) ggzcore_server_join_room(server, atoi(room));
	else ggzcore_server_join_room(server, 0);
}

/* Loop function */
int net_status(void)
{
	int ret;
	int fd;
	fd_set set;

	/*if(ggzcore_server_data_is_pending(server))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));*/

	fd = ggzcore_server_get_fd(server);

	if (fd < 0) {
		fprintf(stderr, "Could not connect to server.\n");
		exit(EXIT_FAILURE);
	}

	FD_ZERO(&set);
	FD_SET(fd, &set);

	ret = select(fd + 1, &set, NULL, NULL, NULL);
	if(ret == 1) ggzcore_server_read_data(server, fd);

	ret = status;
	if(status == NET_GOTREADY) status = NET_NOOP;
	if(status == NET_LOGIN) status = NET_NOOP;
	if(status == NET_INPUT) status = NET_NOOP;
	return ret;
}

/* Get an entry from the queue */
Guru *net_input(void)
{
	if(queue)
	{
		return queue[queuelen - 2];
	}
	return NULL;
}

/* Let grubby speak */
void net_output(Guru *output)
{
	char *token;
	char *msg;

	if(!room) return;

	/* Handle multi-line answers */
	if(!output->message) return;
/*printf("DEBUG: output is at %i\n", output);
printf("DEBUG: net_output(%s)\n", output->message);*/
	msg = strdup(output->message);
	token = strtok(msg, "\n");
	while(token)
	{
		switch(output->type)
		{
			case GURU_CHAT:
				ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, token);
				break;
			case GURU_PRIVMSG:
				ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, output->player, token);
				break;
			case GURU_ADMIN:
				ggzcore_room_chat(room, GGZ_CHAT_ANNOUNCE, NULL, token);
				break;
		}
		token = strtok(NULL, "\n");
	}
	free(msg);
}

/* Callback for successful connection */
GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data)
{
	/*nasty ggzcore bug?*/
	while((!ggzcore_server_is_online(server)) && (status == NET_NOOP))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));

	if(status == NET_NOOP)
	{
		ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, guruname, "");
		ggzcore_server_login(server);
	}
	return GGZ_HOOK_OK;
}

/* Callback for login */
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);
	return GGZ_HOOK_OK;
}

/* Callback for rooms list */
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data)
{
	status = NET_LOGIN;
	return GGZ_HOOK_OK;
}

/* Callback for joining a room */
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data)
{
	room = ggzcore_server_get_cur_room(server);

	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
	ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, net_hook_chat);

	status = NET_GOTREADY;
	return GGZ_HOOK_OK;
}

/* Common error callback */
GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data)
{
	printf(_("ERROR: %s\n"), (char*)event_data);
	status = NET_ERROR;
	return GGZ_HOOK_OK;
}

/* Callback for new players coming in */
GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data)
{
	char *player;

	player = (char*)event_data;
	/*printf(">> ENTER: %s\n", player);*/
	net_internal_queueadd(player, NULL, GURU_ENTER);
	status = NET_INPUT;
	return GGZ_HOOK_OK;
}

/* Callback for players leaving the room */
GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data)
{
	char *player;

	player = (char*)event_data;
	/*printf("<< LEAVE: %s\n", player);*/
	net_internal_queueadd(player, NULL, GURU_LEAVE);
	status = NET_INPUT;
	return GGZ_HOOK_OK;
}

/* Chat callback which passes message to grubby */
GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data)
{
	int type;
	char *roomname;
	time_t t;
	char *ts;
	GGZChatEventData *chat = event_data;

	/* Ignore all self-generates messages */
	if (strcmp(chat->sender, guruname)
	    && strcmp(chat->sender, guruguestname))
	{
		if (chat->type == GGZ_CHAT_PERSONAL) type = GURU_PRIVMSG;
		else type = GURU_CHAT;
		net_internal_queueadd(chat->sender, chat->message, type);
		status = NET_INPUT;
	}

	/* Log all messages to the logfile if enabled */
	if(logstream)
	{
		if(room) roomname = ggzcore_room_get_name(room);
		else roomname = "-";
		t = time(NULL);
		ts = ctime(&t);
		ts[strlen(ts) - 1] = 0;
		fprintf(logstream, "%s (%s) [%s]: %s\n", ts, roomname,
			chat->sender, chat->message);
		fflush(logstream);
	}
	
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_chatfail(unsigned int id, void *event_data, void *user_data)
{
	printf("Chat failed!\n");
	return GGZ_HOOK_OK;
}

