/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"
#include "module.h"

int guru_init()
{
	guru_module_init();
	return 1;
}

char *guru_work(const char *message)
{
	return guru_module_work(message, 0);
}

int guru_close()
{
	return 1;
}



