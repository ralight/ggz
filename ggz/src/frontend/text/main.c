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
#include "loop.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>


/* Event loop timeout value */
#define TIMEOUT 1


/* Termination handler */
RETSIGTYPE term_handle(int signum)
{
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
	char *u_path, *debugfile;
	GGZOptions opt;

	output_init();
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	output_banner();

	/* Setup options and initialize ggzcore lib */
	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;
	/*g_path = string_cat(GGZCONFDIR, "/ggz-text.rc");*/
        u_path = string_cat(getenv("HOME"), "/.ggz/ggz-text.rc");
	ggzcore_conf_initialize(NULL, u_path);
	free(u_path);

	debugfile = string_cat(getenv("HOME"),"/.ggz/ggz-text.debug");
	opt.debug_file = ggzcore_conf_read_string("Debug", "File", debugfile);
	free(debugfile);
	opt.debug_levels = (GGZ_DBG_ALL & ~GGZ_DBG_HOOK & ~GGZ_DBG_MEMDETAIL); 
	ggzcore_init(opt);
	free(opt.debug_file);

	output_status();
	output_prompt();

	/* Event loop */
	loop_init(TIMEOUT);
	loop_add_fd(STDIN_FILENO, input_command, NULL);
	loop();

	ggzcore_destroy();

	output_shutdown();

	return 0;
}

