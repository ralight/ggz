/* 
 * File: games/spades.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game functions for Spades
 * $Id: spades.h 2391 2001-09-07 13:10:52Z jdorje $
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

/* special bids */
#define SPADES_NIL		1
#define SPADES_NO_BLIND		2	/* a choice *not* to bid blind nil */
#define SPADES_DOUBLE_NIL	3	/* double nil and blind nil are the
					   same */

#define GSPADES ( *(spades_game_t *)(game.specific) )
typedef struct spades_game_t {
	/* options */
	int nil_value;		/* 0 for none; generally 50 or 100 */
	int double_nil_value;	/* 0 for none; generally 100 or 200 */
	int minimum_team_bid;	/* the minimum bid by one team */
	int nil_tricks_count;	/* do tricks by a nil bidder count toward the 
				   bid? */

	/* data */
	int show_hand[4];	/* this is 0 if we're supposed to conceal the 
				   hand (for blind bids */
	int bags[2];		/* # of overtricks ("sandbags") taken by each 
				   team */
} spades_game_t;
