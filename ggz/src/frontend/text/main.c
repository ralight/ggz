/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/15/00
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

#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>


#define TIMEOUT 500

/* Termination handler */
RETSIGTYPE term_handle(int signum)
{
        ggzcore_event_process_all(); 
        ggzcore_destroy(); 
        output_shutdown(); 
	exit(1);
}

char* string_cat(char *s1, char *s2)
{
	char *new;

	new = malloc(strlen(s1) + strlen(s2) + 1); /* Leave space for NULL */
	
	if (!new)  /* malloc() failed */
		exit(-1);
		
	strcpy(new, s1);
	strcat(new, s2);

	return new;
}
	

int main(void)
{
	char *g_path, *u_path, *debugfile;
	GGZOptions opt;
	struct pollfd fd[1] = {{STDIN_FILENO, POLLIN, 0}};

	output_init();
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	output_banner();

	/* Setup options and initialize ggzcore lib */
	opt.flags = GGZ_OPT_PARSER;
	g_path = string_cat(GGZCONFDIR, "/ggz-text.rc");
        u_path = string_cat(getenv("HOME"), "/.ggz/ggz-text.rc");
	ggzcore_conf_initialize(g_path, u_path);
	free(g_path);
	free(u_path);

	debugfile = ggzcore_conf_read_string("Debug", "File", "/tmp/ggz-text.debug");
	opt.debug_file = malloc((strlen(debugfile)+10)*sizeof(char*));
	sprintf(opt.debug_file, "%s.%d", debugfile, getpid());
	opt.debug_levels = (GGZ_DBG_ALL & ~GGZ_DBG_POLL); 

	ggzcore_init(opt);
	free(opt.debug_file);

	/* Register for callbacks */
	server_register();
	state_register();

	output_status();
	output_prompt();
	for (;;) {
		if (ggzcore_event_poll(fd, 1, TIMEOUT)) {
			if (input_command(fd[0].revents) < 0)
				break;
			else
				output_prompt();
		}
		output_status();
	}

	ggzcore_event_process_all();
	ggzcore_destroy();

	output_shutdown();

	return 0;
}
