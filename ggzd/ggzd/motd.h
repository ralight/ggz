/*
 * File: motd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 02/05/2000
 * Desc: Handle message of the day functions
 * $Id: motd.h 5923 2004-02-14 21:12:29Z jdorje $
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

/* Read the MOTD file.  Currently this can only safely be done when no
   threads are active on the server.  Call it with NULL to disable the
   MOTD (or clean up when done). */
void motd_read_file(const char *file);

/* Returns 'true' if motd is defined */
bool motd_is_defined(void);

/* Return number of lines in the motd */
int motd_get_num_lines(void);

/* Parse a line of the motd, filling in parameters */
char *motd_get_line(int line);
