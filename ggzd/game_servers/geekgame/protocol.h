#ifndef GEEKGAME_PROTOCOL_H
#define GEEKGAME_PROTOCOL_H

enum NetworkOpcodes
{
	op_server_greeting  = 100,
	op_server_newplayer = 101,
	op_server_gamestart = 102,
	op_server_gamestop  = 103,
	op_server_gameend   = 104,

	op_client_presentation = 200,
	op_client_ruleset = 201,
	op_client_move = 202,

	op_mode_reserved = 1000,
	op_mode_easy = 1001,
	op_mode_matrix = 1002,
	op_mode_havoc = 1003,
	op_mode_haxor = 1004
};

#endif

