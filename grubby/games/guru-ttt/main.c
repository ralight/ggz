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

#include <ggz.h>
#include <ggz_common.h>

#include <ggzpassive.h>

static int gamenum = -1;
static int gamemove = -1;
static char gameturn = -1;
static int seats[2];
static char names[2][17];

static char board[9];
static int movemove = 0;

#define TTT_MSG_SEAT     0
#define TTT_MSG_PLAYERS  1
#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1

static void ttt_ai(void)
{
	int op, i;
	int nummove, move;
	char status, space, winner;
	
	if (ggz_read_int(ggz_gamefd, &op) < 0)
	{
		/* ... */
		return;
	}

	switch(op)
	{
		case TTT_MSG_SEAT:
			ggz_read_int(ggz_gamefd, &gamenum);
			break;
		case TTT_MSG_PLAYERS:
			for(i = 0; i < 2; i++)
			{
				ggz_read_int(ggz_gamefd, &seats[i]);
				if(seats[i] != GGZ_SEAT_OPEN)
				{
					ggz_read_string(ggz_gamefd, names[i], 17);
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
			ggz_read_char(ggz_gamefd, &status);
			if(status == 0) board[gamemove] = (gamenum == 0 ? 'x' : 'o');
			break;
		case TTT_MSG_MOVE:
			ggz_read_int(ggz_gamefd, &nummove);
			ggz_read_int(ggz_gamefd, &move);
			board[move] = (nummove == 0 ? 'x' : 'o');
			break;
		case TTT_SND_SYNC:
			ggz_read_char(ggz_gamefd, &gameturn);
			for(i = 0; i < 9; i++)
			{
				ggz_read_char(ggz_gamefd, &space);
				if(space < 0) board[i] = ' ';
				else board[i] = (space == 0 ? 'x' : 'o');
			}
			break;
		case TTT_MSG_GAMEOVER:
			ggz_read_char(ggz_gamefd, &winner);
			gameturn = -1;
			ggz_done = 1;
			break;
	}

	if(movemove == 1)
	{
		sleep(2);
		ggz_write_int(ggz_gamefd, TTT_SND_MOVE);
		ggz_write_int(ggz_gamefd, gamemove);
		movemove = 0;
	}
}

static void ttt_init()
{
	int i;

	for(i = 0; i < 9; i++)
		board[i] = ' ';
}

int main(int argc, char *argv[])
{
	ttt_init();

	ggzpassive_sethandler(&ttt_ai);
	ggzpassive_start();

	return 0;
}

