/* 
 * File: ai.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.c 3425 2002-02-20 03:45:35Z jdorje $
 *
 * This file contains the frontend for GGZCards' AI module.
 * Specific AI's are in the ai/ directory.  This file contains an array
 * of AI function sets.  Each game specifies its AI.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ai.h"
#include "common.h"

static char* path = NULL;

const char *ai_get_name(player_t p)
{
	const char *name;
	
	name = get_player_name(p);
	if (name == NULL)
		name = "Bot";
	return name;
}


void start_ai(player_t p, char* ai_type)
{
	/* It would be really cool if we could use the ggzmod library
	   to do this part... */
	int fd_pair[2];
	int pid;
	char cmd[1024];
	char *argv[] = {cmd, NULL};
	
	if (ai_type == NULL)
		ai_type = "random";
		
	assert(get_player_status(p) == GGZ_SEAT_BOT);
	
	snprintf(cmd, sizeof(cmd),
	         "%s/ggzd.ggzcards.ai-%s", path, ai_type);
	
	printf("We're going to exec %s.\n\n", cmd);
	
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair) < 0)
		ggz_error_sys_exit("socketpair failed");
		
	if ( (pid = fork()) < 0)
		ggz_error_sys_exit("fork failed");
	else if (pid == 0) {
		/* child */
		close(fd_pair[0]);
		
		if (fd_pair[1] != 3) {
			if (dup2(fd_pair[1], 3) != 3
			    || close(fd_pair[1]) < 0)
				ggz_error_sys_exit("dup/close failed");
		}
		
		execv(argv[0], argv);
		
		ggz_error_sys_exit("exec of %s failed", argv[0]);
	} else {
		/* parent */
		close(fd_pair[1]);
		
		game.players[p].fd = fd_pair[0];
		game.players[p].pid = pid;
	}	
}
			
void init_path(const char *exec_cmd)
{
	int i = strlen(exec_cmd);
	
	path = ggz_strdup(exec_cmd);
	
	while (path[i] != '/') {
		path[i] = '\0';
		i--;
	}
}
