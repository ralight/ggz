/* 
 * File: ggz_server.h
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 10/24/01
 * Desc: GGZDMOD wrapper
 * $Id: ggz_server.h 2782 2001-12-06 00:24:12Z jdorje $
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

#define GGZ_EVENT_PLAYER GGZDMOD_EVENT_PLAYER_DATA
#define GGZ_EVENT_LAUNCH GGZDMOD_EVENT_STATE
#define GGZ_EVENT_QUIT   GGZDMOD_EVENT_STATE
#define GGZ_EVENT_JOIN   GGZDMOD_EVENT_JOIN
#define GGZ_EVENT_LEAVE   GGZDMOD_EVENT_LEAVE

	/* This interface is depricated and should not be used. */

	/* Return current state: CREATED, WAITING, PLAYING, DONE */
	GGZdModState ggzd_get_state(void);

	GGZdModSeat ggzd_get_seat_status(int seat);
	int ggzd_set_seat_status(int seat, GGZdModSeat status);

	const char *ggzd_get_player_name(int seat);
	int ggzd_set_player_name(int seat, char *name);

	int ggzd_get_player_socket(int seat);
#define ggzd_get_player_udp_socket(seat) (-1)

	int ggzd_debug(const char *fmt, ...);

#define ggzd_seats_num() ggzd_get_seats_num()
#define ggzd_seats_open() ggzd_get_seat_count(GGZ_SEAT_OPEN)
#define ggzd_seats_bot() ggzd_get_seat_count(GGZ_SEAT_BOT)
#define ggzd_seats_reserved() ggzd_get_seat_count(GGZ_SEAT_RESV)
#define ggzd_seats_human() ggzd_get_seat_count(GGZ_SEAT_PLAYER)

	int ggzd_get_seats_num(void);
	int ggzd_get_seat_count(GGZdModSeat status);
	void ggzd_gameover(int status);
	int ggzd_get_gameover(void);

	typedef void (*GGZDHandler) (GGZdModEvent event_id,
				     void *handler_data);

	void ggzd_set_handler(GGZdModEvent event_id,
			      const GGZDHandler handler);

	int ggzd_connect(void);
	int ggzd_disconnect(void);

	void ggzd_dispatch(void);
	int ggzd_io_is_pending(void);
	void ggzd_io_read_all(void);
	int ggzd_main_loop(void);

#ifdef __cplusplus
}
#endif
#endif				/* __GGZ_SERVER_GGZ_H */
