/* 
 * File: ai/aicommon.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: useful functions for AI bots
 * $Id: aicommon.h 2832 2001-12-09 21:41:07Z jdorje $
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include "ai.h"
#include "types.h"

/* FIXME: Right now this code assumes 4 players and 4 suits. */

void ailib_start_hand(void);

void ailib_alert_bid(player_t p, bid_t bid);

void ailib_alert_play(player_t p, card_t play);

/** @brief Has this card been played? */
int libai_is_card_played(char suit, char face);


/** @Remember that the player doesn't have the card. */
void libai_player_doesnt_have_card(player_t p, card_t card);

/* A convenience function; returns the bitmap of cards p has in suit. */
int libai_get_suit_map(player_t p, char suit);

/** @brief Is it possible for the player to have this card? */
int libai_might_player_have_card(player_t p, card_t card);

/** @brief Do we *know* that the seat has the card? */
int libai_is_card_in_hand(seat_t seat, card_t card);

/** @brief Is this the highest card in the suit? */
int libai_is_highest_in_suit(card_t card);

/** @brief How many cards remain out in this suit? */
int libai_cards_left_in_suit(char suit);

/** @brief How many cards has this player played in this suit? */
int libai_cards_played_in_suit(seat_t s, char suit);

/** @brief How many cards do we have in this suit? */
int libai_count_suit(seat_t seat, char suit);

/** @brief Forget what we know about player's holdings in this suit. */
void libai_forget_players_hand(player_t p, char suit);
