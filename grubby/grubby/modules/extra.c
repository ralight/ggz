/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "i18n.h"

#define COMMAND_SAY 1
#define COMMAND_HI 2
#define COMMAND_FUNNY 3
#define COMMAND_FUNNY2 4
#define COMMAND_NONE 0

/* React on hello messages and say-commands and LOL's */
Guru *gurumod_exec(Guru *message)
{
	int command;
	static char *buf = NULL;
	int i, len;

	command = COMMAND_NONE;
	if((message->list)
	&& (message->list[0])
	&& (message->list[1])
	&& (!strcmp(message->list[0], message->guru)))
	{
		if(message->list[2])
			if(!strcmp(message->list[1], "say"))
				command = COMMAND_SAY;
		if(!message->list[2])
			if((!strcasecmp(message->list[1], "hi"))
			|| (!strcasecmp(message->list[1], "hey"))
			|| (!strcasecmp(message->list[1], "hello"))
			|| (!strcasecmp(message->list[1], "moin"))
			|| (!strcasecmp(message->list[1], "welcome"))
			|| (!strcasecmp(message->list[1], "greetings")))
				command = COMMAND_HI;
	}

	if((command != COMMAND_HI)
	&& (message->list)
	&& (message->list[0])
	&& (message->list[1]))
	{
		if(((!strcmp(message->list[0], "hi"))
		|| (!strcasecmp(message->list[1], "hey"))
		|| (!strcasecmp(message->list[0], "hello"))
		|| (!strcasecmp(message->list[0], "moin"))
		|| (!strcasecmp(message->list[0], "welcome"))
		|| (!strcasecmp(message->list[0], "greetings")))
		&& (!strcmp(message->list[1], message->guru)))
		{
			command = COMMAND_HI;
		}
	}

	if(command == COMMAND_NONE)
	{
		if((message->list) && (message->list[0]))
		{
			if((!strcasecmp(message->list[0], "lol"))
			|| (!strcasecmp(message->list[0], "rofl"))
			|| (!strcasecmp(message->list[0], "rotfl"))) command = COMMAND_FUNNY;
			if((!strcasecmp(message->list[0], "heh"))
			|| (!strcasecmp(message->list[0], "hehe"))
			|| (!strcasecmp(message->list[0], "hehehe"))) command = COMMAND_FUNNY2;
		}
	}

	if(buf)
	{
		free(buf);
		buf = NULL;
	}

	switch(command)
	{
		case COMMAND_NONE:
			return NULL;
			break;
		case COMMAND_HI:
			buf = (char*)malloc((message->player ? strlen(message->player) : 0) + 10);
			sprintf(buf, _("Hi %s"), message->player);
			message->message = buf;
			return message;
			break;
		case COMMAND_SAY:
			i = 2;
			len = 0;
			while((message->list) && (message->list[i]))
			{
				buf = (char*)realloc(buf, len + strlen(message->list[i]) + 2);
				if(!len) strcpy(buf, message->list[i]);
				else
				{
					strcat(buf, " ");
					strcat(buf, message->list[i]);
				}
				len += strlen(message->list[i]) + 1;
				i++;
			}
			message->message = buf;
			message->type = GURU_CHAT;
			return message;
			break;
		case COMMAND_FUNNY:
			message->message = _("funny eh?");
			return message;
			break;
		case COMMAND_FUNNY2:
			message->message = _("hehe");
			return message;
			break;
	}
	return NULL;
}

/* Empty init */
void gurumod_init(const char *datadir)
{
}

