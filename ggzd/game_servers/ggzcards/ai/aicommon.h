/* 
 * File: ai/aicommon.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: useful functions for AI bots
 * $Id: aicommon.h 3425 2002-02-20 03:45:35Z jdorje $
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
 *
 * Copyright (C) 2001-2002 GGZ Development Team
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

#include "protocol.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* FIXME: Right now this code assumes 4 players and 4 suits. */

void ailib_start_hand(void);

void ailib_alert_trick(int p);

void ailib_alert_bid(int p, bid_t bid);

void ailib_our_play(int play_hand);
void ailib_alert_play(int p, card_t play);

/** @brief Has this card been played? */
int libai_is_card_played(char suit, char face);

int get_tricks(int player);
int get_leader(void);

/** @Remember that the player doesn't have the card. */
void libai_player_doesnt_have_card(int p, card_t card);

/* A convenience function; returns the bitmap of cards p has in suit. */
int libai_get_suit_map(int p, char suit);

/** @brief Is it possible for the player to have this card? */
int libai_might_player_have_card(int p, card_t card);

/** @brief Do we *know* that the seat has the card?
 *  @note Will cheat if you ask it to. */
int libai_is_card_in_hand(int seat, card_t card);

/** @brief What is my highest card remaining in the suit?
 *  @note Will cheat if you ask it to. */
card_t libai_get_highest_card_in_suit(int seat, char suit);

/** @brief Is this the highest card left out in the suit? */
int libai_is_highest_in_suit(card_t card);

/** @brief How many cards have been played total in this suit? */
int libai_cards_played_in_suit(char suit);

/** @brief How many cards has this player played in this suit? */
int libai_cards_played_in_suit_p(int s, char suit);

/** @brief How many cards do we have in this suit?
 *  @note Will cheat if you ask it to. */
int libai_count_suit(int seat, char suit);

/** @brief Forget what we know about player's holdings in this suit. */
void libai_forget_players_hand(int p, char suit);

