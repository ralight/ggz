/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2004 Josef Spillner <josef@ggzgamingzone.org>
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
#define mode_try   3

/* Grubby's knowledge base */
#define db_file "/grubby/learning.db"

/* Globals */
static char *database = NULL;
static char **knowledge = NULL;
static int knowledgecounter = 0;

/* Learn a word and its meaning */
static void learn(char **wordlist)
{
	FILE *f;
	int i, len;

	if((!wordlist) || (!wordlist[0]) || (!wordlist[1]) || (!wordlist[2])) return;

	knowledge = (char**)realloc(knowledge, (knowledgecounter + 2) * sizeof(char*));
	knowledge[knowledgecounter] = strdup(wordlist[1]);
	knowledge[knowledgecounter + 1] = strdup(wordlist[2]);
	i = 3;
	len = strlen(knowledge[knowledgecounter + 1]);
	while(wordlist[i])
	{
		len += strlen(wordlist[i]) + 1;
		knowledge[knowledgecounter + 1] = (char*)realloc(knowledge[knowledgecounter + 1], len + 1);
		strcat(knowledge[knowledgecounter + 1], " ");
		strcat(knowledge[knowledgecounter + 1], wordlist[i]);
		i++;
	}

	f = fopen(database, "a");
	if(!f) return;
	fprintf(f, "%s\t%s\n", knowledge[knowledgecounter], knowledge[knowledgecounter + 1]);
	fclose(f);

	knowledgecounter += 2;
}

/* Teach a learned word to a player if known */
static char *teach(const char *word)
{
	static char *ret = NULL;
	int i;

	for(i = 0; i < knowledgecounter; i++)
	{
		if((knowledge[i]) && (!strcmp(knowledge[i], word)))
		{
			if(ret) free(ret);
			ret = strdup(knowledge[i + 1]);
			return ret;
		}
	}
	return NULL;
}

/* Read in all the knowledge */
static void cache(void)
{
	FILE *f;
	char buffer[256];
	char *token;

	f = fopen(database, "r");
	if(!f) return;
	while(fgets(buffer, sizeof(buffer), f))
	{
		token = strtok(buffer, "\t");
		if(token)
		{
			knowledge = (char**)realloc(knowledge, (knowledgecounter + 2) * sizeof(char*));
			knowledge[knowledgecounter] = strdup(token);
			knowledge[knowledgecounter + 1] = NULL;
			token = strtok(NULL, "\t");
			if(token)
			{
				token[strlen(token) - 1] = 0;
				knowledge[knowledgecounter + 1] = strdup(token);
			}
			knowledgecounter += 2;
		}
	}
	fclose(f);
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
	if(!strcmp(message->player, "[Server]")) return NULL;

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
		mode = mode_try;
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
		case mode_try:
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

	cache();
}

