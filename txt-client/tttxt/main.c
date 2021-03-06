/*
 * TTTXT - Console client for the Tic-Tac-Toe game.
 * This application is part of the ggz-txt-client package.
 *
 * Copyright (C) 2004-2008 Josef Spillner <josef@ggzgamingzone.org>
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

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <ggz_common.h>
#include <ggzmod.h>
#include <ggz_dio.h>

#define COLOR_WHITE     "\e[0m\e[37m"
#define COLOR_BLUE      "\e[0m\e[34m"
#define COLOR_RED       "\e[0m\e[31m"
#define COLOR_GREY      "\e[1m\e[30m"

static struct winsize window;
static int tty_des;
static int maxline;
static int ggzmode;
static int refresh;

static GGZMod *mod = NULL;
static int gamefd = -1;
static GGZDataIO *dio = NULL;

static int gamemove = -1;
static char gameturn = -1;

static char board[9];

#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1

static void ggz_network(void);

static void gotoxy(int x, int y)
{
	printf("\e[%d;%df", y, x);

	if(y > maxline) maxline = y;
}

static void initwindow(void)
{
	fflush(NULL);
	printf("\e[2J");
	ioctl(tty_des, TIOCGWINSZ, &window);
	fflush(NULL);

	maxline = 0;
	refresh = 1;

	gotoxy(10, 3);
	printf("%s%s", COLOR_RED, "TTTXT");
	printf("%s%s", COLOR_WHITE, " - ");
	printf("%s%s", COLOR_BLUE, "TicTacToe text console game");
}

static void drawgameboard(void)
{
	int i, j;
	char s[4];

	initwindow();

	gotoxy(10, 5);
	printf("%s%s%s%s%s%s%s%s%s%s", COLOR_WHITE,
		"You are (", COLOR_RED, "x", COLOR_WHITE,
		"), opponent is (", COLOR_BLUE, "o", COLOR_WHITE,
		").");

	gotoxy(20, 7);
	printf("%s%s", COLOR_WHITE, "_____________");
	gotoxy(20, 8);
	printf("%s%s", COLOR_WHITE, "|   |   |   |");
	gotoxy(20, 9);
	printf("%s%s", COLOR_WHITE, "-------------");
	gotoxy(20, 10);
	printf("%s%s", COLOR_WHITE, "|   |   |   |");
	gotoxy(20, 11);
	printf("%s%s", COLOR_WHITE, "-------------");
	gotoxy(20, 12);
	printf("%s%s", COLOR_WHITE, "|   |   |   |");
	gotoxy(20, 13);
	printf("%s%s", COLOR_WHITE, "-------------");

	for(j = 0; j < 3; j++)
		for(i = 0; i < 3; i++)
		{
			gotoxy(22 + i * 4, 8 + j * 2);
			snprintf(s, sizeof(s), "%i", j * 3 + i + 1);
			if(board[j * 3 + i] == ' ')
				printf("%s%s", COLOR_GREY, s);
			else if(board[j * 3 + i] == 'x')
				printf("%s%s", COLOR_RED, "xxx");
			else if(board[j * 3 + i] == 'o')
				printf("%s%s", COLOR_BLUE, "ooo");
		}

	gotoxy(10, 15);
	printf("%s%s%s%s", COLOR_RED, "E",
		COLOR_WHITE, "xit to main menu");
}

static void drawabout(void)
{
	initwindow();

	printf("%s", COLOR_WHITE);

	gotoxy(10, 5);
	printf("Copyright (C) 2004-2008 Josef Spillner <josef@ggzgamingzone.org>");
	gotoxy(10, 6);
	printf("This game is part of the GGZ Gaming Zone.");
	gotoxy(10, 7);
	printf("http://www.ggzgamingzone.org/gameclients/tttxt/");

	gotoxy(10, 9);
	printf("%s%s%s%s", COLOR_RED, "E",
		COLOR_WHITE, "xit to main menu");
}

static void drawhighscores(void)
{
	initwindow();

	gotoxy(10, 5);
	printf("%s%s %s%s", COLOR_BLUE, "1",
		COLOR_WHITE, "xxx");

	gotoxy(10, 6);
	printf("%s%s %s%s", COLOR_BLUE, "2",
		COLOR_WHITE, "yyy");

	gotoxy(10, 8);
	printf("%s%s%s%s", COLOR_RED, "E",
		COLOR_WHITE, "xit to main menu");
}

static void drawmenu(void)
{
	initwindow();

	if(ggzmode)
	{
		gotoxy(10, 5);
		printf("%s%s%s%s", COLOR_RED, "G",
			COLOR_WHITE, "ame");
	}
	else
	{
		gotoxy(10, 5);
		printf("%s%s%s%s", COLOR_GREY, "G",
			COLOR_WHITE, "ame (only possible in GGZ mode)");
	}

	gotoxy(10, 6);
	printf("%s%s%s%s", COLOR_RED, "H",
		COLOR_WHITE, "ighscores");

	gotoxy(10, 7);
	printf("%s%s%s%s", COLOR_RED, "A",
		COLOR_WHITE, "bout");

	gotoxy(10, 9);
	printf("%s%s%s%s", COLOR_RED, "E",
		COLOR_WHITE, "xit TTTXT");
}

static void cleanupwindow(void)
{
	gotoxy(1, 17);
	printf("%s\n", COLOR_WHITE);
}

static void query(void)
{
	gotoxy(1, maxline + 2);
	maxline -= 2;
	refresh = 0;
	printf("%s%s %s", COLOR_BLUE, "?", COLOR_WHITE);
}


static unsigned char readkey(void)
{
	unsigned char buf[2];
	int ret;

	refresh = 1;

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	ret = 0;
	while(ret < 1)
	{
		if(ggzmode) ggz_network();
		if(refresh)
		{
			query();
			fflush(NULL);
		}

		ret = read(STDIN_FILENO, buf, sizeof(buf));
		if(ret == -1)
		{
			if(errno != EAGAIN) return 0;
		}
	}
	if(ret == 1) buf[0] = '\n';

	return buf[0];
}

static void game_handle_io(GGZDataIO *dio, void *userdata)
{
	int op, i;
	int nummove, move;
	char status, space, winner;
	int is_spectator, seat_num;

	ggz_dio_get_int(dio, &op);

	switch(op)
	{
		case TTT_REQ_MOVE:
			break;
		case TTT_RSP_MOVE:
			ggzmod_get_player(mod, &is_spectator, &seat_num);
			ggz_dio_get_char(dio, &status);
			if(status == 0) board[gamemove] = (seat_num == 0 ? 'x' : 'o');
			drawgameboard();
			break;
		case TTT_MSG_MOVE:
			ggz_dio_get_int(dio, &nummove);
			ggz_dio_get_int(dio, &move);
			board[move] = (nummove == 0 ? 'x' : 'o');
			drawgameboard();
			break;
		case TTT_SND_SYNC:
			ggz_dio_get_char(dio, &gameturn);
			for(i = 0; i < 9; i++)
			{
				ggz_dio_get_char(dio, &space);
				if(space < 0) board[i] = ' ';
				else board[i] = (space == 0 ? 'x' : 'o');
			}
			break;
		case TTT_MSG_GAMEOVER:
			ggz_dio_get_char(dio, &winner);
			gameturn = -1;
			break;
	}
}

static void handle_ggz(void)
{
	ggzmod_dispatch(mod);
}

static void handle_ggzmod_server(GGZMod * ggzmod, GGZModEvent e,
				 const void *data)
{
	const int *fd = data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	gamefd = *fd;

	dio = ggz_dio_new(gamefd);
	ggz_dio_set_read_callback(dio, game_handle_io, NULL);
	ggz_dio_set_auto_flush(dio, true);
}

static void ggz_init(void)
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

static void ggz_finish(void)
{
	ggzmod_disconnect(mod);
	ggzmod_free(mod);
}

static void ggz_network(void)
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
			if(FD_ISSET(gamefd, &set)) ggz_dio_read_data(dio);
		}
		if(FD_ISSET(serverfd, &set)) handle_ggz();
	}
}

int main(int argc, char *argv[])
{
	int done;
	unsigned char key;

	ggzmode = 0;

	if(ggzmod_is_ggz_mode())
	{
		ggzmode = 1;

		ggz_init();
	}

	while(1)
	{
		drawmenu();

		switch(readkey())
		{
			case 0:
			case 'e':
			case 'E':
				cleanupwindow();
				return 0;
			case 'g':
			case 'G':
				if(!ggzmode) break;

				done = 0;
				while(!done)
				{
					drawgameboard();

					key = readkey();
					switch(key)
					{
						case 0:
							cleanupwindow();
							return 0;
						case 'e':
						case 'E':
							done = 1;
							break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							gamemove = key - 49;
							ggz_dio_packet_start(dio);
							ggz_dio_put_int(dio, TTT_SND_MOVE);
							ggz_dio_put_int(dio, gamemove);
							ggz_dio_packet_end(dio);
							break;
					}
				}
				break;
			case 'h':
			case 'H':
				drawhighscores();

				done = 0;
				while(!done)
				{
					switch(readkey())
					{
						case 0:
							cleanupwindow();
							return 0;
						case 'e':
						case 'E':
							done = 1;
					}
				}
				break;
			case 'a':
			case 'A':
				drawabout();

				done = 0;
				while(!done)
				{
					switch(readkey())
					{
						case 0:
							cleanupwindow();
							return 0;
						case 'e':
						case 'E':
							done = 1;
					}
				}
				break;
		}
	}

	cleanupwindow();

	if(ggzmode) ggz_finish();

	return 0;
}

