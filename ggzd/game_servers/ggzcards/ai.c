/* 
 * File: ai.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.c 3978 2002-04-14 08:10:28Z jdorje $
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

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wait.h>

#include "ai.h"
#include "common.h"
#include "net.h"

static char* path = NULL;

const char *ai_get_name(player_t p)
{
	const char *name;
	
	name = get_player_name(p);
	if (name == NULL)
		name = "Bot";
	return name;
}


void start_ai(game_t *g, player_t p, char* ai_type)
{
	/* It would be really cool if we could use the ggzmod library
	   to do this part... */
	int fd_pair[2];
#ifdef DEBUG
	int err_fd_pair[2];
#endif /* DEBUG */
	int pid;
	char cmd[1024];
	char *argv[] = {cmd, NULL};
	
	if (ai_type == NULL)
		ai_type = "random";
		
	assert(get_player_status(p) == GGZ_SEAT_BOT);
	assert(game.players[p].fd == -1);
	assert(game.players[p].pid == -1);
#ifdef DEBUG
	assert(game.players[p].err_fd == -1);
#endif
	
	snprintf(cmd, sizeof(cmd),
	         "%s/ggzd.ggzcards.ai-%s", path, ai_type);
	
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair) < 0
#ifdef DEBUG
	    || pipe(err_fd_pair) < 0
#endif /* DEBUG */
	   )
		ggz_error_sys_exit("socketpair/pipe failed");

	if ( (pid = fork()) < 0)
		ggz_error_sys_exit("fork failed");
	else if (pid == 0) {
		/* child */
		close(fd_pair[0]);
#ifdef DEBUG
		close(err_fd_pair[0]);
#endif /* DEBUG */
		
		if (fd_pair[1] != 3) {
			if (dup2(fd_pair[1], 3) != 3
			    || close(fd_pair[1]) < 0)
				ggz_error_sys_exit("dup/close failed");
		}
		
		/* Close stdin, stdout, and stderr. */
		if (fclose(stdin) < 0 ||
		    fclose(stdout) < 0 ||
		    fclose(stderr) < 0) {
			/* There's not much we can do... */
			assert(0);
		}
		
#ifdef DEBUG
		/* Move our pipe to stderr. */
		assert(err_fd_pair[1] != 2);
		if (dup2(err_fd_pair[1], 2) != 2
		    || close(err_fd_pair[1]) < 0)
			ggz_error_sys_exit("dup/close failed");
#endif /* DEBUG */
		
		execv(argv[0], argv);
		
		ggz_error_sys_exit("exec of %s failed", argv[0]);
	} else {
		/* parent */
		close(fd_pair[1]);
		g->players[p].fd = fd_pair[0];
		
#ifdef DEBUG
		close(err_fd_pair[1]);
		g->players[p].err_fd = err_fd_pair[0];
#endif /* DEBUG */
		g->players[p].pid = pid;
	}	
}


void stop_ai(player_t p)
{
	pid_t pid;

	/* Clean up process. */
	assert(game.players[p].pid > 0);
	kill(game.players[p].pid, SIGINT);
	pid = waitpid(game.players[p].pid, NULL, 0);
	if (pid != game.players[p].pid)
		ggz_error_msg_exit("Killing pid %d failed.", game.players[p].pid);
	game.players[p].pid = -1;
	
	/* Clean up FD's. */
	if (close(game.players[p].fd) < 0
#ifdef DEBUG
	    || close(game.players[p].err_fd) < 0
#endif
	   )
		ggz_error_sys("Close of AI fd's failed (player %d)", p);
		
	game.players[p].fd = -1;
#ifdef DEBUG
	game.players[p].err_fd = -1;
#endif
}


/* Currently unused and untested. */
void restart_ai(player_t p)
{
	stop_ai(p);
	start_ai(&game, p, "random");
	send_sync(p);
}


#ifdef DEBUG
/* We handle debugging messages from the AI in a very cool / hackish way.
   When we fork off the AI process, we open up a pipe FD from stderr to
   us.  We monitor that FD along with all of our thousands of other FD's.
   When the AI process writes to stderr, the data comes to us and we read
   it in this function. */
void handle_ai_stderr(player_t ai)
{
	char buf[4096], *this, *next;
	int res = read(game.players[ai].err_fd, buf, sizeof(buf) - 1);
	
	if (res < 0) {
		/* FIXME: something's wrong with the AI */
	}
	buf[res] = '\0'; /* yes, this is necessary */
		
	this = buf;
	while ( (next = strchr(this, '\n')) ) {
		*next = '\0';
		
		ggzdmod_log(game.ggz, "AI %d: %s", ai, this);
		
		this = next + 1;
	}
	
	/* Yes, this can fail if we get more than sizeof(buf) bytes at
	   once.  Not likely! */
	if (*this != '\0')
		ggzdmod_log(game.ggz, "AI %d: %s", ai, this);
}
#endif
			
void init_path(const char *exec_cmd)
{
	int i = strlen(exec_cmd);
	
	path = ggz_strdup(exec_cmd);
	
	while (path[i] != '/') {
		path[i] = '\0';
		i--;
	}
}
