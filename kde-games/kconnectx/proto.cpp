//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KConnectX includes
#include "proto.h"

// GGZ-KDE-Games includes
#include <kggzmod/player.h>

// KDE includes
#include <kdebug.h>

// Qt includes
#include <qdatastream.h>
#include <qsocketdevice.h>

// Empty constructor
Proto::Proto()
{
	m_kggzmod = new KGGZMod::Module("kconnectx");

	m_dev = 0;
	m_net = 0;

	connect(m_kggzmod, SIGNAL(signalEvent(const KGGZMod::Event&)), SLOT(slotEvent(const KGGZMod::Event&)));
	connect(m_kggzmod, SIGNAL(signalError()), SLOT(slotError()));
	connect(m_kggzmod, SIGNAL(signalNetwork(int)), SLOT(slotNetwork(int)));
}

// Even more empty destructor
Proto::~Proto()
{
	delete m_net;
	delete m_dev;
}

void Proto::slotEvent(const KGGZMod::Event& event)
{
	kdDebug() << "PROTO: Event! type=" << event.type() << endl;

	if(event.type() == KGGZMod::Event::self)
	{
		kdDebug() << "info about myself!" << endl;
		KGGZMod::SelfEvent se = (KGGZMod::SelfEvent)event;
		kdDebug() << "Player: " << se.self() << endl;
	}
	else if(event.type() == KGGZMod::Event::launch)
	{
		kdDebug() << "game server was launched!" << endl;
//m_kggzmod->sendRequest(KGGZMod::StateRequest(KGGZMod::Module::playing));
	}
	else if(event.type() == KGGZMod::Event::stats)
	{
		kdDebug() << "statistics!" << endl;
	}
	else if(event.type() == KGGZMod::Event::server)
	{
		kdDebug() << "game server connection!" << endl;
		KGGZMod::ServerEvent se = (KGGZMod::ServerEvent)event;
		kdDebug() << "File descriptor: " << se.fd() << endl;
	}
	else if(event.type() == KGGZMod::Event::seat)
	{
		kdDebug() << "seat change!" << endl;
	}
}

void Proto::slotError()
{
	kdDebug() << "PROTO: Error!" << endl;
}

void Proto::slotNetwork(int fd)
{
	int opcode;
	char *tmp;
	QByteArray buffer;
	Q_LONG available, oldavailable;
	QDataStream net(buffer, IO_ReadOnly);

	kdDebug() << "PROTO: Network! fd=" << fd << endl;

	if(!m_dev)
	{
		m_dev = new QSocketDevice(fd, QSocketDevice::Stream);
		m_net = new QDataStream(m_dev);
	}

	available = 0;

	while((!available) || (!net.atEnd()))
	{

	while(true)
	{
		if(m_dev->bytesAvailable() > (Q_LONG)10000)
		{
			kdDebug() << "PROTO: network overflow" << endl;
			m_kggzmod->disconnect();
			return;
		}

		kdDebug() << "PROTO: bytesavailable=" << m_dev->bytesAvailable() << endl;
		buffer.resize(buffer.size() + m_dev->bytesAvailable());
		oldavailable = m_dev->readBlock(buffer.data() + available, m_dev->bytesAvailable());
		if(oldavailable == -1)
		{
			kdDebug() << "PROTO: network error" << endl;
			m_kggzmod->disconnect();
			return;
		}
		available += oldavailable;
		m_dev->waitForMore(100);
		if(m_dev->bytesAvailable() == 0) break;
	}

	buffer.truncate(available);
	kdDebug() << "BUFFER: " << buffer << endl;

	net >> opcode;
	kdDebug() << "PROTO: opcode=" << opcode << endl;

	if(opcode == msgseat)
	{
		net >> m_data.myseat;
		kdDebug() << "PROTO: +seat" << endl;
		kdDebug() << " seat: " << m_data.myseat << endl;

		// ignore since kggzmod does this for us
	}
	else if(opcode == msgplayers)
	{
		net >> m_data.type1;
		if(m_data.type1 != KGGZMod::Player::open)
		{
			net >> tmp;
			m_data.name1 = QString(tmp);
			free(tmp);
		}
		net >> m_data.type2;
		if(m_data.type2 != KGGZMod::Player::open)
		{
			net >> tmp;
			m_data.name2 = QString(tmp);
			free(tmp);
		}
		kdDebug() << "PROTO: +players" << endl;
		kdDebug() << " type1: " << m_data.type1 << endl;
		kdDebug() << " name1: " << m_data.name1 << endl;
		kdDebug() << " type2: " << m_data.type2 << endl;
		kdDebug() << " name2: " << m_data.name2 << endl;

		// ignore since kggzmod does this for us
	}
	else if(opcode == msgmove)
	{
		net >> m_data.move;
		kdDebug() << "PROTO: +move" << endl;
		kdDebug() << " move: " << m_data.move << endl;

		emit signalMove(m_data.move);
	}
	else if(opcode == msggameover)
	{
		net >> m_data.winner;
		kdDebug() << "PROTO: +gameover" << endl;
		kdDebug() << " winner: " << m_data.winner << endl;

		emit signalOver(m_data.winner);
	}
	else if(opcode == reqmove)
	{
		kdDebug() << "PROTO: +reqmove" << endl;

		emit signalMoveRequest();
	}
	else if(opcode == rspmove)
	{
		net >> m_data.movestatus;
		net >> m_data.move;
		kdDebug() << "PROTO: +rspmove" << endl;
		kdDebug() << " status: " << m_data.movestatus << endl;
		kdDebug() << " move: " << m_data.move << endl;

		emit signalMoveResponse(m_data.movestatus, m_data.move);
	}
	else if(opcode == sndsync)
	{
		kdDebug() << "PROTO: +sync" << endl;

		// ignore
	}
	else if(opcode == msgoptions)
	{
		net >> m_data.boardheight;
		net >> m_data.boardwidth;
		net >> m_data.connectlength;
		kdDebug() << "PROTO: +msgoptions" << endl;
		kdDebug() << " board width: " << m_data.boardwidth << endl;
		kdDebug() << " board height: " << m_data.boardheight << endl;
		kdDebug() << " connect length: " << m_data.connectlength << endl;

		emit signalOptions(m_data.boardwidth, m_data.boardheight, m_data.connectlength);
	}
	else if(opcode == reqoptions)
	{
		net >> m_data.minboardwidth;
		net >> m_data.maxboardwidth;
		net >> m_data.minboardheight;
		net >> m_data.maxboardheight;
		net >> m_data.minconnectlength;
		net >> m_data.maxconnectlength;
		kdDebug() << "PROTO: +reqoptions" << endl;
		kdDebug() << " min board width: " << m_data.minboardwidth << endl;
		kdDebug() << " max board width: " << m_data.maxboardwidth << endl;
		kdDebug() << " min board height: " << m_data.minboardheight << endl;
		kdDebug() << " max board height: " << m_data.maxboardheight << endl;
		kdDebug() << " min connect length: " << m_data.minconnectlength << endl;
		kdDebug() << " max connect length: " << m_data.maxconnectlength << endl;

		emit signalOptionsRequest(m_data.minboardwidth, m_data.maxboardwidth,
			m_data.minboardheight, m_data.maxboardheight,
			m_data.minconnectlength, m_data.maxconnectlength);
	}
	else if(opcode == msgchat)
	{
		kdDebug() << "PROTO: +msgchat" << endl;

		// ignore
	}
	else if(opcode == rspchat)
	{
		kdDebug() << "PROTO: +rspchat" << endl;

		// ignore
	}
	else
	{
		kdDebug() << "PROTO: unknown opcode, treat as error!" << endl;
		m_kggzmod->disconnect();
		return;
	}

	}
}

KGGZMod::Module *Proto::mod() const
{
	return m_kggzmod;
}

void Proto::sendMove(int move)
{
	if(!m_dev)
	{
		slotError();
		return;
	}

	*m_net << sndmove;
	*m_net << move;
}

void Proto::sendOptions(char boardwidth, char boardheight, char connectlength)
{
	if(!m_dev)
	{
		slotError();
		return;
	}

	*m_net << sndoptions;
	*m_net << (Q_INT8)boardheight;
	*m_net << (Q_INT8)boardwidth;
	*m_net << (Q_INT8)connectlength;
}

void Proto::sendRestart()
{
	if(!m_dev)
	{
		slotError();
		return;
	}

	*m_net << reqnewgame;
}

