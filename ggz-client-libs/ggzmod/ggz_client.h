/*
 * File: ggz_client.h
 * Author: GGZ Development Team
 * Project: GGZMod library
 * Desc: GGZ game module functions
 * $Id: ggz_client.h 2210 2001-08-23 22:08:51Z jdorje $
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
 * @brief Interface for ggzmod library.
 *
 * This file contains all libggzmod functions used by game clients to
 * interface with GGZ.  Just include ggz_client.h and make sure your program
 * is linked with libggzmod.  Then use the functions below as appropriate.
 */

#ifndef __GGZ_CLIENT_GGZ_H
#define __GGZ_CLIENT_GGZ_H

/**
 * This does the physical work of connecting the game client to the GGZ client
 * (which will then relay information through the ggz server to your game 
 * server).  It should be called by your game at the appropriate time.
 *
 * @return The file descriptor for the communications socket (or -1 on failure).
 */
int ggz_client_connect(void);

/**
 * This destroys all of ggzmod's internal data.  It does not yet disconnect 
 * the communications socket.
 *
 * @return 0 on success; -1 on failure.
 */
int ggz_client_quit(void);

/**
 * This prepares for connection to the GGZ client.  It should be
 * called before ggz_client_connect.
 *
 * @param game_name The name of the card game; currently unused.
 * @return 0 on success; -1 on failure.
 */
int ggz_client_init(char *game_name);

/**
 *  This returns the file descriptor of the communications socket to
 *  the ggz client.
 *
 * @return The FD integer, or -1 on no connection.
 */
int ggz_client_get_sock(void);

#endif /* __GGZ_CLIENT_GGZ_H */
