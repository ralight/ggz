/*
 * Interface to the Velena Four-in-a-row AI
 * Copyright (C) 2006 Josef Spillner, josef@ggzgamingzone.org
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ai-velena.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

static pid_t pid;
static int enginechannel;
static int death;
static int movestatus;
static int gameover;

static int velena_rows;
static int velena_columns;
static int velena_level;
static char board[256];
static int lastcolumn;

static void velena_wait(void);

/* Public part: interface to game server */

void velena_ai_init(int rows, int columns, int level)
{
	velena_rows = rows;
	velena_columns = columns;
	velena_level = level;

	board[0] = '\0';
}

int velena_ai_move(int column)
{
	snprintf(board, sizeof(board), "%s%i", board, column);

	/* FIXME: how can we know if a player move succeeded? */
	/* -> check min/max column at least */
	return 1;
}

int velena_ai_find(int *column)
{
	char level;
	char str[300];

	if(velena_level == VELENA_EASY) level = 'a';
	else if(velena_level == VELENA_MEDIUM) level = 'b';
	else if(velena_level == VELENA_HARD) level = 'c';
	else return 0;

	snprintf(str, sizeof(str), "%c%s0\n", level, board);
	write(enginechannel, str, strlen(str));

	lastcolumn = -1;
	velena_wait();
	if(!velena_check()) return 0;
	if(lastcolumn == -1) return 0;

	*column = lastcolumn;

	return 1;
}

void velena_ai_output(void)
{
	/* unsupported */
	return;
}

int velena_ai_gameover(void)
{
	/* unsupported */
	return gameover;
}

/* Internal part: interface to velena */

/* 1 = success, 0 = failure */
int velena_launch()
{
	char *const argv[] = {"velena-connectx", NULL};
	int channel[2];

	socketpair(AF_UNIX, SOCK_STREAM, 0, channel);

	pid = fork();
	if(pid == -1)
	{
		fprintf(stderr, "velena AI: could not fork\n");
		return 0;
	}
	else if(pid == 0)
	{
		dup2(channel[0], STDIN_FILENO);
		dup2(channel[0], STDOUT_FILENO);
		execvp(argv[0], argv);
		fprintf(stderr, "velena AI: unexpected exec error\n");
		_exit(-1);
	}

	/* parent */
	fcntl(channel[1], F_SETFL, O_NONBLOCK);
	enginechannel = channel[1];
	death = 0;
	gameover = 0;

	return 1;
}

/* death = 1: ok, 0: velena has exited */
int velena_check()
{
	return !death;
}

void velena_shutdown()
{
	kill(pid, SIGTERM);
	kill(pid, SIGKILL);
	death = 1;
}

static void examine(const char *buffer)
{
	char *orig = NULL;
	char *tok = NULL;
	int ret;
	int column;

	orig = strdup(buffer);
	tok = strtok(orig, "\n");
	while(tok)
	{
		/*ggz_debug(DEBUG_AI, "# %s\n", tok);*/

		ret = sscanf(tok, "Velena answers in %d", &column);
		if(ret == 1)
		{
			/*ggz_debug(DEBUG_AI, "** move! (%i)\n", column);*/
			lastcolumn = column;
			movestatus = 1;
		}
		else
		{
			if(strstr(tok, "Syntax error"))
				movestatus = 2;
			if(strstr(tok, "Positional error"))
				gameover = 1;
		}

		tok = strtok(NULL, "\n");
	}
	free(orig);
}

void velena_wait()
{
	int ret;
	char buffer[256];

	movestatus = 0;
	while(movestatus == 0)
	{
		ret = read(enginechannel, buffer, sizeof(buffer));
		if(ret > 0)
		{
			buffer[ret] = 0;
			examine(buffer);
		}
		else if(ret == 0)
		{
			fprintf(stderr, "velena AI: ciao\n");
			death = 1;
			break;
		}

		ret = waitpid(pid, NULL, WNOHANG);
		if(ret != 0)
		{
			death = 1;
			break;
		}
	}
}

