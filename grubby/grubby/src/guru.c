/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"
#include "module.h"

/* Initialize grubby */
Gurucore *guru_init()
{
	return guru_module_init();
}

/* Pass a message to the bot */
Guru *guru_work(Guru *message)
{
	return guru_module_work(message, 0);
}

/* Shutdown properly */
int guru_close()
{
	return 1;
}



