/*
 * File: main.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: main.c 7737 2006-01-06 22:34:17Z jdorje $
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
	default_file = g_strjoin("/", getenv("HOME"), ".ggz/ggz-gtk.debug", NULL);
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

	ggz_gtk_initialize(NULL, NULL, NULL, NULL);

#ifdef DEBUG
	init_debug();
#endif

#if GTK_CHECK_VERSION(2, 6, 0)
	/* Support for command line options since Gtk+ 2.6 */

	gchar **option_urls = NULL;

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

	if(option_urls)
		option_url = option_urls[0];

#else
	gtk_init(&argc, &argv);
#endif

	init_version = ggzcore_conf_read_string("INIT", "VERSION", VERSION);
	if (ggzcore_conf_read_int("INIT", "FIRST", 0) == 0 ||
	    strcmp(init_version, VERSION) !=0 )
	{
		first_create_or_raise();
	} else {
		win_main = create_win_main();
		ggz_sensitivity_init();
		gtk_widget_show_all(win_main);
		ggz_gtk_login_raise();

		/* Auto-connect to GGZ URI */
		if (option_url) {
			char *uri, *uribase;
			char *host = NULL, *user = NULL, *port = NULL;
			uribase = ggz_strdup(option_url);
			uri = uribase;
			if (strstr(uri, "ggz://"))
				uri += 6;
			if (strchr(uri, '@'))
				user = strsep(&uri, "@");
			if (strchr(uri, ':')) {
				host = strsep(&uri, ":");
				port = strsep(&uri, ":");
			} else {
				host = uri;
			}

			Server serv;
			serv.name = NULL;
			serv.host = host;
			serv.port = atoi(port);
			serv.type = GGZ_LOGIN_GUEST;
			serv.login = user;
			serv.password = NULL;

			login_set_entries(serv);

			if(host)
				ggz_free(host);
			if(port)
				ggz_free(port);
			if(user)
				ggz_free(user);
			ggz_free(uribase);
		}
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
