/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 - 2003 Josef Spillner, dr_maux@users.sourceforge.net

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TELGGZ_CHAT_H
#define TELGGZ_CHAT_H

/* Enter the main loop */
void chat_loop(void);

/* Request server id from player */
int chat_getserver(void);

/* Request username from player */
char *chat_getusername(void);

/* Request password from player */
char *chat_getpassword(void);

/* Attempt to connect to a GGZ server */
void chat_connect(const char *host, int port, const char *username, const char *password);

#endif

