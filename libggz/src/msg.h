/*
 * File: msg.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Debug and error messages
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


#include <config.h>

/* FIXME: these need to be standardized and cleaned up */

/** Debugging categories */
typedef enum {
	GGZ_DBG_EVENT     = 0x00000001,
	GGZ_DBG_NET       = 0x00000002,
	GGZ_DBG_USER      = 0x00000004,
	GGZ_DBG_SERVER    = 0x00000008,
	GGZ_DBG_CONF      = 0x00000010,
	GGZ_DBG_POLL      = 0x00000020,
	GGZ_DBG_STATE     = 0x00000040,
	GGZ_DBG_PLAYER    = 0x00000080,
	GGZ_DBG_ROOM      = 0x00000100,
	GGZ_DBG_TABLE     = 0x00000200,
	GGZ_DBG_GAMETYPE  = 0x00000400,
	GGZ_DBG_HOOK      = 0x00000800,
	GGZ_DBG_INIT      = 0x00001000,
	GGZ_DBG_MEMORY    = 0x00002000,
	GGZ_DBG_MEMDETAIL = 0x00004000,
	GGZ_DBG_MODULE    = 0x00008000,
	GGZ_DBG_GAME      = 0x00010000,
	GGZ_DBG_XML       = 0x00020000,
	GGZ_DBG_ALL       = 0xFFFFFFFF
} GGZDebugLevel;

void _ggz_debug_init(GGZDebugLevel levels, const char* file);
void _ggz_debug(const GGZDebugLevel level, const char *fmt, ...);
void _ggz_error_sys(const char *fmt, ...);
void _ggz_error_sys_exit(const char *fmt, ...);
void _ggz_error_msg(const char *fmt, ...);
void _ggz_error_msg_exit(const char *fmt, ...);
void _ggz_debug_cleanup(void);
