/*
 * ggz-txt - Command-line core client for the GGZ Gaming Zone.
 * This application is part of the ggz-txt-client package.
 *
 * loop.h: Functions for handling main IO loop.
 *
 * Copyright (C) 2000 Brent Hendricks
 * Copyright (C) 2001-2008 GGZ Gaming Zone Development Team
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

/* Callback type for various fd conditions (read, write, etc) */
typedef void (*callback)(void);

/* Iniitalize event loop (timeout in seconds) */
void loop_init(int timeout);

/* Add a file descriptor to the event loop */
void loop_add_fd(unsigned int fd, callback read, callback destroy);

/* Remove file descriptor to the event loop */
void loop_remove_fd(unsigned int fd);

/* Start event loop processing */
void loop(void);

/* Quite out of the loop */
void loop_quit(void);

