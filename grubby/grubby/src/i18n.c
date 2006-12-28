/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ggz.h>

#include "config.h"
#include "player.h"
#include "i18n.h"

/* Global variables */
static char *stdlang = NULL;
static char *systemstdlang = NULL;

/* Initializes the i18n subsystem */
void guru_i18n_initialize(const char *language, const char *systemlanguage)
{
	if(language) stdlang = ggz_strdup(language);
	if(systemlanguage) systemstdlang = ggz_strdup(systemlanguage);
}

/* Frees memory allocated for i18n */
void guru_i18n_cleanup(void)
{
	if(stdlang){
		ggz_free(stdlang);
		stdlang = NULL;
	}
	if(systemstdlang){
		ggz_free(systemstdlang);
		systemstdlang = NULL;
	}
}

/* Sets the language to the given locale code */
void guru_i18n_setlanguage(const char *language)
{
	extern int _nl_msg_cat_cntr;

	if(!language) return;

	setenv("LANGUAGE", language, 1);
	++_nl_msg_cat_cntr;
}

/* Toggles the translation catalogue */
void guru_i18n_setcatalog(int guru)
{
	if(guru)
	{
		bindtextdomain("guru", PREFIX "/share/locale");
		textdomain("guru");
		guru_i18n_setlanguage(stdlang);
	}
	else
	{
		bindtextdomain("grubby", PREFIX "/share/locale");
		textdomain("grubby");
		guru_i18n_setlanguage(systemstdlang);
	}
}

/* Saves the language dependend on the player's origin */
static void setlanguage(char *player, char *language)
{
	Player *p;

	p = guru_player_lookup(player);
	if(!p)
	{
		p = guru_player_new();
		p->name = player;
		p->firstseen = time(NULL);
	}

	p->language = language;
	guru_player_save(p);
}

/* Check whether player says his language */
char *guru_i18n_check(char *player, char *message, int language)
{
	char *token, *msg;
	int i, c;
	char *ret;
	Player *p;

	if(!message) return NULL;

	ret = NULL;
	msg = ggz_strdup(message);
	token = strtok(msg, " .,:");
	i = 0;
	c = 0;
	while(token)
	{
		if((i == 1) && (!strcasecmp(token, "i"))) c++;
		if((i == 2) && (!strcasecmp(token, "am"))) c++;
		if((i == 3) && (!strcasecmp(token, "from"))) c++;
		if((i == 4) && (c == 3))
		{
			setlanguage(player, token);
			guru_i18n_setlanguage(token);
			ret = __("Your language has been registered.");
		}
		i++;
		token = strtok(NULL, " .,:");
	}
	ggz_free(msg);

	if(!ret)
	{
		if(language)
		{
			p = guru_player_lookup(player);
			if((p) && (p->language)){
				guru_i18n_setlanguage(p->language);
				if(p){
					guru_player_free(p);
				}
			}else{
				guru_i18n_setlanguage(stdlang);
			}
		}
		else guru_i18n_setlanguage(stdlang);
	}

	return ret;
}

/* Translate a message or set of messages */
/* FIXME: memory issues (as always) and better interface needed */
char *guru_i18n_translate(char *player, char *messageset)
{
	char *token, *tmptoken;
	char *message;
	static char *ret = NULL;
	int i;
	char *dup;
	Player *p;

	if(!messageset) return NULL;
	messageset = ggz_strdup(messageset); /* FIXME: eeek! this bug is not normal! */

	/*printf("--trans1: %s, %s\n", player, messageset);*/
	if(player)
	{
		p = guru_player_lookup(player);
		if((!p) || (!p->language))
		{
			return messageset;
		}
		else guru_i18n_setlanguage(p->language);
	}
	/*printf("--trans2: %s, %s\n", player, messageset);*/

	if(ret)
	{
		ggz_free(ret);
		ret = NULL;
	}

	guru_i18n_setcatalog(1);

	message = _(messageset);

	/*printf("MESSAGE: %s\n", message);
	printf("MESSAGESET: %s\n", messageset);*/
	if(strcmp(message, messageset))
	{
		guru_i18n_setcatalog(0);
		return ggz_strdup(message); /* FIXME: another leak */
	}
	
	dup = ggz_strdup(messageset);
	messageset = dup;
	token = strtok(messageset, "\n");
	i = 0;
	while(token)
	{
		/*printf("Lookup: %s\n", token);*/

		tmptoken = (char*)ggz_malloc(strlen(token) + 2);
		strcpy(tmptoken, token);
		strcat(tmptoken, "\n");

		message = _(tmptoken);

		ggz_free(tmptoken);

		/*printf("* translating \"%s\" to \"%s\"\n", token, message);*/
		ret = (char*)ggz_realloc(ret, (ret ? strlen(ret) : 0) + strlen(message) + (ret ? 2 : 1));
		if(!i) strcpy(ret, message);
		else
		{
			strcat(ret, "\n");
			strcat(ret, message);
		}
		token = strtok(NULL, "\n");
		i++;
	}
	ggz_free(dup);

	guru_i18n_setcatalog(0);

	return ret;
}

