/*
 * File: gametype.h
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: gametype.h 5174 2002-11-03 19:37:36Z jdorje $
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

#include <ggz_common.h>

#ifndef __GAMETYPE_H_
#define __GAMETYPE_H_


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
	GGZNumberList player_allow_list;

	/* Bitmask of alowable numbers of bots */
	GGZNumberList bot_allow_list;

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
			    const GGZNumberList player_allow_list,
			    const GGZNumberList bot_allow_list,
			    unsigned int spectators_allowed,
			    const char* desc,
			    const char* author, 
			    const char *url);

void _ggzcore_gametype_free(struct _GGZGameType *type);

unsigned int _ggzcore_gametype_get_id(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_name(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_version(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_prot_engine(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_prot_version(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_author(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_url(struct _GGZGameType *type);

const char * _ggzcore_gametype_get_desc(struct _GGZGameType *type);

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
