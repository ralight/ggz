/*
 * File: main.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000 Justin Zaun.
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
#include <ggzcore.h>

#include "about.h"
#include "chat.h"
#include "client.h"
#include "ggz.h"
#include "license.h"
#include "login.h"

GtkWidget *win_main;
GtkWidget *dlg_about;
GtkWidget *dlg_license;


int main (int argc, char *argv[])
{
	GGZOptions opt;

	opt.flags = GGZ_OPT_PARSER;
	opt.global_conf = "/etc/ggz-text.rc";
	opt.user_conf = "~/.ggz-txtrc";
	opt.local_conf = NULL;

	ggzcore_init(opt);
	ggz_event_init();
	
	gtk_init(&argc, &argv);
	chat_allocate_colors();
	g_main_set_poll_func((GPollFunc)ggzcore_event_poll);

	dlg_about = create_dlg_about();
	win_main = create_win_main();
	dlg_license = create_dlg_license();
	login_create_or_raise();

	ggz_sensitivity_init();
	gtk_widget_show(win_main);

	gtk_main();

	ggzcore_destroy();

	return 0;
}

