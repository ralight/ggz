/*
 * File: menus.h
 * Author: GGZ Development Team
 * Project: GGZ GTK Games
 * Date: 10/25/2002
 * Desc: Main window menus
 * $Id: menus.h 5954 2004-02-21 08:17:22Z jdorje $
 *
 * The point of this file is to help games to achieve a consistent look
 * and feel in their menus.  The TABLE_MENU and HELP_MENU should be used
 * by all games.  A "Game" menu should provide game-specific featurs.  An
 * "Options" menu should be used if necessary for options.
 *
 * Copyright (C) 2002 GGZ Development Team.
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

#include "ggzintl.h"
#include "dlg_about.h"
#include "dlg_chat.h"
#include "dlg_players.h"

#define TABLE_MENU \
	{_("/_Table"), NULL, NULL, 0, "<Branch>"}, \
	{_("/Table/Player _list"), "<ctrl>L", \
	 create_or_raise_dlg_players, 0, NULL}, \
	{_("/Table/Chat _window"), "<ctrl>W", \
	 toggle_chat_window, 0, "<CheckItem>"}, \
	{_("/Table/Sit down"), NULL, do_sit, 0, NULL}, \
	{_("/Table/Stand up"), NULL, do_stand, 0, NULL}, \
	{_("/Table/_Sync with server"), "<ctrl>S", game_resync, 0, NULL}, \
	{_("/Table/E_xit"), "<ctrl>X", game_exit, 0, NULL}

/* These functions should be defined in the game's code. */
void game_resync(void);
void game_exit(void);

/* These macro string values can be used to access items in the menu.  If a
   menu item is renamed, it'll be easily updated here. */
#define TABLE _("/Table")
#define TABLE_PLAYER_LIST _("/Table/Player list")
#define TABLE_CHAT_WINDOW _("/Table/Chat window")
#define TABLE_SIT _("/Table/Sit down")
#define TABLE_STAND _("/Table/Stand up")
#define TABLE_SYNC _("/Table/Sync with server")
#define TABLE_EXIT _("/Table/Exit")

#define HELP_MENU \
	{_("/_Help"), NULL, NULL, 0, "<LastBranch>"}, \
	{_("/Help/_About"), "<ctrl>A", create_or_raise_dlg_about, 0, NULL}

#define HELP _("/Help")
#define HELP_ABOUT _("/Help/About")

/* This function can be easily used (perhaps in conjunction with the
   above macros) to sensitize or desensitize a menu item. */
GtkWidget *ggz_create_menus(GtkWidget *window,
			    GtkItemFactoryEntry *items,
			    const unsigned int num_items);
GtkWidget *get_menu_item(const char *item);
void set_menu_sensitive(const char *menu, gboolean sensitive);
