/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/* Guru i18n functions */

#ifndef GURU_I18N_H
#define GURU_I18N_H

#include <libintl.h>

/* Macro: use this to mark messages to be translated */
#define _(x) gettext(x)

/* Initializes the i18n subsystem */
void guru_i18n_initialize();

/* Sets the language to the given locale code */
void guru_i18n_setlanguage(const char *language);

#endif

