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

struct _GGZClientState {

	/* State ID */
	GGZStateID id;

	/* Current room on game server */
	int room;

	/* Room to which we are transitioning */
	int trans_room;

	/* Verbosity level for rooms */
	char room_verbose;
	
	/* Current table we're at */
	int table;

	/* Table to which we are transitioning */
	int trans_table;
	
	/* Server/user profile */
	GGZProfile profile;
};


/* Global state variable */
extern struct _GGZClientState _ggzcore_state;

void _ggzcore_state_init(void);
void _ggzcore_state_destroy(void);

int _ggzcore_state_event_is_valid(GGZEventID id);
void _ggzcore_state_transition(GGZEventID id);


#endif /* __STATE_H__ */

