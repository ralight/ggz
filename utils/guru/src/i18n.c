/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/* Guru i18n functions */

#include "i18n.h"
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Initializes the i18n subsystem */
void guru_i18n_initialize()
{
	bindtextdomain("guru", "/usr/local/share/locale");
	textdomain("guru");
	setlocale(LC_ALL, "");

	guru_i18n_setlanguage("de"); /* FIXME: get player info */
}

/* Sets the language to the given locale code */
void guru_i18n_setlanguage(const char *language)
{
	extern int _nl_msg_cat_cntr;
	
	setenv("LANGUAGE", language, 1);
	++_nl_msg_cat_cntr;
}

/* Translate a message or set of messages */
char *guru_i18n_translate(char *messageset)
{
	char *token;
	char *message;
	static char *ret = NULL;
	int i;
	char *dup;

	if(!messageset) return NULL;
	if(ret)
	{
		free(ret);
		ret = NULL;
	}

printf("STRTOK: '%s'\n", messageset);
	dup = strdup(messageset);
	messageset = dup;
	token = strtok(messageset, "\n");
printf("STRTOK DONE.\n");
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

