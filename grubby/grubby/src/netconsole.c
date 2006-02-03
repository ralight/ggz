/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2005 Josef Spillner <josef@ggzgamingzone.org>
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

/* Globals */
int status = NET_NOOP;
Guru **queue = NULL;
int queuelen = 1;
char *guruname = NULL;
char *gurupassword = NULL;
FILE *logstream = NULL;
char *playername;

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
	guru->playertype = PLAYER_UNKNOWN;
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

	/* Insert structure into queue */
	queuelen++;
	queue = (Guru**)realloc(queue, sizeof(Guru*) * queuelen);
	queue[queuelen - 2] = guru;
	queue[queuelen - 1] = NULL;
}

/* Dummy connection: just setup the console */
void net_connect(const char *host, int port, const char *name, const char *password)
{
	char buffer[16];

	guruname = (char*)name;
	gurupassword = (char*)password;

	printf("Enter your name please:\n");
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;
	playername = strdup(buffer);

	/*fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);*/

	status = NET_GOTREADY;
}

/* Change the current room (dummy function for netconsole) */
void net_join(const char *room)
{
}

/* Loop function */
int net_status()
{
	int ret;
	int num;
	char buffer[1024];

	num = read(STDIN_FILENO, buffer, sizeof(buffer));
	if(num > 0)
	{
		buffer[num - 1] = 0;
		/*printf("Got: %s\n", buffer);*/
		chat(buffer);
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
	token = strtok(msg, "\n");
	while(token)
	{
		switch(output->type)
		{
			case GURU_CHAT:
				printf("> %s\n", token);
				break;
			case GURU_PRIVMSG:
				printf("-> %s: %s\n", output->player, token);
				break;
			case GURU_ADMIN:
				printf(">> %s\n", token);
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
	net_internal_queueadd(playername, message, /*GURU_CHAT*/GURU_PRIVMSG);
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

