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
	
	token = strtok(strdup(message->message), tokenizer);
	outtoken = NULL;
	while((token) && (!outtoken))
	{
		if(!strcasecmp(token, "java")) outtoken = "Java? You don't know _my_ skills!";
		if(!strcasecmp(token, "basic")) outtoken = "I though Basic is for beginners only. You must be childish.";
		if(!strcasecmp(token, "c")) outtoken = "Don't talk about C. It's so crappy, nobody used it. .NET rulez.";
		if(!strcasecmp(token, "pascal")) outtoken = "Oh, I've learned that at school. However, it sucks when compared to Prolog.";
		token = strtok(NULL, tokenizer);
	}
	while(token) token = strtok(NULL, tokenizer);
	if(outtoken) sleep(strlen(outtoken) / 30);
	message->message = outtoken;
	message->type = GURU_CHAT;
	return message;
}


