/*
 * File: game.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 09/10/00
 * Desc: Game functions
 * $Id: game.c 3716 2002-04-01 21:51:03Z dr_maux $
 *
 * Copyright (C) 2000 - 2002 Josef Spillner
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

/* Header file */
#include "game.h"

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Easysock includes */
#include <ggz.h>

/* Global game variables */
struct hastings_game_t hastings_game;

/* Setup game state and board */
void game_init(GGZdMod *ggz)
{
	int i, j;

	/* Nobody's move yet */
	hastings_game.turn = -1;

	/* Initialization state */
	/*hastings_game.state = HASTINGS_STATE_WAIT;*/

	/* set up some knights*/
	memmove(hastings_game.board[0], "11            444  ", 19);
	memmove(hastings_game.board[1], "113333         4 4 ", 19);
	memmove(hastings_game.board[2], " 1 3 5      6 6 6  ", 19);
	memmove(hastings_game.board[3], "77  55        66   ", 19);
	memmove(hastings_game.board[4], "77   5   0002222   ", 19);
	memmove(hastings_game.board[5], " 7   5   0 0 2     ", 19);

	/* Humancode-to-computercode conversion */
	for (i = 0; i < 6; i++)
	{
	 	for (j = 0; j < 19; j++)
		{
			if(hastings_game.board[i][j] == 32) hastings_game.board[i][j] = -1;
			else hastings_game.board[i][j] -= 48;
			/*if(hastings_game.board[i][j] > hastings_game.playernum - 1) hastings_game.board[i][j] = -1;*/
			/*if(hastings_game.board[i][j] > ggzdmod_get_num_seats(ggz) - 1) hastings_game.board[i][j] = -1;*/
		}
	}

	/* hexagon fields: this is difficult to understand */
	/* but it works (one line is two columns, and flip it two times to fit) */
	memmove(hastings_game.boardmap[0], "xxxxxxxxxxxx xxxx  ", 19);
	memmove(hastings_game.boardmap[1], "xxxxxxxxxx     x x ", 19);
	memmove(hastings_game.boardmap[2], "xx x xxxxxxxx x x  ", 19);
	memmove(hastings_game.boardmap[3], "xx  xxxxxxxxxxxx   ", 19);
	memmove(hastings_game.boardmap[4], "xx   xxxxxxxxxxx   ", 19);
	memmove(hastings_game.boardmap[5], " x   x x x x x     ", 19);
	
	hastings_game.ggz = ggz;
}


/* Handle event from GGZ server */
void game_handle_ggz(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	switch(event)
	{
		case GGZDMOD_EVENT_STATE:
			if (*(GGZdModState*)data == GGZDMOD_STATE_CREATED)
				game_update(HASTINGS_EVENT_LAUNCH, NULL);
			break;
		case GGZDMOD_EVENT_JOIN:
			game_update(HASTINGS_EVENT_JOIN, data);
			break;
		case GGZDMOD_EVENT_LEAVE:
			game_update(HASTINGS_EVENT_LEAVE, data);
			break;
		default:
			ggzdmod_log(hastings_game.ggz, "game_handle_ggz: bad event.");
	}
}


/* Calculate if all seats are full */
static int seats_full(void)
{
	return ggzdmod_count_seats(hastings_game.ggz, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(hastings_game.ggz, GGZ_SEAT_RESERVED) == 0;
}


/* Handle message from player */
void game_handle_player(GGZdMod *ggz, GGZdModEvent event, void *seat_data)
{
	int num;
	int fd, op;

	num = *(int*)seat_data;
	fd = ggzdmod_get_seat(hastings_game.ggz, num).fd;

	if (fd < 0 || ggz_read_int(fd, &op) < 0) return;

	ggzdmod_log(hastings_game.ggz, "## handle player: %i\n", num);

	switch (op)
	{
		case HASTINGS_SND_MOVE:
			if(game_handle_move(num) == 0)
				game_update(HASTINGS_EVENT_MOVE, seat_data);
			break;
		case HASTINGS_REQ_INIT:
			/*game_init(hastings_game.ggz);*/
			game_send_sync(num);
			if (seats_full())
			{
				hastings_game.state = HASTINGS_STATE_PLAYING;
				game_move();
			}
			break;
		case HASTINGS_REQ_SYNC:
			game_send_sync(num);
			break;
		default:
			/* Unrecognized opcode */
			ggzdmod_log(hastings_game.ggz, "ERROR: unrecognized player opcode %d.", op);
			break;
	}
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggzdmod_get_seat(hastings_game.ggz, seat).fd;

	ggzdmod_log(hastings_game.ggz, "Sending player %d's seat num\n", seat);

	if (ggz_write_int(fd, HASTINGS_MSG_SEAT) < 0 || ggz_write_int(fd, seat) < 0) return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j;
	GGZSeat seat, seat2;

	/* Determine number of players, 8 at a maximum */
	hastings_game.playernum = ggzdmod_get_num_seats(hastings_game.ggz);
	ggzdmod_log(hastings_game.ggz, "%i players found; sending number to clients...\n", hastings_game.playernum);

	for (j = 0; j < hastings_game.playernum; j++)
	{
		hastings_game.players[j] = 1;

		seat = ggzdmod_get_seat(hastings_game.ggz, j);
		if(seat.type != GGZ_SEAT_PLAYER)
		{
			ggzdmod_log(hastings_game.ggz, "Player %i is a unassigned or bot (%i).\n", j, seat.type);
			continue;
		}

		ggzdmod_log(hastings_game.ggz, "Sending player list to player %d\n", j);

		if(ggz_write_int(seat.fd, HASTINGS_MSG_PLAYERS) < 0) return -1;
		if(ggz_write_int(seat.fd, hastings_game.playernum) < 0) return -1;

		for(i = 0; i < hastings_game.playernum; i++)
		{
			seat2 = ggzdmod_get_seat(hastings_game.ggz, i);
			if(ggz_write_int(seat.fd, seat2.type) < 0) return -1;
			if((seat.type != GGZ_SEAT_OPEN) && (seat2.name))
				if(ggz_write_string(seat.fd, seat2.name) < 0)
					return -1;
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num)
{
	int i, fd;
	int status = 0;
	GGZSeat seat;

	for(i = 0; i < hastings_game.playernum; i++)
	{
		if(i != num)
		{
			seat = ggzdmod_get_seat(hastings_game.ggz, i);
			fd = seat.fd;

			/* If player is a computer, don't need to send */
			if (seat.type != GGZ_SEAT_PLAYER) continue;

			ggzdmod_log(hastings_game.ggz, "Sending player %d's move to player %d\n", num, i);

			if (ggz_write_int(fd, HASTINGS_MSG_MOVE) < 0
			    || ggz_write_int(fd, num) < 0
			    || ggz_write_int(fd, hastings_game.move_src_x) < 0
			    || ggz_write_int(fd, hastings_game.move_src_y) < 0
			    || ggz_write_int(fd, hastings_game.move_dst_x) < 0
			    || ggz_write_int(fd, hastings_game.move_dst_y) < 0)
				status = -1;
		}
	}

	return status;
}


/* Send out board layout */
int game_send_sync(int num)
{
	int i, j, fd;
	GGZSeat seat;
	char knight;

	/* Find the right player */
	fd = ggzdmod_get_seat(hastings_game.ggz, num).fd;
	if(fd < 0) return -1;
	ggzdmod_log(hastings_game.ggz, "Handling sync for player %d\n", num);

	/* First player? */
	if (hastings_game.turn == -1) hastings_game.turn = 0;

	/* Send synchonization opcode and turn */
	if((ggz_write_int(fd, HASTINGS_SND_SYNC) < 0) || (ggz_write_int(fd, hastings_game.turn) < 0)) return -1;

	/* Syncing knights; mask out nonexistant knights */
	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
		{
			knight = hastings_game.board[i][j];
			if(knight != -1)
			{
				seat = ggzdmod_get_seat(hastings_game.ggz, knight);
				if((seat.type != GGZ_SEAT_PLAYER) && (seat.type != GGZ_SEAT_BOT)) knight = -1;
			}
			if (ggz_write_char(fd, knight) < 0) return -1;
		}

	/* Syncing map */
	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
			if (ggz_write_char(fd, hastings_game.boardmap[i][j]) < 0) return -1;

	return 0;
}


/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;

	for (i = 0; i < hastings_game.playernum; i++) {
		if ( (fd = ggzdmod_get_seat(hastings_game.ggz, i).fd) == -1)
			continue;

		ggzdmod_log(hastings_game.ggz, "Sending game-over to player %d\n", i);

		if (ggz_write_int(fd, HASTINGS_MSG_GAMEOVER) < 0
		    || ggz_write_char(fd, winner) < 0)
			return -1;
	}

	return 0;
}


/* Do the next move*/
int game_move(void)
{
	int num, worked;

	num = hastings_game.turn;
	ggzdmod_log(hastings_game.ggz, "game_move() called when it is player %d's turn.", num);

	if (ggzdmod_get_seat(hastings_game.ggz, num).type == GGZ_SEAT_BOT)
	{
		game_bot_move(num);
		game_update(HASTINGS_EVENT_MOVE, NULL);
	}
	else worked = game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd;

	fd = ggzdmod_get_seat(hastings_game.ggz, num).fd;
	
	ggzdmod_log(hastings_game.ggz, "Requesting move from player %d.", num);

	if (ggz_write_int(fd, HASTINGS_REQ_MOVE) < 0)
		return -1;

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num)
{
	int fd;
	char status;

	fd = ggzdmod_get_seat(hastings_game.ggz, num).fd;

	ggzdmod_log(hastings_game.ggz, "Handling move for player %d\n", num);

	if ((ggz_read_int(fd, &hastings_game.move_src_x) < 0)
	|| (ggz_read_int(fd, &hastings_game.move_src_y) < 0)
 	|| (ggz_read_int(fd, &hastings_game.move_dst_x) < 0)
  	|| (ggz_read_int(fd, &hastings_game.move_dst_y) < 0))
		return -1;

	/* Check validity of move */
	status = game_check_move(num, 0);

	/* Send back move status */
	if ((ggz_write_int(fd, HASTINGS_RSP_MOVE) < 0) || (ggz_write_char(fd, status))) return -1;

	/* If move simply invalid, ask for resubmit */
	if(((status == HASTINGS_ERR_BOUND || status == HASTINGS_ERR_EMPTY)) && game_req_move(num) < 0) return -1;

	if (status != 0) return 1;

	return 0;
}

/* Check for valid move */
char game_check_move(int num, int enemyallowed)
{
	char knight;
	GGZSeat seat;

	/* Check for correct state */
	if (hastings_game.state != HASTINGS_STATE_PLAYING)
		return HASTINGS_ERR_STATE;
	/* Check for correct turn */
	if (num != hastings_game.turn)
		return HASTINGS_ERR_TURN;

	/* Check for out of bounds move */
	if (hastings_game.move_src_x < 0 || hastings_game.move_src_x > 5)
		return HASTINGS_ERR_BOUND;
 	if (hastings_game.move_src_y < 0 || hastings_game.move_src_y > 18)
		return HASTINGS_ERR_BOUND;
	if (hastings_game.move_dst_x < 0 || hastings_game.move_dst_x > 5)
		return HASTINGS_ERR_BOUND;
 	if (hastings_game.move_dst_y < 0 || hastings_game.move_dst_y > 18)
		return HASTINGS_ERR_BOUND;

  	/* cannot jump three miles away */
	if(abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 1)
		return HASTINGS_ERR_DIST;
	if((hastings_game.move_dst_x - hastings_game.move_src_x == 1)
	&& (abs(hastings_game.move_dst_y - hastings_game.move_src_y) > 1))
		return HASTINGS_ERR_DIST;
	if((hastings_game.move_dst_y - hastings_game.move_src_y == 0)
	&& (abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 0))
		return HASTINGS_ERR_DIST;
	if(abs(hastings_game.move_dst_y - hastings_game.move_src_y) > 2)
		return HASTINGS_ERR_DIST;
	if((abs(hastings_game.move_dst_y - hastings_game.move_src_y) == 2)
	&& (abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 0))
		return HASTINGS_ERR_DIST;

	/* prevent more strange jumps */
	if((hastings_game.move_dst_y - hastings_game.move_src_y == -1)
	&& (hastings_game.move_dst_x - hastings_game.move_src_x == -1)
	&& (!(hastings_game.move_src_y % 2)))
		return HASTINGS_ERR_DIST;
	if((hastings_game.move_dst_x - hastings_game.move_src_x == -1)
	&& (hastings_game.move_dst_y - hastings_game.move_src_y == 1)
	&& (!(hastings_game.move_src_y % 2)))
		return HASTINGS_ERR_DIST;
	if((hastings_game.move_dst_x - hastings_game.move_src_x == 1)
	&& (hastings_game.move_dst_y - hastings_game.move_src_y == -1)
	&& (hastings_game.move_src_y % 2))
		return HASTINGS_ERR_DIST;

	/* Check for moving from empty field (should not be possible ?!) */
	if (hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] == -1)
		return HASTINGS_ERR_EMPTY;
	/* Check for duplicated move */
	if (hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] == hastings_game.turn)
		return HASTINGS_ERR_FULL;
	/* Check for move onto water */
	if (hastings_game.boardmap[hastings_game.move_dst_x][hastings_game.move_dst_y] == 32)
		return HASTINGS_ERR_MAP;

	/* Check for other knights whereever necessary */
	knight = hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y];
	seat = ggzdmod_get_seat(hastings_game.ggz, knight);
	if((seat.type != GGZ_SEAT_PLAYER) && (seat.type != GGZ_SEAT_BOT)) knight = -1;
	if(knight != -1)
	{
		/* Check for team collegues */
		if (hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] % 2 ==
		    hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] % 2)
			return HASTINGS_ERR_FULL;
		/* Check for enemies */
		if(enemyallowed)
		{
			if (hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] % 2 ==
				!(hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] % 2))
				return HASTINGS_ERR_ENEMY;
		}
	}

	return 0;
}


/* My personal AI (tm) :) */
int game_bot_set(int me, int i, int j, int wanted)
{
	int x, y, k, l;

	x = i;
	y = j;
	hastings_game.move_src_x = i;
	hastings_game.move_src_y = j;

	for(k = -2; k < 3; k++)
		for(l = -2; l < 3; l++)
		{
			hastings_game.move_dst_x = k + i;
			hastings_game.move_dst_y = l + j;
			if((game_check_move(me, 1) == wanted) && (x == i) && (y == j))
			{
				x = hastings_game.move_dst_x;
				y = hastings_game.move_dst_y;
			}
		}

	/* in case of a normal move: north troops must go southwards */
	if((wanted == 0) && (j < 10))
	{
		for(k = 2; k > -3; k--)
			for(l = 2; l > -3; l--)
			{
				hastings_game.move_dst_x = k + i;
				hastings_game.move_dst_y = l + j;
				if((game_check_move(me, 1) == wanted) && (x == i) && (y == j))
				{
					x = hastings_game.move_dst_x;
					y = hastings_game.move_dst_y;
				}
			}
	}
	
	hastings_game.move_dst_x = x;
	hastings_game.move_dst_y = y;

	if((x != i) || (y != j)) return 1;

	return 0;
}

/* Do bot moves */
/* TODO: algorithm of passivism + check for win :-) */
void game_bot_move(int me)
{
	int i, j;
	int moved;
	
	ggzdmod_log(hastings_game.ggz, "Getting bot's move for player %d.", me);

	moved = 0;
	
	/* Try to find an enemy first */
	for(i = 0; i < 6; i++)
	{
		for(j = 0; j < 19; j++)
		{
			if(hastings_game.board[i][j] == me) moved = game_bot_set(me, i, j, -8);
			if(moved) break;
		}
		if(moved) break;
	}
	
	/* If no enemy found, lurk around over the map */
	if(!moved)
	{
		for (i = 0; i < 6; i++)
		{
			for (j = 0; j < 19; j++)
			{
				if(hastings_game.board[i][j] == me) moved = game_bot_set(me, i, j, 0);
				if(moved) break;
			}
			if(moved) break;
		}
	}

	sleep(1);

	/* no move found - player dead */
	if(!moved)
	{
		hastings_game.move_src_x = -1;
		hastings_game.players[me] = 0;
	}
}


/* Check for a win */
char game_check_win(void)
{
	/* TODO: own check */

	return -1;
}


/* Update game state */
int game_update(int event, void* data)
{
	int seat;
	char victor;
	int i;

	switch(event)
	{

	case HASTINGS_EVENT_LAUNCH:
		if (hastings_game.state != HASTINGS_STATE_INIT) return -1;
		hastings_game.state = HASTINGS_STATE_WAIT;
		break;

	case HASTINGS_EVENT_JOIN:
		if (hastings_game.state != HASTINGS_STATE_WAIT) return -1;

		seat = *(int*)data;
		game_send_seat(seat);
		game_send_players();
		break;

	case HASTINGS_EVENT_LEAVE:
		if (hastings_game.state == HASTINGS_STATE_PLAYING)
		{
			hastings_game.state = HASTINGS_STATE_WAIT;
			game_send_players();
		}
		break;

	case HASTINGS_EVENT_MOVE:
		if (hastings_game.state != HASTINGS_STATE_PLAYING) return -1;

		ggzdmod_log(hastings_game.ggz, "Player %d from square %d/%d",
			hastings_game.turn, hastings_game.move_src_x, hastings_game.move_src_y);
 		ggzdmod_log(hastings_game.ggz, "to square %d/%d\n",
			hastings_game.move_dst_x, hastings_game.move_dst_y);

		/* disallow dead bots to move (should not happen), and prevent foreign knights to be moved */
		if((hastings_game.move_src_x > -1)
		&& (hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] == hastings_game.turn)
		&& ((!data) || (*(int*)data == hastings_game.turn)))
		{
			hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] = -1;
			hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] = hastings_game.turn;
			game_send_move(hastings_game.turn);
		}
		else return -1;

		if((victor = game_check_win()) < 0)
		{
			for(i = 0; i < 8; i++)
			{
				hastings_game.turn = (hastings_game.turn + 1) % hastings_game.playernum;
				if((hastings_game.players[hastings_game.turn]) || (i == hastings_game.playernum - 1)) break;
			}
			if(i < hastings_game.playernum - 1) game_move();
		}
		else i = hastings_game.playernum - 1;

		if(i == hastings_game.playernum - 1)
		{
			hastings_game.state = HASTINGS_STATE_DONE;
			game_send_gameover(victor);
			/* Notify GGZ server of game over */
			/*ggz_done();*/ /* TODO: is this safe? */
		}
		ggzdmod_log(hastings_game.ggz, "## post-move\n");
		break;
	}

	return 0;
}

