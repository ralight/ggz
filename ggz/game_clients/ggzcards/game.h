/* $Id: game.h 2080 2001-07-23 13:01:31Z jdorje $ */
/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
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

#include <assert.h>

#include "hand.h"

#ifndef __GAME_H__
#define __GAME_H__

/* if this is defined, the animation code will be used.
 * I've disable it because I broke it... */
/* #define ANIMATION */

extern void game_send_bid(int);
extern void game_play_card(card_t card);
extern void game_handle_table_click(char);

/* GGZ define */
#define GGZ_SEAT_OPEN		-1

/* substitutes for snprintf.
 * Returns the number of characters put into the buffer. */
extern int ggz_snprintf(char*, int, char *, ...);

/* preliminary internationalization macros */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif /* __GAME_H__ */


