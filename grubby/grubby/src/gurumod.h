/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2004 Josef Spillner, <josef@ggzgamingzone.org>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GURU_MOD_H
#define GURU_MOD_H

/* Message types */
#define GURU_CHAT 1
#define GURU_DIRECT 2
#define GURU_PRIVMSG 3
#define GURU_ADMIN 4
#define GURU_ENTER 5
#define GURU_LEAVE 6
#define GURU_GAME 7
#define GURU_TICK 8

/* Player types */
#define PLAYER_UNKNOWN 0
#define PLAYER_GUEST 1
#define PLAYER_REGISTERED 2
#define PLAYER_HOST 3
#define PLAYER_ADMIN 4

/* Message structure to be passes around to the plugins */
struct guru_t
{
	char *player;
	int playertype;
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
