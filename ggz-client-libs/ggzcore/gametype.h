/*
 * File: gametype.h
 * Author: GGZ Development Team
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: gametype.h 7402 2005-08-13 22:24:02Z josef $
 *
 * This file contains functions for handling game types.
 *
 * Copyright (C) 1998-2005 GGZ Development Team.
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

#include <ggzcore.h>

#ifndef __GAMETYPE_H_
#define __GAMETYPE_H_


/* Create a new gametype object */
GGZGameType* _ggzcore_gametype_new(void);

/* Initialize a gametype with the given properties */
void _ggzcore_gametype_init(GGZGameType *gametype,
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

void _ggzcore_gametype_add_namedbot(GGZGameType *gametype,
			    const char *botname,
			    const char *botclass);

void _ggzcore_gametype_free(GGZGameType *type);

/* Utility functions used by _ggzcore_list */
int   _ggzcore_gametype_compare(void* p, void* q);
void* _ggzcore_gametype_create(void* p);
void  _ggzcore_gametype_destroy(void* p);

#endif /*__GAMETYPE_H_*/
