//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxproto.h"

// GGZ includes
#include <easysock.h>

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
}

// Even more empty destructor
KTicTacTuxProto::~KTicTacTuxProto()
{
}

// Connect to the local game socket
void KTicTacTuxProto::connect()
{
	/*char fd_name[64];
	struct sockaddr_un addr;

	sprintf(fd_name, "/tmp/TicTacToe.%d", getpid());
	if((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) exit(-1);
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
	if(::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) exit(-1);*/
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

	es_read_int(fd, &op);
	return op;
}

// Get one's own seat number
int KTicTacTuxProto::getSeat()
{
	es_read_int(fd, &num);
}

// Receive the player names
int KTicTacTuxProto::getPlayers()
{
	for(int i = 0; i < 2; i++)
	{
		es_read_int(fd, &seats[i]);
		if(seats[i] != -1) es_read_string(fd, (char*)&names[i], 17);
	}
}

// Ask whether move was ok
int KTicTacTuxProto::getMoveStatus()
{
	char status;

	es_read_char(fd, &status);

	if(status == 0) board[move % 3][move / 3] = player;
}

// Get opponent's move
int KTicTacTuxProto::getOpponentMove()
{
	int move;

	es_read_int(fd, &move);
	board[move % 3][move / 3] = opponent;
}

// Oooops... volunteers :-)
int KTicTacTuxProto::getSync()
{
	char space;

	es_read_char(fd, &turn);
	for(int i = 0; i < 9; i++)
	{
		es_read_char(fd, &space);
		if(space >= 0) board[i % 3][i / 3] = space + 1;
	}
}

// Read the winner over the network
int KTicTacTuxProto::getGameOver()
{
	es_read_char(fd, &winner);
}

// Send the options
int KTicTacTuxProto::sendOptions()
{
	es_write_int(fd, 0);
}

// Send the own move, to be approved
int KTicTacTuxProto::sendMyMove()
{
	es_write_int(fd, sndmove);
	es_write_int(fd, move);
}

// Synchronize game
void KTicTacTuxProto::sendSync()
{
	es_write_int(fd, reqsync);
}

