/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

enum Status
{
	STATUS_GUEST,
	STATUS_REGISTERED,
	STATUS_ADMIN,
	STATUS_OWNER
};

struct player_t
{
	char *name;
	char *realname;
	int firstseen, lastseen, lastactive;
	char *language;
	char *contact;
	char *publicinfo;
	int status;
	struct player_t *origin;
};

typedef struct player_t Player;

void guru_player_policy(int duplication);
Player *guru_player_lookup(const char *name);
void guru_player_save(Player *p);
void guru_player_free(Player *p);
Player *guru_player_new();

#endif

