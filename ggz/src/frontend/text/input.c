/*
 * File: input.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 *
 * Functions for inputing commands from the user
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
#include "input.h"
#include "output.h"

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE_LENGTH 80

static void input_handle_connect(char* line);
static char delim[] = " \n";


int input_command(short events)
{
	char line[LINE_LENGTH];
	char* command;

	printf("user input detected\n");
	if (events & POLLIN) {
		if (!fgets(line, sizeof(line)/sizeof(char), stdin))
			return -1;
			
		command = strtok(line, delim);
		printf("user command: %s\n", command);
		if (strcmp(command, "connect") == 0) {
			input_handle_connect((char*)line);
		}
		else if (strcmp(command, "disconnect") == 0) {
			ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);
		}
		
	}
	return 0;
}


static void input_handle_connect(char* line)
{
	GGZProfile *profile;
	char* arg;

	if (!(profile = calloc(1, sizeof(GGZProfile))))
		ggzcore_error_sys_exit("malloc() failed in input_handle_connect");

	arg = strtok(NULL, delim);
	if (arg && strcmp(arg, "") != 0)
		profile->host = strdup(arg);
	else 
		profile->host = strdup("localhost");
	
	arg = strtok(NULL, delim);
	if (arg && strcmp(arg, "") != 0)
		profile->port = atoi(arg);
	else
		profile->port = 5688;

	profile->type = GGZ_LOGIN_GUEST;
	profile->login = strdup(getenv("LOGNAME"));

	/* FIXME: provide a destroy function that frees the appropriate mem */
	ggzcore_event_trigger(GGZ_USER_LOGIN, profile, NULL);
}







