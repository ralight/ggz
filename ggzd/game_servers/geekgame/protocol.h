#ifndef GEEKGAME_PROTOCOL_H
#define GEEKGAME_PROTOCOL_H

enum NetworkOpcodes
{
	op_server_greeting,
	op_server_newplayer,
	op_server_gamestart,
	op_server_gamestop,
	op_server_gameend,

	op_client_presentation
};

#endif

