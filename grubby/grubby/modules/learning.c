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
#include "i18n.h"

/* Learning modes */
#define mode_none  0
#define mode_teach 1
#define mode_learn 2

/* Grubby's knowledge base */
#define db_file "/grubby/learning.db"

/* Globals */
char *database;

/* Learn a word and its meaning */
static void learn(char **wordlist)
{
	FILE *f;
	int i;

	f = fopen(database, "a");
	if(!f) return;
	i = 0;
	while((wordlist) && (wordlist[i]))
	{
		if(i == 1) fprintf(f, "%s\t", wordlist[i]);
		if(i > 2) fprintf(f, "%s ", wordlist[i]);
		i++;
	}
	fprintf(f, "\n");
	fclose(f);
}

/* Teach a learned word to a player if known */
static char *teach(const char *word)
{
	FILE *f;
	static char *ret = NULL;
	char buffer[128];
	char *token;

	if(!word) return NULL;
	f = fopen(database, "r");
	if(!f) return NULL;
	while(fgets(buffer, sizeof(buffer), f))
	{
		token = strtok(buffer, "\t");
		if((token) && (!strcasecmp(token, word)))
		{
			token = strtok(NULL, "\t");
			if(token)
			{
				if(ret) free(ret);
				ret = strdup(token);
				fclose(f);
				ret[strlen(ret) - 1] = 0;
				return ret;
			}
		}
	}
	fclose(f);
	return NULL;
}

/* Get message and decide what to do with it */
Guru *gurumod_exec(Guru *message)
{
	int i, mode;
	char *ret;
	char *request;

	if((message->type != GURU_DIRECT)
	&& (message->type != GURU_PRIVMSG))
		return NULL;
	if(!message->message) return NULL;
	if(message->priority == 10) return NULL;
	
	request = NULL;
	i = 0;
	mode = mode_none;
	while((message->list) && (message->list[i]))
	{
		if((i == 2) && (!strcmp(message->list[i], "is"))) mode = mode_learn;
		if((i == 2) && (!strcmp(message->list[i], "are"))) mode = mode_learn;
		if((i == 2) && (!strcmp(message->list[i], "has"))) mode = mode_learn;
		if((i == 2) && (!strcmp(message->list[i], "have"))) mode = mode_learn;
		i++;
	}
	if(i == 2)
	{
		mode = mode_teach;
		request = message->list[1];
	}
	if(mode == mode_learn)
	{
		if((i > 2) && (!strcasecmp(message->list[1], "what")))
		{
			mode = mode_teach;
			request = message->list[3];
		}
	}

	ret = NULL;

	switch(mode)
	{
		case mode_none:
			return NULL;
			break;
		case mode_learn:
			learn(message->list);
			ret = __("OK, learned that.");
			break;
		case mode_teach:
			ret = teach(request);
			if(!ret) ret = __("You're too curious - I don't know everything.");
			break;
	}

	message->message = ret;
	return message;
}

/* Initialization */
void gurumod_init(const char *datadir)
{
	database = (char*)malloc(strlen(datadir) + strlen(db_file) + 1);
	strcpy(database, datadir);
	strcat(database, db_file);
}

