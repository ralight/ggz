#include "module.h"
#include "module_private.h"

#include <kggzmod/statistics.h>
#include "player_private.h"
#include "statistics_private.h"

#include "misc_private.h"

#include <kdebug.h>

#include <stdlib.h> // for free() and getenv()
#include <netinet/in.h> // for ntohl()

using namespace KGGZMod;

// Implementation notes:
// 1) all classes have private counterparts to keep the API clean
//  this is a bit clumsy in the source but also offers ABI compatibility
// 2) classes always take ownership of their private objects
//  i.e. after calling obj->init(opriv) the caller mustn't delete opriv
//  and certainly not do anything else including calling init() again

Module::Module(QString name)
: QObject()
{
	d = new ModulePrivate();

	d->m_name = name;

	d->m_fd = -1;

	d->m_dev = 0;
	d->m_net = 0;
	d->m_notifier = 0;
	d->m_gnotifier = 0;

	d->m_state = created;

	d->m_playerseats = 0;
	d->m_spectatorseats = 0;

	d->m_myseat = -1;

	connect(d, SIGNAL(signalEvent(const KGGZMod::Event&)), this, SIGNAL(signalEvent(const KGGZMod::Event&)));
	connect(d, SIGNAL(signalError()), this, SIGNAL(signalError()));
	connect(d, SIGNAL(signalNetwork(int)), this, SIGNAL(signalNetwork(int)));

	d->connect();
}

Module::~Module()
{
	d->disconnect();
	delete d;
}

void Module::sendRequest(Request request)
{
	d->sendRequest(request);
}

QValueList<Player*> Module::players() const
{
	return d->m_players;
}

QValueList<Player*> Module::spectators() const
{
	return d->m_spectators;
}

Module::State Module::state() const
{
	return d->m_state;
}

void ModulePrivate::sendRequest(Request request)
{
	if(!m_net)
	{
		kdDebug() << "[kggzmod] error: not connected" << endl;
		return;
	}

	kdDebug() << "[kggzmod] debug: send a request" << endl;

	Request::Type opcode = request.type();
	// FIXME: in networking we always assume sizeof(opcode) = 4!

	if(opcode == Request::state)
	{
		*m_net << opcode;
		*m_net << (Q_INT8)request.data["state"].toInt();
	}
	if(opcode == Request::stand)
	{
		*m_net << opcode;
	}
	if(opcode == Request::sit)
	{
		*m_net << opcode;
		*m_net << request.data["seat"].toInt();
	}
	if(opcode == Request::boot)
	{
		*m_net << opcode;
		*m_net << request.data["player"];
	}
	if(opcode == Request::bot)
	{
		*m_net << opcode;
		*m_net << request.data["seat"].toInt();
	}
	if(opcode == Request::open)
	{
		*m_net << opcode;
		*m_net << request.data["seat"].toInt();
	}
	if(opcode == Request::chat)
	{
		*m_net << opcode;
		*m_net << request.data["message"];
	}
	if(opcode == Request::info)
	{
		*m_net << opcode;
		*m_net << request.data["seat"].toInt();
	}
}

void ModulePrivate::slotGGZEvent()
{
	int opcode, ret;
	char *_host;
	int _port;
	char *_player, *_message;
	int _fd;
	int _isspectator, _seat;
	int _seattype;
	int _hasrecord, _hasrating, _hasranking, _hashighscore;
	int _wins, _losses, _ties, _forfeits, _rating, _ranking, _highscore;
	int _num;
	char *_realname, *_photo;
	QValueList<Player*>::Iterator it;
	QByteArray buffer;
	Q_LONG available, oldavailable, maxavailable;
	QDataStream net(buffer, IO_ReadOnly);

	available = 0;

	kdDebug() << "[kggzmod] debug: input from GGZ has arrived" << endl;

	while((!available) || (!net.atEnd()))
	{

	while(true)
	{
		if(m_dev->bytesAvailable() > (Q_LONG)10000)
		{
			kdDebug() << "[kggzmod] error: network overflow" << endl;
			emit signalError();
			return;
		}

		kdDebug() << "[kggzmod] debug: bytesavailable=" << m_dev->bytesAvailable() << endl;
		maxavailable = m_dev->bytesAvailable();

		// FIXME: this dull hack is so we can handle msgserverfd correctly!
		if(maxavailable == 5)
		{
			maxavailable = 4;
		}

		buffer.resize(buffer.size() + maxavailable);
		oldavailable = m_dev->readBlock(buffer.data() + available, maxavailable);
		if(oldavailable == -1)
		{
			kdDebug() << "[kggzmod] error: network error" << endl;
			emit signalError();
			return;
		}
		available += oldavailable;

		// Part 2 of the dull msgserverfd hack
		if(maxavailable == 4)
		{
			QDataStream testnet(buffer, IO_ReadOnly);
			testnet >> opcode;
			if(opcode == msgserverfd)
			{
				kdDebug() << "[kggzmod] debug: msgserverfd shortcut" << endl;
				break;
			}
		}

		m_dev->waitForMore(100);
		if(m_dev->bytesAvailable() == 0) break;
	}

	buffer.truncate(available);
	kdDebug() << "[kggzmod] debug: BUFFER = " << buffer << endl;

	net >> opcode;
	kdDebug() << "[kggzmod] info: got GGZ input " << opcode << endl;

	if((opcode < msglaunch) || (opcode > msginfo))
	{
		disconnect();
		kdDebug() << "[kggzmod] error: unknown opcode" << endl;
		emit signalError();
		return;
	}

	// FIXME: also treat ggzmod 'error' events as signalError()!

	if(opcode == msglaunch)
	{
		Event e(Event::launch);
		emit signalEvent(e);

		// FIXME: ggzmod doesn't provide a "launch" event

		sendRequest(StateRequest(Module::connected));
	}
	if(opcode == msgserver)
	{
		//Event e(Event::server);
		net >> _host;
		net >> _port;
		net >> _player;
		//e.data["host"] = QString(_host);
		//e.data["player"] = QString(_player);
		//e.data["port"] = QString::number(_port);
		free(_host);
		free(_player);
		//emit signalEvent(e);

		// FIXME: we don't handle this variant

		disconnect();
		kdDebug() << "[kggzmod] error: we don't handle msgserver" << endl;
		emit signalError();
	}
	if(opcode == msgserverfd)
	{
		sendRequest(StateRequest(Module::waiting));

		Event e(Event::server);
		// FIXME: this is a send_fd operation, might not be portable!
		ret = readfiledescriptor(m_fd, &_fd);
		if(!ret)
		{
			disconnect();
			kdDebug() << "[kggzmod] error: socket reading failed" << endl;
			emit signalError();
			return;
		}

		kdDebug() << "[kggzmod] debug: server fd = " << _fd << endl;
		e.data["fd"] = QString::number(_fd);
		emit signalEvent(e);

		m_gnotifier = new QSocketNotifier(_fd, QSocketNotifier::Read, this);
		QObject::connect(m_gnotifier, SIGNAL(activated(int)), SIGNAL(signalNetwork(int)));
	}
	if(opcode == msgplayer)
	{
		Event e(Event::self);
		net >> _player;
		net >> _isspectator;
		net >> _seat;
		e.data["player"] = QString(_player);
		free(_player);

		m_myseat = _seat;
		m_myspectator = (_isspectator != 0);

		insertPlayer((_isspectator ? Player::spectator : Player::player),
			e.data["player"], _seat);
		e.m_player = findPlayer((_isspectator ? Player::spectator : Player::player),
			e.data["player"]);

		emit signalEvent(e);
	}
	if(opcode == msgseat)
	{
		Event e(Event::seat);
		net >> _seat;
		net >> _seattype;
		net >> _player;
		e.data["player"] = QString(_player);
		free(_player);

		insertPlayer((Player::Type)_seattype, e.data["player"], _seat);
		e.m_player = findPlayer((Player::Type)_seattype, e.data["player"]);

		if(_seat >= m_playerseats) m_playerseats = _seat + 1;

		emit signalEvent(e);
	}
	if(opcode == msgspectatorseat)
	{
		Event e(Event::seat);
		net >> _seat;
		net >> _player;
		e.data["player"] = QString(_player);
		free(_player);

		insertPlayer(Player::spectator, e.data["player"], _seat);
		e.m_player = findPlayer(Player::spectator, e.data["player"]);

		if(_seat >= m_spectatorseats) m_spectatorseats = _seat + 1;

		emit signalEvent(e);
	}
	if(opcode == msgchat)
	{
		Event e(Event::chat);
		net >> _player;
		net >> _message;
		e.data["player"] = QString(_player);
		e.data["message"] = QString(_message);
		free(_player);
		free(_message);

		e.m_player = findPlayer(Player::player, e.data["player"]);

		emit signalEvent(e);
	}
	if(opcode == msgstats)
	{
		Event e(Event::stats);

		for(int i = 0; i < m_playerseats + m_spectatorseats; i++)
		{
			net >> _hasrecord;
			net >> _hasrating;
			net >> _hasranking;
			net >> _hashighscore;
			net >> _wins;
			net >> _losses;
			net >> _ties;
			net >> _forfeits;
			net >> _rating;
			net >> _ranking;
			net >> _highscore;

			Statistics *stat = new Statistics();
			StatisticsPrivate *statpriv = new StatisticsPrivate();
			statpriv->hasrecord = false;
			statpriv->hasrating = false;
			statpriv->hasranking = false;
			statpriv->hashighscore = false;

			if(_hasrecord)
			{
				statpriv->wins = _wins;
				statpriv->losses = _wins;
				statpriv->ties = _ties;
				statpriv->forfeits = _forfeits;
				statpriv->hasrecord = true;
			}
			if(_hasrating)
			{
				statpriv->rating = _rating;
				statpriv->hasrating = true;
			}
			if(_hasranking)
			{
				statpriv->ranking = _ranking;
				statpriv->hasranking = true;
			}
			if(_hashighscore)
			{
				statpriv->highscore = _highscore;
				statpriv->hashighscore = true;
			}
			stat->init(statpriv);

			Player *p;
			if(i < m_playerseats) p = *(m_players.at(i));
			else p = *(m_spectators.at(i));
			p->d->m_stats = stat;
		}

		emit signalEvent(e);
	}
	if(opcode == msginfo)
	{
		Event e(Event::info);

		net >> _num;

		for(int i = 0; i < _num; i++)
		{
			net >> _seat;
			net >> _realname;
			net >> _photo;
			net >> _host;
			//e.data["realname"] = QString(_realname);
			//e.data["photo"] = QString(_photo);
			//e.data["host"] = QString(_host);
			QString realname(_realname);
			QString photo(_photo);
			QString host(_host);
			free(_host);
			free(_photo);
			free(_realname);

			for(it = m_players.begin(); it != m_players.end(); it++)
			{
				if((*it)->seat() == _seat)
				{
					Player *p = (*it);
					p->d->m_realname = realname;
					p->d->m_hostname = host;
					p->d->m_photo = photo;
					break;
				}
			}
		}

		emit signalEvent(e);
	}

	}
}

void ModulePrivate::connect()
{
	kdDebug() << "[kggzmod] debug: connect() to GGZ" << endl;

	QString ggzmode = getenv("GGZMODE");
	if(ggzmode.isNull())
	{
		kdDebug() << "[kggzmod] info: GGZMODE not set, ignore" << endl;
		// FIXME: alternatively throw error as well?
		return;
	}

	QString ggzsocket = getenv("GGZSOCKET");
	if(ggzsocket.isNull())
	{
		kdDebug() << "[kggzmod] error: GGZSOCKET not set" << endl;
		emit signalError();
		return;
	}

	m_fd = ggzsocket.toInt();
	kdDebug() << "[kggzmod] debug: use socket " << ggzsocket << endl;
	kdDebug() << "[kggzmod] debug: numeric socket " << m_fd << endl;

	m_dev = new QSocketDevice(m_fd, QSocketDevice::Stream);
	m_net = new QDataStream(m_dev);

	m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
	QObject::connect(m_notifier, SIGNAL(activated(int)), SLOT(slotGGZEvent()));

	if(!m_dev->isValid())
	{
		disconnect();
		kdDebug() << "[kggzmod] error: socket is erroneous" << endl;
		emit signalError();
		// FIXME: signal is not propagated???
		return;
	}

	kdDebug() << "[kggzmod] debug: connect() is finished" << endl;
}

void ModulePrivate::disconnect()
{
	delete m_gnotifier;

	delete m_notifier;
	delete m_net;
	delete m_dev;

	m_dev = 0;
	m_net = 0;
	m_notifier = 0;
	m_gnotifier = 0;
}

Player *ModulePrivate::self() const
{
	if(m_myspectator)
	{
		return *(m_spectators.at(m_myseat));
	}
	else
	{
		return *(m_players.at(m_myseat));
	}

	return 0;
}

Player* ModulePrivate::findPlayer(Player::Type seattype, QString name)
{
	QValueList<Player*>::Iterator it;

	if(seattype == Player::spectator)
	{
		for(it = m_spectators.begin(); it != m_spectators.end(); it++)
		{
			if((*it)->name() == name)
			{
				return (*it);
			}
		}
	}
	else
	{
		for(it = m_players.begin(); it != m_players.end(); it++)
		{
			if((*it)->name() == name)
			{
				return (*it);
			}
		}
	}

	return 0;
}

void ModulePrivate::insertPlayer(Player::Type seattype, QString name, int seat)
{
	QValueList<Player*>::Iterator it;

	Player *p = new Player();
	PlayerPrivate *ppriv = new PlayerPrivate();
	ppriv->m_type = seattype;
	ppriv->m_name = name;
	ppriv->m_seat = seat;
	ppriv->m_stats = 0;
	p->init(ppriv);

	if(seattype == Player::spectator)
	{
		for(it = m_spectators.begin(); it != m_spectators.end(); it++)
		{
			if((*it)->name() == p->name())
			{
				it = m_spectators.remove(it);
				break;
			}
		}
		m_spectators.append(p);
	}
	else
	{
		for(it = m_players.begin(); it != m_players.end(); it++)
		{
			if((*it)->name() == p->name())
			{
				it = m_players.remove(it);
				break;
			}
		}
		m_players.append(p);
	}
}

bool Module::isGGZ()
{
	if(getenv("GGZMODE")) return true;
	else return false;
}

Player *Module::self() const
{
	return d->self();
}

