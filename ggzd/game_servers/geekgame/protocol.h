#ifndef GEEKGAME_PROTOCOL_H
#define GEEKGAME_PROTOCOL_H

enum NetworkOpcodes
{
	op_server_greeting  = 100,
	op_server_newplayer = 101,
	op_server_gamestart = 102,
	op_server_gamestop  = 103,
	op_server_gameend   = 104,

	op_client_presentation = 200
};

#endif

