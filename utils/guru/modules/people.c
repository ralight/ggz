/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/*
grubby my name is Santa Claus
grubby who is santa
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "player.h"

void gurumod_init()
{
}

Guru *gurumod_exec(Guru *message)
{
	int i, j;
	char *realname, *name, *contact;
	Player *p;
	static char *info = NULL;

	if(!info) info = (char*)malloc(1024);
	i = 0;
	while((message->list) && (message->list[i])) i++;

	if(i > 4)
	{
		if((!strcmp(message->list[1], "my"))
		&& (!strcmp(message->list[3], "is")))
		{
			realname = NULL;
			contact = NULL;
			strcpy(info, message->list[4]);
			j = 5;
			while(message->list[j])
			{
				strcat(info, " ");
				strcat(info, message->list[j]);
				j++;
			}
			if(!strcmp(message->list[2], "name")) realname = info;
			if(!strcmp(message->list[2], "email")) contact = info;
			if(!strcmp(message->list[2], "pager")) contact = info;
			p = guru_player_lookup(message->player);
			if(!p)
			{
				p = guru_player_new();
				p->firstseen = 1; /* now() */
				p->name = message->player;
			}
			if(realname) p->realname = realname;
			if(contact) p->contact = contact;
			guru_player_save(p);
			message->message = "OK, registered your information";
			message->type = GURU_PRIVMSG;
			return message;
		}
	}
	if(i == 4)
	{
		if((!strcmp(message->list[1], "who"))
		&& (!strcmp(message->list[2], "is")))
		{
			name = message->list[3];
			p = guru_player_lookup(name);
			if(p)
			{
				realname = (p->realname ? p->realname : "unknown");
				contact = (p->contact ? p->contact : "unknown");
				sprintf(info, "Name: %s, Contact: %s", realname, contact);
				message->message = info;
			}
			else message->message = "Sorry, I don't know who this is.";
			return message;
		}
	}
	if(i == 5)
	{
		if((!strcmp(message->list[1], "have"))
		&& (!strcmp(message->list[2], "you"))
		&& (!strcmp(message->list[3], "seen")))
		{
			p = guru_player_lookup(message->list[4]);
			if(p)
			{
				sprintf(info, "Yeah, he was here at %i", p->lastseen);
				message->message = info;
			}
			else message->message = "Nope, never seen this guy here.";
			return message;
		}
	}
	return NULL;
}

