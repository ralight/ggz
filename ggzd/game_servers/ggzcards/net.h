/* 
 * File: net.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.h 2453 2001-09-11 19:20:55Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
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

#include "types.h"

int send_player_list(player_t);
int send_play(card_t card, seat_t);
int send_table(player_t);
int send_sync(player_t);
int send_sync_all();
int send_gameover(int winner_cnt, player_t * winners);
int req_play(player_t, seat_t);
int rec_play(player_t);
int send_badplay(player_t, char *);
int send_hand(player_t, seat_t, int);
int req_newgame(player_t);
int send_newgame(void);
int send_trick(player_t winner);
