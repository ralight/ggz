/*
 * TicTacToe game module for Guru
 * Copyright (C) 2003 Josef Spillner, josef@ggzgamingzone.org
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ggz_common.h>
#include <ggzmod.h>

GGZMod *mod = NULL;
int gamefd = -1;
int gamenum = -1;
int gamemove = -1;
char gameturn = -1;
int seats[2];
char names[2][17];

char board[9];
int movemove = 0;

#define TTT_MSG_SEAT     0
#define TTT_MSG_PLAYERS  1
#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1

static void game_handle_io(void)
{
	int op, i;
	int nummove, move;
	char status, space, winner;
	
	if (ggz_read_int(gamefd, &op) < 0)
	{
		/* ... */
		return;
	}

	switch(op)
	{
		case TTT_MSG_SEAT:
			ggz_read_int(gamefd, &gamenum);
			break;
		case TTT_MSG_PLAYERS:
			for(i = 0; i < 2; i++)
			{
				ggz_read_int(gamefd, &seats[i]);
				if(seats[i] != GGZ_SEAT_OPEN)
				{
					ggz_read_string(gamefd, names[i], 17);
				}
			}
			break;
		case TTT_REQ_MOVE:
			for(i = 0; i < 9; i++)
			{
				if(board[i] == ' ')
				{
					gamemove = i;
					movemove = 1;
					break;
				}
			}
			break;
		case TTT_RSP_MOVE:
			ggz_read_char(gamefd, &status);
			if(status == 0) board[gamemove] = (gamenum == 0 ? 'x' : 'o');
			break;
		case TTT_MSG_MOVE:
			ggz_read_int(gamefd, &nummove);
			ggz_read_int(gamefd, &move);
			board[move] = (nummove == 0 ? 'x' : 'o');
			break;
		case TTT_SND_SYNC:
			ggz_read_char(gamefd, &gameturn);
			for(i = 0; i < 9; i++)
			{
				ggz_read_char(gamefd, &space);
				if(space < 0) board[i] = ' ';
				else board[i] = (space == 0 ? 'x' : 'o');
			}
			break;
		case TTT_MSG_GAMEOVER:
			ggz_read_char(gamefd, &winner);
			gameturn = -1;
			break;
	}
}

static void handle_ggz()
{
	ggzmod_dispatch(mod);
}

static void handle_ggzmod_server(GGZMod * ggzmod, GGZModEvent e, void *data)
{
	int fd = *(int*)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	gamefd = fd;
}

static void ggz_init()
{
	int i, ret;

	for(i = 0; i < 9; i++)
		board[i] = ' ';

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_ggzmod_server);

	ret = ggzmod_connect(mod);
	if(ret < 0)
	{
		fprintf(stderr, "Couldn't initialize GGZ.\n");
		exit(-1);
	}
}

static void ggz_finish()
{
	ggzmod_disconnect(mod);
	ggzmod_free(mod);
}

static void ggz_network()
{
	int maxfd;
	fd_set set;
	struct timeval tv;
	int ret;
	int serverfd;

	FD_ZERO(&set);

	serverfd = ggzmod_get_fd(mod);

	FD_SET(serverfd, &set);
	maxfd = serverfd;
	if(gamefd >= 0)
	{
		FD_SET(gamefd, &set);
		if(gamefd > maxfd) maxfd = gamefd;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select(maxfd + 1, &set, NULL, NULL, &tv);

	if(ret)
	{
		if(gamefd >= 0)
		{
			if(FD_ISSET(gamefd, &set)) game_handle_io();
		}
		if(FD_ISSET(serverfd, &set)) handle_ggz();
	}
}

int main(int argc, char *argv[])
{
	int done;

	ggz_init();

	done = 0;
	while (!done)
	{
		ggz_network();

		if(movemove == 1)
		{
			sleep(2);
			ggz_write_int(gamefd, TTT_SND_MOVE);
			ggz_write_int(gamefd, gamemove);
			movemove = 0;
		}
	}

	ggz_finish();

	return 0;
}

