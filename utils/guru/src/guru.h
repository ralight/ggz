/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_H
#define GURU_H

#include "module.h"
#include "gurumod.h"

Gurucore *guru_init();
Guru *guru_work(Guru *message);
int guru_close();

#endif

