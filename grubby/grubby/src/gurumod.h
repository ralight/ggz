/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MOD_H
#define GURU_MOD_H

/* Message types */
#define GURU_CHAT 1
#define GURU_PRIVMSG 2
#define GURU_ADMIN 3
#define GURU_ENTER 4
#define GURU_LEAVE 5
#define GURU_GAME 6

/* Message structure to be passes around to the plugins */
struct guru_t
{
	char *player;
	char *message;
	int type;
	char **list;
	int priority;
	char *guru;
	char *datadir;
};

typedef struct guru_t Guru;

Guru *gurumod_exec(Guru *message);
void gurumod_init(const char *datadir);

#endif
