/*
 * File: ggz_client.h
 * Author: GGZ Development Team
 * Project: GGZMod library
 * Desc: GGZ game module functions
 * $Id: ggz_client.h 2867 2001-12-10 22:30:06Z jdorje $
 *
 * Copyright (C) 2000 GGZ devel team
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

/**
 * @file ggz_client.h
 * @brief The interface for the ggzmod library used by game clients.
 *
 * This file contains all libggzmod functions used by game clients to
 * interface with GGZ.  Just include ggz_client.h and make sure your program
 * is linked with libggzmod.  Then use the functions below as appropriate.
 *
 * Under the GGZ model, instead of connecting directly to the game server a
 * game will connect to the GGZ client using a similar socket.  The GGZ client
 * and server will then relay packets between the game client and server.  To
 * the client, everything should look the same except that the method used to
 * connect is different.
 *
 * @verbatim
 *                                 connect()
 *                          +--------...-->--------+
 *                          |                      |
 *                          |                      V
 *    ggz_connect()   +-----+------+         +-----+------+   ggzd_connect()
 *              +---->+ GGZ client |         | GGZ server +<----+
 *              |     +------------+         +------------+     |
 *              |              ^                |               |
 *           +--------+        |                |           +--------+
 *           |  game  |        |                |           |  game  |
 *           | client |        +-----...-<------+           | server |
 *           +--------+                                     +--------+
 * @endverbatim
 *
 * Here is a generic example of how a game should be changed to use GGZ.  Say
 * the old code looks like this:
 * @code
 *     void main() {
 *         int fd;
 *
 *         // your typical connection function
 *         fd = connect_to_server();
 *         ...
 *     }
 * @endcode
 * Then your new code might look like this:
 * @code
 *     int main() {
 *         int fd;
 *
 *         if (with_ggz) // typically determined by command-line option
 *             fd = ggz_connect();
 *         else          // if not using GGZ, use the old method
 *             fd = connect_to_server();
 *         ...
 *     }
 * @endcode
 *
 * Here is an example of how a GTK client may connect:
 * @code
 *     int fd;
 *
 *     // this function reads input from the socket (server) and handles it
 *     void game_handle_io(gpointer data, gint source, GdkInputCondition cond) {
 *         ...
 *     }
 *
 *     int main(int argc, char** argv) {
 *         fd = ggz_connect();          // connect to GGZ
 *         if (fd < 0) return -1;
 *         gtk_init(&argc, &argv);
 *         gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);
 *         gtk_main();
 *         ggz_disconnect();            // disconnect from GGZ
 *         return 0;
 *     }
 * @endcode
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */

#ifndef __GGZ_CLIENT_GGZ_H
#define __GGZ_CLIENT_GGZ_H

/** @brief Seat status values.
 *
 * Each "seat" at a table of a GGZ game can have one of these values.
 * They are used by ggzdmod and the game server; their use in game
 * clients is completely optional.
 */
typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZdModSeat;

/**
 * @brief Connects to GGZ.
 *
 * This does the physical work of connecting to GGZ.  It should be called
 * by the game client upon startup.
 *
 * @return The file descriptor for the TCP communications socket (or -1 on failure).
 * @see ggz_get_sock
 * @todo Should it just return 0 on success, -1 on failure?
 */
int ggz_connect(void);

/**
 * @brief Disconnects from GGZ.
 *
 * This disconnects from GGZ and destroys all internal data.  It
 * should be called by the game client before exiting.
 *
 * @return 0 on success, -1 on failure.
 */
int ggz_disconnect(void);

/**
 * @brief Returns the GGZ TCP communications socket.
 *
 * This returns the file descriptor of the TCP communications
 * socket.  The socket can be used for two-way communication to
 * the game server through TCP.
 *
 * @return The FD integer, or -1 on no connection.
 */
int ggz_get_sock(void);

/**
 * @brief Returns the GGZ UDP communications socket.
 *
 * This returns the file descriptor of the UDP communications
 * socket.  The socket can be used for two-way communication to
 * the game server through UDP.
 *
 * @return The FD integer, or -1 on no connection.
 * @todo Is this a good way to access the UDP socket?
 * @todo This functionality is not yet implemented; it will return -1.
 */
int ggz_get_udp_sock(void);

#endif /* __GGZ_CLIENT_GGZ_H */
