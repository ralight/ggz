/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MODULE_H
#define GURU_MODULE_H

#include "gurumod.h"

typedef void (*netconnectfunc)(const char *host, int port, const char *name);
typedef void (*netjoinfunc)(int room);
typedef int (*netstatusfunc)();
typedef Guru* (*netinputfunc)();
typedef void (*netoutputfunc)(Guru *output);

struct gurucore_t
{
	char *host;
	char *owner;
	char *name;
	void *nethandle;
	int autojoin;
	netconnectfunc net_connect;
	netjoinfunc net_join;
	netstatusfunc net_status;
	netinputfunc net_input;
	netoutputfunc net_output;
};

typedef struct gurucore_t Gurucore;

Gurucore *guru_module_init();
int guru_module_add(const char *modulealias);
int guru_module_remove(const char *modulealias);
Guru *guru_module_work(Guru *message, int priority);

#endif

