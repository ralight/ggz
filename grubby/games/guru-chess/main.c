/*
 * Chess game module for Guru
 * Copyright (C) 2004 Josef Spillner, josef@ggzgamingzone.org
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

#include "i18n.h"
#include "game.h"

#include "ggzpassive.h"

#include "proto.h"
#include "ai.h"

static int myturn;
static int channel;

static void chess_ai(void)
{
	char opcode;
	int fd;
	char seat, version;
	char seats[2], names[2][32];
	int time;
	int movelength;
	char movex, movey, movex2, movey2, moveoptions;
	int movefrom, moveto, ret;
	char gameover;
	int update, update2;
	int random;

	fd = ggz_gamefd;

	ggz_read_char(fd, &opcode);

	switch(opcode)
	{
		case CHESS_MSG_SEAT:
			ggz_read_char(fd, &seat);
			ggz_read_char(fd, &version);
			printf("seat=%i version=%i\n", seat, version);
			myturn = 1;
			break;
		case CHESS_MSG_PLAYERS:
			ggz_read_char(fd, &seats[0]);
			if(seats[0] != GGZ_SEAT_OPEN) ggz_read_string(fd, names[0], sizeof(names[0]));
			else names[0][0] = 0;
			ggz_read_char(fd, &seats[1]);
			if(seats[1] != GGZ_SEAT_OPEN) ggz_read_string(fd, names[1], sizeof(names[1]));
			else names[1][0] = 0;
			printf("player1=%s player2=%s\n", names[0], names[1]);
			break;
		case CHESS_REQ_TIME:
			/* not needed */
			break;
		case CHESS_RSP_TIME:
			ggz_read_int(fd, &time);
			printf("clock type: %i\n", time >> 24);
			printf("clock limits: %im%is %im%is\n",
					time & 0xFFFFFF, time & 0xFFFFFF, time & 0xFFFFFF, time & 0xFFFFFF);
			break;
		case CHESS_MSG_START:
			printf("start!\n");
			break;
		case CHESS_REQ_MOVE:
			/* server */
			break;
		case CHESS_MSG_MOVE:
			printf("got move\n");
			ggz_read_int(fd, &movelength);
			if(movelength == 5)
			{
				ggz_read_char(fd, &movex);
				ggz_read_char(fd, &movey);
				ggz_read_char(fd, &movex2);
				ggz_read_char(fd, &movey2);
				ggz_read_char(fd, &moveoptions);
				movex -= 'A';
				movey -= '1';
				movex2 -= 'A';
				movey2 -= '1';
				printf("move: %i/%i -> %i/%i (%i)\n", (int)movex, (int)movey,
						(int)movex2, (int)movey2, (int)moveoptions);

				if(channel != -1)
				{
					if(!myturn)
					{
						random = rand() % 10;
						if(random == 0)
						{
							ggz_write_int(channel, GURU_GAME_MESSAGE);
							ggz_write_string(channel, __("This pawn move was great!"));
						}
						else if(random == 1)
						{
							ggz_write_int(channel, GURU_GAME_MESSAGE);
							ggz_write_string(channel, __("I must admit, he's not playing bad."));
						}
					}
				}

				/* apply move and find countermove */
				movefrom = movex * 8 + movey;
				moveto = movex2 * 8 + movey2;
				chess_ai_move(movefrom, moveto, 1);
				if(myturn)
				{
					ret = chess_ai_find(C_BLACK, &movefrom, &moveto);
				}
				else ret = -1;

				myturn = (myturn + 1) % 2;
				if(ret == 1)
				{
					movey = movefrom / 8;
					movex = movefrom % 8;
					movey2 = moveto / 8;
					movex2 = moveto % 8;

					printf("send move %i/%i -> %i/%i\n",
						(int)movex, (int)movey, (int)movex2, (int)movey2);

					/* send move */
					ggz_write_char(fd, CHESS_REQ_MOVE);
					ggz_write_int(fd, 6);
					ggz_write_char(fd, movex + 'A');
					ggz_write_char(fd, movey + '1');
					ggz_write_char(fd, movex2 + 'A');
					ggz_write_char(fd, movey2 + '1');
					ggz_write_char(fd, 0);
					ggz_write_char(fd, 0);
				}
				else if(ret == 0)
				{
					printf("cannot send move\n");
				}
			}
			break;
		case CHESS_MSG_GAMEOVER:
			ggz_read_char(fd, &gameover);
			printf("game over: %i\n", (int)gameover);
			break;
		case CHESS_REQ_UPDATE:
			/* server */
			break;
		case CHESS_RSP_UPDATE:
			ggz_read_int(fd, &update);
			ggz_read_int(fd, &update2);
			printf("update: %i:%i, %i:%i\n", update, update,
					update2, update2);
			break;
		case CHESS_MSG_UPDATE:
			/* server */
			break;
		case CHESS_REQ_FLAG:
			/* server */
			break;
		case CHESS_REQ_DRAW:
			printf("got draw\n");
			break;
		default:
			/* unknown opcode */
			printf("unknown opcode %i\n", opcode);
			break;
	}
}

static void chess_init()
{
	chess_ai_init(C_NONE, 5);
	channel = ggz_make_unix_socket(GGZ_SOCK_CLIENT, "/tmp/grubby.games");
	if(channel == -1)
	{
		printf("Warning: Game communication channel inactive for chess\n");
	}
}

int main(int argc, char *argv[])
{
	chess_init();

	ggzpassive_sethandler(&chess_ai);
	ggzpassive_start();

	return 0;
}

