/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2005 Josef Spillner <josef@ggzgamingzone.org>
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
static int status = NET_NOOP;
static GGZServer *server = NULL;
static GGZRoom *room = NULL;
static GGZGame *game = NULL;
static Guru **queue = NULL;
static int queuelen = 1;
static char *guruname = NULL;
static char *gurupassword = NULL;
static FILE *logstream = NULL;
static int tableid = -1;
static int gamefd = -1;
static int channelfd = -1;
static time_t lasttick = 0;

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
static GGZHookReturn net_hook_typelist(unsigned int id, const void *event_data, const void *user_data);

static GGZHookReturn net_hook_table(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_launched(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_negotiated(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_playing(unsigned int id, const void *event_data, const void *user_data);

static GGZHookReturn net_hook_channel(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn net_hook_ready(unsigned int id, const void *event_data, const void *user_data);

static void net_internal_game(GGZPlayer *player);
static void net_internal_gameprepare(const char *playername);

/* Initialize the net functions */
static void net_internal_init(void)
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	ret = ggzcore_init(opt);
}

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
		token = strtok(listtoken, " ,./:?!\'");
		i = 0;
		while(token)
		{
			guru->list = (char**)realloc(guru->list, (i + 2) * sizeof(char*));
			guru->list[i] = (char*)malloc(strlen(token) + 1);
			strcpy(guru->list[i], token);
			guru->list[i + 1] = NULL;
			i++;
			token = strtok(NULL, " ,./:?!\'");
		}
		free(listtoken);
	}
	else
	{
		guru->message = NULL;
		guru->list = NULL;
	}

	/* Recognize direct speech */
	if((guru->type == GURU_CHAT) && (guru->list) && (guru->list[0]))
		if(!strcasecmp(guru->list[0], guruname))
			guru->type = GURU_DIRECT;

	/* Insert structure into queue */
	queuelen++;
	queue = (Guru**)realloc(queue, sizeof(Guru*) * queuelen);
	queue[queuelen - 2] = guru;
	queue[queuelen - 1] = NULL;
}

/* Transparently connect to any host */
void net_connect(const char *host, int port, const char *name, const char *password)
{
	guruname = (char*)name;
	gurupassword = (char*)password;

	net_internal_init();

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
	ggzcore_server_add_event_hook(server, GGZ_TYPE_LIST, net_hook_typelist);

	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_CONNECTED, net_hook_channel);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY, net_hook_ready);

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
	struct timeval to;
	struct timeval *top;
	struct timespec req;
	time_t currenttick;

	to.tv_sec = 0;
	to.tv_usec = 0;
	top = &to;
	if((channelfd == -1) && (gamefd == -1)) top = NULL;

	/*if(ggzcore_server_data_is_pending(server))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));*/

	if(channelfd == -1)
	{
		fd = ggzcore_server_get_fd(server);

		if (fd < 0) {
			fprintf(stderr, "Could not connect to server.\n");
			exit(EXIT_FAILURE);
		}

		FD_ZERO(&set);
		FD_SET(fd, &set);

		ret = select(fd + 1, &set, NULL, NULL, top);
		if(ret == 1) ggzcore_server_read_data(server, fd);
	}

	if(gamefd != -1)
	{
		FD_ZERO(&set);
		FD_SET(gamefd, &set);
		ret = select(gamefd + 1, &set, NULL, NULL, top);
		if(ret == 1) ggzcore_game_read_data(game);
	}

	if(channelfd != -1)
	{
		FD_ZERO(&set);
		FD_SET(channelfd, &set);
		ret = select(channelfd + 1, &set, NULL, NULL, top);
		if(ret == 1) ggzcore_server_read_data(server, channelfd);
	}

	if(status == NET_NOOP)
	{
		currenttick = time(NULL);
		if(currenttick - lasttick > 1)
		{
			net_internal_queueadd(NULL, NULL, GURU_TICK);
			status = NET_INPUT;
			lasttick = currenttick;
		}

		req.tv_sec = 0;
		req.tv_nsec = 500000;
		nanosleep(&req, NULL);
	}

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
			case GURU_DIRECT:
				ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, token);
				break;
			case GURU_PRIVMSG:
				ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, output->player, token);
				break;
			case GURU_ADMIN:
				ggzcore_room_chat(room, GGZ_CHAT_ANNOUNCE, NULL, token);
				break;
			case GURU_GAME:
				ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, token);
				net_internal_gameprepare(output->player);
				break;
		}
		token = strtok(NULL, "\n");
	}
	free(msg);
}

/* Callback for successful connection */
GGZHookReturn net_hook_connect(unsigned int id, const void *event_data, const void *user_data)
{
	/*nasty ggzcore bug?*/
	while((!ggzcore_server_is_online(server)) && (status == NET_NOOP))
		ggzcore_server_read_data(server, ggzcore_server_get_fd(server));

	if(status == NET_NOOP)
	{
		if(gurupassword)
			ggzcore_server_set_logininfo(server, GGZ_LOGIN, guruname, gurupassword);
		else
			ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, guruname, "");
		ggzcore_server_login(server);
	}
	return GGZ_HOOK_OK;
}

/* Callback for login */
GGZHookReturn net_hook_login(unsigned int id, const void *event_data, const void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);
	ggzcore_server_list_gametypes(server, 1);
	return GGZ_HOOK_OK;
}

/* Callback for rooms list */
GGZHookReturn net_hook_roomlist(unsigned int id, const void *event_data, const void *user_data)
{
	status = NET_LOGIN;
	return GGZ_HOOK_OK;
}

/* Callback for type list */
GGZHookReturn net_hook_typelist(unsigned int id, const void *event_data, const void *user_data)
{
	return GGZ_HOOK_OK;
}

/* Callback for joining a room */
GGZHookReturn net_hook_enter(unsigned int id, const void *event_data, const void *user_data)
{
	room = ggzcore_server_get_cur_room(server);

	/*ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST, net_hook_playerlist);*/
	ggzcore_room_list_players(room);
	ggzcore_room_list_tables(room, 0, 0);

	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, net_hook_roomenter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, net_hook_roomleave);
	ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, net_hook_chat);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_JOINED, net_hook_table);

	status = NET_GOTREADY;
	return GGZ_HOOK_OK;
}

/* Common error callback */
GGZHookReturn net_hook_fail(unsigned int id, const void *event_data, const void *user_data)
{
	const char *msg = event_data;

	printf(_("ERROR: %s\n"), msg);
	status = NET_ERROR;
	return GGZ_HOOK_OK;
}

/* Callback for new players coming in */
GGZHookReturn net_hook_roomenter(unsigned int id, const void *event_data, const void *user_data)
{
	const char *player;
	const GGZRoomChangeEventData *data = event_data;

	player = data->player_name;
	/*printf(">> ENTER: %s\n", player);*/
	net_internal_queueadd(player, NULL, GURU_ENTER);
	status = NET_INPUT;
	return GGZ_HOOK_OK;
}

/* Callback for players leaving the room */
GGZHookReturn net_hook_roomleave(unsigned int id, const void *event_data, const void *user_data)
{
	const char *player;
	const GGZRoomChangeEventData *data = event_data;

	player = data->player_name;
	/*printf("<< LEAVE: %s\n", player);*/
	net_internal_queueadd(player, NULL, GURU_LEAVE);
	status = NET_INPUT;
	return GGZ_HOOK_OK;
}

/* Let grubby launch a game */
void net_internal_game(GGZPlayer *player)
{
	GGZTable *table;
	GGZModule *module;
	GGZGameType *gametype;
	const char *name, *engine, *version;
	int j;

	table = ggzcore_player_get_table(player);
	if(table)
	{
		tableid = ggzcore_table_get_id(table);
		printf("at table id %i\n", tableid);
		//ggzcore_room_join_table(room, tableid, 0);

		/*printf("modules: %i\n", ggzcore_module_get_num());
		printf("# modules: %i\n", ggzcore_module_get_num_by_type("TicTacToe", "TicTacToe", "3"));*/

		gametype = ggzcore_room_get_gametype(room);
		if(!gametype)
		{
			printf("No type found.\n");
		}
		else
		{
			name = ggzcore_gametype_get_name(gametype);
			engine = ggzcore_gametype_get_prot_engine(gametype);
			version = ggzcore_gametype_get_prot_version(gametype);

			module = NULL;
			for(j = 0; j < ggzcore_module_get_num_by_type(name, engine, version); j++)
			{
				module = ggzcore_module_get_nth_by_type(name, engine, version, j);
				if(!strcmp(ggzcore_module_get_frontend(module), "guru")) break;
				module = NULL;
			}
			if(module)
			{
				printf("module: %p\n", module);

				game = ggzcore_game_new();
				ggzcore_game_init(game, server, module);

				ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCHED, net_hook_launched);
				ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATED, net_hook_negotiated);
				ggzcore_game_add_event_hook(game, GGZ_GAME_PLAYING, net_hook_playing);

				ggzcore_game_launch(game);
			}
			else
			{
				printf("No module found with frontend guru\n");
			}
		}
	}
	else
	{
		printf("Not at table\n");
	}
}

void net_internal_gameprepare(const char *playername)
{
	char *playertmp;
	GGZPlayer *player;
	int i;

	printf("game: check %i players\n", ggzcore_room_get_num_players(room));
	for(i = 0; i < ggzcore_room_get_num_players(room); i++)
	{
		player = ggzcore_room_get_nth_player(room, i);
		playertmp = ggzcore_player_get_name(player);
		printf("got %s (%s)\n", playername, playertmp);
		if(!strcmp(playername, playertmp))
		{
			net_internal_game(player);
			break;
		}
	}
}

/* Chat callback which passes message to grubby */
GGZHookReturn net_hook_chat(unsigned int id, const void *event_data, const void *user_data)
{
	int type;
	char *roomname;
	time_t t;
	char *ts;
	const GGZChatEventData *chat = event_data;

	/* Ignore all self-generates messages */
	if (strcmp(chat->sender, guruname))
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

GGZHookReturn net_hook_chatfail(unsigned int id, const void *event_data, const void *user_data)
{
	printf("Chat failed!\n");
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_table(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- joined table\n");

	//ggzcore_game_launch(game);

	//ggzcore_module_launch(module);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_launched(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- launched; create gamefd\n");

	gamefd = ggzcore_game_get_control_fd(game);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_negotiated(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- negotiated\n");

	ggzcore_server_create_channel(server);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_playing(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- playing\n");

	ggzcore_room_join_table(room, tableid, 0);

	return GGZ_HOOK_OK;
}

#if 0 /* Unused. */
GGZHookReturn net_hook_over(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- over; kill gamefd\n");

	ggzcore_room_leave_table(room, 1);
	ggzcore_game_free(game);

	game = NULL;
	gamefd = -1;

	return GGZ_HOOK_OK;
}
#endif

GGZHookReturn net_hook_channel(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- channelconnected; create channelfd\n");

	channelfd = ggzcore_server_get_channel(server);

	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_ready(unsigned int id, const void *event_data, const void *user_data)
{
	printf("-- channelready; pass channelfd\n");

	ggzcore_game_set_server_fd(game, channelfd);
	channelfd = -1;

	return GGZ_HOOK_OK;
}

