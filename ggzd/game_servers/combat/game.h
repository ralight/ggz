/*
 * File: game.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: server specific header file
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#define CBT_SERVER_ERROR -1
#define CBT_SERVER_OK 0
#define CBT_SERVER_JOIN 1
#define CBT_SERVER_LEFT 2
#define CBT_SERVER_QUIT 3

// Init stuff
void game_init();

// Handles the protocol
int game_handle_ggz(int, int*);
int game_handle_player(int);

// Sends info to the player
void game_send_seat(int);
void game_send_options(int);
void game_send_sync(int);
void game_send_players();

// Gets info from the player
void game_get_options(int);

// Tells the player to do stuff
void game_request_options(int);
void game_start();
