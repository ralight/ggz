//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxproto.h"

// GGZ-KDE-Games includes
#include <kggzmod/module.h>
#include <kggzmod/player.h>
#include <kggzmod/statistics.h>

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

// System includes
#include <stdio.h>

// Empty constructor
KTicTacTuxProto::KTicTacTuxProto()
{
	seatnum = -1;
	names[0][0] = 0;
	names[1][0] = 0;

	stats_record = false;
	stats_wins = -1;
	stats_losses = -1;
	stats_ties = -1;
	stats_forfeits = -1;

	mod = NULL;
	fd = -1;
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
	delete mod;
}

int KTicTacTuxProto::num()
{
	if(!mod) return 0;
	return mod->self()->seat();
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
	int op, ret;

	ret = ggz_read_int(fd, &op);
	if(ret == -1) return -1;

	return op;
}

// Get one's own seat number
int KTicTacTuxProto::getSeat()
{
	return ggz_read_int(fd, &seatnum);
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

	if(num() < 0)
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
	KGGZMod::Statistics *stats = mod->self()->stats();
	if((stats) && (stats->hasRecord()))
	{
		stats_record = true;
		stats_wins = stats->wins();
		stats_losses = stats->losses();
		stats_ties = stats->ties();
		stats_forfeits = stats->forfeits();
	}
	else
	{
		stats_record = false;
	}
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

