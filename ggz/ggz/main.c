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
#include <stdio.h>

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
#include "support.h"

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
char *local_conf_fname = NULL;

/* Aray of GdkColors currently used for chat and MOTD */
GdkColor colors[] = 
{
	{0, 0, 0, 0},			/* 0   black	*/
	{0, 0, 0, 0xcccc},		/* 1   bule			*/
	{0, 0xcccc, 0, 0},		/* 2   red			*/
	{0, 0xbbbb, 0xbbbb, 0},		/* 3   yellow/brown		*/
	{0, 0xbbbb, 0, 0xbbbb},		/* 4   purple			*/
	{0, 0xffff, 0xaaaa, 0},		/* 5   orange			*/
	{0, 0x7777, 0x7777, 0x7777},	/* 6   grey			*/
	{0, 0, 0xcccc, 0xcccc},		/* 7   aqua			*/
	{0, 0, 0, 0xcccc},		/* 8   blue markBack		*/
	{0, 0, 0, 0},			/* 9   black			*/
	{0, 0xcccc, 0xcccc, 0xcccc},	/* 10  white			*/
	{0, 0xffff, 0xffff, 0},		/* 11  yellow			*/
	{0, 0, 0xffff, 0},		/* 12  green			*/
	{0, 0, 0xffff, 0xffff},		/* 13  light aqua		*/
	{0, 0, 0, 0xffff},		/* 14  blue			*/
	{0, 0xffff, 0, 0xffff},		/* 15  pink			*/ 
	{0, 0x9999, 0x9999, 0x9999},	/* 16  light grey		*/
	{0, 0xeeee, 0xeeee, 0xeeee},	/* 17  white markFore		*/
	{0, 0, 0, 0},			/* 18  foreground (black)	*/
	{0, 0xffff, 0xffff, 0xffff}	/* 19  background (white)	*/
};


gint main(gint argc, gchar *argv[])
{
	gint i;

	/* This sets the location of the translations of all the text in the
	   interface. */
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE);

	
	/* Read configuration values */
	parse_args(argc, argv);
	if(ggzrc_initialize(local_conf_fname) != 0) {
		fprintf(stderr, _("ERROR: No valid configuration file loaded\n"));
		fprintf(stderr, _("Gnu Gaming Zone can not continue\n"));
		exit(1);
	}

	gtk_init(&argc, &argv);
	es_err_func_set(err_sock);

	/* Signal handlers */
	signal(SIGCHLD, game_dead);
	
	/* Allocate standared colors */
        if (!colors[0].pixel)        /* don't do it again */
        {
                for (i = 0; i < 20; i++)
                {
                        colors[i].pixel = (gulong) ((colors[i].red & 0xff00) * 256 +
                                        (colors[i].green & 0xff00) +
                                        (colors[i].blue & 0xff00) / 256);
                        if (!gdk_color_alloc (gdk_colormap_get_system(),
                            &colors[i]))
                                g_error("*** GGZ: Couldn't alloc color\n");
                }
        }


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

	/* Initilize GUI */
	login_disconnect();

	gtk_main();

	ggzrc_commit_changes();
	ggzrc_cleanup();

	return 0;
}
