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
#include <stdlib.h>

#include "about.h"
#include "chat.h"
#include "client.h"
#include "ggz.h"
#include "license.h"
#include "login.h"


extern GtkWidget *win_main;


int main (int argc, char *argv[])
{
	GGZOptions opt;

	opt.flags = GGZ_OPT_PARSER;
	opt.global_conf = "/etc/ggz/ggz.conf";
	opt.user_conf = g_strdup_printf("%s/.ggzrc", getenv("HOME"));;

	ggzcore_init(opt);
	ggz_event_init();
	ggz_state_init();
	
	gtk_init(&argc, &argv);
	chat_allocate_colors();
	g_main_set_poll_func((GPollFunc)ggzcore_event_poll);

	win_main = create_win_main();
	ggz_sensitivity_init();
	gtk_widget_show(win_main);
	login_create_or_raise();


	gtk_main();

	ggzcore_destroy();

	return 0;
}

