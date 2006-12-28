/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2004 Josef Spillner, <josef@ggzgamingzone.org>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "i18n.h"
#include <ggz.h>

/* Configuration file for bad words */
#define BADWORDCONF "/.ggz/grubby/modbadword.rc"

/* Plugin-global list of bad words */
char **badwordlist;

/* Load all bad words from the configuration */
void gurumod_init(const char *datadir)
{
	char *path, *home;
	int handle;
	int ret, i;
	int count;

	home = getenv("HOME");
	path = (char*)ggz_malloc(strlen(home) + strlen(BADWORDCONF) + 1);
	strcpy(path, home);
	strcat(path, BADWORDCONF);
	handle = ggz_conf_parse(path, GGZ_CONF_RDONLY);
	ggz_free(path);
	if(handle < 0) return;
	ret = ggz_conf_read_list(handle, "badwords", "badwords", &count, &badwordlist);

	printf("[ ");
	for(i = 0; i < count; i++)
		printf("|");
	printf(" ] ");
	if(ret < 0) badwordlist = NULL;
}

/* Check whether a message contains such a bad word */
Guru *gurumod_exec(Guru *message)
{
	int i, j;
	enum Types {typenone, typefromgrubby, typetogrubby};
	int type;
	static char *buf = NULL;
	int bad, you;

	if(!message->message) return NULL;

	type = typenone;
	if(message->type == GURU_PRIVMSG) type = typetogrubby;
	else if(message->type == GURU_DIRECT) type = typetogrubby;
	else if((message->list) && (message->list[0]))
	{
		if((message->player) && (!strcmp(message->guru, message->player))) type = typefromgrubby;
	}
	if(type == typefromgrubby) return NULL;

	bad = 0;
	you = 0;
	j = 0;
	while((message->list) && (message->list[j]))
	{
		i = 0;
		while((badwordlist) && (badwordlist[i]))
		{
			if(!strcasecmp(message->list[j], badwordlist[i])) bad = 1;
			i++;
		}
		if(!strcasecmp(message->list[j], "you")) you = 1;
		j++;
	}
	if(!bad)
	{
		i = 0;
		while((badwordlist) && (badwordlist[i]))
		{
			if(strstr(message->message, badwordlist[i])) bad = 1;
			i++;
		}
	}

	if(bad)
	{
		if(type == typetogrubby)
		{
			if(!you)
			{
				if(!buf) buf = (char*)ggz_malloc(strlen(message->player) + 1024);
				strcpy(buf, message->player);
				strcat(buf, __(" is a dirty evil bastard."));
				message->message = buf;
			}
			else message->message = __("You aren't any better, bugger.");
			return message;
		}
		else
		{
			message->message = __("Watch your language please!");
			return message;
		}
	}

	return NULL;
}

