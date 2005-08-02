/*
 * File: shared.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 02/25/2002
 * Desc: Card data for the GGZCards client and server
 * $Id: shared.h 4162 2002-05-05 20:16:23Z jdorje $
 *
 * This contains miscellaneous definitions common to both GGZCards
 * client and server.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 2002 Brent Hendricks.
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

#ifndef __SHARED_H__
#define __SHARED_H__

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

typedef int bool;

#ifndef MIN
# define MIN(a, b) ( (a) < (b) ? (a) : (b) )
#endif

#ifndef MAX
# define MAX(a, b) ( (a) > (b) ? (a) : (b) )
#endif

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* __SHARED_H__ */
