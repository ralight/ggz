/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/15/00
 * $Id: main.c 5316 2003-01-12 11:41:37Z dr_maux $
 *
 * Main loop
 *
 * Copyright (C) 2000 Brent Hendricks.
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
#include <ggzcore.h>
#include "server.h"
#include "input.h"
#include "output.h"
#include "state.h"
#include "loop.h"

#include <ggz.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>


/* Event loop timeout value */
#define TIMEOUT 1


/* Termination handler */
static RETSIGTYPE term_handle(int signum)
{
        ggzcore_destroy(); 
        output_shutdown(); 
	exit(1);
}

static char* string_cat(char *s1, char *s2)
{
	char *new;

	new = ggz_malloc(strlen(s1) + strlen(s2) + 1); /* Leave space for NULL */
	strcpy(new, s1);
	strcat(new, s2);
	
	return new;
}

#if DEBUG
static void init_debug(void)
{
	char *default_file, *debug_file;
	const char **debug_types;
	int num_types, i;

	/* Inititialze debugging */
	default_file = string_cat(getenv("HOME"),"/.ggz/ggz-text.debug");
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
	char *u_path;
	GGZOptions opt;
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"reverse", no_argument, 0, 'r'}
	};
	int optindex;
	int option;
	int opt_reverse = 0;

	while(1)
	{
		option = getopt_long(argc, argv, "hrv", options, &optindex);
		if(option == -1) break;
		switch(option)
		{
			case 'h':
				printf("The GGZ Gaming Zone Text Client\n");
				printf("http://ggz.sourceforge.net/\n");
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

	output_init(opt_reverse);
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	output_banner();

	/* Use local config file */
	/*g_path = string_cat(GGZCONFDIR, "/ggz-text.rc");*/
        u_path = string_cat(getenv("HOME"), "/.ggz/ggz-text.rc");
	ggzcore_conf_initialize(NULL, u_path);
	ggz_free(u_path);

#ifdef DEBUG
	init_debug();
#endif

	/* Setup options and initialize ggzcore lib */
	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;
	ggzcore_init(opt);
	
	output_status();
	output_prompt();

	/* Event loop */
	loop_init(TIMEOUT);
	loop_add_fd(STDIN_FILENO, input_command, NULL);
	loop();

	if (server) {
		server_disconnect();
		server_destroy();
	}
	ggzcore_destroy();
	output_shutdown();

#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#endif
	
	return 0;
}

