/* 
 * File: game.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.h 2940 2001-12-18 22:17:50Z jdorje $
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

#include "common.h"

#ifndef __GAME_H__
#define __GAME_H__

extern int table_max_hand_size;

/*
 * This first group is called by the table and
 * propogates to the client-common interface.
 */

/** @brief Initializes the game.
 *  @note A lot of other initialization is also done in main() */
void game_init(void);

/** @brief Sends the bid to the server, and updates the graphics.
 *  @param bid An index into the list of bids sent by the server. */
void game_send_bid(int bid);

/** @brief Sends the card to the server, and updates the graphics.
 *  @param card The specific card to play (from the appropriate hand) */
void game_play_card(card_t card);

/** @brief Handles IO from the server; called any time data is pending. */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond);



/*
 * This second group is callbacks called
 * by the client-common code.
 */

void game_get_newgame(void);
void game_alert_newgame(void);
void game_handle_gameover(int num_winners, int *winners);
void game_alert_player(int player, GGZSeatType status, const char *name);
void game_setup_table(void);
void game_alert_hand_size(int max_hand_size);
void game_display_hand(int player);
void game_get_bid(int possible_bids, char **bid_choices);
void game_get_play(int hand);
void game_alert_badplay(char *err_msg);
void game_alert_play(int player, card_t card, int pos);
void game_alert_table(void);
void game_alert_trick(int player);
void game_set_text_message(const char *mark, const char *message);
void game_set_cardlist_message(const char *mark, int *lengths,
			       card_t ** cardlist);
void game_set_player_message(int player, const char *message);
int game_handle_game_message(int fd, int game, int size);


/* 
 * Extra support functions
 */

/* preliminary internationalization macros */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif /* __GAME_H__ */
