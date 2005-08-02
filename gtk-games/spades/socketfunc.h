/*
 * File: socketfunc.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 4/16/98
 *
 * Declarations for my useful socket functions
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


#ifndef _SOCKETFUNC_H
#define _SOCKETFUNC_H

#define SERVER 0
#define CLIENT 1
#define SPADEPORT 7626
#define TAUNTPORT 7627

#define ERR_SOCK_OPEN -1
#define ERR_SOCK_BIND -2
#define ERR_HOST      -3
#define ERR_SOCK_CONN -4
#define ERR_OFFSET     4

int makesocket(unsigned short int, short int, char *);

int writestring(int, const char *);

int readstring(int, char **);

int writeint(int, const int);

int readint(int, int *);

#endif
