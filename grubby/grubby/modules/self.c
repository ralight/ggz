/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2003 Josef Spillner, <josef@ggzgamingzone.org>
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

static int process_uptime()
{
	float uptime;
	int minutes;
	int jiffies;
	unsigned int tmp1;
	int tmp2;
	char tmp3;
	char tmp4[256];
	FILE *f;
	char buffer[1024];

	minutes = -1;
	uptime = -1.0;
	jiffies = -1;

	f = fopen("/proc/uptime", "r");
	/*printf("Check uptime: %p\n", f);*/
	if(f)
	{
		fgets(buffer, sizeof(buffer), f);
		sscanf(buffer, "%f", &uptime);
		/*printf("Uptime is: %3.1f seconds\n", uptime);*/
		fclose(f);
	}

	f = fopen("/proc/self/stat", "r");
	/*printf("Check process time: %p\n", f);*/
	if(f)
	{
		fgets(buffer, sizeof(buffer), f);
		sscanf(buffer, "%d %s %c %d %d %d %d %d "
			"%u %u %u %u %u %d %d %d %d %d %d %u %u %d",
			&tmp2, tmp4, &tmp3, &tmp2, &tmp2, &tmp2, &tmp2, &tmp2,
			&tmp1, &tmp1, &tmp1, &tmp1, &tmp1, &tmp2, &tmp2, &tmp2, &tmp2,
			&tmp2, &tmp2, &tmp1, &tmp1,
			&jiffies);
		/*printf("Start time is: %i jiffies\n", jiffies);*/
		fclose(f);
	}

	if((uptime > 0.0) && (jiffies > 0))
	{
		minutes = (int)((uptime - ((float)jiffies / 100)) / 60);
	}

	return minutes;
}

/* Grubby's non-visual about dialog */
Guru *gurumod_exec(Guru *message)
{
	int i, active;
	char buffer[1024];
	int minutes;

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
			snprintf(buffer, sizeof(buffer), _("I'm %s, your favorite chat bot!\n"
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

		/* How long has grubby been running? */
		if((i == 1) && (!strcasecmp(message->list[i], "uptime")) && (active))
		{
			minutes = process_uptime();

			if(minutes < 0)
				snprintf(buffer, sizeof(buffer), _("No uptime information found."));
			else
				snprintf(buffer, sizeof(buffer),
					_("I've been running for %i hours and %i minutes."),
					minutes / 60, minutes % 60);
			message->message = buffer;
			message->type = GURU_PRIVMSG;
			return message;
		}

		i++;
	}

	return NULL;
}

