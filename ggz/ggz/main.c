/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 2/18/00
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <config.h>

#include <gtk/gtk.h>
#include <signal.h>

#include "datatypes.h"
#include "parse_opt.h"
#include "easysock.h"
#include "err_func.h"
#include "ggzrc.h"

#include "dlg_main.h"
#include "dlg_login.h"
#include "mnu_tables.h"
#include "mnu_players.h"
#include "game.h"

/* Main global data structures */
struct ConnectInfo connection;
struct Game game;
struct Users users;
struct GameTypes game_types;
extern GtkWidget *main_win;
extern GtkWidget *dlg_login;
GtkWidget *mnu_tables;
GtkWidget *mnu_players;
GtkWidget *dlg_launch;
gint selected_table;
gint selected_type;
struct GameTables tables;
struct Rooms room_info;

/* Aray of GdkColors currently used for chat */
GdkColor colors[] = 
{
	{0, 0, 0xcccc, 0},		/* 0   green		Self	*/
	{0, 0, 0, 0xcccc},		/* 1   bule		Others	*/
	{0, 0xcccc, 0, 0},		/* 2   red		Buddies	*/
	{0, 0xbbbb, 0xbbbb, 0},		/* 3   yellow/brown		*/
	{0, 0xbbbb, 0, 0xbbbb},		/* 4   purple			*/
	{0, 0xffff, 0xaaaa, 0},		/* 5   orange			*/
	{0, 0x7777, 0x7777, 0x7777},	/* 6   grey			*/
	{0, 0, 0xcccc, 0xcccc},		/* 7   aqua			*/
	{0, 0, 0, 0xcccc},		/* 8   blue markBack		*/
	{0, 0, 0, 0},			/* 9   black			*/
};


gint main(gint argc, gchar *argv[])
{
	parse_args(argc, argv);
	gtk_init(&argc, &argv);
	es_err_func_set(err_sock);

	/* Load up the ggz.rc file */
	ggzrc_initialize();

	/* Signal handlers */
	signal(SIGCHLD, game_dead);
	
	/* Popup Menus */
	mnu_tables = create_mnu_tables();
	mnu_players = create_mnu_players();

	/* Windows/Dialogs */
	main_win = create_main_win();
	gtk_widget_show(main_win);
	dlg_login = create_dlg_login();
	gtk_widget_show(dlg_login);
	
	/* Setup Rooms */
	connection.new_room = -1;
	connection.cur_room = -3;

	login_disconnect();

	gtk_main();

	ggzrc_cleanup();

	return 0;
}
