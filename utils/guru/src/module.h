/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MODULE_H
#define GURU_MODULE_H

int guru_module_init();
int guru_module_add(const char *modulename);
int guru_module_remove(const char *modulename);
char *guru_module_work(const char *message, int priority);

#endif

