/*
 * File: client.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
 *
 * This rather empty file declares the client function which actually
 * runs most of the client side of the spades game
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef _CLIENT_H
#define _CLIENT_H

#define NET_FAIL      -3
#define NET_OK         0


int CheckWriteString(int msgsock, char *message);
int CheckWriteInt(int msgsock, int message);
int CheckReadString(int msgsock, char **message);
int CheckReadInt(int msgsock, int *message);

void NetClose(void);
RETSIGTYPE die(int sig);

#endif
