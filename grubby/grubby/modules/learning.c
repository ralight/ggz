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

/* Learning modes */
#define mode_none  0
#define mode_teach 1
#define mode_learn 2
/* Grubby's knowledge base */
#define db_file "/.ggz/grubby/learning.db"

/* Globals */
char *database;

/* Learn a word and its meaning */
void learn(char **wordlist)
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
char *teach(const char *word)
{
	FILE *f;
	static char *ret = NULL;
	char buffer[128];
	char *token;

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

	i = 0;
	mode = mode_none;
	printf("DEBUG: learning\n");
	while((message->list) && (message->list[i]))
	{
		if((i == 0) && (strcmp(message->list[0], message->guru)))
		{
			printf("DEBUG: %s != %s\n%s\n", message->list[0], message->guru, message->message);
			return NULL;
		}
		if((i == 2) && (!strcmp(message->list[i], "is"))) mode = mode_learn;
		i++;
	}
	if(i == 2) mode = mode_teach;

	/* INSANE MEMORY PROBLEM!!! */
	message->message = strdup(message->message);
	ret = NULL;

	switch(mode)
	{
		case mode_none:
			return NULL;
			break;
		case mode_learn:
			printf("DEBUG: LEARN STUFF\n");
			learn(message->list);
			ret = "OK, learned that.";
			break;
		case mode_teach:
			printf("DEBUG: Teach about STUFF\n");
			ret = teach(message->list[1]);
			if(!ret) ret = "You're too curious - I don't know everything.";
			break;
	}

	message->message = ret;
	return message;
}

/* Initialization */
void gurumod_init()
{
	char *home;

	home = getenv("HOME");
	database = (char*)malloc(strlen(home) + strlen(db_file) + 1);
	strcpy(database, home);
	strcat(database, db_file);
}

