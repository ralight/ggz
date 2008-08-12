/////////////////////////////////////////////////////////////////////
//
// KTicTacTux: Tic-Tac-Toe game for KDE 4
// http://www.ggzgamingzone.org/gameclients/ktictactux/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxproto.h"
#include "proto.h"

// GGZ-KDE-Games includes
#include <kggzmod/module.h>
#include <kggzmod/player.h>
#include <kggzmod/statistics.h>

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
	proto = NULL;
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
	delete proto;
	delete mod;
}

int KTicTacTuxProto::num()
{
	if(!mod)
		return 0;
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

// Ask whether move was ok
void KTicTacTuxProto::handleMoveStatus(const rspmove& xmove)
{
	if(xmove.status == 0)
		board[move % 3][move / 3] = player;
}

// Get opponent's move
void KTicTacTuxProto::handleOpponentMove(const msgmove& move)
{
	if(num() < 0)
	{
		if(move.player == 0)
			board[move.move % 3][move.move / 3] = opponent;
		else
			board[move.move % 3][move.move / 3] = player;
	}
	else
	{
		board[move.move % 3][move.move / 3] = opponent;
	}
}

// Oooops... volunteers :-)
void KTicTacTuxProto::handleSync(const sndsync& sync)
{
	qint8 space;

	for(int i = 0; i < 9; i++)
	{
		qint8 space = sync.space[i];

		if(space == 0)
			board[i % 3][i / 3] = opponent;
		else if(space == 1)
			board[i % 3][i / 3] = player;
		else
			board[i % 3][i / 3] = none;
	}
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
/*int KTicTacTuxProto::sendOptions()
{
	*packet->outputstream() << 0;
	packet->flush();

	return 1;
}*/

// Send the own move, to be approved
void KTicTacTuxProto::sendMyMove()
{
	sndmove xmove;
	xmove.move_c = move;
	proto->ggzcomm_sndmove(xmove);
}

// Synchronize game
void KTicTacTuxProto::sendSync()
{
	proto->ggzcomm_reqsync(reqsync());
}

