/* $Id: common.h 2072 2001-07-23 00:38:33Z jdorje $ */
/*
 * File: common.h
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Frontend to GGZCards Client-Common
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

/** This is the file descriptor that can be used for communication with the
 *  game server (via GGZ).  In theory, it should only be used internally
 *  by the library - but that's still a work in progress.
 *  @see client_initialize
 */
extern int ggzfd;


/** @defgroup Setup
 *  Setup functions to be called by the frontend
 *  @{ */

/** This function should be called when the client first launches.  It
  * initializes all internal data and makes the connection to the GGZ
  * client. */
void client_initialize();

/** This function should be called just before the client exits. */
void client_quit();

/** @} end of Setup */


/** Handles the debug message appropriately.
 *  @param fmt a printf-style format string.
 *  @param ... printf-style arguments. */
void client_debug(const char *fmt, ...);


/** @defgroup Messaging
 * Functions used for the messaging system.
 * @{ */

/** Should be called when a WH_MESSAGE_GLOBAL is received.
 *  @return 0 on success, -1 on failure */
int handle_message_global();

/** Should be called when a WH_MESSAGE_PLAYER is received.
 *  @return 0 on success, -1 on failure */
int handle_message_player();

/** A gui-dependent function called to set the global message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from handle_message_global.
 *  @param mark The "mark" ID tag string of the message.
 *  @param msg The message itself. */
extern void table_set_global_message(const char *mark, const char *msg);

/** A gui-dependent function called to set a player message.
 *  This should be defined by the frontend code and is accessed by a
 *  callback from handle_message_player.
 *  @param player The player number for which the message is intended.
 *  @param msg The message itself. */
extern void table_set_player_message(int player, const char *msg);

/** @} end of Messaging */
