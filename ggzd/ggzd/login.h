/*
 * File: login.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 6/22/00
 * Desc: Functions for handling player logins
 * $Id: login.h 3420 2002-02-19 08:04:27Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#ifndef _GGZ_LOGIN_H
#define _GGZ_LOGIN_H

#include <config.h>
#include <players.h>

typedef enum {
	GGZ_LOGIN,
	GGZ_LOGIN_GUEST,
	GGZ_LOGIN_NEW
} GGZLoginType;


GGZPlayerHandlerStatus login_player(GGZLoginType type, GGZPlayer* player,
                                   char *name, char *password);
int  login_player_anon(GGZPlayer* player, char *name);
int  login_player_new(GGZPlayer* player, char *name);
int  login_player_normal(GGZPlayer* player, char *name, char *password);
GGZPlayerHandlerStatus logout_player(GGZPlayer* player);


#endif /* _GGZ_LOGIN_H */
