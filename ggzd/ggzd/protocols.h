/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 4051 2002-04-22 19:40:12Z jdorje $
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

/* Chat subops - these are defined separately in ggzcore */
							/* PMCCCCCC */
#define GGZ_CHAT_NORMAL		(unsigned char) 0x40	/* 01000000 */
#define GGZ_CHAT_ANNOUNCE	(unsigned char) 0x60	/* 01100000 */
#define GGZ_CHAT_BEEP		(unsigned char) 0x80	/* 10000000 */
#define GGZ_CHAT_PERSONAL	(unsigned char) 0xC0	/* 11000000 */

/* Update opcodes */
typedef enum {
	GGZ_UPDATE_DELETE = 0,
	GGZ_UPDATE_ADD	  = 1,
	GGZ_UPDATE_LEAVE  = 2,
	GGZ_UPDATE_JOIN	  = 3,
	GGZ_UPDATE_STATE  = 4,
	GGZ_UPDATE_LAG	  = 5,
	GGZ_UPDATE_SEAT   = 6,
	GGZ_UPDATE_DESC   = 7
} GGZUpdateOpcode;

#endif /*_GGZ_PROTOCOL_H*/
