/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 4819 2002-10-08 23:32:22Z jdorje $
 *
 * Copyright (C) 1999 Brent Hendricks.
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

#ifndef _GGZ_PROTOCOL_H
#define _GGZ_PROTOCOL_H

#include <ggz_common.h>

/* I thought of putting this into ggz_common (in libggz) along with the
   protocol opcodes, but it really needs to stay tied to the network code
   itself (in ggzd and ggzcore). */
#define GGZ_CS_PROTO_VERSION 8

/* Update opcodes */
typedef enum {
	GGZ_UPDATE_DELETE = 0,
	GGZ_UPDATE_ADD	  = 1,
	GGZ_UPDATE_LEAVE  = 2,
	GGZ_UPDATE_JOIN	  = 3,
	GGZ_UPDATE_STATE  = 4,
	GGZ_UPDATE_LAG	  = 5,
	GGZ_UPDATE_SEAT   = 6,
	GGZ_UPDATE_DESC   = 7,
	GGZ_UPDATE_SPECTATOR_LEAVE = 8,
	GGZ_UPDATE_SPECTATOR_JOIN = 9,
	GGZ_UPDATE_SPECTATOR = 10
} GGZUpdateOpcode;

#endif /*_GGZ_PROTOCOL_H*/
