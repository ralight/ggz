/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>

/* Use Guru functions */
#include "i18n.h"

/* Issue a test message which is only translated once */
void messagetest()
{
	printf(_("If you read plain English here, kick your system administrator.\n"));
}

/* Main function for the test program */
int main(int argc, char *argv[])
{
	guru_i18n_initialize();

	printf("Hi, I'm Guru, the next-generation Grubby.\n");
	printf("I'd like to talk with you in German a bit.\n");

	guru_i18n_setlanguage("de");
	messagetest();

	guru_i18n_setlanguage("en");
	printf("Or want some tasty french?\n");

	guru_i18n_setlanguage("fr");
	messagetest();
	
	guru_i18n_setlanguage("en");
	printf("Now, back to work...\n");

	return 0;
}

