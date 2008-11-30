/*
 * ggz-txt - Command-line core client for the GGZ Gaming Zone.
 * This application is part of the ggz-txt-client package.
 *
 * main.c: Main loop.
 *
 * Copyright (C) 2000 Brent Hendricks
 * Copyright (C) 2001-2008 GGZ Gaming Zone Development Team
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

#include <ggz.h>
#include <ggzcore.h>

#include "server.h"
#include "input.h"
#include "output.h"
#include "state.h"
#include "loop.h"
#include "support.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#ifdef HAVE_READLINE_READLINE_H
# include <readline/readline.h>
# include <sys/types.h>
# include <fcntl.h>
#endif
#ifdef HAVE_READLINE_HISTORY_H
#include <readline/history.h>
#endif

#include <locale.h>

/* Event loop timeout value */
#define TIMEOUT 1


/* Termination handler */
static RETSIGTYPE term_handle(int signum)
{
#ifdef HAVE_READLINE_HISTORY_H
	clear_history();
#endif
#ifdef HAVE_READLINE_READLINE_H
	rl_cleanup_after_signal();
	rl_reset_terminal(NULL);
#endif
	ggzcore_destroy(); 
	output_shutdown(); 

	exit(1);
}

#ifdef DEBUG
static void init_debug(void)
{
	char *default_file, *debug_file;
	const char **debug_types;
	int num_types, i;

	/* Inititialze debugging */
	default_file = ggz_strbuild("%s/.ggz/ggz-text.debug", getenv("HOME"));
	debug_file = ggzcore_conf_read_string("Debug", "File", default_file);
	ggzcore_conf_read_list("Debug", "Types", &num_types, (char***)&debug_types);
	ggz_debug_init(debug_types, debug_file);

	/* Free up memory */
	for (i = 0; i < num_types; i++)
		ggz_free(debug_types[i]);
	if (debug_types)
		ggz_free(debug_types);
	ggz_free(debug_file);
	ggz_free(default_file);
}
#endif /* DEBUG */

int main(int argc, char *argv[])
{
	char *u_path, *startup_path;
	char *autouri = NULL;
	GGZOptions opt;
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"reverse", no_argument, 0, 'r'}
	};
	int optindex = 0;
	int option;
	int opt_reverse = 0;
	FILE *startup;
	char command[1024];
#ifdef HAVE_READLINE_HISTORY_H
	char *historyfile;
#endif

#ifdef ENABLE_NLS
	bindtextdomain("ggz-txt", PREFIX "/share/locale");
	textdomain("ggz-txt");
#endif
	setlocale(LC_ALL, "");

	while(1)
	{
		option = getopt_long(argc, argv, "hrv", options, &optindex);
		if(option == -1) break;
		switch(option)
		{
			case 'h':
				printf(_("The GGZ Gaming Zone Text Client\n"));
				printf(_("http://www.ggzgamingzone.org/clients/console/\n\n"));
				printf(_("Call: ggz-txt [options] [uri]:\n"));
				printf(_("-r / --reverse: Use reverse colours\n"));
				printf(_("-h / --help: Display this help\n"));
				printf(_("-v / --version: Display version information\n"));
				exit(0);
				break;
			case 'v':
				printf("%s\n", VERSION);
				exit(0);
				break;
			case 'r':
				opt_reverse = 1;
				break;
		}
	}

	if(optind < argc) {
		autouri = argv[optind];
	}

	/* Use local config file */
	/*g_path = ggz_strbuild("%s/ggz-text.rc", GGZCONFDIR);*/
	u_path = ggz_strbuild("%s/.ggz/ggz-text.rc", getenv("HOME"));
	ggzcore_conf_initialize(NULL, u_path);
	ggz_free(u_path);

#ifdef DEBUG
	init_debug();
#endif

	/* Setup options and initialize ggzcore lib */
	if(!ggz_check_library(LIBGGZ_VERSION_IFACE)) {
		fprintf(stderr, "FATAL: libggz version mismatch (needs %s).\n", LIBGGZ_VERSION_IFACE);
		return -1;
	}

	if(!ggzcore_check_library(GGZCORE_VERSION_IFACE)) {
		fprintf(stderr, "FATAL: ggzcore version mismatch (needs %s).\n", GGZCORE_VERSION_IFACE);
		return -1;
	}

	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES | GGZ_OPT_THREADED_IO;
	ggzcore_init(opt);
	ggz_tls_init(NULL, NULL, NULL);

	/* Set up the terminal window */
	output_init(opt_reverse);
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	output_banner();

	output_status();
	output_prompt();

	/* Event loop */
	loop_init(TIMEOUT);
#ifdef HAVE_READLINE_READLINE_H
# ifdef HAVE_READLINE_HISTORY_H
	using_history();
	historyfile = ggz_strbuild("%s/.ggz/ggz-text.history", getenv("HOME"));
	read_history(historyfile);
# endif
	/*rl_callback_handler_install("\e[2K\e[1m\e[37mGGZ\e[1m\e[30m>\e[1m\e[37m ", input_commandline);*/
	rl_callback_handler_install("GGZ>> ", input_commandline);
	fcntl(STDIN_FILENO, F_SETFD, O_NONBLOCK);
	loop_add_fd(STDIN_FILENO, rl_callback_read_char, NULL);
#else
	loop_add_fd(STDIN_FILENO, input_command, NULL);
#endif

	/* Auto connection? */
	if (autouri) {
		ggz_uri_t uri = ggz_uri_from_string(autouri);
		server_init(uri.host,
			(uri.port ? uri.port : 5688),
			(uri.password ? GGZ_LOGIN : GGZ_LOGIN_GUEST),
			(uri.user ? uri.user : getenv("LOGNAME")),
			uri.password, 0, NULL);
	}

	/* Startup script? */
	startup_path = ggz_strbuild("%s/.ggz/ggz-txt.startup", getenv("HOME"));
	startup = fopen(startup_path, "r");
	ggz_free(startup_path);
	if(startup) {
		while(fgets(command, sizeof(command), startup)) {
			command[strlen(command) - 1] = '\0';
			if(strlen(command) > 0) {
				if(command[0] == '#') continue;
				output_text(_("Startup command: %s"), command);
				input_commandhandler(command);
			}
		}
		fclose(startup);
	}

	loop();

	if (server) {
		server_disconnect();
		server_destroy();
	}
	ggzcore_destroy();
	output_shutdown();

#ifdef HAVE_READLINE_HISTORY_H
	write_history(historyfile);
	ggz_free(historyfile);
	clear_history();
#endif
#ifdef HAVE_READLINE_READLINE_H
	rl_cleanup_after_signal();
	rl_reset_terminal(NULL);
#endif

#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#endif
	
	return 0;
}

