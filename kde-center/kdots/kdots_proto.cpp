/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_proto.h"
#include "kdots.h"

// GGZ includes
//#include <ggz.h>
//#include <ggz_common.h>

//KDotsProto *KDotsProto::self;

#include <kggzmod/module.h>
#include <kggzmod/player.h>
#include <kggzmod/statistics.h>
#include <kggznet/kggzpacket.h>

KDotsProto::KDotsProto(KDots *game)
{
	state = stateinit;
	//mod = NULL;
	//gameobject = game;
	//self = this;

	//fd = -1;
	//fdcontrol = -1;

	Q_UNUSED(game);

	m_packet = NULL;
}

KDotsProto::~KDotsProto()
{
	delete m_packet;
}

void KDotsProto::sync()
{
	*m_packet->outputstream() << reqsync;
	//ggz_write_int(fd, reqsync);
}

void KDotsProto::init()
{
	state = stateinit;
	*m_packet->outputstream() << reqnewgame;
	//ggz_write_int(fd, reqnewgame);
}

int KDotsProto::getOpcode()
{
	qint32 op;

	*m_packet->inputstream() >> op;
	//ggz_read_int(fd, &op);
	return op;
}

int KDotsProto::getStatus()
{
	qint8 status;

	*m_packet->inputstream() >> status;
	//ggz_read_char(fd, &status);
	return status;
}

int KDotsProto::getSyncMove()
{
	qint8 move;

	*m_packet->inputstream() >> move;
	//ggz_read_char(fd, &move);
	return move;
}

int KDotsProto::getSyncScore()
{
	qint32 score;

	*m_packet->inputstream() >> score;
	//ggz_read_int(fd, &score);
	return score;
}

void KDotsProto::getSeat()
{
	*m_packet->inputstream() >> num;
	//ggz_read_int(fd, &num);
}

void KDotsProto::getPlayers()
{
	qint32 seattmp;
	QString player;

	for(int i = 0; i < 2; i++)
	{
		*m_packet->inputstream() >> seattmp;
		//ggz_read_int(fd, &seattmp);
		if((seattmp == KGGZMod::Player::player) || (seattmp == KGGZMod::Player::bot))
			*m_packet->inputstream() >> player;
			//ggz_read_string(fd, (char*)&players[i], 17);
	}
}

void KDotsProto::connect()
{
	KGGZMod::Module *module = new KGGZMod::Module("kdots");
	m_packet = new KGGZPacket();

	Q_UNUSED(module);

	// FIXME: add connections
	//connect(proto->packet, SIGNAL(signalPacket()), SLOT(slotPacket()));
	//connect(proto->mod, SIGNAL(signalError()), SLOT(slotError()));
	//connect(proto->mod, SIGNAL(signalNetwork(int)), proto->packet, SLOT(slotNetwork(int)));
	//connect(proto->mod, SIGNAL(signalEvent(const KGGZMod::Event&)), SLOT(slotEvent(const KGGZMod::Event&)));

	//mod = ggzmod_new(GGZMOD_GAME);
	//ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_server);
	//ggzmod_connect(mod);
	//fdcontrol = ggzmod_get_fd(mod);
}

void KDotsProto::disconnect()
{
	//if(mod)
	//{
	//	ggzmod_disconnect(mod);
	//	ggzmod_free(mod);
	//}

	delete m_packet;
	m_packet = NULL;
}

void KDotsProto::sendMove(int x, int y, int direction)
{
	*m_packet->outputstream() << direction;
	*m_packet->outputstream() << (qint32)x;
	*m_packet->outputstream() << (qint32)y;
	//ggz_write_int(fd, direction);
	//ggz_write_char(fd, x);
	//ggz_write_char(fd, y);

	m_lastx = x;
	m_lasty = y;
}

void KDotsProto::sendOptions(int wantwidth, int wantheight)
{
	width = wantwidth;
	height = wantheight;
	*m_packet->outputstream() << sndoptions;
	*m_packet->outputstream() << (qint32)width;
	*m_packet->outputstream() << (qint32)height;
	//ggz_write_int(fd, sndoptions);
	//ggz_write_char(fd, width);
	//ggz_write_char(fd, height);
}

void KDotsProto::getOptions()
{
	*m_packet->inputstream() >> width;
	*m_packet->inputstream() >> height;
	//ggz_read_char(fd, &width);
	//ggz_read_char(fd, &height);
}

int KDotsProto::getMove()
{
	qint8 status;
	qint8 s, x, y;

	*m_packet->inputstream() >> status;
	*m_packet->inputstream() >> s;
	//ggz_read_char(fd, &status);
	//ggz_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		*m_packet->inputstream() >> x;
		*m_packet->inputstream() >> y;
		//ggz_read_char(fd, &x);
		//ggz_read_char(fd, &y);
	}

	return (int)status;
}

void KDotsProto::getOppMove(int direction)
{
	qint8 s, x, y, nx, ny;

	Q_UNUSED(direction);

	if(state == stateopponent) state = statewait;

	*m_packet->inputstream() >> nx;
	*m_packet->inputstream() >> ny;
	*m_packet->inputstream() >> s;
	//ggz_read_char(fd, &nx);
	//ggz_read_char(fd, &ny);
	//ggz_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		*m_packet->inputstream() >> x;
		*m_packet->inputstream() >> y;
		//ggz_read_char(fd, &x);
		//ggz_read_char(fd, &y);
	}

	movex = nx;
	movey = ny;
	if(turn == -1) turn = !num;
}

/*
void KDotsProto::dispatch()
{
	ggzmod_dispatch(mod);
}

void KDotsProto::handle_server(GGZMod *mod, GGZModEvent e,
			       const void *data)
{
	Q_UNUSED(e);

	self->fd = *(const int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	self->gameobject->input();
}

GGZMod *KDotsProto::getMod()
{
	return mod;
}
*/
