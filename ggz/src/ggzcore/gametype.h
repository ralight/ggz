/*
 * File: gametype.h
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This file contains functions for handling game types
 *
 * Copyright (C) 1998 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#ifndef __GAMETYPE_H_
#define __GAMETYPE_H_


typedef enum {
	GGZ_ALLOW_ONE   =   1,    /* %0000 0001 */
	GGZ_ALLOW_TWO   =   2,    /* %0000 0010 */
	GGZ_ALLOW_THREE =   4,    /* %0000 0100 */
	GGZ_ALLOW_FOUR  =   8,    /* %0000 1000 */
	GGZ_ALLOW_FIVE  =  16,    /* %0001 0000 */
	GGZ_ALLOW_SIX   =  32,    /* %0010 0000 */
	GGZ_ALLOW_SEVEN =  64,    /* %0100 0000 */
	GGZ_ALLOW_EIGHT = 128     /* %1000 0000 */
} GGZAllowed;


/* Structure describing specific game type */
struct _GGZGameType {
	
	/* Name of the game */
	char *name;

	/* Version */
	char *version;

	/* Protocol engine */
	char *prot_engine;

	/* Protocol version */
	char *prot_version;

	/* Game description */
	char *desc;

	/* Author */
	char *author;

	/* Homepage for game */
	char *url;

	/* Bitmask of alowable numbers of players */
	GGZAllowed allow_players;

	/* Bitmask of alowable numbers of bots */
	GGZAllowed allow_bots;

	/* Whether spectators are allowed or not */
	unsigned int spectators_allowed;
	
	/* ID of this game on the server */
	unsigned int id;
};


/* Create a new gametype object */
struct _GGZGameType* _ggzcore_gametype_new(void);

/* Initialize a gametype with the given properties */
void _ggzcore_gametype_init(struct _GGZGameType *gametype,
			    const unsigned int id,
			    const char* name, 
			    const char* version,
			    const char* prot_engine,
			    const char* prot_version,
			    const GGZAllowed allow_players,
			    const GGZAllowed allow_bots,
				unsigned int spectators_allowed,
			    const char* desc,
			    const char* author, 
			    const char *url);

void _ggzcore_gametype_free(struct _GGZGameType *type);

unsigned int _ggzcore_gametype_get_id(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_name(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_version(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_prot_engine(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_prot_version(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_author(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_url(struct _GGZGameType *type);

char*  _ggzcore_gametype_get_desc(struct _GGZGameType *type);

/* Return the maximum number of allowed players/bots */
unsigned int _ggzcore_gametype_get_max_players(struct _GGZGameType *type);
unsigned int _ggzcore_gametype_get_max_bots(struct _GGZGameType *type);
unsigned int _ggzcore_gametype_get_spectators_allowed(struct _GGZGameType *type);

/* Verify that a paticular number of players/bots is valid */
unsigned int _ggzcore_gametype_num_players_is_valid(struct _GGZGameType *type, unsigned int num);
unsigned int _ggzcore_gametype_num_bots_is_valid(struct _GGZGameType *type, unsigned int num);


/* Utility functions used by _ggzcore_list */
int   _ggzcore_gametype_compare(void* p, void* q);
void* _ggzcore_gametype_create(void* p);
void  _ggzcore_gametype_destroy(void* p);

#endif /*__GAMETYPE_H_*/
