#include "kggzmodule.h"

#include <kdebug.h>

#include <qsocketnotifier.h>

#include <unistd.h>
#include <stdlib.h>

using namespace KGGZ;

///////////////////////////////
// Message

Message::Message(int type)
{
	m_type = type;
}

int Message::type()
{
	return m_type;
}

///////////////////////////////
// Statistics

Statistics::Statistics()
{
	wins = -1;
	losses = -1;
	ties = -1;
	forfeits = -1;
	rating = -1;
	ranking = -1;
	highscore = -1;
}

///////////////////////////////
// Player

Player::Player(Type type)
{
	m_type = type;
	seat = -1;
}

Player::Player()
{
	m_type = open;
	// use none?
}

//Player& Player::operator=(const Player& p)
//{
//	Player *p2 = new Player(p.type());
//
//	//p2.x = 
//
//	return *p2;
//}

///////////////////////////////
// Module

Module::Module(QString name)
: QObject()
{
	m_fd = -1;

	m_dev = 0;
	m_net = 0;
	m_notifier = 0;

	Q_UNUSED(name);

	connect();
}

Module::~Module()
{
	disconnect();
}

void Module::sendRequest(Message message)
{
	if(!m_net)
	{
		kdDebug() << "[kggz::module] error: not connected" << endl;
	}

	int opcode = message.type();

	if(opcode == reqstate)
	{
		*m_net << reqstate;
		*m_net << message.data["state"].toInt();
	}
	if(opcode == reqstand)
	{
		*m_net << reqstand;
	}
	if(opcode == reqsit)
	{
		*m_net << reqsit;
	}
	if(opcode == reqboot)
	{
		*m_net << reqboot;
		*m_net << message.data["player"];
	}
	if(opcode == reqbot)
	{
		*m_net << reqbot;
	}
	if(opcode == reqopen)
	{
		*m_net << reqopen;
	}
	if(opcode == reqchat)
	{
		*m_net << reqchat;
		*m_net << message.data["message"];
	}
}

QValueList<Player> Module::players()
{
	return m_players;
}

Module::State Module::state()
{
	return m_state;
}

int Module::fd()
{
	return m_fd;
}

void Module::slotEvent()
{
	int opcode;
	char *_host, *_player, *_message;
	int _port;
	int _isspectator, _seat;
	int _seattype;
	int _hasrecord, _hasrating, _hasranking, _hashighscore;
	int _wins, _losses, _ties, _forfeits, _rating, _ranking, _highscore;
	QValueList<Player>::Iterator it;

	*m_net >> opcode;

	kdDebug() << "[kggz::module] info: got input " << opcode << endl;

	if((opcode < msglaunch) || (opcode > msgstats))
	{
		disconnect();
		emit signalError();
	}

	if(opcode == msglaunch)
	{
		Message m(launch);
		emit signalEvent(m);
	}
	if(opcode == msgserver)
	{
		Message m(server);
		*m_net >> _host;
		*m_net >> _port;
		*m_net >> _player;
		m.data["host"] = QString(_host);
		m.data["player"] = QString(_player);
		m.data["port"] = _port;
		free(_host);
		free(_player);
		emit signalEvent(m);
	}
	if(opcode == msgplayer)
	{
		Message m(self);
		*m_net >> _player;
		*m_net >> _isspectator;
		*m_net >> _seat;
		m.data["player"] = QString(_player);
		free(_player);

		insertPlayer((_isspectator ? Player::spectator : Player::player),
			m.data["player"], _seat);

		emit signalEvent(m);
	}
	if(opcode == msgseat)
	{
		Message m(seat);
		*m_net >> _seat;
		*m_net >> _seattype;
		*m_net >> _player;
		m.data["player"] = QString(_player);
		free(_player);

		insertPlayer((Player::Type)_seattype, m.data["player"], _seat);

		if(_seat >= m_playerseats) m_playerseats = _seat + 1;

		emit signalEvent(m);
	}
	if(opcode == msgspectatorseat)
	{
		Message m(seat);
		*m_net >> _seat;
		*m_net >> _player;
		m.data["player"] = QString(_player);
		free(_player);

		insertPlayer(Player::spectator, m.data["player"], _seat);

		if(_seat >= m_spectatorseats) m_spectatorseats = _seat + 1;

		emit signalEvent(m);
	}
	if(opcode == msgchat)
	{
		Message m(chat);
		*m_net >> _player;
		*m_net >> _message;
		m.data["player"] = QString(_player);
		m.data["message"] = QString(_message);
		free(_player);
		free(_message);
		emit signalEvent(m);
	}
	if(opcode == msgstats)
	{
		Message m(stats);

		for(int i = 0; i < m_playerseats + m_spectatorseats; i++)
		{
			*m_net >> _hasrecord;
			*m_net >> _hasrating;
			*m_net >> _hasranking;
			*m_net >> _hashighscore;
			*m_net >> _wins;
			*m_net >> _losses;
			*m_net >> _ties;
			*m_net >> _forfeits;
			*m_net >> _rating;
			*m_net >> _ranking;
			*m_net >> _highscore;

			Statistics stat;
			if(_hasrecord)
			{
				stat.wins = _wins;
				stat.losses = _wins;
				stat.ties = _ties;
				stat.forfeits = _forfeits;
			}
			if(_hasrating)
			{
				stat.rating = _rating;
			}
			if(_hasranking)
			{
				stat.ranking = _ranking;
			}
			if(_hashighscore)
			{
				stat.highscore = _highscore;
			}

			for(it = m_players.begin(); it != m_players.end(); it++)
			{
				int condition = false;
				if(i < m_playerseats)
				{
					int x = i;
					if(((*it).seat == x) && ((*it).type() != Player::spectator))
					{
						condition = true;
					}
				}
				else
				{
					int x = i - m_playerseats;
					if(((*it).seat == x) && ((*it).type() == Player::spectator))
					{
						condition = true;
					}
				}
				if(condition)
				{
					(*it).stats = stat;
					it = m_players.end();
				}
				i++;
			}
		}

		emit signalEvent(m);
	}
}

void Module::connect()
{
	QString ggzmode = getenv("GGZMODE");
	if(ggzmode.isNull())
	{
		kdDebug() << "[kggz::module] info: GGZMODE not set, ignoring" << endl;
	}

	QString ggzsocket = getenv("GGZSOCKET");
	if(ggzsocket.isNull())
	{
		kdDebug() << "[kggz::module] error: GGZSOCKET not set" << endl;
	}

	int ggzfd = ggzsocket.toInt();

	m_dev = new QSocketDevice(ggzfd, QSocketDevice::Stream);
	m_net = new QDataStream(m_dev);

	m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
	QObject::connect(m_notifier, SIGNAL(activated(int)), SLOT(slotEvent()));
}

void Module::disconnect()
{
	delete m_notifier;
	delete m_net;
	delete m_dev;
}

void Module::insertPlayer(Player::Type seattype, QString name, int seat)
{
	QValueList<Player>::Iterator it;

	Player p(seattype);
	p.name = name;
	p.seat = seat;

	for(it = m_players.begin(); it != m_players.end(); it++)
	{
		if((*it).name == p.name)
		{
			it = m_players.remove(it);
			it = m_players.end();
		}
	}
	m_players.append(p);
}

