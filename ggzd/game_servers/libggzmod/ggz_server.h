/*
 * File: ggz.h
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 * $Id: ggz_server.h 2228 2001-08-25 14:09:43Z jdorje $
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

/** Seat assignment values */
enum {
	GGZ_SEAT_OPEN	= -1, /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT	= -2, /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV	= -3, /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE	= -4, /**< This seat does not exist. */
	GGZ_SEAT_PLAYER	= -5  /**< The seat has a regular player in it. */
};

/** A GGZ seat; one spot at a table. */
struct ggz_seat_t {
	/** The seat assignment value */
	int assign;
	/** The name of the player at the seat */
	char name[MAX_USER_NAME_LEN +1];
	/** A file descriptor for communicating with that player */
	int fd;
};

/** The array of seats at the table */
extern struct ggz_seat_t* ggz_seats;

/** Sends a debugging message to ggzd to be logged.
 *  @param fmt a printf-style format string
 *  @param ... a printf-stype list of arguments
 *  @return 0 on success, -1 on failure */
int ggzdmod_debug(const char *fmt, ...);

int ggzdmod_seats_open(void);
int ggzdmod_seats_num(void);
int ggzdmod_seats_bot(void);
int ggzdmod_seats_reserved(void);
int ggzdmod_seats_human(void);

int ggzdmod_fd_max(void);

/*
 * libggzdmod currently allows for an event-driven interface:
 *  - In the new-style event-driven method ("chess's way"), the main
 *    GGZ loop is in ggzdmod.  Functions are registered by the game
 *    server to handle specific GGZ events.  All GGZ handling is done
 *    automatically by the GGZ loop.
 */

enum {
	GGZ_EVENT_LAUNCH	= 0,  /**< a game launch event from ggzd */
	GGZ_EVENT_JOIN		= 1,  /**< a player join event from ggzd */
	GGZ_EVENT_LEAVE		= 2,  /**< a player leave event from ggzd */
	GGZ_EVENT_QUIT		= 3,  /**< a game over event from ggzd */
	GGZ_EVENT_PLAYER	= 4,  /**< a message from a client/player */
	GGZ_EVENT_TICK		= 5   /**< a passed-time event */
};

/* Set a handler for a specific event.
 * passing GGZ_EVENT_DEFAULT sets the handler for all unhandled events */
/* NOTE that when the handler is called the ggzdmod event will
 * already have been handled from the GGZ end; you *don't* have to
 * call a ggzdmod event function from above */
typedef int (*GGZHandler)(int event_id, void *handler_data);
void ggzdmod_set_handler(int event_id, const GGZHandler handler);

/* Setup functions */
int ggzdmod_connect(void);
int ggzdmod_disconnect(void);

/* this function should be called when there's GGZ data ready
 * to be read */
int ggzdmod_dispatch(void);

/* this function may not be a part of the final API */
int ggzdmod_read_data(void);

/* Open the ggz socket and wait for events,
 * calling handlers when necessary
 * Handles connect and disconnect also. */
int ggzdmod_main(void);


#endif /* __GGZ_SERVER_GGZ_H */
