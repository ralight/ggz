/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef NET_H
#define NET_H

#include "module.h"

/* Available generic states of the net module */
enum States
{
	NET_NOOP,
	NET_LOGIN,
	NET_ERROR,
	NET_GOTREADY,
	NET_INPUT
};

/* Interface */
void net_connect(const char *host, int port, const char *name, const char *guestname);
void net_join(const char *room);
int net_status(void);
Guru *net_input(void);
void net_output(Guru *output);
void net_logfile(const char *logfile);

#endif

