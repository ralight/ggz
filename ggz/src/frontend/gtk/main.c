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
#include <unistd.h>
#ifdef ENABLE_NLS
#include <libintl.h>
#endif

#include "about.h"
#include "chat.h"
#include "client.h"
#include "first.h"
#include "ggz.h"
#include "license.h"
#include "login.h"
#include "server.h"
#include "support.h"

extern GtkWidget *win_main;
GGZServer *server = NULL;



int main (int argc, char *argv[])
{
	GGZOptions opt;
	char *global_conf, *user_conf, *debugfile;

#ifdef ENABLE_NLS
	bindtextdomain("ggz-gtk", NULL);
	textdomain("ggz-gtk");
	setlocale(LC_ALL, "");
#endif
	
	/*global_conf = "/etc/ggz/ggz.conf";*/
	/* We don't support this quite yet */
	global_conf = NULL;
	user_conf = g_strdup_printf("%s/.ggz/ggz-gtk.rc", getenv("HOME"));
	ggzcore_conf_initialize(global_conf, user_conf);
	g_free(user_conf);

	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;

	/* Provide a sensible default for the debug file */
	debugfile = g_strjoin("/", getenv("HOME"), ".ggz/ggz-gtk.debug", NULL);
	opt.debug_file = ggzcore_conf_read_string("DEBUG", "FILE", debugfile);
	g_free(debugfile);

	opt.debug_levels = (GGZ_DBG_ALL & ~GGZ_DBG_POLL & ~GGZ_DBG_MEMDETAIL);
	ggzcore_init(opt);
	free(opt.debug_file);
	server_profiles_load();
	
	gtk_init(&argc, &argv);
	chat_allocate_colors();

	if (ggzcore_conf_read_int("INIT", "FIRST", 0) == 0 ||
	    strcmp(ggzcore_conf_read_string("INIT", "VERSION", VERSION), VERSION) !=0 )
	{
		first_create_or_raise();
	} else {
		win_main = create_win_main();
		ggz_sensitivity_init();
		gtk_widget_show(win_main);
		login_create_or_raise();
	}

	gtk_main();

	ggzcore_destroy();

	return 0;
}

