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

#if 0
/* Structure describing specific game type */
struct _GGZGameType {
	
	/* Name of the game */
	char *name;

	/* Version */
	char *version;

	/* Game description */
	char *desc;

	/* Author */
	char *author;

	/* Homepage for game */
	char *url;

	/* Bitmask of alowable numbers of players 
	GGZAllowed allow_players;*/

	/* Bitmask of alowable numbers of bots 
	GGZAllowed allow_bots;*/
	
	/* ID of this game on the server (or -1 if not supported) */
	int id;

	/* FIXME: Some way of identifiying which module to run on the client */
	/* possible use another id?  or perhaps a pointer would be best */
};
#endif

#if 0
void _ggzcore_gametype_list_clear(void);

int _ggzcore_gametype_list_add(const unsigned int id, const char* name, const char* game,
                           const unsigned int players, const int bots,  const char* desc,
                           const char* author, const char *url);

int _ggzcore_gametype_list_remove(const unsigned int id);

int _ggzcore_gametype_list_replace(const unsigned int id, const char* name, 
			       const unsigned int game, const char* desc);

#endif


#endif /*__GAMETYPE_H_*/
