/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2003 Josef Spillner, <josef@ggzgamingzone.org>
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

static void game(void)
{
	printf("****\n");
}

/* Grubby is a gaming bot */
Guru *gurumod_exec(Guru *message)
{
	int i, max;
	char buffer[1024];
	
	i = 0;
	while((message->list) && (message->list[i])) i++;
	max = i;

	if(max == 4)
	{
		/* Joining a game */
		if((!strcasecmp(message->list[0], message->guru))
		&& (!strcasecmp(message->list[1], "join"))
		&& (!strcasecmp(message->list[2], "my"))
		&& (!strcasecmp(message->list[3], "game")))
		{
			game();

			snprintf(buffer, sizeof(buffer), _("Sure let's play..."));
			message->message = buffer;
			message->type = GURU_GAME;
			return message;
		}
	}

	return NULL;
}

