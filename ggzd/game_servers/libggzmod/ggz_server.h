/*	$Id: ggz_server.h 2187 2001-08-23 06:43:28Z jdorje $	*/
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

/* Useful functions */
void ggzdmod_debug(const char *fmt, ...);

int ggzdmod_seats_open(void);
int ggzdmod_seats_num(void);
int ggzdmod_seats_bot(void);
int ggzdmod_seats_reserved(void);
int ggzdmod_seats_human(void);

int ggzdmod_fd_max(void);

/*
 * FIXME: libggzdmod currently allows for *either* interface:
 *  - In the old-style manual method ("teg's way"), the main GGZ
 *    loop is in the game code.  This code calls ggzdmod functions
 *    to handle GGZ events (as well as doing its own handling).
 *  - In the new-style event-driven method ("chess's way"), the main
 *    GGZ loop is in ggzdmod.  Functions are registered by the game
 *    server to handle specific GGZ events.  All GGZ handling is done
 *    automatically by the GGZ loop.
 *
 * Right now, these methods are entirely incompatible - using the
 * functions together will lead to Very Bad Things.  They shouldn't
 * even be in the same library - I've only put them together in this
 * file so that all code is out in the open and a sensible way for
 * them to work together can be thought of.
 *
 * It should be safe to convert your games to use libggzdmod through
 * either of these interfaces; although one or both of them will
 * probably be changed slightly it shouldn't be anything too drastic.
 */

/*
 * These allow the old-style GGZ interface.
 * The main GGZ loop should be in the game server, and
 * these functions are called to talk to GGZ.
 */

/* Setup functions */
int ggzdmod_connect(void);
int ggzdmod_disconnect(void);
/* No extra functions are needed at this time.
 * Later a stat-reporting function will be added. */

/* Event functions */
int ggzdmod_game_launch(void);
int ggzdmod_game_over(void);
int ggzdmod_player_join(int* seat, int *fd);
int ggzdmod_player_leave(int* seat, int *fd);

/* end of old-style GGZ interface */


/*
 * These allow the event-driven GGZ interface
 * The main GGZ loop is in ggz_server_main, and
 * callback functions are registered for specific events.
 */

enum {
	GGZ_EVENT_LAUNCH	= 0,
	GGZ_EVENT_JOIN		= 1,
	GGZ_EVENT_LEAVE		= 2,
	GGZ_EVENT_QUIT		= 3,
	GGZ_EVENT_PLAYER	= 4
};

/* Set a handler for a specific event */
typedef void (*GGZHandler)(int event_id, void *handler_data);
void ggzdmod_set_handler(int event_id, const GGZHandler handler);

/* Open the ggz socket and wait for events,
 * calling handlers when necessary */
int ggzdmod_main(char* game_name);

/* end of event-driven GGZ interface */


#endif /* __GGZ_SERVER_GGZ_H */
