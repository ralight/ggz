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
static char *inputstr = NULL;
static int status = NET_NOOP;
static FILE *irc = NULL;
static char *chatroom = NULL;
static char *chatname = NULL;

void chat(const char *message);

/* Establish connection: log into an IRC server */
void net_irc_connect(const char *ircuri)
{
	int fd;
	ggz_uri_t uri;

	uri = ggz_uri_from_string(ircuri);
	printf("IRC connection: %s@%s (channel %s)\n", uri.user, uri.host, uri.path);

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

	setbuf(irc, NULL);
	fcntl(fd, F_SETFL, O_NONBLOCK);

	fprintf(irc, "NICK %s\r\n", uri.user);
	fprintf(irc, "USER %s %s %s :%s\r\n", uri.user, "localhost", "localhost", "GGZ IRC gate");

	/* FIXME: wait for IRC reverse lookup, we're lazy and simply sleep for (hopefully enough) time */
	sleep(7);

	chatroom = ggz_strdup(uri.path);
	chatname = ggz_strdup(uri.user);

	fprintf(irc, "JOIN %s\r\n", chatroom);

	ggz_uri_free(uri);
}

/* Loop function */
int net_irc_status()
{
	char buffer[1024];
	int oldstatus;

	if(status == NET_NOOP)
	{
		fgets(buffer, sizeof(buffer), irc);
		if(strlen(buffer) > 0)
		{
			buffer[strlen(buffer) - 2] = '\0';
			chat(buffer);
		}
	}

	oldstatus = status;
	status = NET_NOOP;
	return oldstatus;
}

/* Get an entry from the queue */
char *net_irc_input()
{
	return inputstr;
}

/* Let the gate speak */
void net_irc_output(const char *output)
{
	fprintf(irc, "PRIVMSG %s :%s\r\n", chatroom, output);
}

void chat(const char *message)
{
	char *part;
	char *tmp, *token;
	char *player;

	/* Extract player name */
	player = ggz_strdup(message + 1);
	part = strstr(player, "!");
	if(part)
	{
		part[0] = '\0';
	}
	else
	{
		ggz_free(player);
		return;
	}

	/* Bounce off our own messages */
	if(!ggz_strcmp(player, chatname))
	{
		ggz_free(player);
		return;
	}

	/* Extract message and message type */
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
					inputstr = ggz_strbuild("[%s] %s", player, token + 1);
					status = NET_INPUT;
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

	ggz_free(player);
}

