/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/*
grubby my name is Santa Claus
grubby who is santa
grubby have you seen someone
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "player.h"
#include "i18n.h"
#include <time.h>

/* Dummy init function */
void gurumod_init(const char *datadir)
{

}

/* Handle all requests related to known people */
Guru *gurumod_exec(Guru *message)
{
	int i, j;
	char *realname, *name, *contact, *language;
	Player *p;
	static char *info = NULL;
	int firsttime;
	time_t t;
	char *ts;

	if(!info) info = (char*)malloc(1024);

	srand(time(NULL));

	p = guru_player_lookup(message->player);
	firsttime = 0;
	if(!p)
	{
		firsttime = 1;
		p = guru_player_new();
		p->name = message->player;
		p->firstseen = time(NULL);
	}
	p->lastseen = time(NULL);
	guru_player_save(p);

	if(!message->message)
	{
		/* Send random greeting to joining players */
		if(message->type == GURU_ENTER)
		{
			if(!firsttime)
			{
				switch(rand() % 9)
				{
					case 0:
						strcpy(info, __("Nice to see you here again, "));
						strcat(info, message->player);
						break;
					case 1:
						strcpy(info, message->player);
						strcat(info, __(": Great you come here!"));
						break;
					default:
						return NULL;
						break;
				}
			}
			else
			{
				strcpy(info, __("Hi "));
				strcat(info, message->player);
				strcat(info, __(", I'm "));
				strcat(info, message->guru);
				strcat(info, __(". I have never seen you before here.\nType '"));
				strcat(info, message->guru);
				strcat(info, __(" help' to change this :)"));
			}
			message->message = info;
			message->type = GURU_CHAT;
			return message;
		}
		if(message->type == GURU_LEAVE)
		{
			/* Send best wishes to leaving players */
			switch(rand() % 30)
			{
				case 0:
					strcpy(info, __("See you later, "));
					strcat(info, message->player);
					message->message = info;
					break;
				case 1:
					message->message = __("Have a nice rest.");
					break;
				case 2:
					strcpy(info, message->player);
					strcat(info, __(": Don't stay away too long."));
					message->message = info;
					break;
				case 3:
					message->message = __("Eh, why has he gone?");
					break;
				default:
					message->message = NULL;
			}
			message->type = GURU_CHAT;
			return message;
		}
	}

	i = 0;
	while((message->list) && (message->list[i])) i++;

	if((message->type != GURU_DIRECT)
	&& (message->type != GURU_PRIVMSG))
		return NULL;

	if(i > 4)
	{
		/* Register information about a player */
		if((!strcmp(message->list[1], "my"))
		&& (!strcmp(message->list[3], "is")))
		{
			realname = NULL;
			contact = NULL;
			strcpy(info, message->list[4]);
			j = 5;
			while((message->list[j]) && (j < 15))
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
				p->firstseen = time(NULL);
				p->name = message->player;
			}
			if(realname) p->realname = realname;
			if(contact) p->contact = contact;
			guru_player_save(p);
			message->message = __("OK, registered your information.");
			message->type = GURU_PRIVMSG;
			return message;
		}
	}
	if(i == 4)
	{
		/* Show information about a player */
		if((!strcmp(message->list[1], "who"))
		&& (!strcmp(message->list[2], "is")))
		{
			name = message->list[3];
			p = guru_player_lookup(name);
			if(p)
			{
				realname = (p->realname ? p->realname : "unknown");
				contact = (p->contact ? p->contact : "unknown");
				language = (p->language ? p->language : "unknown");
				sprintf(info, "%s: %s, %s: %s, %s: %s",
					__("Name"), realname, __("Contact"), contact, __("Language"), language);
				message->message = info;
			}
			else message->message = __("Sorry, I don't know who this is.");
			return message;
		}
	}
	if(i == 5)
	{
		/* Lookup the date when this player was seen the last time */
		if((!strcmp(message->list[1], "have"))
		&& (!strcmp(message->list[2], "you"))
		&& (!strcmp(message->list[3], "seen")))
		{
			if(!strcmp(message->player, message->list[4]))
			{
				message->message = __("I'm looking right at you now :)");
			}
			else
			{
				p = guru_player_lookup(message->list[4]);
				if(p)
				{
					t = p->lastseen;
					ts = ctime(&t);
					sprintf(info, __("Yeah, he was here at %s"), ts);
					message->message = info;
				}
				else message->message = __("Nope, never seen this guy here.");
			}
			return message;
		}
	}
	return NULL;
}

