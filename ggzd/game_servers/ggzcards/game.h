/* 
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: default game functions
 * $Id: game.h 2766 2001-11-28 07:31:42Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It now
 * contains the default game functions; that is, the set of game functions
 * that may be used as defaults.  Each game (in games/) will define its
 * own functions as well.
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

#include <config.h>		/* Site-specific config */

#include <stdio.h>		/* just for debugging */

#include "types.h"

/* Game-specific functions */

extern struct game_function_pointers game_funcs;

/** @brief Is this game valid?
 *  @return TRUE iff the game is valid with the current settings. */
int game_is_valid_game();

/** @brief Initialize the game. */
void game_init_game();

/** @brief Set up game options
 *  Calls add_option once for each game option. */
void game_get_options();

/** @brief Handles a single option
 *  Performs any handling necessary to set the option.
 *  @param option The name (key string) of the option
 *  @param value The value the option takes
 *  @see options.h */
int game_handle_option(char *option, int value);

/** @brief Return a descriptive string for the option's value
 *  @param buf The buffer in which to place the string
 *  @param buf_len The size of the buffer
 *  @param option The name (key string) of the option
 *  @param value The value we want a description for
 *  @return A usable pointer to the buffer */
char *game_get_option_text(char *buf, int buf_len, char *option, int value);

/** @brief Set the player message for the given player.
 *  This function should use the messaging functions to create
 *  a player message.  This text message is automatically passed to
 *  the clients by the core code.
 *  @param player The player for which to create a message
 *  @see message.h */
void game_set_player_message(player_t player);

/** @brief Set the text message for the given bid.
 *  Creates a text description of the bid is created
 *  and places it into the buffer.
 *  @param buf a buffer in which to put the text
 *  @param buf_len the length of the buffer
 *  @param bid the bid for which to create a message for */
int game_get_bid_text(char *buf, size_t buf_len, bid_t bid);

/** @brief Called at the beginning of the bidding sequence.
 *  Does any special handling necessary before beginning bidding. */
void game_start_bidding();

/** @brief Get the next bid. */
int game_get_bid();

/** @brief Specialty handling of a bid. */
void game_handle_bid(player_t p, bid_t bid);

/** @brief Specialty handling to prep for next bid. */
void game_next_bid();

/** @brief Specialty handling before starting to play a hand. */
void game_start_playing();

/** @brief Check the validity of a play. */
char *game_verify_play(card_t card);

/** @brief Prep for the next play. */
void game_next_play();

/** @brief Get the next play. */
void game_get_play(player_t p);

/** @brief Specialty handling of a played card. */
void game_handle_play(card_t c);

/** @brief Deal a hand. */
int game_deal_hand();

/** @brief Specialty handling for the end of a trick. */
void game_end_trick();

/** @brief Specialty handling for the end of a hand. */
void game_end_hand();

/** @brief Specialty handling before the start of a game. */
void game_start_game();

/** @brief Check to see if the game should be over. */
int game_test_for_gameover();

/** @brief Specialty handling when a game is over. */
int game_handle_gameover();

/** @brief Makes one card act like another.
 *  This makes a card behave entirely as if it were a different card.
 *  @param card the card that is being mapped
 *  @return the card it is mapped to
 *  @see euchre_map_card */
card_t game_map_card(card_t card);

/** @brief Compares two cards for hand-sorting purposes.
 *  @return -1, 0, 1 if card1 is lower, equal, or higher than card2 */
int game_compare_cards(card_t card1, card_t card2);

/** @brief Sends a hand to a player.
 *  send_hand is called after determining whether the player should be
 *  shown the hand.  The default case, of course, is to show the player
 *  only their own hand.
 *  @param p the player the hand is being sent to
 *  @param s the seat of the hand being shown */
int game_send_hand(player_t p, seat_t s);
