/*
 * GGZ Gaming Zone TicTacToe 3D
 * Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
 * $Id: main.c 6903 2005-01-25 18:57:38Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include "display.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_WINSOCK_H
# include <winsock.h>
#endif

#include <SDL/SDL.h>

#include <ggz_common.h>
#include <ggzmod.h>

GGZMod *mod = NULL;
int gamefd = -1;
int gamenum = -1;
int gamemove = -1;
char gameturn = -1;
int seats[2];
char names[2][17];

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
			cursor = 0;
			cursormove = 2;
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

static void handle_ggzmod_server(GGZMod * ggzmod, GGZModEvent e,
				 const void *data)
{
	const int *fd = data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	gamefd = *fd;
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
	SDL_Surface *screen;
	int done;
	Uint8 *keys;

	SDL_Init(SDL_INIT_VIDEO);

	ggz_init();

	screen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL | SDL_RESIZABLE);
	if (!screen)
	{
    	fprintf(stderr, "Couldn't set 640x480 GL video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	SDL_WM_SetCaption("TicTacToe 3D", "ttt3d");

	init();
	reshape(screen->w, screen->h);
	done = 0;
	while (!done)
	{
		SDL_Event event;

		idle();

		ggz_network();

		if(movemove == 1)
		{
			ggz_write_int(gamefd, TTT_SND_MOVE);
			ggz_write_int(gamefd, gamemove);
		}

		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_OPENGL|SDL_RESIZABLE);
					if(screen) reshape(screen->w, screen->h);
					break;
				case SDL_QUIT:
					done = 1;
					break;
			}
		}

		keys = SDL_GetKeyState(NULL);

		if(keys[SDLK_ESCAPE])
		{
			done = 1;
		}
		if(keys[SDLK_UP])
		{
			view_rotx += 0.2;
		}
		if(keys[SDLK_DOWN])
		{
			view_rotx -= 0.2;
		}
		if(keys[SDLK_LEFT])
		{
			view_roty += 0.2;
		}
		if(keys[SDLK_RIGHT])
		{
			view_roty -= 0.2;
		}
		if(keys[SDLK_SPACE])
		{
			if(cursor >= 0) cursormove = 10;
		}
		if(keys[SDLK_RETURN])
		{
			if(cursor >= 0)
			{
				/*board[cursor] = 'x';*/
				gamemove = cursor;
				cursor = -1;
				movemove = 10;
			}
		}
		if(keys[SDLK_t])
		{
			usetexturesmove = 10;
		}

		draw();
	}

	ggz_finish();

	SDL_Quit();

	return 0;
}

