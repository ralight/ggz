/*
 * File: game.h
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 3/1/01
 * $Id: game.h 10274 2008-07-10 21:38:34Z jdorje $
 *
 * Functions for handling game events
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


#include "ggzcore.h"
#include "support.h"

/** Initialize a game client.
 *
 *  @param spectating 1 if we're spectating, 0 if we're joining
 */
int INTERNAL game_initialize(int spectating);

int INTERNAL game_launch(void);

void INTERNAL game_channel_ready(void);

void INTERNAL game_quit(void);

void INTERNAL game_destroy(void);

int INTERNAL game_play(void);

gboolean INTERNAL can_launch_gametype(const GGZGameType *gt);
