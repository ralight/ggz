/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MOD_H
#define GURU_MOD_H

#define GURU_CHAT 1
#define GURU_PRIVMSG 2
#define GURU_ADMIN 3

struct guru_t
{
	char *player;
	char *message;
	int type;
	char **list;
	int priority;
};

typedef struct guru_t Guru;

#endif

