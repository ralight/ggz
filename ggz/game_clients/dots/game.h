/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots game module
 * Date: 04/27/2000
 * Desc: Routines to manipulate the CtD board
 * $Id: game.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

/* Connect the Dots protocol */
/* Messages from server */
#define DOTS_MSG_SEAT     0
#define DOTS_MSG_PLAYERS  1
#define DOTS_MSG_MOVE_H   2
#define DOTS_MSG_MOVE_V   3
#define DOTS_MSG_GAMEOVER 4
#define DOTS_REQ_MOVE     5
#define DOTS_RSP_MOVE     6
#define DOTS_SND_SYNC     7
#define DOTS_MSG_OPTIONS  8
#define DOTS_REQ_OPTIONS  9

/* Move errors */
#define DOTS_ERR_STATE   -1
#define DOTS_ERR_TURN    -2
#define DOTS_ERR_BOUND   -3
#define DOTS_ERR_FULL    -4

/* Messages from client */
#define DOTS_SND_MOVE_H   0
#define DOTS_SND_MOVE_V   1
#define DOTS_REQ_SYNC     2
#define DOTS_SND_OPTIONS  3
#define DOTS_REQ_NEWGAME  4

/* Connect the Dots game states */
#define DOTS_STATE_INIT        0
#define DOTS_STATE_WAIT        1
#define DOTS_STATE_MOVE        2
#define DOTS_STATE_DONE        3
#define DOTS_STATE_OPPONENT    4
#define DOTS_STATE_CHOOSE      5

/* Connect the Dots game events */
#define DOTS_EVENT_LAUNCH      0
#define DOTS_EVENT_JOIN        1
#define DOTS_EVENT_LEAVE       2
#define DOTS_EVENT_MOVE_H      3
#define DOTS_EVENT_MOVE_V      4


/* Exposed variables */
extern guint8 board_width, board_height;
extern guint8 vert_board[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT - 1];
extern guint8 horz_board[MAX_BOARD_WIDTH - 1][MAX_BOARD_HEIGHT];
extern gint8 owners_board[MAX_BOARD_WIDTH - 1][MAX_BOARD_HEIGHT - 1];
extern GdkColor fg_color, bg_color, p1_color, p2_color;

/* Global functions exported from game.c */
extern void board_init(guint8, guint8);
extern void board_handle_expose_event(GtkWidget *, GdkEventExpose *);
extern void board_handle_click(GtkWidget *, GdkEventButton *);
extern void board_handle_pxb_expose(void);
extern void statusbar_message(gchar *);
extern gint8 board_opponent_move(guint8);
extern void board_redraw(void);
extern void game_apply_colors(GdkColor, GdkColor, GdkColor, GdkColor);
extern void game_write_colors(void);
