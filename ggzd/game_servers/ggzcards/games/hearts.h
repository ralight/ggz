/*
 * File: games/hearts.h
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Hearts
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


#define GHEARTS ( *(hearts_game_t *)(game.specific) )
typedef struct hearts_game_t {
	int points_on_hand[10];	/* the points each player has taken this hand.  Works for up to 10 players. */

	/* options */
	int jack_diamonds;	/* is the jack-of-diamonds rule in effect? */
	int no_blood;		/* no blood on the first trick */

	int jack_winner;	/* who has won the jack of diamonds this hand? */
	char lead_card_face;	/* the card that leads first.  It's a club.  Two-deck issues aren't dealt with. */
} hearts_game_t;

