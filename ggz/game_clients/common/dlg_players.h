/* 
 * File: dlg_players.c
 * Author: Jason Short
 * Project: GGZ GTK Games
 * Date: 10/13/2002 (moved from GGZCards)
 * Desc: Create the "Players" Gtk dialog
 * $Id: dlg_players.h 4917 2002-10-14 22:27:43Z jdorje $
 *
 * Copyright (C) 2002 GGZ Development Team
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

#include <gtk/gtk.h>

#include <ggzmod.h>

/* Call this function before using any player lists.  It will register
   GGZMod event handlers for the SEAT, SPECTATOR, and STATE events -
   if the game wants its own handlers for these events you should
   register them *after* calling this function, and then be sure to
   call update_player_lists when the events occur. */
void init_player_list(GGZMod *ggzmod);

/* The player list will attach itself to the SEAT and SPECTATOR ggzmod
   events, but if you override them you should call this function instead.
   You should also call it when the STATE changes out of CREATED, if you
   have an handler registered for that event. */
void update_player_lists(void);

/* Creates a widget containing a pleyer list. */
GtkWidget *create_playerlist_widget(void);

/* Creates or raises the main player list dialog window. */
void create_or_raise_dlg_players(void);
