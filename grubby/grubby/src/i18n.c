/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "i18n.h"
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "player.h"

/* Initializes the i18n subsystem */
void guru_i18n_initialize()
{
	bindtextdomain("guru", PREFIX "/share/locale");
	textdomain("guru");
	setlocale(LC_ALL, "");
}

/* Sets the language to the given locale code */
void guru_i18n_setlanguage(const char *language)
{
	extern int _nl_msg_cat_cntr;
	
	setenv("LANGUAGE", language, 1);
	++_nl_msg_cat_cntr;
}

/* Saves the language dependend on the player's origin */
void setlanguage(char *player, char *language)
{
	Player *p;

	p = guru_player_lookup(player);
	if(p)
	{
		printf("||||| %s is from %s |||||\n", player, language);
		p->language = language;
		guru_player_save(p);
	}
}

/* Check whether player says his language */
void guru_i18n_check(char *player, char *message)
{
	char *token;
	int i, c;

	if(!message) return;
	message = strdup(message);
	token = strtok(message, " .,:");
	i = 0;
	c = 0;
	while(token)
	{
		if((i == 1) && (!strcasecmp(token, "i"))) c++;
		if((i == 2) && (!strcasecmp(token, "am"))) c++;
		if((i == 3) && (!strcasecmp(token, "from"))) c++;
		if((i == 4) && (c == 3)) setlanguage(player, token);
		i++;
		token = strtok(NULL, " .,:");
	}
	free(message);
}

/* Translate a message or set of messages */
/* FIXME: memory issues (as always) and better interface needed */
char *guru_i18n_translate(char *player, char *messageset)
{
	char *token;
	char *message;
	static char *ret = NULL;
	int i;
	char *dup;
	Player *p;

	if(!messageset) return NULL;
	messageset = strdup(messageset); /* FIXME: eeek! this bug is not normal! */

printf("--trans1: %s, %s\n", player, messageset);
	if(player)
	{
		p = guru_player_lookup(player);
		if((!p) || (!p->language))
		{
			return messageset;
		}
		else guru_i18n_setlanguage(p->language);
	}
printf("--trans2: %s, %s\n", player, messageset);

	if(ret)
	{
		free(ret);
		ret = NULL;
	}

	message = _(messageset);
printf("MESSAGE: %s\n", message);
printf("MESSAGESET: %s\n", messageset);
	if(strcmp(message, messageset)) return strdup(message); /* FIXME: another leak */
	
	dup = strdup(messageset);
	messageset = dup;
	token = strtok(messageset, "\n");
	i = 0;
	while(token)
	{
		printf("Lookup: %s\n", token);
		message = _(token);
		printf("* translating \"%s\" to \"%s\"\n", token, message);
		ret = (char*)realloc(ret, (ret ? strlen(ret) : 0) + strlen(message) + (ret ? 2 : 1));
		if(!i) strcpy(ret, message);
		else
		{
			strcat(ret, "\n");
			strcat(ret, message);
		}
		token = strtok(NULL, "\n");
		i++;
	}
	free(dup);

	return ret;
}

