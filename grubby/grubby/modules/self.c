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

/* Empty init */
void gurumod_init(const char *datadir)
{
}

/* Grubby's non-visual about dialog */
Guru *gurumod_exec(Guru *message)
{
	int i, active;
	char buffer[1024];
	
	i = 0;
	active = 0;
	while((message->list) && (message->list[i]))
	{
		/* Make sure people talk with grubby */
		if((i == 0) && (!strcasecmp(message->list[i], message->guru)))
		{
			active = 1;
		}

		/* Let grubby tell about himself */
		if((i == 1) && (!strcasecmp(message->list[i], "about")) && (active))
		{
			snprintf(buffer, 1024, _("I'm %s, your favorite chat bot!\n"
					"I'm here to answer your question, and learn more about you.\n"
					"You may type '%s help ' to get to know what I understand.\n"
					"Have fun :-)"), message->guru, message->guru);
			message->message = buffer;
			message->type = GURU_PRIVMSG;
			return message;
		}

		/* Show all available commands, independent of active plugins */
		if((i == 1) && (!strcasecmp(message->list[i], "help")) && (active))
		{
			message->message = _("This is the list of public commands I understand:\n"
					"about: let me tell about myself\n"
					"do i have any messages: let me look up if there's news for you\n"
					"tell <nick> <message>: store message for player nick\n"
					"who is <nick>: request information about nick\n"
					"have you seen <nick>: ask me when I last saw nick\n"
					"my (email | pager) is <contact>: register contact information\n"
					"<nick> is from <countrycode>: register language\n"
					"my name is <fullname>: register you as fullname");
			message->type = GURU_PRIVMSG;
			return message;
		}
		i++;
	}

	return NULL;
}

