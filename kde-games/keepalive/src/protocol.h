// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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

#ifndef KEEPALIVE_PROTOCOL_H
#define KEEPALIVE_PROTOCOL_H

// Opcodes for the Keepalive protocol
enum KeepaliveOpcodes
{
	op_map,
	op_init,
	op_player,
	op_name,
	op_moved,
	op_login,
	op_move
};

// op_map: map size and contents
// width, height
// op_init: Initial position
// x, y
// op_player: List of 1 or more new players on the map
// count, {$name, x, y}
// op_name: The player's character name
// $name
// op_moved: a player has moved
// $name, x, y
//
// op_login: Attempt to get ones own character back
// $username, $password
// op_move: Move of a player
// x, y

#endif

