///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_proto.h"
#include "kdots.h"

// GGZ includes
#include <ggz.h>
#include <ggz_common.h>

KDotsProto *KDotsProto::self;

KDotsProto::KDotsProto(KDots *game)
{
	state = stateinit;
	mod = NULL;
	gameobject = game;
	self = this;

	fd = -1;
	fdcontrol = -1;
}

KDotsProto::~KDotsProto()
{
}

void KDotsProto::sync()
{
	ggz_write_int(fd, reqsync);
}

void KDotsProto::init()
{
	state = stateinit;
	ggz_write_int(fd, reqnewgame);
}

int KDotsProto::getOpcode()
{
	int op;

	ggz_read_int(fd, &op);
	return op;
}

int KDotsProto::getStatus()
{
	char status;

	ggz_read_char(fd, &status);
	return status;
}

int KDotsProto::getSyncMove()
{
	char move;

	ggz_read_char(fd, &move);
	return move;
}

int KDotsProto::getSyncScore()
{
	int score;

	ggz_read_int(fd, &score);
	return score;
}

void KDotsProto::getSeat()
{
	ggz_read_int(fd, &num);
}

void KDotsProto::getPlayers()
{
	int seattmp;

	for(int i = 0; i < 2; i++)
	{
		ggz_read_int(fd, &seattmp);
		if((seattmp == GGZ_SEAT_PLAYER) || (seattmp == GGZ_SEAT_BOT))
			ggz_read_string(fd, (char*)&players[i], 17);
	}
}

void KDotsProto::connect()
{
	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_server);
	ggzmod_connect(mod);
	fdcontrol = ggzmod_get_fd(mod);
}

void KDotsProto::disconnect()
{
	if(mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}
}

void KDotsProto::sendMove(int x, int y, int direction)
{
	ggz_write_int(fd, direction);
	ggz_write_char(fd, x);
	ggz_write_char(fd, y);

	m_lastx = x;
	m_lasty = y;
}

void KDotsProto::sendOptions(int wantwidth, int wantheight)
{
	width = wantwidth;
	height = wantheight;
	ggz_write_int(fd, sndoptions);
	ggz_write_char(fd, width);
	ggz_write_char(fd, height);
}

void KDotsProto::getOptions()
{
	ggz_read_char(fd, &width);
	ggz_read_char(fd, &height);
}

int KDotsProto::getMove()
{
	char status;
	char s, x, y;

	ggz_read_char(fd, &status);
	ggz_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		ggz_read_char(fd, &x);
		ggz_read_char(fd, &y);
	}

	return (int)status;
}

void KDotsProto::getOppMove(int direction)
{
	char s, x, y, nx, ny;

	if(state == stateopponent) state = statewait;

	ggz_read_char(fd, &nx);
	ggz_read_char(fd, &ny);
	ggz_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		ggz_read_char(fd, &x);
		ggz_read_char(fd, &y);
	}

	movex = nx;
	movey = ny;
	if(turn == -1) turn = !num;
}

void KDotsProto::dispatch()
{
	ggzmod_dispatch(mod);
}

void KDotsProto::handle_server(GGZMod *mod, GGZModEvent e,
			       const void *data)
{
	self->fd = *(const int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	self->gameobject->input();
}

