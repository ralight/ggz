///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

// KCC includes
#include "kccproto.h"
#include "kcc.h"

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

// System includes
#include <stdio.h>

// Static members
KCCProto *KCCProto::self;

// Empty constructor
KCCProto::KCCProto(KCC *game)
{
	names[0][0] = 0;
	names[1][0] = 0;
	stats[0] = 0;
	stats[1] = 0;
	num = -1;
	max = 0;
	status = errother;
	state = statenone;

	mod = NULL;
	fdcontrol = -1;
	fd = -1;

	gameobject = game;
	self = this;
}

// Even more empty destructor
KCCProto::~KCCProto()
{
}

// Connect to the local game socket
void KCCProto::connect()
{
	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_server);
	ggzmod_connect(mod);
	fdcontrol = ggzmod_get_fd(mod);
}

// Initialize the board
void KCCProto::init()
{
	char tboard[15][17] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0  },
		{ 0, 0, 0, 0, 6, 6, 6, 6, 0, 7, 7, 7, 7, 0, 0, 0, 0 },
		{0, 0, 0, 0, 6, 6, 6, 1, 1, 1, 7, 7, 7, 0, 0, 0, 0  },
		{ 0, 0, 0, 0, 6, 1, 1, 1, 1, 1, 1, 1, 7, 0, 0, 0, 0 },
//		{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0  },
		{ 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0 },
		{0, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 0  },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3 },
		{0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0  },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
//		{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0  },
		{ 0, 0, 0, 0, 4, 4, 1, 1, 1, 1, 1, 5, 5, 0, 0, 0, 0 },
		{0, 0, 0, 0, 4, 4, 4, 4, 1, 5, 5, 5, 5, 0, 0, 0, 0  },
		{ 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0 },
		{0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0  },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	for(int i = 0; i < 15 * 17; i++)
		board[i / 17][i % 17] = tboard[i / 17][i % 17];

	if(max)
	{
		for(int i = 0; i < 15 * 17; i++)
			if(board[i / 17][i % 17] > max + 1)
				board[i / 17][i % 17] = 1;
	}

	state = stateinit;
	turn = none;
}

// Get opcode
int KCCProto::getOp()
{
	int op;

	ggz_read_int(fd, &op);
	return op;
}

// Get one's own seat number
int KCCProto::getSeat()
{
	ggz_read_int(fd, &max);
	ggz_read_int(fd, &num);
	init();
	return num;
}

// Receive the player names
int KCCProto::getPlayers()
{
	int ret = 0;
	int count;

	ggz_read_int(fd, &count);
	for(int i = 0; i < count; i++)
	{
		ret |= ggz_read_int(fd, &seats[i]);
		if((seats[i] == GGZ_SEAT_PLAYER) || (seats[i] == GGZ_SEAT_BOT))
			ret |= ggz_read_string(fd, (char*)&names[i], 17);
	}

	return ret;
}

// Ask whether move was ok
int KCCProto::getMoveStatus()
{
	int ret;

	ret = ggz_read_char(fd, &status);

	return ret;
}

// Get opponent's move
int KCCProto::getOpponentMove()
{
	int seat;
	char x1, x2, y1, y2;
	int ret = 0;
	int tmp;

	ret |= ggz_read_int(fd, &seat);
	ret |= ggz_read_char(fd, &y1);
	ret |= ggz_read_char(fd, &x1);
	ret |= ggz_read_char(fd, &y2);
	ret |= ggz_read_char(fd, &x2);

	x1 = ((x1 + 2) - (y1 % 2)) / 2;
	x2 = ((x2 + 2) - (y2 % 2)) / 2;
	y1 = 16 - y1;
	y2 = 16 - y2;

	m_ox1 = x1;
	m_ox2 = x2;
	m_oy1 = y1;
	m_oy2 = y2;

	//tmp = board[(int)x1][(int)y1];
	//board[(int)x2][(int)y2] = tmp;
	//board[(int)x1][(int)y1] = 1;

	return ret;
}

// Oooops... volunteers :-)
int KCCProto::getSync()
{
	//char space;
	int ret = 0;

	/*ret |= ggz_read_char(fd, &turn);
	for(int i = 0; i < 9; i++)
	{
		ret |= ggz_read_char(fd, &space);
		if(space == 0) board[i % 3][i / 3] = opponent;
		else if(space == 1) board[i % 3][i / 3] = player;
		else board[i % 3][i / 3] = none;
	}*/

	return ret;
}

// Read the winner over the network
int KCCProto::getGameOver()
{
	return ggz_read_char(fd, &winner);
}

// Read statistics
void KCCProto::getStatistics()
{
	ggz_read_int(fd, &stats[0]);
	ggz_read_int(fd, &stats[1]);
}

// Send the own move, to be approved
int KCCProto::sendMyMove(int x, int y, int x2, int y2)
{
	int ret = 0;

	y = 16 - y;
	y2 = 16 - y2;
	x = (x * 2) + (y % 2) - 2;
	x2 = (x2 * 2) + (y2 % 2) - 2;

	ret |= ggz_write_int(fd, cc_snd_move);
	ret |= ggz_write_char(fd, y);
	ret |= ggz_write_char(fd, x);
	ret |= ggz_write_char(fd, y2);
	ret |= ggz_write_char(fd, x2);

	return ret;
}

// Synchronize game
void KCCProto::sendSync()
{
//	ggz_write_int(fd, reqsync);
}

// Fetch statistics
void KCCProto::sendStatistics()
{
//	ggz_write_int(fd, reqstats);
}

// Callbacks
void KCCProto::handle_server(GGZMod *mod, GGZModEvent e, void *data)
{
	self->fd = *(int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	self->gameobject->network();
}

void KCCProto::dispatch()
{
	ggzmod_dispatch(mod);
}

void KCCProto::shutdown()
{
	if(mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}
}

