/*
 * Chess game module for Guru
 * Variant: Interface for GNU Chess/XBoard AI
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

#include "ai-gnuchess.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

static pid_t pid;
static int xboardchannel;
static int death;
static char move[32];
static int movestatus;
static int gameover;

static void gnuchess_wait(void);

/* Public part: interface to game server */

void gnuchess_ai_init(int color, int depth)
{
	/* unsupported */
	return;
}

int gnuchess_ai_move(int from, int to, int force)
{
	char str[32];

	str[0] = (from % 8) + 'a';
	str[1] = (from / 8) + '0' + 1;
	str[2] = (to % 8) + 'a';
	str[3] = (to / 8) + '0' + 1;
	str[4] = '\n';
	str[5] = '\0';

	write(xboardchannel, str, strlen(str));

	return 1;
}

int gnuchess_ai_find(int color, int *from, int *to)
{
	int x, y;

	gnuchess_wait();
	if(!gnuchess_check()) return 0;

	/* FIXME: actually, this is the client's fault */
	if(movestatus == 2) return 0;

	y = move[0] - 'a';
	x = move[1] - '0' - 1;
	*from = (y + x * 8);
	y = move[2] - 'a';
	x = move[3] - '0' - 1;
	*to = (y + x * 8);

	return 1;
}

void gnuchess_ai_output(void)
{
	/* unsupported */
	return;
}

int gnuchess_ai_rochade(int color, int which)
{
	/* unsupported */
	return 0;
}

int gnuchess_ai_exchange(int pos, int *figure)
{
	/* unsupported */
	return 0;
}

int gnuchess_ai_checkmate(void)
{
	/* unsupported */
	return gameover;
}

/* Internal part: interface to gnuchess */

/* 1 = success, 0 = failure */
int gnuchess_launch()
{
	char *const argv[] = {"gnuchess", "--xboard", NULL};
	int channel[2];

	socketpair(AF_UNIX, SOCK_STREAM, 0, channel);

	pid = fork();
	if(pid == -1)
	{
		fprintf(stderr, "xboard AI: could not fork\n");
		return 0;
	}
	else if(pid == 0)
	{
		/* we're the d00d! */
		dup2(channel[0], STDIN_FILENO);
		dup2(channel[0], STDOUT_FILENO);
		execvp(argv[0], argv);
		fprintf(stderr, "xboard AI: aliens ate my cat\n");
		_exit(-1);
	}

	/* parent */
	fcntl(channel[1], F_SETFL, O_NONBLOCK);
	xboardchannel = channel[1];
	death = 0;
	gameover = 0;

	return 1;
}

/* death = 1: ok, 0: gnuchess has exited */
int gnuchess_check()
{
	return !death;
}

void gnuchess_shutdown()
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
	int seq;
	char answer[32];

	orig = strdup(buffer);
	tok = strtok(orig, "\n");
	while(tok)
	{
		/*ggz_debug(DEBUG_AI, "# %s\n", tok);*/

		ret = sscanf(tok, "%d. ... %s", &seq, answer);
		if(ret == 2)
		{
			/*ggz_debug(DEBUG_AI, "** move! (%i, %s)\n", seq, answer);*/
			move[0] = answer[0];
			move[1] = answer[1];
			move[2] = answer[2];
			move[3] = answer[3];
			move[4] = '\0';
			movestatus = 1;
		}
		else
		{
			if(strstr(answer, "Illegal move"))
				movestatus = 2;
			if(strstr(answer, "wins as black"))
				gameover = 1;
		}

		tok = strtok(NULL, "\n");
	}
	free(orig);
}

void gnuchess_wait()
{
	/*int status;*/
	int ret;
	char buffer[256];

	movestatus = 0;
	while(movestatus == 0)
	{
		ret = read(xboardchannel, buffer, sizeof(buffer));
		if(ret > 0)
		{
			buffer[ret] = 0;
			examine(buffer);
		}
		else if(ret == 0)
		{
			fprintf(stderr, "xboard AI: ciao\n");
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

