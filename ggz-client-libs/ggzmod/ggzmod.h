/* 
 * File: ggzmod.h
 * Author: GGZ Development Team
 * Project: GGZMod library
 * Desc: GGZ game client module functions
 * $Id: ggzmod.h 3111 2002-01-14 05:53:06Z jdorje $
 *
 * This contains basic functions that can be used by game client programs
 * so that they can interface as a GGZ module.
 *
 * Copyright (C) 2000-2002 GGZ Development Team
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
 * @file ggzmod.h
 * @brief The interface for the ggzmod library used by game clients.
 *
 * This file contains all libggzmod functions used by game clients to
 * interface with GGZ.  Just include ggzmod.h and make sure your program
 * is linked with libggzmod.  Then use the functions below as appropriate.
 *
 * Under the GGZ model, instead of connecting directly to the game server a
 * game will be connected by GGZ.  To the client, everything should look the
 * same except that the method used to connect is different.
 *
 * @verbatim
 *                                 connect()
 *                          +--------...-->--------+
 *                          |                      |
 *                          |                      V
 *   ggzmod_connect() +-----+------+         +-----+------+ ggzdmod_connect()
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
 *             fd = ggzmod_connect();
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
 *         fd = ggzmod_connect();       // connect to GGZ
 *         if (fd < 0) return -1;
 *         gtk_init(&argc, &argv);
 *         gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);
 *         gtk_main();
 *         ggzmod_disconnect();         // disconnect from GGZ
 *         return 0;
 *     }
 * @endcode
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */

#ifndef __GGZ_CLIENT_GGZ_H
#define __GGZ_CLIENT_GGZ_H

#include <ggz_common.h>

/**
 * @brief Connects to GGZ.
 *
 * This does the physical work of connecting to GGZ.  It should be called
 * by the game client upon startup.
 *
 * @return The file descriptor for the TCP communications socket (or -1 on failure).
 * @see ggzmod_get_fd
 */
int ggzmod_connect(void);

/**
 * @brief Disconnects from GGZ.
 *
 * This disconnects from GGZ and destroys all internal data.  It
 * should be called by the game client before exiting.
 *
 * @return 0 on success, -1 on failure.
 */
int ggzmod_disconnect(void);

/**
 * @brief Returns the GGZ TCP communications socket.
 *
 * This returns the file descriptor of the TCP communications
 * socket.  The socket can be used for two-way communication to
 * the game server through TCP.
 *
 * @return The FD integer, or -1 on no connection.
 */
int ggzmod_get_fd(void);

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
int ggzmod_get_udp_fd(void);

#endif /* __GGZ_CLIENT_GGZ_H */
