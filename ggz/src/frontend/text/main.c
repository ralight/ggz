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

#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>

#define TIMEOUT 500

/* Termination handler */
RETSIGTYPE term_handle(int signum)
{
        ggzcore_event_process_all(); 
        ggzcore_destroy(); 
        output_shutdown(); 
	exit(1);
}

int main(void)
{
	GGZOptions opt;
	struct pollfd fd[1] = {{STDIN_FILENO, POLLIN, 0}};

	output_init();
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	output_banner();

	/* Setup options and initialize ggzcore lib */
	opt.flags = GGZ_OPT_PARSER;
	opt.global_conf = "/etc/ggz-text.rc";
	opt.user_conf = "~/.ggz-txtrc";
	opt.local_conf = NULL;

	ggzcore_init(opt);
	
	ggzcore_event_connect(GGZ_SERVER_LOGIN, server_login_ok);
	ggzcore_event_connect(GGZ_SERVER_CONNECT_FAIL, server_connect_fail);
	ggzcore_event_connect(GGZ_SERVER_LOGIN_FAIL, server_login_fail);
	ggzcore_event_connect(GGZ_SERVER_CHAT_MSG, server_chat_msg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_ANNOUNCE, server_chat_announce);
	ggzcore_event_connect(GGZ_SERVER_CHAT_PRVMSG, server_chat_prvmsg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_BEEP, server_chat_beep);
	ggzcore_event_connect(GGZ_SERVER_LOGOUT, server_logout);
	ggzcore_event_connect(GGZ_SERVER_ROOM_ENTER, server_room_enter);
	ggzcore_event_connect(GGZ_SERVER_ROOM_LEAVE, server_room_leave);

	output_prompt(1);
	for (;;) {
		if (poll(fd, 1, TIMEOUT)) {
			if (input_command(fd[0].revents) < 0)
				break;
			else
				output_prompt(0);
		}
		ggzcore_event_process_all();
		output_status();
	}

	ggzcore_event_process_all();
	ggzcore_destroy();

	output_shutdown();

	return 0;
}
