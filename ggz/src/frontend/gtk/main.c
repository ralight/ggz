/*
 * File: main.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: main.c 5197 2002-11-04 00:31:34Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000-2002 Justin Zaun.
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

#include "config.h"

#include <ggz.h>   /* libggz */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifdef ENABLE_NLS
#include <libintl.h>
#endif

#include <ggzcore.h>
#include <ggz.h>

#include "about.h"
#include "chat.h"
#include "client.h"
#include "first.h"
#include "ggzclient.h"
#include "license.h"
#include "login.h"
#include "server.h"
#include "support.h"


static void init_debug(void)
{
	char *default_file, *debug_file, **debug_types;
	int num_types, i;

	/* Inititialze debugging */
	default_file = g_strjoin("/", getenv("HOME"), ".ggz/ggz-gtk.debug", NULL);
	debug_file = ggzcore_conf_read_string("Debug", "FILE", default_file);
	g_free(default_file);

	ggzcore_conf_read_list("Debug", "Types", &num_types, (char***)&debug_types);
	ggz_debug_init((const char**)debug_types, debug_file);

	/* Free up memory */
	for (i = 0; i < num_types; i++)
		ggz_free(debug_types[i]);
	ggz_free(debug_types);
	ggz_free(debug_file);
}


int main (int argc, char *argv[])
{
	GGZOptions opt;
	char *global_conf, *user_conf, *init_version;
	
#ifdef ENABLE_NLS
	bindtextdomain("ggz-gtk", PACKAGE_LOCALE_DIR);
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
	
	ggzcore_init(opt);

#ifdef DEBUG
	init_debug();
#endif

	server_profiles_load();
	
	gtk_init(&argc, &argv);
	chat_init();

	init_version = ggzcore_conf_read_string("INIT", "VERSION", VERSION);
	if (ggzcore_conf_read_int("INIT", "FIRST", 0) == 0 ||
	    strcmp(init_version, VERSION) !=0 )
	{
		first_create_or_raise();
	} else {
		win_main = create_win_main();
		ggz_sensitivity_init();
		gtk_widget_show(win_main);
		login_create_or_raise();
	}
	ggz_free(init_version);

	gtk_main();

	chat_save_lists();
	server_profiles_cleanup();
	chat_lists_cleanup();

	ggzcore_destroy();
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#endif	

	return 0;
}
