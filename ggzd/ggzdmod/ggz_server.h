/* 
 * File: ggz_server.h
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 10/24/01
 * Desc: GGZDMOD wrapper
 * $Id: ggz_server.h 2823 2001-12-09 08:16:26Z jdorje $
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


#ifndef __GGZ_SERVER_GGZ_H
#define __GGZ_SERVER_GGZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ggzdmod.h"

	extern GGZdMod *ggzdmod;

	/* This interface is depricated and should not be used.  I
	   am in the process of phasing it out. */

#define ggzd_get_seat_status(seat) (ggzdmod_get_seat(ggzdmod, seat).type)

#define ggzd_get_player_name(seat) (ggzdmod_get_seat(ggzdmod, seat).name)
	int ggzd_set_player_name(int seat, char *name);

#define ggzd_get_player_socket(seat) (ggzdmod_get_seat(ggzdmod, seat).fd)

#ifdef __cplusplus
}
#endif
#endif				/* __GGZ_SERVER_GGZ_H */
