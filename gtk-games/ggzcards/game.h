/* 
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.h 10188 2008-07-08 03:20:24Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "client.h"
#include "layout.h"

#ifndef __GAME_H__
#define __GAME_H__

/* Standard debugging types.  These are necessary for alternate AIs. */
#define DBG_BID "bid"
#define DBG_PLAY "play"
#define DBG_AI "misc"

/* 
 * Current list of preferences.
 *
 * To add a new preference, add a value for it here and insert
 * information about it into the pref_types array in preferences.c.
 */
typedef struct {
	/* Animation prefs */
	bool animation;
	bool multiple_animation;
	bool longer_clearing_delay;
#ifdef DEBUG
	bool no_clearing_delay;
#endif

	/* Game interface prefs */
	bool single_click_play;
	bool collapse_hand;
	bool cardlists;
#undef CAN_BID_ON_TABLE
#ifdef CAN_BID_ON_TABLE
	bool bid_on_table;
#endif
	bool bidding_tooltips;
	bool large_dominoes;

	/* Meta-game interface prefs */
	bool autostart;
	bool use_default_options;
	bool confirm_exit;

#ifdef DEBUG
	/* Debugging prefs */
	bool use_ai;
#endif
} Preferences;

#ifndef CAN_BID_ON_TABLE
/* Currently bidding on the table doesn't work.  I can't get the
 * bid widget to properly overlay the drawing widget. */
#  define BID_ON_TABLE (FALSE)
#else
#  define BID_ON_TABLE preferences.bid_on_table
#endif

/* Our set of preferences.  See above. */
extern Preferences preferences;

/* This isn't set to 1 until we start a game; until then we keep up the splash 
   screen. */
extern int game_started;

/** @brief Cards being drawn on the table.
 *
 *  This is the list of the cards that we currently have
 *  drawn on the table.  It differs from the core list in
 *  the ggzcards struct, since not all cards that are "on"
 *  the table are being drawn.
 */
extern card_t table_cards[MAX_NUM_PLAYERS];

/* 
 * This first group is called by the table and
 * propogates to the client-common interface.
 */

/** @brief Initializes the game.
 *  @note A lot of other initialization is also done in main() */
void game_init(void);

/** @brief Called when we're ready to start a new game.
 *
 *  When we're ready to start a new game, this function should
 *  be called to tell the server.
 *  @see game_get_newgame
 */
void game_send_newgame(void);

/** @brief Sends the bid to the server, and updates the graphics.
 *  @param bid An index into the list of bids sent by the server. */
void game_send_bid(int bid);

/** @brief Sends the option list to the server, and updates
 *  the graphics.
 *  @param option_count The number of options to send.
 *  @param options_selection The option_count option choices. */
void game_send_options(int option_count, int *options_selection);

/** @brief Sends the card to the server, and updates the graphics.
 *  @param card The specific card to play (from the appropriate hand) */
void game_play_card(int card_num);

/** @brief Handles IO from GGZ. */
gboolean game_handle_ggz(GIOChannel * source, GIOCondition cond,
			 gpointer data);

/** @brief Handles IO from the server; called any time data is pending. */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond);

#endif /* __GAME_H__ */
