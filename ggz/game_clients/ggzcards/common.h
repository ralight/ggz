/* $id$ */
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


/** This function should be called when the client first launches.  It
  * initializes all internal data and makes the connection to the GGZ
  * client. */
void client_initialize(void);

/** This function should be called just before the client exits. */
void client_quit(void);


/** Handles the debug message appropriately.
 *  @param fmt a printf-style format string.
 *  @param ... printf-style arguments. */
void client_debug(const char *fmt, ...);
