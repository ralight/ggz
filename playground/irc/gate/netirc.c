/*******************************************************************
*
* ggz-ircgate - Gateway between GGZ room chat and IRC channel
* Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
* Based on Grubby chatbot sources
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
//static char *message = NULL;
static int status = NET_NOOP;
static FILE *irc = NULL;
static char *chatroom = NULL;

/* Establish connection: log into an IRC server */
void net_irc_connect(const char *ircuri)
{
	int fd;
	ggz_uri_t uri;

	uri = ggz_uri_from_string(ircuri);

	fd = ggz_make_socket(GGZ_SOCK_CLIENT, 6667, uri.host);
	if(fd < 0)
	{
		fprintf(stderr, "ERROR (irc): Connection refused.\n");
		ggz_uri_free(uri);
		status = NET_ERROR;
		return;
	}
	irc = fdopen(fd, "a+");
	if(!irc)
	{
		fprintf(stderr, "ERROR (irc): Cannot create stream.\n");
		ggz_uri_free(uri);
		status = NET_ERROR;
		return;
	}

	fprintf(irc, "NICK %s\r\n", "_ggz-ircgate_");
	fflush(irc);
	fprintf(irc, "USER %s %s %s :%s\r\n", "_ggz-ircgate_", "somehost", "localhost", "_ggz-ircgate_");
	fflush(irc);

	chatroom = ggz_strdup(uri.path);

	fprintf(irc, "JOIN %s\r\n", chatroom);
	fflush(irc);

	ggz_uri_free(uri);

	status = NET_LOGIN;
}

/* Loop function */
int net_irc_status()
{
	//int ret;
	char buffer[1024];

	if(status == NET_NOOP)
	{
		fgets(buffer, sizeof(buffer), irc);
		buffer[strlen(buffer) - 2] = 0;

		//chat(buffer);
	}

	return NET_NOOP;
}

/* Get an entry from the queue */
char *net_irc_input()
{
	return NULL;
}

/* Let the gate speak */
void net_irc_output(const char *output)
{
	fprintf(irc, "PRIVMSG %s :%s\r\n", chatroom, output);
	fflush(irc);
}

void chat(const char *message)
{
	//time_t t;
	//char *ts;
	char *part;
	char *tmp, *token;
	char *msg, *player;
	int type;

	/* Extract player name */
	player = ggz_strdup(message + 1);
	part = strstr(player, "!");
	if(part) part[0] = 0;
	else player = NULL;

	if(!player) return;

	/* Extract message and message type */
	type = 0;
	msg = NULL;
	part = strstr(message, "PRIVMSG");
	if(part)
	{
		tmp = ggz_strdup(part);
		token = strtok(tmp, " ");
		if(token)
		{
			token = strtok(NULL, " ");
			if(token)
			{
				if(!strcmp(token, chatroom))
				{
					token = strtok(NULL, "\r\n");
					msg = ggz_strdup(token + 1);
				}
			}
		}
		ggz_free(tmp);
	}
	else
	{
		if(strstr(message, " JOIN "))
		{
		}
		if(strstr(message, " QUIT "))
		{
		}
	}

	/*if(!type){
		ggz_free(player);
		if(msg) ggz_free(msg);
		return;
	}*/
	/*if((!msg) && ((type == GURU_PRIVMSG) || (type == GURU_CHAT))){
		ggz_free(player);
		return;
	}*/

	/* Ignore all self-generates messages */
	/*net_internal_queueadd(player, msg, type);*/
	status = NET_INPUT;

	if(msg) ggz_free(msg);
	ggz_free(player);
}

