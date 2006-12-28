/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

/* Type of the player */
enum Playertypes
{
	STATUS_GUEST,
	STATUS_REGISTERED,
	STATUS_HOST,
	STATUS_ADMIN,
	STATUS_OWNER
};

/* All data known about a player */
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

/* Player interface functions */
void guru_player_init(const char *datadir);
void guru_player_policy(int duplication);
Player *guru_player_lookup(const char *name);
void guru_player_save(Player *p);
void guru_player_free(Player *p);
Player *guru_player_new(void);
void guru_player_cleanup(void);

#endif

