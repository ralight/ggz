/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "i18n.h"

void gurumod_init()
{
}

Guru *gurumod_exec(Guru *message)
{
	int i;
	
	i = 0;
	while((message->list) && (message->list[i]))
	{
		if((i == 1) && (!strcasecmp(message->list[i], "about")))
		{
			message->message = _("I'm Grubby, your favorite chat bot!\n"
					"I'm here to answer your question, and learn more about you.\n"
					"You may type 'grubby help' to get to know what I understand.\n"
					"Have fun :-)");
			message->type = GURU_PRIVMSG;
			return message;
		}
		if((i == 1) && (!strcasecmp(message->list[i], "help")))
		{
			message->message = _("This is the list of public commands I understand:\n"
					"about: let me tell about myself\n"
					"do i have any messages: let me look up if there's news for you\n"
					"tell <nick> <message>: store message for player nick\n"
					"who is <nick>: request information about nick\n"
					"have you seen <nick>: ask me when I last saw nick\n"
					"my (email | pager) is <contact>: register contact information\n"
					"my name is <fullname>: register you as fullname");
			message->type = GURU_PRIVMSG;
			return message;
		}
		i++;
	}
	return NULL;
}


