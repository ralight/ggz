/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MODULE_H
#define GURU_MODULE_H

#include "gurumod.h"

/* Net function handlers */
typedef void (*netconnectfunc)(const char *host, int port, const char *name, const char *guestname);
typedef void (*netjoinfunc)(const char *room);
typedef int (*netstatusfunc)(void);
typedef Guru* (*netinputfunc)(void);
typedef void (*netoutputfunc)(Guru *output);
typedef void (*netlogfunc)(const char *logfile);

/* Player function handlers */
typedef void (*playerinitfunc)(const char *datadir);

/* I18n function handlers */
typedef void (*i18ninitfunc)(const char *lang);
typedef char* (*i18ntransfunc)(char *player, char *messageset);
typedef char* (*i18ncheckfunc)(char *player, char *message, int language);

/* Grubby's structure */
struct gurucore_t
{
	char *host;
	char *owner;
	char *name;
	char *language;
	char *guestname;
	char *datadir;
	char *logfile;
	void *nethandle;
	void *i18nhandle;
	void *playerhandle;
	char *autojoin;
	netconnectfunc net_connect;
	netjoinfunc net_join;
	netstatusfunc net_status;
	netinputfunc net_input;
	netoutputfunc net_output;
	netlogfunc net_log;
	i18ninitfunc i18n_init;
	i18ntransfunc i18n_translate;
	i18ncheckfunc i18n_check;
};

typedef struct gurucore_t Gurucore;

/* Module/plugin functions */
Gurucore *guru_module_init(const char *datadir);
int guru_module_shutdown(Gurucore *guru);
int guru_module_add(const char *modulealias);
int guru_module_remove(const char *modulealias);
Guru *guru_module_work(Guru *message, int priority);

#endif

