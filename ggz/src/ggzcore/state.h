/*
 * File: state.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for handling state manipulations
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

#ifndef __STATE_H__
#define __STATE_H__

#include <ggzcore.h>

typedef enum {
	GGZ_STATE_OFFLINE,
	GGZ_STATE_CONNECTING,
	GGZ_STATE_ONLINE,
	GGZ_STATE_LOGGING_IN,
	GGZ_STATE_LOGGED_IN,
	GGZ_STATE_ENTERING_ROOM,
	GGZ_STATE_IN_ROOM,
	GGZ_STATE_JOINING_TABLE,
	GGZ_STATE_AT_TABLE,
	GGZ_STATE_LEAVING_TABLE,
	GGZ_STATE_LOGGING_OUT
} GGZStateID;


struct _GGZState {

	/* State ID */
	GGZStateID id;

	/* Current room on game server */
	int room;

	/* Room to which we are transitioning */
	int trans_room;

	/* Current table we're at */
	int table;

	/* Table to which we are transitioning */
	int trans_table;

	/* Server/user profile */
	GGZProfile profile;
};


/* Global state variable */
extern struct _GGZState _ggzcore_state;

void _ggzcore_state_init(void);

unsigned char _ggzcore_state_event_isvalid(GGZEventID id);

void _ggzcore_state_set(GGZStateID id);


#endif /* __STATE_H__ */

