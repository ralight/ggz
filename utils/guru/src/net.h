/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef NET_H
#define NET_H

#include "module.h"

enum States
{
	NET_NOOP,
	NET_LOGIN,
	NET_ERROR,
	NET_GOTREADY,
	NET_INPUT
};

void net_connect(const char *host, int port, const char *name, const char *guestname, const char *logfile);
void net_join(int room);
int net_status();
Guru *net_input();
void net_output(Guru *output);

#endif

