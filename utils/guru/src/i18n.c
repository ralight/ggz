/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

/* Guru i18n functions */

#include "i18n.h"
#include <locale.h>
#include <stdlib.h>

/* Initializes the i18n subsystem */
void guru_i18n_initialize()
{
	bindtextdomain("guru", "../tmp");
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

