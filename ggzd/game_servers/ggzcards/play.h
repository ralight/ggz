/*
 * File: play.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Functions and data for playing system
 * $Id: play.h 3482 2002-02-27 04:36:02Z jdorje $
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#include "types.h"

/* Request player p to play from seat s's hand. */
void req_play(player_t p, seat_t s);

/* Handle a play from a client.  This will verify the play
   and call handle_play_event if necessary. */
void handle_client_play(player_t p, card_t card);
