//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxproto.h"

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

// System includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Empty constructor
KTicTacTuxProto::KTicTacTuxProto()
{
	names[0][0] = 0;
	names[1][0] = 0;
	stats[0] = 0;
	stats[1] = 0;
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
}

// Connect to the local game socket
void KTicTacTuxProto::connect()
{
	fd = 3;
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
	ggz_read_int(fd, &num);
}

// Receive the player names
int KTicTacTuxProto::getPlayers()
{
	for(int i = 0; i < 2; i++)
	{
		ggz_read_int(fd, &seats[i]);
		if((seats[i] == GGZ_SEAT_PLAYER) || (seats[i] == GGZ_SEAT_BOT))
			ggz_read_string(fd, (char*)&names[i], 17);
	}
}

// Ask whether move was ok
int KTicTacTuxProto::getMoveStatus()
{
	char status;

	ggz_read_char(fd, &status);

	if(status == 0) board[move % 3][move / 3] = player;
}

// Get opponent's move
int KTicTacTuxProto::getOpponentMove()
{
	int move;

	ggz_read_int(fd, &move);
	board[move % 3][move / 3] = opponent;
}

// Oooops... volunteers :-)
int KTicTacTuxProto::getSync()
{
	char space;

	ggz_read_char(fd, &turn);
	for(int i = 0; i < 9; i++)
	{
		ggz_read_char(fd, &space);
		if(space >= 0) board[i % 3][i / 3] = space + 1;
	}
}

// Read the winner over the network
int KTicTacTuxProto::getGameOver()
{
	ggz_read_char(fd, &winner);
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
	ggz_write_int(fd, 0);
}

// Send the own move, to be approved
int KTicTacTuxProto::sendMyMove()
{
	ggz_write_int(fd, sndmove);
	ggz_write_int(fd, move);
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

