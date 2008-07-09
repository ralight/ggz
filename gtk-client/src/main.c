/*
 * File: main.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: main.c 10242 2008-07-09 00:48:24Z jdorje $
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
#include <locale.h>
#include <libintl.h>
#endif
#ifdef WIN32
/* windows.h required for LoadLibrary */
#include <windows.h>
#endif

#include <ggzcore.h>
#include <ggz.h>

#include "about.h"
#include "chat.h"
#include "client.h"
#include "first.h"
#include "ggzclient.h"
#include "ggz-gtk.h"
#include "license.h"
#include "login.h"
#include "server.h"
#include "support.h"

/* Global command line option */
static gchar *option_url = NULL;

static void init_debug(void)
{
#ifdef WIN32
	/* exchndl.dll is the exception handler library from mingw-utils
	   If the app causes an exception, exchndl.dll will write a back
	   trace to $PROGNAME.RPT. If exchndl.dll is not found, the app
	   can just continue as normal.
	*/
	if (LoadLibrary("exchndl.dll") == NULL){
		fprintf(stderr, "exchndl.dll could not be loaded\n");
	}
#endif

	char *default_file, *debug_file, **debug_types;
	int num_types, i;

	/* Inititialze debugging */
#ifdef WIN32
	default_file = g_strjoin("\\", getenv("APPDATA"), ".ggz\\ggz-gtk.debug", NULL);
#else
	default_file = g_strjoin("/", getenv("HOME"), ".ggz/ggz-gtk.debug", NULL);
#endif
	debug_file = ggzcore_conf_read_string("Debug", "FILE", default_file);
	g_free(default_file);

	ggzcore_conf_read_list("Debug", "Types", &num_types, (char***)&debug_types);
	ggz_debug_init((const char**)debug_types, debug_file);

	/* Free up memory */
	for (i = 0; i < num_types; i++)
		ggz_free(debug_types[i]);
	if (debug_types) ggz_free(debug_types);
	ggz_free(debug_file);
}


int main (int argc, char *argv[])
{
	char *init_version;
	gboolean ret;
	
#ifdef ENABLE_NLS
	/* GTK2 always uses UTF-8 so we tell gettext to output its
	 * translations in that. */
	bindtextdomain("ggz-gtk", PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset("ggz-gtk", "UTF-8");
	textdomain("ggz-gtk");
	setlocale(LC_ALL, "");
#endif

	ggz_gtk_initialize(TRUE, NULL, NULL, NULL, NULL, NULL, NULL);

#ifdef DEBUG
	init_debug();
#endif

#if GTK_CHECK_VERSION(2, 6, 0)
	/* Support for command line options since Gtk+ 2.6 */

	gchar **option_urls = NULL;
	char *option_log = NULL;

	GOptionEntry entries[] = {
		{"log", 'l', 0, G_OPTION_ARG_STRING, &option_log,
			N_("Enable session log"), "file/stderr"},
		{G_OPTION_REMAINING, ' ', 0, G_OPTION_ARG_STRING_ARRAY, &option_urls,
			N_("URL for autoconnection"), NULL},
		{NULL},
	};

	ret = gtk_init_with_args(&argc, &argv,
		N_("GGZ Core Client for Gtk+"),
		entries,
		"ggz-gtk",
		NULL);
	if(!ret) {
		fprintf(stderr, _("Wrong arguments, try %s --help.\n"), argv[0]);
		return -1;
	}

	login_set_option_log(option_log);

	if(option_urls)
		option_url = option_urls[0];

#else
	gtk_init(&argc, &argv);
#endif

	init_version = ggzcore_conf_read_string("INIT", "VERSION", VERSION);
	main_window = create_win_main();
	ggz_sensitivity_init();

	if (ggzcore_conf_read_int("INIT", "FIRST", 0) == 0 ||
	    strcmp(init_version, VERSION) !=0 ) {
		first_raise();
	}

	gtk_widget_show_all(main_window);

	/* Auto-connect to GGZ URI */
	if (option_url) {
		ggz_uri_t uri;

		uri = ggz_uri_from_string(option_url);

		Server serv;
		serv.name = NULL;
		serv.host = uri.host;
		serv.port = (uri.port ? uri.port : 5688);
		serv.type = (uri.password ? GGZ_LOGIN : GGZ_LOGIN_GUEST);
		serv.login = uri.user;
		serv.password = uri.password;

		login_set_entries(serv);

		ggz_uri_free(uri);
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
