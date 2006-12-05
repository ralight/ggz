#include "module.h"
#include "module_private.h"

//#include <kggzmod/player.h>
#include <kggzmod/statistics.h>
#include "player_private.h"
#include "statistics_private.h"

#include "misc_private.h"

#include <kdebug.h>

//#include <qsocketnotifier.h>
//#include <qsocketdevice.h>
//#include <qdatastream.h>

//#include <unistd.h>
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

	connect(d, SIGNAL(signalEvent(KGGZMod::Event)), this, SIGNAL(signalEvent(KGGZMod::Event)));
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

/*int Module::fd() const
{
	return d->m_fd;
}*/

void ModulePrivate::sendRequest(Request request)
{
	if(!m_net)
	{
		kdDebug() << "[kggzmod] error: not connected" << endl;
		return;
	}

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
	QValueList<Player*>::Iterator it;

	kdDebug() << "[kggzmod] debug: input from GGZ has arrived" << endl;
	*m_net >> opcode;

	kdDebug() << "[kggzmod] info: got GGZ input " << opcode << endl;

	if((opcode < msglaunch) || (opcode > msginfo))
	{
		disconnect();
		kdDebug() << "[kggzmod] error: unknown opcode" << endl;
		emit signalError();
	}

	// FIXME: also treat opcode 'msgerror' as signalError()!

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
		*m_net >> _host;
		*m_net >> _port;
		*m_net >> _player;
		//e.data["host"] = QString(_host);
		//e.data["player"] = QString(_player);
		//e.data["port"] = QString::number(_port);
		free(_host);
		free(_player);
		//emit signalEvent(e);

		// FIXME: we don't handle this variant
	}
	if(opcode == msgserverfd)
	{
		sendRequest(StateRequest(Module::waiting));

		Event e(Event::server);
		// FIXME: this is a send_fd operation!
		//*m_net >> _fd;
		//_fd = ntohl(_fd);
		ret = readfiledescriptor(m_fd, &_fd);
		if(!ret)
		{
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
		*m_net >> _player;
		*m_net >> _isspectator;
		*m_net >> _seat;
		e.data["player"] = QString(_player);
		free(_player);

		m_myseat = _seat;
		m_myspectator = (_isspectator != 0);

		insertPlayer((_isspectator ? Player::spectator : Player::player),
			e.data["player"], _seat);

		emit signalEvent(e);
	}
	if(opcode == msgseat)
	{
		Event e(Event::seat);
		*m_net >> _seat;
		*m_net >> _seattype;
		*m_net >> _player;
		e.data["player"] = QString(_player);
		free(_player);

		insertPlayer((Player::Type)_seattype, e.data["player"], _seat);

		if(_seat >= m_playerseats) m_playerseats = _seat + 1;

		emit signalEvent(e);
	}
	if(opcode == msgspectatorseat)
	{
		Event e(Event::seat);
		*m_net >> _seat;
		*m_net >> _player;
		e.data["player"] = QString(_player);
		free(_player);

		insertPlayer(Player::spectator, e.data["player"], _seat);

		if(_seat >= m_spectatorseats) m_spectatorseats = _seat + 1;

		emit signalEvent(e);
	}
	if(opcode == msgchat)
	{
		Event e(Event::chat);
		*m_net >> _player;
		*m_net >> _message;
		e.data["player"] = QString(_player);
		e.data["message"] = QString(_message);
		free(_player);
		free(_message);
		emit signalEvent(e);
	}
	if(opcode == msgstats)
	{
		Event e(Event::stats);

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

			Statistics *stat = new Statistics();
			StatisticsPrivate *statpriv = new StatisticsPrivate();
			if(_hasrecord)
			{
				statpriv->wins = _wins;
				statpriv->losses = _wins;
				statpriv->ties = _ties;
				statpriv->forfeits = _forfeits;
			}
			if(_hasrating)
			{
				statpriv->rating = _rating;
			}
			if(_hasranking)
			{
				statpriv->ranking = _ranking;
			}
			if(_hashighscore)
			{
				statpriv->highscore = _highscore;
			}
			stat->init(statpriv);

			/*for(it = m_players.begin(); it != m_players.end(); it++)
			{
				int condition = false;
				if(i < m_playerseats)
				{
					int x = i;
					if(((*it)->seat() == x) && ((*it)->type() != Player::spectator))
					{
						condition = true;
					}
				}
				else
				{
					int x = i - m_playerseats;
					if(((*it)->seat() == x) && ((*it)->type() == Player::spectator))
					{
						condition = true;
					}
				}
				if(condition)
				{
					(*it)->d->m_stats = stat;
					it = m_players.end();
				}
				i++;
			}*/

			Player *p;
			if(i < m_playerseats) p = *(m_players.at(i));
			else p = *(m_spectators.at(i));
			p->d->m_stats = stat;
		}

		emit signalEvent(e);
	}
}

void ModulePrivate::connect()
{
	kdDebug() << "[kggzmod] debug: connect() to GGZ" << endl;

	QString ggzmode = getenv("GGZMODE");
	if(ggzmode.isNull())
	{
		kdDebug() << "[kgg:mod] info: GGZMODE not set, ignore" << endl;
		// FIXME: alternatively throw error as well?
		return;
	}

	QString ggzsocket = getenv("GGZSOCKET");
	if(ggzsocket.isNull())
	{
		kdDebug() << "[kggzmod] error: GGZSOCKET not set" << endl;
		emit signalError();
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
		kdDebug() << "[kggzmod] error: socket is erroneous" << endl;
		emit signalError();
		// FIXME: signal is not propagated???
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
	QValueList<Player*>::Iterator it;

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

void ModulePrivate::insertPlayer(Player::Type seattype, QString name, int seat)
{
	QValueList<Player*>::Iterator it;

	Player *p = new Player();
	PlayerPrivate *ppriv = new PlayerPrivate();
	ppriv->m_type = seattype;
	ppriv->m_name = name;
	ppriv->m_seat = seat;
	p->init(ppriv);

	if(seattype == Player::spectator)
	{
		for(it = m_players.begin(); it != m_players.end(); it++)
		{
			if((*it)->name() == p->name())
			{
				it = m_spectators.remove(it);
				it = m_spectators.end();
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
				it = m_players.end();
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

