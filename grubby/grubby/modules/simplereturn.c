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

/* Just return any message without modification */
Guru *gurumod_exec(Guru *message)
{
	if(message->priority == 10) return NULL;

	if((message->type == GURU_DIRECT)
	|| (message->type == GURU_PRIVMSG))
		return message;

	return NULL;
}

/* Empty init */
void gurumod_init(const char *datadir)
{
}

