/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2003, 2004 Josef Spillner, <josef@ggzgamingzone.org>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <ggz.h>
#include <unistd.h>
#include <time.h>

#include "gurumod.h"
#include "i18n.h"
#include "game.h"

static int channel;
static int cchannel;
static time_t channeldelay;

/* Empty init */
void gurumod_init(const char *datadir)
{
	channel = ggz_make_unix_socket(GGZ_SOCK_SERVER, "/tmp/grubby.games");
	if(channel == -1)
	{
		printf("Warning: Game communication channel inactive\n");
	}
	listen(channel, 1);
	cchannel = -1;
	channeldelay = 0;
}

static int network(void)
{
	fd_set set;
	struct timeval tv;
	int ret;

	FD_ZERO(&set);

	FD_SET(cchannel, &set);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select(cchannel + 1, &set, NULL, NULL, &tv);

	if(ret)
	{
		if(FD_ISSET(cchannel, &set))
		{
			return 1;
		}
	}
	return 0;
}

static Guru *game(Guru *message)
{
	int opcode;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char buffer[1024];

	if(cchannel == -1)
	{
		addrlen = sizeof(addr);
		cchannel = accept(channel, (struct sockaddr*)&addr, &addrlen);
		if(cchannel == -1)
		{
			printf("'Warning: Game communication channel unaccepted\n");
		}
	}
	if(channel == -1) return NULL;
	if(!network()) return NULL;

	if(!ggz_read_int(cchannel, &opcode))
	{
		if(opcode == GURU_GAME_MESSAGE)
		{
			ggz_read_string(cchannel, buffer, sizeof(buffer));
			message->message = buffer;
			message->type = GURU_CHAT;
			return message;
		}
	}

	return NULL;
}

/* Grubby is a gaming bot */
Guru *gurumod_exec(Guru *message)
{
	int i, max;
	char buffer[1024];

	if(message->type == GURU_TICK)
	{
		if(!channeldelay) return NULL;
		if(time(NULL) - channeldelay < 2) return NULL;
		return game(message);
	}

	if((message->type != GURU_DIRECT)
	&& (message->type != GURU_PRIVMSG))
		return NULL;

	i = 0;
	while((message->list) && (message->list[i])) i++;
	max = i;

	if(max == 4)
	{
		/* Joining a game */
		if((!strcasecmp(message->list[1], "join"))
		&& (!strcasecmp(message->list[2], "my"))
		&& (!strcasecmp(message->list[3], "game")))
		{
			channeldelay = time(NULL);
			snprintf(buffer, sizeof(buffer), __("Sure let's play..."));
			message->message = buffer;
			message->type = GURU_GAME;
			return message;
		}
	}

	return NULL;
}

