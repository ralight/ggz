/* 
 * File: ai.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.c 10226 2008-07-08 18:29:16Z jdorje $
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

#include <glob.h>
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
#include "message.h"
#include "net.h"

static ai_module_t *modules = NULL;
static char *path = NULL;

/* The "Random" module is handled separately; it doesn't require
   a specfile.  There are two reasons for this.  (1) It really is
   a separate entity, capable of functioning for all games (whereas
   other AI modules will only function for one game).  (2) The
   AI specfiles can't be read when GGZD is running under "make test";
   but if the random AI is hard-coded it can still run. */
static ai_module_t random_ai = {
	.name = "Random",
	.game = NULL, /* all games */
	.path = "ggzd.ggzcards.ai-random", /* path added later */
	.next = NULL /* not used */
};

void start_ai(player_t p, const ai_module_t *module)
{
	/* It would be really cool if we could use the ggzmod library
	   to do this part... */
	int fd_pair[2];
#ifdef DEBUG
	int err_fd_pair[2];
#endif /* DEBUG */
	int pid;
	char *argv[] = {module->path, NULL};
		
	ggz_debug(DBG_AI, "Starting AI for player %d as %s.", p, module->name);
		
	assert(get_player_status(p) == GGZ_SEAT_BOT);
	assert(game.players[p].dio == NULL);
	assert(game.players[p].pid == -1);
#ifdef DEBUG
	assert(game.players[p].err_fd == -1);
#endif
	
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
		(void) close(fd_pair[0]);
#ifdef DEBUG
		(void) close(err_fd_pair[0]);
#endif /* DEBUG */
		
		if (fd_pair[1] != 3) {
			if (dup2(fd_pair[1], 3) != 3
			    || close(fd_pair[1]) < 0)
				ggz_error_sys_exit("dup/close failed");
		}

		/* We used to close stdin, stdout, and stderr here,
		   but really that's best left up to someone else.
		   Currently GGZ does it when in daemon mode. */
		
#ifdef DEBUG
		/* Move our pipe to stderr. */
		if (err_fd_pair[1] != STDERR_FILENO
		    && (dup2(err_fd_pair[1], STDERR_FILENO) != STDERR_FILENO
		        || close(err_fd_pair[1]) < 0))
			ggz_error_sys_exit("dup/close failed");
#endif /* DEBUG */
		
		execv(argv[0], argv);
		
		ggz_error_sys_exit("exec of %s failed", argv[0]);
	} else {
		/* parent */
		(void) close(fd_pair[1]);
		game.players[p].dio = ggz_dio_new(fd_pair[0]);
		
#ifdef DEBUG
		(void) close(err_fd_pair[1]);
		game.players[p].err_fd = err_fd_pair[0];
#endif /* DEBUG */
		game.players[p].pid = pid;
	}	
}


void stop_ai(player_t p)
{
	pid_t pid;
	
	ggz_debug(DBG_AI, "Stopping AI for player %d.", p);
	
	/* Check to see if the AI has been spawned yet.  It's much easier to
	   check here than elsewhere. */
	if (game.players[p].pid < 0) {
		assert(game.players[p].dio == NULL);
#ifdef DEBUG
		assert(game.players[p].err_fd < 0);
#endif
		return;
	}

	/* Clean up process. */
	kill(game.players[p].pid, SIGINT);
	pid = waitpid(game.players[p].pid, NULL, 0);
	if (pid != game.players[p].pid)
		ggz_error_msg_exit("Killing pid %d failed.", game.players[p].pid);
	game.players[p].pid = -1;
	
	/* Clean up FD's. */
	if (close(ggz_dio_get_socket(game.players[p].dio)) < 0
#ifdef DEBUG
	    || close(game.players[p].err_fd) < 0
#endif
	   )
		ggz_error_sys("Close of AI fd's failed (player %d)", p);

	ggz_dio_free(game.players[p].dio);
	game.players[p].dio = NULL;
#ifdef DEBUG
	game.players[p].err_fd = -1;
#endif
}


/* Currently unused and untested. */
void restart_ai(player_t p)
{
	stop_ai(p);
#ifdef DEBUG
	set_global_message("", "AI player %d has stopped responding.", p);
	/* After we return here, the bot should just be ignored.  However,
	   it will never respond.  So the game will be frozen, but the
	   table can still function appropriately. */
	return;
#endif
	ggz_debug(DBG_AI, "Restarting AI for player %d.", p);
	start_ai(p, &random_ai);
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
		/* something's wrong with the AI */
		restart_ai(ai);
		return;
	}
	buf[res] = '\0'; /* yes, this is necessary */
		
	this = buf;
	while ( (next = strchr(this, '\n')) ) {
		*next = '\0';
		
		ggz_debug(DBG_AI, "AI %d: %s", ai, this);
		
		this = next + 1;
	}
	
	/* Yes, this can fail if we get more than sizeof(buf) bytes at
	   once.  Not likely! */
	if (*this != '\0')
		ggz_debug(DBG_AI, "AI %d: %s", ai, this);
}
#endif

static void read_ai_module(char *aispec)
{
	int file;
	ai_module_t *module = ggz_malloc(sizeof(*module));

	ggz_debug(DBG_AI, "Reading AI module %s.", aispec);

	file = ggz_conf_parse(aispec, GGZ_CONF_RDONLY);
	if (file < 0)
		goto error;

	module->name = ggz_conf_read_string(file,
					    "AI", "name",
					    NULL);
	module->game = ggz_conf_read_string(file,
					    "AI", "game",
					    NULL);
	module->path = ggz_conf_read_string(file,
					    "AI", "path",
					    NULL);

	if (!module->name || !module->game || !module->path) {
		ggz_error_msg("Invalid AI module %s.",
			      aispec);
		goto error;
	}

	if (module->path[0] != '/') {
		char *cmd = module->path;
		int len = strlen(path) + strlen(cmd) + 2;
		module->path = ggz_malloc(len);
		snprintf(module->path, len, "%s/%s", path, cmd);
		ggz_free(cmd);
	}

	ggz_debug(DBG_AI, "Read AI module %s for %s as %s.",
		  module->name, module->game, module->path);

	module->next = modules;
	modules = module;
	
	return;

error:
	ggz_free(module->name);
	ggz_free(module->game);
	ggz_free(module->path);
	ggz_free(module);
}

/* Record the path of the ggzcards executable; the AI modules must
   be in the same location. */
static void set_path(const char *exec_cmd)
{
	int i;

	path = ggz_strdup(exec_cmd);
	for (i = strlen(exec_cmd); path[i] != '/' && i >= 0; i--)
		path[i] = '\0';	
}

/* Initialize fallback "random" module */
static void init_random_ai(void)
{
	int len = strlen(path) + strlen(random_ai.path) + 2;
	char *rand_ai = ggz_malloc(len);

	snprintf(rand_ai, len, "%s/%s", path, random_ai.path);
	random_ai.path = rand_ai;
}

void read_ai_modules(const char *exec_cmd)
{
	const char *pattern = GGZDDATADIR "/ggzcards/*.aispec";
	glob_t pglob;
	int i;

	set_path(exec_cmd);

	init_random_ai();

	ggz_debug(DBG_AI, "Looking for files %s.", pattern);
	glob(pattern, GLOB_ERR | GLOB_MARK, NULL, &pglob);

	for (i = 0; i < pglob.gl_pathc; i++)
		read_ai_module(pglob.gl_pathv[i]);

	globfree(&pglob);
}

ai_module_t *choose_ai_module(void)
{
	ai_module_t *best = &random_ai;
	ai_module_t *module;

	for (module = modules; module; module = module->next) {
		if (!strcmp(module->game, game.data->name)) {
			/* FIXME - give the user a choice if there's more
			   than one available module. */
			best = module;
		}
	}

	/* NULL can be returned; this means we have no module (and use
	   the fallback random AI) */
	return best;
}
