/*
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.h 3457 2002-02-24 19:57:25Z jdorje $
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

/*
 * Current list of preferences.
 *
 * To add a new preference, add a value for it here and insert
 * information about it into the pref_types array in preferences.c.
 */
typedef struct {
	int animation;
	int faster_animation;
	int smoother_animation;
	int multiple_animation;
	int longer_clearing_delay;
	
	int cardlists;
	int bid_on_table;
	
	int autostart;
	int use_default_options;
} Preferences;

/* Our set of preferences.  See above. */
extern Preferences preferences;

/* This is the maximum hand size we can sustain.  The client-common code will 
   tell us when we need to increase it. */
extern int table_max_hand_size;

/* This isn't set to 1 until we start a game; until then we keep up the
   splash screen. */
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

/** @brief Handles IO from the server; called any time data is pending. */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond);

/** @brief Request a sync from the server, and prepare to receive it. */
void game_request_sync(void);


/* 
 * This second group is callbacks called
 * by the client-common code.
 */

void game_get_newgame(void);
void game_alert_newgame(void);
void game_alert_newhand(void);
void game_handle_gameover(int num_winners, int *winners);
void game_alert_player(int player, GGZSeatType status, const char *name);
void game_setup_table(void);
void game_alert_hand_size(int max_hand_size);
void game_display_hand(int player);
void game_get_bid(int possible_bids, bid_t *bid_choices,
                  char **bid_descriptions);
void game_get_play(int hand);
void game_alert_bid(int bidder, bid_t bid);
void game_alert_badplay(char *err_msg);
void game_alert_play(int player, card_t card, int pos);
void game_alert_table(void);
void game_alert_trick(int player);
void game_set_text_message(const char *mark, const char *message);
void game_set_cardlist_message(const char *mark, int *lengths,
			       card_t ** cardlist);
void game_set_player_message(int player, const char *message);
int game_handle_game_message(int fd, const char *game, int size);


/* 
 * Extra support functions
 */

/* preliminary internationalization macros */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif /* __GAME_H__ */
