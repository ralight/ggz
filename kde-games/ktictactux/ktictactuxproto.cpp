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
#include <kggzpacket.h>

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

// System includes
#include <stdio.h>

// Empty constructor
KTicTacTuxProto::KTicTacTuxProto()
{
	stats_record = false;
	stats_wins = -1;
	stats_losses = -1;
	stats_ties = -1;
	stats_forfeits = -1;

	mod = NULL;
	packet = NULL;
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
	delete packet;
	delete mod;
}

int KTicTacTuxProto::num()
{
	if(!mod) return -1;
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
	int op;

	*packet->inputstream() >> op;

	return op;
}

// Ask whether move was ok
int KTicTacTuxProto::getMoveStatus()
{
	Q_INT8 status;

	*packet->inputstream() >> status;

	if(status == 0) board[move % 3][move / 3] = player;

	return 1;
}

// Get opponent's move
int KTicTacTuxProto::getOpponentMove()
{
	int move;
	int nummove;

	*packet->inputstream() >> nummove;
	*packet->inputstream() >> move;

	if(num() < 0)
	{
		if(nummove == 0) board[move % 3][move / 3] = opponent;
		else board[move % 3][move / 3] = player;
	}
	else board[move % 3][move / 3] = opponent;

	return 1;
}

// Oooops... volunteers :-)
int KTicTacTuxProto::getSync()
{
	Q_INT8 space;

	*packet->inputstream() >> turn;

	for(int i = 0; i < 9; i++)
	{
		*packet->inputstream() >> space;

		if(space == 0) board[i % 3][i / 3] = opponent;
		else if(space == 1) board[i % 3][i / 3] = player;
		else board[i % 3][i / 3] = none;
	}

	return 1;
}

// Read the winner over the network
int KTicTacTuxProto::getGameOver()
{
	*packet->inputstream() >> winner;

	return winner;
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
	*packet->outputstream() << 0;
	packet->flush();

	return 1;
}

// Send the own move, to be approved
int KTicTacTuxProto::sendMyMove()
{
	*packet->outputstream() << sndmove;
	*packet->outputstream() << move;
	packet->flush();

	return 1;
}

// Synchronize game
void KTicTacTuxProto::sendSync()
{
	*packet->outputstream() << reqsync;
	packet->flush();
}

