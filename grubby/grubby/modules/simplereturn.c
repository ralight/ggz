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

/* Just return any message without modification */
Guru *gurumod_exec(Guru *message)
{
	if(message->priority == 10) return NULL;
	return message;
}

/* Empty init */
void gurumod_init()
{
}

