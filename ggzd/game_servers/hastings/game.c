/*
 * File: game.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 09/10/00
 * Desc: Game functions
 *
 * Copyright (C) 2000 Josef Spillner
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

/* System includes */
#include <stdlib.h>

/* GGZ includes */
#include <ggz.h>
#include <game.h>
#include <protocols.h>

/* Easysock */
#include <easysock.h>

/* Global game variables */
struct hastings_game_t hastings_game;

/* Setup game state and board */
void game_init(void)
{
	int i, j;

	/* Nobody's move yet */
	hastings_game.turn = -1;

	/* Initialization state */
	hastings_game.state = HASTINGS_STATE_INIT;

	/* set up some knights*/
	memmove(hastings_game.board[0], "11            555  ", 19);
	memmove(hastings_game.board[1], "113333         5 5 ", 19);
	memmove(hastings_game.board[2], " 1 3 4      6 6 6  ", 19);
	memmove(hastings_game.board[3], "22  44        66   ", 19);
	memmove(hastings_game.board[4], "22   4   0007777   ", 19);
	memmove(hastings_game.board[5], " 2   4   0 0 7     ", 19);

	/* Humancode-to-computercode conversion */
	for (i = 0; i < 6; i++)
	{
	 	for (j = 0; j < 19; j++)
		{
			if(hastings_game.board[i][j] == 32) hastings_game.board[i][j] = -1;
			else hastings_game.board[i][j] -= 48;
			if(hastings_game.board[i][j] > hastings_game.playernum - 1) hastings_game.board[i][j] = -1;
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
}


/* Handle message from GGZ server */
int game_handle_ggz(int ggz_fd, int* p_fd)
{
	int op, seat, status = -1;

	if (es_read_int(ggz_fd, &op) < 0) return -1;

	switch (op)
	{
		case REQ_GAME_LAUNCH:
			if(ggz_game_launch() == 0) status = game_update(HASTINGS_EVENT_LAUNCH, NULL);
			break;
		case REQ_GAME_JOIN:
			if(ggz_player_join(&seat, p_fd) == 0)
			{
				status = game_update(HASTINGS_EVENT_JOIN, &seat);
				status = 1;
			}
			break;
		case REQ_GAME_LEAVE:
			if((status = ggz_player_leave(&seat, p_fd)) == 0)
			{
				game_update(HASTINGS_EVENT_LEAVE, &seat);
				status = 2;
			}
			break;
		case RSP_GAME_OVER:
			status = 3; /* Signal safe to exit */
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}

	return status;
}


/* Handle message from player */
int game_handle_player(int num)
{
	int fd, op, status;

	fd = ggz_seats[num].fd;

	if (es_read_int(fd, &op) < 0) return -1;

	switch (op)
	{
		case HASTINGS_SND_MOVE:
			if((status = game_handle_move(num)) == 0) game_update(HASTINGS_EVENT_MOVE, NULL);
			break;
		case HASTINGS_REQ_INIT:
			game_init();
			status = game_send_sync(num);
			if (!ggz_seats_open())
			{
				hastings_game.state = HASTINGS_STATE_PLAYING;
				game_move();
			}
			break;
		case HASTINGS_REQ_SYNC:
			status = game_send_sync(num);
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}

	return status;
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d's seat num", seat);

	if (es_write_int(fd, HASTINGS_MSG_SEAT) < 0 || es_write_int(fd, seat) < 0) return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;

	/* The number of players is unknown yet */
	hastings_game.playernum = 0;

	/* Determine number of players, 8 at a maximum */
	for(i = 0; i < 8; i++)
	{
		if((!ggz_seats[i].assign) && (!hastings_game.playernum)) hastings_game.playernum = i;
	}
	ggz_debug("%i players found; sending number to client", hastings_game.playernum);

	for (j = 0; j < hastings_game.playernum; j++)
	{
		hastings_game.players[j] = 1;

		if((fd = ggz_seats[j].fd) == -1) continue;

		ggz_debug("Sending player list to player %d", j);

		if(es_write_int(fd, HASTINGS_MSG_PLAYERS) < 0) return -1;
		if(es_write_int(fd, hastings_game.playernum) < 0) return -1;

		for(i = 0; i < hastings_game.playernum; i++)
		{
			if(es_write_int(fd, ggz_seats[i].assign) < 0) return -1;
			if((ggz_seats[i].assign != GGZ_SEAT_OPEN) && (es_write_string(fd, ggz_seats[i].name) < 0)) return -1;
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num)
{
	int i, fd;

	for(i = 0; i < hastings_game.playernum; i++)
	{
		if(i != num)
		{
			fd = ggz_seats[i].fd;

			/* If player is a computer, don't need to send */
			if (fd == -1) return 0;

			ggz_debug("Sending player %d's move to player %d", num, i);

			if (es_write_int(fd, HASTINGS_MSG_MOVE) < 0
			    || es_write_int(fd, num) < 0
			    || es_write_int(fd, hastings_game.move_src_x) < 0
			    || es_write_int(fd, hastings_game.move_src_y) < 0
			    || es_write_int(fd, hastings_game.move_dst_x) < 0
			    || es_write_int(fd, hastings_game.move_dst_y) < 0)
				return -1;
		}
	}

	return 0;
}


/* Send out board layout */
int game_send_sync(int num)
{
	int i, j, fd = ggz_seats[num].fd;

	ggz_debug("Handling sync for player %d", num);

	/* First player? */
	if (hastings_game.turn == -1) hastings_game.turn = 0;

	if((es_write_int(fd, HASTINGS_SND_SYNC) < 0) || (es_write_int(fd, hastings_game.turn) < 0)) return -1;

	/* Syncing knights */
	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
			if (es_write_char(fd, hastings_game.board[i][j]) < 0) return -1;

	/* Syncing map */
	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
			if (es_write_char(fd, hastings_game.boardmap[i][j]) < 0) return -1;

	return 0;
}


/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;

	for (i = 0; i < hastings_game.playernum; i++) {
		if ( (fd = ggz_seats[i].fd) == -1)
			continue;

		ggz_debug("Sending game-over to player %d", i);

		if (es_write_int(fd, HASTINGS_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}

	return 0;
}


/* Do the next move*/
int game_move(void)
{
	int num = hastings_game.turn;

	if (ggz_seats[num].assign == GGZ_SEAT_BOT)
	{
		game_bot_move(num);
		game_update(HASTINGS_EVENT_MOVE, NULL);
	}
	else game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd = ggz_seats[num].fd;

	if (es_write_int(fd, HASTINGS_REQ_MOVE) < 0) return -1;

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num)
{
	int fd = ggz_seats[num].fd;
	char status;

	ggz_debug("Handling move for player %d", num);

	if ((es_read_int(fd, &hastings_game.move_src_x) < 0)
	|| (es_read_int(fd, &hastings_game.move_src_y) < 0)
 	|| (es_read_int(fd, &hastings_game.move_dst_x) < 0)
  	|| (es_read_int(fd, &hastings_game.move_dst_y) < 0))
		return -1;

	/* Check validity of move */
	status = game_check_move(num);

	/* Send back move status */
	if ((es_write_int(fd, HASTINGS_RSP_MOVE) < 0)  || (es_write_char(fd, status))) return -1;

	/* If move simply invalid, ask for resubmit */
	if(((status == -3 || status == -4)) && game_req_move(num) < 0) return -1;

	if (status != 0) return 1;

	return 0;
}

/* Check for valid move */
char game_check_move(int num)
{
	/* Check for correct state */
	if (hastings_game.state != HASTINGS_STATE_PLAYING)
		return -1;
		/* Check for correct turn */
	if (num != hastings_game.turn)
		return -2;

	/* Check for out of bounds move */
	if (hastings_game.move_src_x < 0 || hastings_game.move_src_x > 5)
		return -3;
 	if (hastings_game.move_src_y < 0 || hastings_game.move_src_y > 18)
		return -3;
	if (hastings_game.move_dst_x < 0 || hastings_game.move_dst_x > 5)
		return -3;
 	if (hastings_game.move_dst_y < 0 || hastings_game.move_dst_y > 18)
		return -3;

  	/* cannot jump three miles away */
	if(abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 1)
		return -6;
	if((hastings_game.move_dst_x - hastings_game.move_src_x == 1)
	&& (abs(hastings_game.move_dst_y - hastings_game.move_src_y) > 1))
		return -6;
	if((hastings_game.move_dst_x - hastings_game.move_src_x == -1)
	&& (hastings_game.move_dst_y - hastings_game.move_src_y == 1)
	&& (!(hastings_game.move_src_y % 2)))
		return -6;
	if((hastings_game.move_dst_y - hastings_game.move_src_y == 0)
	&& (abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 0))
		return -6;
	if(abs(hastings_game.move_dst_y - hastings_game.move_src_y) > 2)
		return -6;
	if((abs(hastings_game.move_dst_y - hastings_game.move_src_y) == 2)
	&& (abs(hastings_game.move_dst_x - hastings_game.move_src_x) > 0))
		return -6;

	/* Check for moving from empty field (should not be possible ?!) */
	if (hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] == -1)
		return -4;
	/* Check for duplicated move */


	if (hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] == hastings_game.turn)
		return -5;

	/* Check for move onto water */
	if (hastings_game.boardmap[hastings_game.move_dst_x][hastings_game.move_dst_y] == 32)
		return -7;

	return 0;
}


/* My personal AI (tm) :) */
int game_bot_set(int me, int i, int j)
{
	int x, y, k, l;

	x = i;
	y = j;
	hastings_game.move_src_x = i;
	hastings_game.move_src_y = j;

	for(k = -1; k < 2; k++)
		for(l = -1; l < 2; l++)
		{
			hastings_game.move_dst_x = k + i;
			hastings_game.move_dst_y = l + j;
			if((game_check_move(me) == 0) && (x == i) && (y == j))
			{
				x = hastings_game.move_dst_x;
				y = hastings_game.move_dst_y;
			}
		}
	hastings_game.move_dst_x = x;
	hastings_game.move_dst_y = y;

	if((x != i) || (y != j)) return 1;

	return 0;
}

/* Do bot moves */
void game_bot_move(int me)
{
/* TODO: algorithm of passivism + check for win :-) */
/* REMARK: deleted very much code */
/* me: that's the bot */

	int i, j;
	int moved;

	moved = 0;
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 19; j++)
		{
			if(hastings_game.board[i][j] == me) moved = game_bot_set(me, i, j);
			if(moved) break;
		}
		if(moved) break;
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
	/* REMARK: deleted very much code */

	/* No one won yet */
	/* if (hastings_game.move_count == 9) */ /* hehe */
		/*return 2;*/  /* Cat's game */ /* Where's the dog? */

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

		ggz_debug("Player %d from square %d/%d", hastings_game.turn, hastings_game.move_src_x, hastings_game.move_src_y);
 		ggz_debug("to square %d/%d", hastings_game.move_dst_x, hastings_game.move_dst_y);

		// disallow dead bots to move (should not happen)
		if(hastings_game.move_src_x > -1)
		{
			hastings_game.board[hastings_game.move_src_x][hastings_game.move_src_y] = -1;
			hastings_game.board[hastings_game.move_dst_x][hastings_game.move_dst_y] = hastings_game.turn;
			game_send_move(hastings_game.turn);
		}

		/*printf("Pre:   --%i--\n", hastings_game.players[hastings_game.turn]);
		printf("(turn) --%i--\n", hastings_game.turn);*/

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

		/*printf("After: --%i--\n", hastings_game.players[hastings_game.turn]);
		printf("(turn) --%i--\n", hastings_game.turn);
		printf("(i)    --%i--\n\n", i);*/

		if(i == hastings_game.playernum - 1)
		{
			hastings_game.state = HASTINGS_STATE_DONE;
			game_send_gameover(victor);
			/* Notify GGZ server of game over */
			/*ggz_done();*/ /* TODO: is this safe? */
		}
		break;
	}

	return 0;
}
