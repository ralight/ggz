/* 
 * File: game.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.h 2931 2001-12-18 07:27:02Z jdorje $
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

#include <gtk/gtk.h>

#include "hand.h"

#ifndef __GAME_H__
#define __GAME_H__

/* if this is defined, the animation code will be used. I've disable it
   because I broke it... */
/* #define ANIMATION */

#ifdef ANIMATION
/* Are we currently in the process of animating? */
extern int animating;
#endif


/** @brief Sends the bid to the server, and updates the graphics.
 *  @param bid An index into the list of bids sent by the server. */
void game_send_bid(int bid);

/** @brief Sends the card to the server, and updates the graphics.
 *  @param card The specific card to play (from the appropriate hand) */
void game_play_card(card_t card);

/** @brief Handles IO from the server; called any time data is pending. */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond);

/** @brief Initializes the game.
 *  @note A lot of other initialization is also done in main() */
void game_init(void);


/* 
 * Extra support functions
 */

/* preliminary internationalization macros */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif /* __GAME_H__ */
