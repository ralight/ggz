/* 
 * File: animation.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 12/18/2001
 * Desc: Animation code for GTK table
 * $Id: animation.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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

#include "client.h"

#define DBG_ANIM "animation"

/** @brief Setup animation data.
 *
 *  Call this on startup, or when the table is resized.
 */
void anim_setup(void);

/** @brief Animate a card.
 *
 *  @param player The player whose card it is (or was).
 *  @param card The card.
 *  @param card_num The position of the card in the hand (if applicable).
 *  @param destination -1 for the table, <player #> for a trick winner.
 *  @return TRUE if an animation is started, FALSE otherwise.
 */
int animation_start(int player, card_t card, int card_num,
		    int destination);

/** @brief Move the cards off of the table.
 *
 *  This function should be called to start the code to animate the cards
 *  off of the table.
 */
void animate_cards_off_table(int winner);

/** @brief Stop an animation.
 *
 *  Stops an animation (all animation).  If success is specified, the
 *  animation will be assumed to be completed.  This is only applicable
 *  for when you are playing a card out to the table - after we find out
 *  whether the play is valid or not the animation will either succeed
 *  or fail.
 */
void animation_stop(int success);
