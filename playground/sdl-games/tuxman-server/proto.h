// Tuxman Server
// Copyright (C) 2003 Josef Spillner <josef@ggzgamingzone.org>
// Tuxman Copyright (C) 2003 Robert Strobl <badwolf@acoderz.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef TUXMAN_PROTO_H
#define TUXMAN_PROTO_H

enum Opcodes
{
	// server to client
	map_list = 100,
	map_name = 101,
	map_monster = 102,
	map_event = 103,

	// client to server
	map_selected = 120,
	map_move = 121
};

enum Events
{
	event_error = 1,
	event_start = 2
};

enum Moves
{
	move_up = 1,
	move_left = 2,
	move_down = 3,
	move_right = 4
};

#endif

