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
	const char *tokenizer = " ,.()?!";
	char *token;
	char *outtoken;
	int i;
	
	token = strtok(strdup(message->message), tokenizer);
	outtoken = NULL;
	i = 0;
	while((token) && (!outtoken))
	{
		if((i == 1) && (!strcasecmp(token, "about"))) outtoken = "I'm the reincarnation of Grubby!";
		if((i == 1) && (!strcasecmp(token, "help"))) outtoken = "Yeah, let me help you my friend.";
		token = strtok(NULL, tokenizer);
		i++;
	}
	while(token) token = strtok(NULL, tokenizer);
	message->message = outtoken;
	message->type = GURU_CHAT;
	return message;
}


