/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include "i18n.h"
#include "config.h"

#ifdef EMBED_RUBY
#include <ruby.h>
#endif

/* Empty init */
void gurumod_init(const char *datadir)
{
}

/* Execute embedded scripts */
Guru *gurumod_exec(Guru *message)
{
	return NULL;
}

