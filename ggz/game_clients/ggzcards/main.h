/* $Id: main.h 2872 2001-12-11 06:15:35Z jdorje $ */
/* 
 * File: main.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
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

#include "common.h"

extern GtkWidget *dlg_main;

/** @brief Called when we get a newgame request from the server.
 *
 *  When the server sends a newgame request, this method will be
 *  called by the client-common code.  We must later send a
 *  newgame response when we're ready to start the game.
 *  @see table_send_newgame
 */
void table_get_newgame(void);

/** @brief Called when we're ready to start a new game.
 *
 *  When we're ready to start a new game, this function should
 *  be called to tell the server.
 *  @see table_get_newgame
 */
void table_send_newgame(void);

void statusbar_message(char *msg);
void messagebar_message(const char *msg);
void menubar_text_message(const char *mark, const char *msg);
void menubar_cardlist_message(const char *mark, int *lengths,
			      card_t ** cardlist);

/* Client-common table functions. */
int table_handle_game_message(int fd, int game, int size);
void table_alert_player(int player, GGZdModSeat status, const char *name);
void table_alert_newgame(void);
