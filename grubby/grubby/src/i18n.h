/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_I18N_H
#define GURU_I18N_H

#include <libintl.h>

/* Macro: use this to mark messages to be translated */
#define _(x) gettext(x)

/* Initializes the i18n subsystem with a default language */
void guru_i18n_initialize(const char *language);

/* Sets the language to the given locale code */
void guru_i18n_setlanguage(const char *language);

/* Check whether player tells his language (notify him if so) */
char *guru_i18n_check(char *player, char *message, int language);

/* Translate a message or set of messages */
char *guru_i18n_translate(char *player, char *messageset);

#endif

