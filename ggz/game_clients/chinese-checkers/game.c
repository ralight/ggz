/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Core game structures and logic
 * $Id: game.c 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2001-2002 Richard Gade.
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
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include "dlg_players.h"
#include "ggzintl.h"

#include "game.h"
#include "display.h"
#include "main.h"
#include "protocol.h"


struct game_t game;
int homes[6][6] = {
	{0, -1, -1, -1, -1, -1},	/* One player game (just filler) */
	{0, 3, -1, -1, -1, -1},
	{0, 2, 4, -1, -1, -1},
	{0, 1, 3, 4, -1, -1},
	{0, 1, 2, 3, 4, -1},	/* Five player game also filler */
	{0, 1, 2, 3, 4, 5}
};

static GSList *path_list;

static int game_make_move(int, int, int, int);
static int game_find_path(int, int, int, int, int);
static void get_theme_data(void);

static GGZMod *mod;

static void handle_ggz(gpointer data, gint source, GdkInputCondition cond)
{
	ggzmod_dispatch(mod);
}

static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e, void *data)
{
	int fd = *(int *)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	game.fd = fd;
	gdk_input_add(fd, GDK_INPUT_READ, main_io_handler, NULL);
}

GGZMod *game_mod(void)
{
	return mod;
}

/* Perform game initialization tasks */
void game_init(void)
{
	char *filename;

	/* Connect to GGZ */
	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER,
			   &handle_ggzmod_server);

	init_player_list(mod);

	ggzmod_connect(mod);

	/* Trap our input from the socket */
	gdk_input_add(ggzmod_get_fd(mod), GDK_INPUT_READ, handle_ggz,
		      NULL);

	/* Get our preferences */
	filename =
	    g_strdup_printf("%s/.ggz/ccheckers-gtk.rc", getenv("HOME"));
	game.conf_handle =
	    ggz_conf_parse(filename, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	g_free(filename);
	get_theme_data();
	game.beep = ggz_conf_read_int(game.conf_handle, "Options",
				      "Beep", 1);

	/* Display the main dialog */
	game_zap_board();
	if (display_init() != 0) {
		/* Theme loading failed, try default dir */
		ggz_free(game.theme);
		game.theme = ggz_strdup("default");
		if (display_init() != 0) {
			ggz_error_msg("Failed to load default theme!");
			exit(1);
		}
	}
}


static char init_board[ROWS][COLS] = {
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
	{{16, 12},
	 {15, 11}, {15, 13},
	 {14, 10}, {14, 12}, {14, 14},
	 {13, 9}, {13, 11}, {13, 13}, {13, 15}},
	/* Position 1 */
	{{12, 0},
	 {11, 1}, {12, 2},
	 {10, 2}, {11, 3}, {12, 4},
	 {9, 3}, {10, 4}, {11, 5}, {12, 6}},
	/* Position 2 */
	{{4, 0},
	 {4, 2}, {5, 1},
	 {4, 4}, {5, 3}, {6, 2},
	 {4, 6}, {5, 5}, {6, 4}, {7, 3}},
	/* Position 3 */
	{{0, 12},
	 {1, 11}, {1, 13},
	 {2, 10}, {2, 12}, {2, 14},
	 {3, 9}, {3, 11}, {3, 13}, {3, 15}},
	/* Position 4 */
	{{4, 24},
	 {5, 23}, {4, 22},
	 {6, 22}, {5, 21}, {4, 20},
	 {7, 21}, {6, 20}, {5, 19}, {4, 18}},
	/* Position 5 */
	{{12, 24},
	 {12, 22}, {11, 23},
	 {12, 20}, {11, 21}, {10, 22},
	 {12, 18}, {11, 19}, {10, 20}, {9, 21}}
};

static char *color[6] =
    { "red", "blue", "green", "yellow", "cyan", "purple" };


/* Clear the game board to blanks */
void game_zap_board(void)
{
	int i, j;

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
			if (init_board[i][j] == ' ')
				game.board[i][j] = BOARD_NONE;
			else
				game.board[i][j] = BOARD_EMPTY;
}


/* Setup the player positions on the board */
void game_init_board(void)
{
	int i, j, home, x, y;
	char *msg;

	for (i = 0; i < game.players; i++) {
		home = homes[game.players - 1][i];
		for (j = 0; j < 10; j++) {
			x = homexy[home][j][0];
			y = homexy[home][j][1];
			game.board[x][y] = i + 1;
		}
	}

	display_set_label_colors();
	display_refresh_board();

	msg = g_strdup_printf(_("You are playing the %s marbles"),
			      color[game.me]);
	display_statusbar(msg);
	g_free(msg);
}


void game_handle_click_event(int r, int c)
{
	static int click_state = 0, ro, co;

	/* Is it my turn ? */
	if (!game.my_turn) {
		gdk_beep();
		display_statusbar(_("Wait for your turn..."));
		return;
	}

	/* We are clicking the origin marble */
	if (click_state == 0) {
		if (game.board[r][c] == game.me + 1) {
			ro = r;
			co = c;
			click_state++;
			display_statusbar(_
					  ("Click where you want to move the marble."));
			return;
		} else {
			gdk_beep();
			return;
		}
	}

	/* We are clicking the destination marble */
	if (game.board[r][c] == 0) {
		if (game_make_move(ro, co, r, c)) {
			click_state = 0;
			display_statusbar(_
					  ("Sending move to GGZ server..."));
			return;
		}
	}

	/* Bad dest */
	gdk_beep();
	display_statusbar(_("Bad move! Click a marble to move."));
	click_state = 0;
	return;
}


static int game_make_move(int ro, int co, int rd, int cd)
{
	/* Verify move */
	if (!game_find_path(0, ro, co, rd, cd))
		return 0;

	game.my_turn = 0;

	game.board[ro][co] = 0;
	game.board[rd][cd] = game.me + 1;

	if (ggz_write_int(game.fd, CC_SND_MOVE) < 0
	    || ggz_write_char(game.fd, ro) < 0
	    || ggz_write_char(game.fd, co) < 0
	    || ggz_write_char(game.fd, rd) < 0
	    || ggz_write_char(game.fd, cd) < 0)
		/* FIXME: Ugly way to die */
		exit(1);

	display_refresh_board();
	display_show_path(path_list);

	return 1;
}


static int game_find_path(int from, int ro, int co, int rd, int cd)
{
	int r, c;
	struct node_t *node;
	static char visited[17][25];

	if (from == 0) {
		/* Zero out the array of nodes we've visited */
		for (r = 0; r < 17; r++)
			for (c = 0; c < 25; c++)
				visited[r][c] = 0;

		/* Clear our path list */
		path_list = NULL;

		/* Check each of the six immediate move directions for dest */
		/* We don't have to check if the cell is occupied, as we    */
		/* know the destination cell to be open */
		if ((ro == rd && co - 2 == cd)
		    || (ro - 1 == rd && co - 1 == cd)
		    || (ro - 1 == rd && co + 1 == cd)
		    || (ro == rd && co + 2 == cd)
		    || (ro + 1 == rd && co + 1 == cd)
		    || (ro + 1 == rd && co - 1 == cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = rd;
			node->cd = cd;
			path_list = g_slist_append(path_list, node);
			return 1;
		}

		visited[ro][co] = 1;

		/* Check all six possible jump directions recursively */
		if ((co - 4 >= 0)
		    && (game.board[ro][co - 2] > 0)
		    && (game.board[ro][co - 4] == 0)
		    && game_find_path(1, ro, co - 4, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro;
			node->cd = co - 4;
			path_list = g_slist_append(path_list, node);
			return 1;
		}
		if ((co - 2 >= 0 && ro - 2 >= 0)
		    && (game.board[ro - 1][co - 1] > 0)
		    && (game.board[ro - 2][co - 2] == 0)
		    && game_find_path(2, ro - 2, co - 2, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro - 2;
			node->cd = co - 2;
			path_list = g_slist_append(path_list, node);
			return 1;
		}
		if ((co + 2 < 25 && ro - 2 >= 0)
		    && (game.board[ro - 1][co + 1] > 0)
		    && (game.board[ro - 2][co + 2] == 0)
		    && game_find_path(3, ro - 2, co + 2, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro - 2;
			node->cd = co + 2;
			path_list = g_slist_append(path_list, node);
			return 1;
		}
		if ((co + 4 < 25)
		    && (game.board[ro][co + 2] > 0)
		    && (game.board[ro][co + 4] == 0)
		    && game_find_path(4, ro, co + 4, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro;
			node->cd = co + 4;
			path_list = g_slist_append(path_list, node);
			return 1;
		}
		if ((co + 2 < 25 && ro + 2 < 17)
		    && (game.board[ro + 1][co + 1] > 0)
		    && (game.board[ro + 2][co + 2] == 0)
		    && game_find_path(5, ro + 2, co + 2, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro + 2;
			node->cd = co + 2;
			path_list = g_slist_append(path_list, node);
			return 1;
		}
		if ((co - 2 >= 0 && ro + 2 < 17)
		    && (game.board[ro + 1][co - 1] > 0)
		    && (game.board[ro + 2][co - 2] == 0)
		    && game_find_path(6, ro + 2, co - 2, rd, cd)) {
			node = g_malloc(sizeof(*node));
			node->ro = ro;
			node->co = co;
			node->rd = ro + 2;
			node->cd = co - 2;
			path_list = g_slist_append(path_list, node);
			return 1;
		}

		/* Failed in all six directions, not a valid move */
		return 0;
	}

	/* Check to make sure we haven't visited here before */
	if (visited[ro][co])
		return 0;
	visited[ro][co] = 1;

	/* Is it soup yet? */
	if (ro == rd && co == cd)
		return 1;

	/* Check all six possible jump directions recursively */
	/* EXCEPT the one we came from */
	if ((from != 4)
	    && (co - 4 >= 0)
	    && (game.board[ro][co - 2] > 0)
	    && (game.board[ro][co - 4] == 0)
	    && game_find_path(1, ro, co - 4, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro;
		node->cd = co - 4;
		path_list = g_slist_append(path_list, node);
		return 1;
	}
	if ((from != 5)
	    && (co - 2 >= 0 && ro - 2 >= 0)
	    && (game.board[ro - 1][co - 1] > 0)
	    && (game.board[ro - 2][co - 2] == 0)
	    && game_find_path(2, ro - 2, co - 2, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro - 2;
		node->cd = co - 2;
		path_list = g_slist_append(path_list, node);
		return 1;
	}
	if ((from != 6)
	    && (co + 2 < 25 && ro - 2 >= 0)
	    && (game.board[ro - 1][co + 1] > 0)
	    && (game.board[ro - 2][co + 2] == 0)
	    && game_find_path(3, ro - 2, co + 2, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro - 2;
		node->cd = co + 2;
		path_list = g_slist_append(path_list, node);
		return 1;
	}
	if ((from != 1)
	    && (co + 4 < 25)
	    && (game.board[ro][co + 2] > 0)
	    && (game.board[ro][co + 4] == 0)
	    && game_find_path(4, ro, co + 4, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro;
		node->cd = co + 4;
		path_list = g_slist_append(path_list, node);
		return 1;
	}
	if ((from != 2)
	    && (co + 2 < 25 && ro + 2 < 17)
	    && (game.board[ro + 1][co + 1] > 0)
	    && (game.board[ro + 2][co + 2] == 0)
	    && game_find_path(5, ro + 2, co + 2, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro + 2;
		node->cd = co + 2;
		path_list = g_slist_append(path_list, node);
		return 1;
	}
	if ((from != 3)
	    && (co - 2 >= 0 && ro + 2 < 17)
	    && (game.board[ro + 1][co - 1] > 0)
	    && (game.board[ro + 2][co - 2] == 0)
	    && game_find_path(6, ro + 2, co - 2, rd, cd)) {
		node = g_malloc(sizeof(*node));
		node->ro = ro;
		node->co = co;
		node->rd = ro + 2;
		node->cd = co - 2;
		path_list = g_slist_append(path_list, node);
		return 1;
	}

	return 0;
}


void game_notify_our_turn(void)
{
	game.my_turn = 1;

	display_statusbar(_("Your move. Click a marble to move"));

	if (game.beep)
		gdk_beep();
}


void game_opponent_move(int seat, int ro, int co, int rd, int cd)
{
	(void)game_find_path(0, ro, co, rd, cd);

	game.board[ro][co] = 0;
	game.board[rd][cd] = seat + 1;

	display_refresh_board();
	display_show_path(path_list);
}


void game_update_config(char *theme, int beep)
{
	if (beep != game.beep) {
		game.beep = beep;
		ggz_conf_write_int(game.conf_handle, "Options",
				   "Beep", game.beep);
	}

	if (strcmp(theme, game.theme)) {
		ggz_free(game.theme);
		game.theme = ggz_strdup(theme);
		ggz_conf_write_string(game.conf_handle, "Options",
				      "Theme", game.theme);
		display_init();
		display_refresh_board();
	}

	ggz_conf_commit(game.conf_handle);
}


static int select_dirs(const struct dirent *d)
{
	int ok = 0;
	struct stat buf;
	char *pathname;

	pathname = g_strdup_printf("%s/ccheckers/pixmaps/%s", GGZDATADIR,
				   d->d_name);
	if (stat(pathname, &buf) == -1)
		perror(pathname);
	if (S_ISDIR(buf.st_mode))
		ok = 1;
	g_free(pathname);

	if (ok && d->d_name[0] != '.')
		return 1;
	return 0;
}


static void get_theme_data(void)
{
	char *theme_dir;
	struct dirent **namelist;
	int i;

	/* Get the directory for themes and the .rc theme setting */
	theme_dir = g_strdup_printf("%s/ccheckers/pixmaps", GGZDATADIR);
	ggz_debug("main", "%s", theme_dir);
	game.theme = ggz_conf_read_string(game.conf_handle,
					  "Options", "Theme", "default");

	/* Scan the theme directory and build an array of installed themes */
	game.num_themes =
	    scandir(theme_dir, &namelist, select_dirs, alphasort);
	game.theme_names = calloc(game.num_themes, sizeof(char *));
	for (i = 0; i < game.num_themes; i++)
		game.theme_names[i] = g_strdup(namelist[i]->d_name);

	g_free(theme_dir);
}


char *get_theme_dir(void)
{
	static char *theme_dir = NULL;

	if (theme_dir != NULL)
		g_free(theme_dir);

	theme_dir = g_strdup_printf("%s/ccheckers/pixmaps/%s", GGZDATADIR,
				    game.theme);

	return theme_dir;
}


#if 0
/* Display the game board in text format */
static void game_print_board(void)
{
	int i, j;

	/* "board" will have to be added to ggz_debug_init
	   in main.c to get this to work.  --JDS */

	ggz_debug("board", "Current game board:");
	for (i = 0; i < 17; i++) {
		char buf[128] = "";
		for (j = 0; j < 25; j++)
			switch (game.board[i][j]) {
			case -1:
				snprintf(buf, sizeof(buf) - strlen(buf),
					 " ");
				break;
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				snprintf(buf, sizeof(buf) - strlen(buf),
					 "%c", '0' + game.board[i][j]);
				break;
			default:
				snprintf(buf, sizeof(buf) - strlen(buf),
					 "?");
			}
		ggz_debug("board", "%s", buf);
	}
}
#endif
