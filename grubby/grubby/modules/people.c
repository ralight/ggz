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
grubby have you seen someone
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "player.h"
#include <time.h>

void gurumod_init()
{
}

Guru *gurumod_exec(Guru *message)
{
	int i, j;
	char *realname, *name, *contact;
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
		if(message->type == GURU_ENTER)
		{
			if(!firsttime)
			{
				switch(rand() % 3)
				{
					case 0:
						strcpy(info, "Nice to see you here again, ");
						strcat(info, message->player);
						break;
					case 1:
						strcpy(info, message->player);
						strcpy(info, ": Great you come here!");
						break;
					default:
						return NULL;
						break;
				}
			}
			else
			{
				strcpy(info, "Hi ");
				strcat(info, message->player);
				strcat(info, ", I'm ");
				strcat(info, message->guru);
				strcat(info, ". I have never seen you before here.\nType 'guru help' to change this :)");
			}
			message->message = info;
			message->type = GURU_CHAT;
			return message;
		}
		if(message->type == GURU_LEAVE)
		{
			switch(rand() % 10)
			{
				case 0:
					strcpy(info, "See you later, ");
					strcat(info, message->player);
					message->message = info;
					break;
				case 1:
					message->message = "Have a nice rest.";
					break;
				case 2:
					strcpy(info, message->player);
					strcat(info, ": Don't stay away too long.");
					message->message = info;
					break;
				case 3:
					message->message = "Eh, why has he gone?";
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
				p->firstseen = time(NULL);
				p->name = message->player;
			}
			if(realname) p->realname = realname;
			if(contact) p->contact = contact;
			guru_player_save(p);
			message->message = "OK, registered your information.";
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
printf("DEBUG: name=%s, contact=%s\n", realname, contact);
				sprintf(info, "Name: %s, Contact: %s", realname, contact);
printf("DEBUG2: %s\n", info);
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
			if(!strcmp(message->player, message->list[4]))
			{
				message->message = "I'm looking right at you now :)";
			}
			else
			{
				p = guru_player_lookup(message->list[4]);
				if(p)
				{
					t = p->lastseen;
					ts = ctime(&t);
					sprintf(info, "Yeah, he was here at %s", ts);
					message->message = info;
				}
				else message->message = "Nope, never seen this guy here.";
			}
			return message;
		}
	}
	return NULL;
}

