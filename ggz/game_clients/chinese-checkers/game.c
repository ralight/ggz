/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Core game structures and logic
 *
 * Copyright (C) 2001 Richard Gade.
 *
 * This program is free software; cou can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at cour option) any later version.
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "game.h"
#include "ggz.h"
#include "display.h"
#include "main.h"
#include "easysock.h"
#include "protocol.h"


struct game_t game;

static void game_zap_board(void);
static void game_print_board(void);
static int game_make_move(int, int, int, int);
static int game_find_path(int, int, int, int, int);


/* Perform game initialization tasks */
void game_init(void)
{
	/* Connect to GGZ */
	ggz_connect();

	/* Trap our input from the socket */
	gdk_input_add(game.fd, GDK_INPUT_READ, main_io_handler, NULL);

	/* Display the main dialog */
	display_init();

	/* Setup the board array */
	game_zap_board();
	game_print_board();
}


static char init_board[17][25] = {
	"            *            ",
	"           * *           ",
	"          * * *          ",
	"         * * * *         ",
	"* * * * * * * * * * * * *",
	" * * * * * * * * * * * * ",
	"  * * * * * * * * * * *  ",
	"   * * * * * * * * * *   ",
	"    * * * * * * * * *    ",
	"   * * * * * * * * * *   ",
	"  * * * * * * * * * * *  ",
	" * * * * * * * * * * * * ",
	"* * * * * * * * * * * * *",
	"         * * * *         ",
	"          * * *          ",
	"           * *           ",
	"            *            "
};

static int homexy[6][10][2] = {
        /* Position 0 */
        { {16, 12},
          {15, 11}, {15, 13},
          {14, 10}, {14, 12}, {14, 14},
          {13, 9},  {13, 11}, {13, 13}, {13, 15} },
        /* Position 1 */
        { {12, 0},
          {11, 1}, {12, 2},
          {10, 2}, {11, 3}, {12, 4},
          {9, 3},  {10, 4}, {11, 5}, {12, 6} },
        /* Position 2 */
        { {4, 0},
          {4, 2}, {5, 1},
          {4, 4}, {5, 3}, {6, 2},
          {4, 6}, {5, 5}, {6, 4}, {7, 3} },
        /* Position 3 */
        { {0, 12},
          {1, 11}, {1, 13},
          {2, 10}, {2, 12}, {2, 14},
          {3, 9},  {3, 11}, {3, 13}, {3, 15} },
        /* Position 4 */
        { {4, 24},
          {5, 23}, {4, 22},
          {6, 22}, {5, 21}, {4, 20},
          {7, 21}, {6, 20}, {5, 19}, {4,18} },
        /* Position 5 */
        { {12, 24},
          {12, 22}, {11, 23},
          {12, 20}, {11, 21}, {10, 22},
          {12, 18}, {11, 19}, {10, 20}, {9, 21} }
};

static int homes[6][6] = {
        { 0, -1, -1, -1, -1, -1 },      /* One player game (just filler) */
        { 0,  3, -1, -1, -1, -1 },
        { 0,  2,  4, -1, -1, -1 },
        { 0,  3,  1,  4, -1, -1 },
        { 0,  1,  2,  3,  4, -1 },      /* Five player game also filler */
        { 0,  3,  1,  4,  2,  5 }
};


/* Clear the game board to blanks */
static void game_zap_board(void)
{
	int i, j;

	for(i=0; i<17; i++)
		for(j=0; j<25; j++)
			if(init_board[i][j] == ' ')
				game.board[i][j] = -1;
			else
				game.board[i][j] = 0;

	display_refresh_board();
}


/* Setup the player positions on the board */
void game_init_board(void)
{
	int i, j, home, x, y;

	for(i=0; i<game.players; i++) {
		home = homes[game.players-1][i];
		for(j=0; j<10; j++) {
			x = homexy[home][j][0];
			y = homexy[home][j][1];
			game.board[x][y] = i+1;
		}
	}

	game_print_board();
	display_refresh_board();
}


/* Display the game board in text format */
static void game_print_board(void)
{
	int i, j;

	printf("Current game board:\n");
	for(i=0; i<17; i++) {
		for(j=0; j<25; j++)
			switch(game.board[i][j]) {
				case -1:
					printf(" ");
					break;
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					printf("%c", '0'+game.board[i][j]);
					break;
				default:
					printf("?");
			}
		printf("\n");
	}
}


void game_handle_click_event(int r, int c)
{
	static int click_state=0, ro, co;

	/* Is it my turn ? */
	if(!game.my_turn) {
		gdk_beep();
		return;
	}

	/* We are clicking the origin marble */
	if(click_state == 0) {
		if(game.board[r][c] == game.me+1) {
			ro = r;
			co = c;
			click_state++;
			return;
		}
		else {
			gdk_beep();
			return;
		}
	}

	/* We are clicking the desitination marble */
	if(game.board[r][c] == 0)
		if(game_make_move(ro, co, r, c)) {
			click_state = 0;
			return;
		}

	/* Bad dest */
	gdk_beep();
	click_state = 0;
	return;
}


static int game_make_move(int ro, int co, int rd, int cd)
{
	/* Verify move */
	if(!game_find_path(0, ro, co, rd, cd))
		return 0;

	game.my_turn = 0;

	game.board[ro][co] = 0;
	game.board[rd][cd] = game.me+1;

	if(es_write_int(game.fd, CC_SND_MOVE) < 0
	   || es_write_char(game.fd, ro) < 0
	   || es_write_char(game.fd, co) < 0
	   || es_write_char(game.fd, rd) < 0
	   || es_write_char(game.fd, cd) < 0)
		/* FIXME: Ugly way to die */
		exit(1);

	display_refresh_board();
	game_print_board();
}


static int game_find_path(int from, int ro, int co, int rd, int cd)
{
	int r, c;
	static char visited[17][25];

	if(from == 0) {
		/* Zero out the array of nodes we've visited */
		for(r=0; r<17; r++)
			for(c=0; c<25; c++)
				visited[r][c] = 0;

		/* Check each of the six immediate move directions for dest */
		/* We don't have to check if the cell is occupied, as we    */
		/* know the destination cell to be open */
		if((ro == rd && co-2 == cd)
		   || (ro-1 == rd && co-1 == cd)
		   || (ro-1 == rd && co+1 == cd)
		   || (ro == rd && co+2 == cd)
		   || (ro+1 == rd && co+1 == cd)
		   || (ro+1 == rd && co-1 == cd) )
			return 1;

		visited[ro][co] = 1;

		/* Check all six possible jump directions recursively */
		if((co-4 >= 0)
		   && (game.board[ro][co-2] > 0)
		   && (game.board[ro][co-4] == 0)
		   && game_find_path(1, ro, co-4, rd, cd) )
			return 1;
		if((co-2 >= 0 && ro-2 >= 0)
		   && (game.board[ro-1][co-1] > 0)
		   && (game.board[ro-2][co-2] == 0)
		   && game_find_path(2, ro-2, co-2, rd, cd) )
			return 1;
		if((co+2 < 25 && ro-2 >= 0)
		   && (game.board[ro-1][co+1] > 0)
		   && (game.board[ro-2][co+2] == 0)
		   && game_find_path(3, ro-2, co+2, rd, cd) )
			return 1;
		if((co+4 < 25)
		   && (game.board[ro][co+2] > 0)
		   && (game.board[ro][co+4] == 0)
		   && game_find_path(4, ro, co+4, rd, cd) )
			return 1;
		if((co+2 < 25 && ro+2 < 17)
		   && (game.board[ro+1][co+1] > 0)
		   && (game.board[ro+2][co+2] == 0)
		   && game_find_path(5, ro+2, co+2, rd, cd) )
			return 1;
		if((co-2 >= 0 && ro+2 < 17)
		   && (game.board[ro+1][co-1] > 0)
		   && (game.board[ro+2][co-2] == 0)
		   && game_find_path(6, ro+2, co-2, rd, cd) )
			return 1;

		/* Failed in all six directions, not a valid move */
		return 0;
	}

	/* Check to make sure we haven't visited here before */
	if(visited[ro][co])
		return 0;
	visited[ro][co] = 1;

	/* Is it soup yet? */
	if(ro == rd && co == cd)
		return 1;

	/* Check all six possible jump directions recursively */
	/* EXCEPT the one we came from */
	if((from != 4)
	   && (co-4 >= 0)
	   && (game.board[ro][co-2] > 0)
	   && (game.board[ro][co-4] == 0)
	   && game_find_path(1, ro, co-4, rd, cd) )
		return 1;
	if((from != 5)
	   && (co-2 >= 0 && ro-2 >= 0)
	   && (game.board[ro-1][co-1] > 0)
	   && (game.board[ro-2][co-2] == 0)
	   && game_find_path(2, ro-2, co-2, rd, cd) )
		return 1;
	if((from != 6)
	   && (co+2 < 25 && ro-2 >= 0)
	   && (game.board[ro-1][co+1] > 0)
	   && (game.board[ro-2][co+2] == 0)
	   && game_find_path(3, ro-2, co+2, rd, cd) )
		return 1;
	if((from != 1)
	   && (co+4 < 25)
	   && (game.board[ro][co+2] > 0)
	   && (game.board[ro][co+4] == 0)
	   && game_find_path(4, ro, co+4, rd, cd) )
		return 1;
	if((from != 2)
	   && (co+2 < 25 && ro+2 < 17)
	   && (game.board[ro+1][co+1] > 0)
	   && (game.board[ro+2][co+2] == 0)
	   && game_find_path(5, ro+2, co+2, rd, cd) )
		return 1;
	if((from != 3)
	   && (co-2 >= 0 && ro+2 < 17)
	   && (game.board[ro+1][co-1] > 0)
	   && (game.board[ro+2][co-2] == 0)
	   && game_find_path(6, ro+2, co-2, rd, cd) )
		return 1;

	return 0;
}


void game_notify_our_turn(void)
{
	game.my_turn = 1;
}


void game_opponent_move(int seat, int ro, int co, int rd, int cd)
{
	game.board[ro][co] = 0;
	game.board[rd][cd] = seat+1;

	display_refresh_board();
}
