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
			message->message = "I'm the reincarnation of Grubby!";
			message->type = GURU_PRIVMSG;
			return message;
		}
		if((i == 1) && (!strcasecmp(message->list[i], "help")))
		{
			message->message = "Yeah, let me help you my friend.";
			message->type = GURU_PRIVMSG;
			return message;
		}
		i++;
	}
	return NULL;
}


