//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxproto.h"
#include "ktictactux.h"

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

// System includes
#include <stdio.h>

// Static members
KTicTacTuxProto *KTicTacTuxProto::self;

// Empty constructor
KTicTacTuxProto::KTicTacTuxProto(KTicTacTux *game)
{
	names[0][0] = 0;
	names[1][0] = 0;
	stats[0] = 0;
	stats[1] = 0;
	num = -1;

	mod = NULL;
	fdcontrol = -1;
	fd = -1;

	gameobject = game;
	self = this;
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
}

// Connect to the local game socket
void KTicTacTuxProto::connect()
{
	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_server);
	ggzmod_connect(mod);
	fdcontrol = ggzmod_get_fd(mod);
}

// Initialize the board
void KTicTacTuxProto::init()
{
	for(int i = 0; i < 9; i++)
		board[i % 3][i / 3] = none;
	state = stateinit;
	turn = none;
}

// Get opcode
int KTicTacTuxProto::getOp()
{
	int op;

	ggz_read_int(fd, &op);
	return op;
}

// Get one's own seat number
int KTicTacTuxProto::getSeat()
{
	return ggz_read_int(fd, &num);
}

// Receive the player names
int KTicTacTuxProto::getPlayers()
{
	int ret = 0;

	for(int i = 0; i < 2; i++)
	{
		ret |= ggz_read_int(fd, &seats[i]);
		if((seats[i] == GGZ_SEAT_PLAYER) || (seats[i] == GGZ_SEAT_BOT))
			ret |= ggz_read_string(fd, (char*)&names[i], 17);
	}

	return ret;
}

// Ask whether move was ok
int KTicTacTuxProto::getMoveStatus()
{
	char status;
	int ret;

	ret = ggz_read_char(fd, &status);

	if(status == 0) board[move % 3][move / 3] = player;

	return ret;
}

// Get opponent's move
int KTicTacTuxProto::getOpponentMove()
{
	int move;
	int nummove;
	int ret = 0;

	ret |= ggz_read_int(fd, &nummove);
	ret |= ggz_read_int(fd, &move);

	if(num < 0)
	{
		if(nummove == 0) board[move % 3][move / 3] = opponent;
		else board[move % 3][move / 3] = player;
	}
	else board[move % 3][move / 3] = opponent;

	return ret;
}

// Oooops... volunteers :-)
int KTicTacTuxProto::getSync()
{
	char space;
	int ret = 0;

	ret |= ggz_read_char(fd, &turn);
	for(int i = 0; i < 9; i++)
	{
		ret |= ggz_read_char(fd, &space);
		if(space == 0) board[i % 3][i / 3] = opponent;
		else if(space == 1) board[i % 3][i / 3] = player;
		else board[i % 3][i / 3] = none;
	}

	return ret;
}

// Read the winner over the network
int KTicTacTuxProto::getGameOver()
{
	return ggz_read_char(fd, &winner);
}

// Read statistics
void KTicTacTuxProto::getStatistics()
{
	ggz_read_int(fd, &stats[0]);
	ggz_read_int(fd, &stats[1]);
}

// Send the options
int KTicTacTuxProto::sendOptions()
{
	return ggz_write_int(fd, 0);
}

// Send the own move, to be approved
int KTicTacTuxProto::sendMyMove()
{
	int ret = 0;

	ret |= ggz_write_int(fd, sndmove);
	ret |= ggz_write_int(fd, move);

	return ret;
}

// Synchronize game
void KTicTacTuxProto::sendSync()
{
	ggz_write_int(fd, reqsync);
}

// Fetch statistics
void KTicTacTuxProto::sendStatistics()
{
	ggz_write_int(fd, reqstats);
}

// Callbacks
void KTicTacTuxProto::handle_server(GGZMod *mod, GGZModEvent e, void *data)
{
	self->fd = *(int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	self->gameobject->network();
}

void KTicTacTuxProto::dispatch()
{
	ggzmod_dispatch(mod);
}

void KTicTacTuxProto::shutdown()
{
	if(mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}
}

