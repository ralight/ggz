/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <ggz.h>

/* Globals */
int status = NET_NOOP;
Guru **queue = NULL;
int queuelen = 1;
char *guruname = NULL;
char *guruguestname = NULL;
FILE *logstream = NULL;
FILE *irc = NULL;
char *playername = NULL;
char *chatroom = NULL;

/* Prototypes */
void chat(const char *message);

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

/* Establish connection: log into an IRC server */
void net_connect(const char *host, int port, const char *name, const char *guestname)
{
	/*char buffer[256];*/
	int fd;
	/*int i;*/

	guruname = (char*)name;
	guruguestname = (char*)guestname;

	fd = ggz_make_socket(GGZ_SOCK_CLIENT, 6667, host);
	if(fd < 0)
	{
		status = NET_ERROR;
		return;
	}
	irc = fdopen(fd, "a+");
	if(!irc)
	{
		status = NET_ERROR;
		return;
	}

	/*printf("Logon IRC...\n");*/
	/*fcntl(fd, F_SETFL, O_NONBLOCK);*/

	/*for(i = 0; i < 4; i++)
	{
		fgets(buffer, sizeof(buffer), irc);
		buffer[strlen(buffer) - 1] = 0;
		printf("Buffer: %s\n", buffer);
	}*/

	fprintf(irc, "NICK %s\r\n", guruname);
	fflush(irc);
	fprintf(irc, "USER %s %s %s :%s\r\n", guruname, "noosphere", "localhost", guruname);
	fflush(irc);

	status = NET_LOGIN;
}

/* Change the current room (channel, in this case) */
void net_join(const char *room)
{
	/*printf("Join IRC room...\n");*/
	fprintf(irc, "JOIN %s\r\n", room);
	fflush(irc);

	chatroom = room;

	status = NET_GOTREADY;
}

/* Loop function */
int net_status()
{
	int ret;
	/*int num;*/
	char buffer[1024];

	if(status == NET_NOOP)
	{
		/*num = read(fileno(irc), buffer, sizeof(buffer));*/
		fgets(buffer, sizeof(buffer), irc);
		buffer[strlen(buffer) - 2] = 0;

		/*if(num > 0)
		{
			buffer[num - 1] = 0;*/
			chat(buffer);
		/*}*/
	}

	ret = status;
	if(status == NET_GOTREADY) status = NET_NOOP;
	if(status == NET_LOGIN) status = NET_NOOP;
	if(status == NET_INPUT) status = NET_NOOP;
	return ret;
}

/* Get an entry from the queue */
Guru *net_input()
{
	if(queue)
	{
printf("input is '%s'\n", queue[queuelen - 2]->message);
		return queue[queuelen - 2];
	}
	return NULL;
}

/* Let grubby speak */
void net_output(Guru *output)
{
	char *token;
	char *msg;

	/* Handle multi-line answers */
	if(!output->message) return;
	msg = strdup(output->message);
	token = strtok(msg, "\r\n");
	while(token)
	{
		switch(output->type)
		{
			case GURU_CHAT:
				fprintf(irc, "PRIVMSG %s :%s\r\n", chatroom, token);
				fflush(irc);
				break;
			case GURU_PRIVMSG:
				/*printf("-> %s: %s\n", output->player, token);*/
				break;
			case GURU_ADMIN:
				/*printf(">> %s\n", token);*/
				break;
		}
		token = strtok(NULL, "\n");
	}
	free(msg);
}

void chat(const char *message)
{
	time_t t;
	char *ts;

	/* Ignore all self-generates messages */
	playername = NULL;
	net_internal_queueadd(playername, message, GURU_CHAT/*GURU_PRIVMSG*/);
	status = NET_INPUT;

	/* Log all messages to the logfile if enabled */
	if(logstream)
	{
		t = time(NULL);
		ts = ctime(&t);
		ts[strlen(ts) - 1] = 0;
		fprintf(logstream, "%s (%s) [%s]: %s\n", ts, "-", playername, message);
		fflush(logstream);
	}
}

