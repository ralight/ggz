/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_H
#define GURU_H

#include "module.h"
#include "gurumod.h"

/* Abstract overall functionality */
Gurucore *guru_init();
Guru *guru_work(Guru *message);
int guru_close();

#endif

