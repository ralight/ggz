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

#include "ggzcore.h"

/* Transition IDs for all of the transitions the GGZ state machine
   makes.  These server as inputs for the state machine */
typedef enum {
	GGZ_TRANS_CONN_TRY,
	GGZ_TRANS_CONN_OK,
	GGZ_TRANS_CONN_FAIL,
	GGZ_TRANS_LOGIN_TRY,
	GGZ_TRANS_LOGIN_OK,
	GGZ_TRANS_LOGIN_FAIL,
	GGZ_TRANS_ENTER_TRY,
	GGZ_TRANS_ENTER_OK,
	GGZ_TRANS_ENTER_FAIL,
	GGZ_TRANS_LOGOUT_TRY,
	GGZ_TRANS_LOGOUT_OK,
	GGZ_TRANS_NET_ERROR,
	GGZ_TRANS_PROTO_ERROR
} GGZTransID;

void _ggzcore_state_transition(GGZTransID trans, GGZStateID *cur);

#endif /* __STATE_H__ */

