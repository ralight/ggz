/*	$Id: ggz_server.h 2178 2001-08-20 01:38:01Z jdorje $	*/
/*
 * File: ggz.h
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#ifndef __GGZ_SERVER_GGZ_H
#define __GGZ_SERVER_GGZ_H

#define MAX_USER_NAME_LEN 16

/* Seat assignment values */
enum {
	GGZ_SEAT_OPEN	= -1,
	GGZ_SEAT_BOT	= -2,
	GGZ_SEAT_RESV	= -3,
	GGZ_SEAT_NONE	= -4,
	GGZ_SEAT_PLAYER	= -5
};

struct ggz_seat_t {
	int assign;
	char name[MAX_USER_NAME_LEN +1];
	int fd;
};

extern struct ggz_seat_t* ggz_seats;

/* Setup functions */
int ggz_server_init(char* game_name);
int ggz_server_connect(void);
int ggz_server_done(void);
void ggz_server_quit(void);

/* Game functions */
int ggz_game_launch(void);
int ggz_game_over(void);

/* Player functions */
int ggz_player_join(int* seat, int *fd);
int ggz_player_leave(int* seat, int *fd);

/* Useful functions */
void ggz_debug(const char *fmt, ...);

int ggz_seats_open(void);
int ggz_seats_num(void);
int ggz_seats_bot(void);
int ggz_seats_reserved(void);
int ggz_seats_human(void);

int ggz_fd_max(void);

#endif /* __GGZ_SERVER_GGZ_H */
