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

/* Participate in any programming discussion */
Guru *gurumod_exec(Guru *message)
{
	int i;
	char *token, *outtoken;

	i = 0;
	outtoken = NULL;
	while((message->list) && (message->list[i]))
	{
		token = message->list[i];
		if(!strcasecmp(token, "java"))
			outtoken = __("Java? You don't know _my_ skills!");
		if(!strcasecmp(token, "basic"))
			outtoken = __("I though Basic is for beginners only. You must be childish.");
		if(!strcasecmp(token, "c"))
			outtoken = __("Don't talk about C. It's so crappy, nobody used it. .NET rulez.");
		if(!strcasecmp(token, "pascal"))
			outtoken = __("Oh, I've learned that at school. However, it sucks when compared to Prolog.");
		if(!strcasecmp(token, "perl"))
			outtoken = __("Yeah, perl. I hate write-only languages.");

		if(outtoken)
		{
			message->message = outtoken;
			return message;
		}
		i++;
	}
	return NULL;
}


