/* $Id: main.h 2940 2001-12-18 22:17:50Z jdorje $ */
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

/** @brief Called when we're ready to start a new game.
 *
 *  When we're ready to start a new game, this function should
 *  be called to tell the server.
 *  @see game_get_newgame
 */
void table_send_newgame(void);

void statusbar_message(char *msg);
void messagebar_message(const char *msg);
void menubar_text_message(const char *mark, const char *msg);
void menubar_cardlist_message(const char *mark, int *lengths,
			      card_t ** cardlist);
