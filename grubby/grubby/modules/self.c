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

#include <ggz.h>

#include "gurumod.h"
#include "i18n.h"

#define GRUBBYCONF "/grubby.rc"

#define can(x) strlen(x) < sizeof(x) - 128

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
	char buffer[2048];
	int minutes;
	int ret;
	char *datadir, *path, *home;
	int handler;
	int count;
	char **list;

	i = 0;

	/* Make sure people talk with grubby */
	if(message->type == GURU_DIRECT) active = 1;
	else if(message->type == GURU_PRIVMSG) active = 1;
	else active = 0;

	while((message->list) && (message->list[i]))
	{
		/* Let grubby tell about himself */
		if((i == 1) && (!strcasecmp(message->list[i], "about")) && (active))
		{
			snprintf(buffer, sizeof(buffer), __("I'm %s, your favorite chat bot!\n"
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
			sprintf(buffer, __("This is the list of public commands I understand:\n"));

			/* Find out grubby's data directory first */
			home = getenv("HOME");
			datadir = (char*)ggz_malloc(strlen(home) + 10);
			strcpy(datadir, home);
			strcat(datadir, "/.ggz");

			/* Open configuration file */
			path = (char*)ggz_malloc(strlen(datadir) + strlen(GRUBBYCONF) + 1);
			strcpy(path, datadir);
			strcat(path, GRUBBYCONF);
			handler = ggz_conf_parse(path, GGZ_CONF_RDONLY);

			ret = ggz_conf_read_list(handler, "guru", "modules", &count, &list);
			if(ret >= 0)
			{
				for(i = 0; i < count; i++)
				{
					if(!strcmp(list[i], "self"))
					{
						if(can(buffer)) strcat(buffer, __("about: let me tell about myself\n"));
						if(can(buffer)) strcat(buffer, __("uptime: disclose my age\n"));
					}
					else if(!strcmp(list[i], "extra"))
					{
						if(can(buffer)) strcat(buffer, __("shut up: make me quiet\n"));
					}
					else if(!strcmp(list[i], "exec"))
					{
						if(can(buffer)) strcat(buffer,
							__("do i have any messages: let me look up if there's news for you\n"));
						if(can(buffer)) strcat(buffer,
							__("tell <nick> <message>: store message for player nick\n"));
					}
					else if(!strcmp(list[i], "people"))
					{
						if(can(buffer)) strcat(buffer,
							__("who is <nick>: request information about nick\n"));
						if(can(buffer)) strcat(buffer,
							__("have you seen <nick>: ask me when I last saw nick\n"));
						if(can(buffer)) strcat(buffer,
							__("my (email | pager) is <contact>: register contact information\n"));
						if(can(buffer)) strcat(buffer,
							__("my name is <fullname>: register you as fullname\n"));
					}
					else if(!strcmp(list[i], "game"))
					{
						if(can(buffer)) strcat(buffer, __("join my game: make me join your table\n"));
					}
					else if(!strcmp(list[i], "learning"))
					{
						if(can(buffer)) strcat(buffer, __("<thing> is <explanation>: Learn a fact\n"));
						if(can(buffer)) strcat(buffer, __("what is <thing>: Get information about a fact\n"));
					}
				}
			}

			/* i18n is always assumed to be present */
			if(can(buffer)) strcat(buffer, __("i am from <countrycode>: register language\n"));

			ggz_conf_close(handler);
			ggz_free(path);
			ggz_free(datadir);

			message->message = buffer;
			message->type = GURU_PRIVMSG;
			return message;
		}

		/* How long has grubby been running? */
		if((i == 1) && (!strcasecmp(message->list[i], "uptime")) && (active))
		{
			minutes = process_uptime();

			if(minutes < 0)
				snprintf(buffer, sizeof(buffer), __("No uptime information found."));
			else
				snprintf(buffer, sizeof(buffer),
					__("I've been running for %i hours and %i minutes."),
					minutes / 60, minutes % 60);
			message->message = buffer;
			message->type = GURU_PRIVMSG;
			return message;
		}

		i++;
	}

	return NULL;
}

