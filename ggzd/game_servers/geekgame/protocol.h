// Geekgame - a game which only real geeks understand
// Copyright (C) 2002, 2003 Josef Spillner, josef@ggzgamingzone.org
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

#ifndef GEEKGAME_PROTOCOL_H
#define GEEKGAME_PROTOCOL_H

#define GEEKGAME_PROTOCOL 1

enum NetworkOpcodes
{
	op_server_greeting   = 10,
	op_server_newplayer  = 11,
	op_server_gamestart  = 12,
	op_server_gamestop   = 13,
	op_server_gameend    = 14,
	op_server_moveresult = 15,
	op_server_moderesult = 16,
	op_server_numplayers = 20,

	op_client_presentation = 30,
	op_client_ruleset      = 31,
	op_client_move         = 32,

	op_mode_reserved = 40,
	op_mode_easy     = 41,
	op_mode_matrix   = 42,
	op_mode_havoc    = 43,
	op_mode_haxor    = 44
};

#endif

