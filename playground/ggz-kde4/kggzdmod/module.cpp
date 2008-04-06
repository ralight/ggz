#include "module.h"
#include "module_private.h"

#include "player_private.h"
#include "event_private.h"
#include "event.h"

#include <kdebug.h>

#include <ggzdmod.h>

using namespace KGGZdMod;

static GGZdMod *s_ggzdmod = NULL;
static Module *s_module = NULL;
static ModulePrivate *s_private = NULL;

void ModulePrivate::ggzdmod_handler(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	s_private->handler(mod, event, data);
}

Module::Module(QString name)
: QObject()
{
	s_module = this;

	d = new ModulePrivate();
	s_private = d;

	d->m_name = name;

	d->m_state = created;

	d->m_playerseats = 0;
	d->m_spectatorseats = 0;

	connect(d, SIGNAL(signalEvent(const KGGZdMod::Event&)), this, SIGNAL(signalEvent(const KGGZdMod::Event&)));

	d->connect();
}

Module::~Module()
{
	d->disconnect();
	delete d;

	delete s_module;
	s_module = NULL;
}

void Module::sendRequest(Request request)
{
	d->sendRequest(request);
}

QList<Player*> Module::players() const
{
	return d->m_players;
}

QList<Player*> Module::spectators() const
{
	return d->m_spectators;
}

Module::State Module::state() const
{
	return d->m_state;
}

void ModulePrivate::handler(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	kDebug() << "[kggzdmod] Event" << event;

	if(event == GGZDMOD_EVENT_STATE)
	{
		GGZdModState oldstate;
		GGZdModState state;

		oldstate = *(GGZdModState*)data;
		state = ggzdmod_get_state(mod);

		m_state = (KGGZdMod::Module::State)state;

		Event e(Event::state);
		EventPrivate *ep = new EventPrivate();
		ep->m_state = (Module::State)state;
		ep->m_oldstate = (Module::State)oldstate;
		e.init(ep);

		if(oldstate == GGZDMOD_STATE_CREATED)
		{
			kDebug() << "** insert" << ggzdmod_get_num_seats(mod) << "seats...";
			// FIXME: work around ggzdmod not reporting initial bot joins
			for(int i = 0; i < ggzdmod_get_num_seats(mod); i++)
			{
				GGZSeat seat = ggzdmod_get_seat(mod, i);
				PlayerPrivate player;
				player.m_type = (Player::Type)seat.type;
				player.m_name = QString(seat.name);
				player.m_seat = seat.num;
				player.m_fd = seat.fd;
				insertPlayer(player);
				kDebug() << "** type" << player.m_type;
			}
		}

		emit signalEvent(e);
	}
	else if((event == GGZDMOD_EVENT_SEAT)
	|| (event == GGZDMOD_EVENT_JOIN)
	|| (event == GGZDMOD_EVENT_LEAVE))
	{
		GGZSeat oldseat;
		GGZSeat seat;

		oldseat = *(GGZSeat*)data;
		seat = ggzdmod_get_seat(mod, oldseat.num);

		PlayerPrivate player;
		PlayerPrivate oldplayer;

		player.m_type = (Player::Type)seat.type;
		player.m_name = QString(seat.name);
		player.m_seat = seat.num;
		player.m_fd = seat.fd;

		oldplayer.m_type = (Player::Type)oldseat.type;
		oldplayer.m_name = QString(oldseat.name);
		oldplayer.m_seat = oldseat.num;
		oldplayer.m_fd = oldseat.fd;

		Event e(Event::playerseat);
		EventPrivate *ep = new EventPrivate();
		ep->m_seat = player;
		ep->m_oldseat = oldplayer;
		e.init(ep);

		removePlayer(oldplayer);
		insertPlayer(player);

		emit signalEvent(e);
	}
	else if((event == GGZDMOD_EVENT_SPECTATOR_SEAT)
	|| (event == GGZDMOD_EVENT_SPECTATOR_JOIN)
	|| (event == GGZDMOD_EVENT_SPECTATOR_LEAVE))
	{
		GGZSpectator oldspectator;
		GGZSpectator spectator;

		oldspectator = *(GGZSeat*)data;
		spectator = ggzdmod_get_spectator(mod, oldspectator.num);

		PlayerPrivate player;
		PlayerPrivate oldplayer;

		player.m_type = Player::spectator;
		player.m_name = QString(spectator.name);
		player.m_seat = spectator.num;
		player.m_fd = spectator.fd;

		oldplayer.m_type = Player::spectator;
		oldplayer.m_name = QString(oldspectator.name);
		oldplayer.m_seat = oldspectator.num;
		oldplayer.m_fd = oldspectator.fd;

		Event e(Event::spectatorseat);
		EventPrivate *ep = new EventPrivate();
		ep->m_spectatorseat = player;
		ep->m_oldspectatorseat = oldplayer;
		e.init(ep);

		removePlayer(oldplayer);
		insertPlayer(player);

		emit signalEvent(e);
	}
	else if(event == GGZDMOD_EVENT_PLAYER_DATA)
	{
		GGZSeat seat;

		int num = *(int*)data;
		seat = ggzdmod_get_seat(mod, num);

		PlayerPrivate player;

		player.m_type = (Player::Type)seat.type;
		player.m_name = QString(seat.name);
		player.m_seat = seat.num;
		player.m_fd = seat.fd;

		Event e(Event::playerdata);
		EventPrivate *ep = new EventPrivate();
		ep->m_seat = player;
		e.init(ep);

		emit signalEvent(e);
	}
	else if(event == GGZDMOD_EVENT_SPECTATOR_DATA)
	{
		GGZSpectator spectator;

		int num = *(int*)data;
		spectator = ggzdmod_get_spectator(mod, num);

		PlayerPrivate player;

		player.m_type = Player::spectator;
		player.m_name = QString(spectator.name);
		player.m_seat = spectator.num;
		player.m_fd = spectator.fd;

		Event e(Event::spectatordata);
		EventPrivate *ep = new EventPrivate();
		ep->m_spectatorseat = player;
		e.init(ep);

		emit signalEvent(e);
	}
	else if(event == GGZDMOD_EVENT_SAVEDGAME)
	{
		QString savegame = (const char*)data;

		Event e(Event::savegame);
		EventPrivate *ep = new EventPrivate();
		ep->m_savegame = savegame;
		e.init(ep);

		emit signalEvent(e);
	}
	else if(event == GGZDMOD_EVENT_ERROR)
	{
		QString message((char*)data);

		Event e(Event::error);
		EventPrivate *ep = new EventPrivate();
		ep->m_message = message;
		e.init(ep);

		emit signalEvent(e);
	}
	else
	{
		kError() << "[kggzdmod] Unknown event from ggzdmod!";
	}
}

void ModulePrivate::sendRequest(Request request)
{
	if(!s_ggzdmod)
	{
		kDebug() << "[kggzdmod] error: not connected";
		return;
	}

	kDebug() << "[kggzdmod] debug: send a request";

	Request::Type requesttype = request.type();

	if(requesttype == Request::state)
	{
		ggzdmod_set_state(s_ggzdmod, (GGZdModState)request.data["state"].toInt());
	}
	if(requesttype == Request::log)
	{
		ggzdmod_log(s_ggzdmod, request.data["message"].toUtf8().data());
	}
	if(requesttype == Request::result)
	{
	}
	if(requesttype == Request::savegame)
	{
		ggzdmod_report_savegame(s_ggzdmod, request.data["filename"].toUtf8().data());
	}
}

void ModulePrivate::connect()
{
	kDebug() << "[kggzdmod] debug: connect() to GGZ";

	s_ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	int ret = ggzdmod_connect(s_ggzdmod);
	if(ret)
	{
		kError() << "[kggzdmod] Could not connect to GGZ!";
		ggzdmod_free(s_ggzdmod);
		s_ggzdmod = NULL;
		return;
	}

	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_STATE, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_JOIN, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_LEAVE, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SEAT, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SPECTATOR_SEAT, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_SAVEDGAME, &ggzdmod_handler);
	ggzdmod_set_handler(s_ggzdmod, GGZDMOD_EVENT_ERROR, &ggzdmod_handler);

	kDebug() << "[kggzdmod] debug: connect() is finished";

	kDebug() << "[kggzdmod] debug: now LOOP because we don't do async ops yet...";
}

void Module::loop()
{
	ggzdmod_loop(s_ggzdmod);
}

void ModulePrivate::disconnect()
{
	int ret = ggzdmod_disconnect(s_ggzdmod);
	if(ret)
	{
		kError() << "[kggzdmod] Could not disconnect from GGZ!";
	}
	ggzdmod_free(s_ggzdmod);
	s_ggzdmod = NULL;
}

// FIXME: should maybe propagate wrapper to Module class, currently only used internally
Player* ModulePrivate::findPlayer(Player::Type seattype, QString name)
{
	QList<Player*>::Iterator it;

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

void ModulePrivate::removePlayer(PlayerPrivate player)
{
	QList<Player*>::Iterator it;

	if(player.m_type == Player::spectator)
	{
		for(it = m_spectators.begin(); it != m_spectators.end(); it++)
		{
			if((*it)->name() == player.m_name)
			{
				kDebug() << "** remove spectator at #" << player.m_seat;
				delete (*it);
				it = m_spectators.erase(it);
				break;
			}
		}
	}
	else
	{
		for(it = m_players.begin(); it != m_players.end(); it++)
		{
			if((*it)->name() == player.m_name)
			{
				kDebug() << "** remove player at #" << player.m_seat;
				delete (*it);
				it = m_players.erase(it);
				break;
			}
		}
	}
}

void ModulePrivate::insertPlayer(PlayerPrivate player)
{
	QList<Player*>::Iterator it;

	Player *p = new Player();
	PlayerPrivate *ppriv = new PlayerPrivate();
	ppriv->m_type = player.m_type;
	ppriv->m_name = player.m_name;
	ppriv->m_seat = player.m_seat;
	ppriv->m_fd = player.m_fd;
	p->init(ppriv);

	if(player.m_type == Player::spectator)
	{
		kDebug() << "** insert spectator at #" << player.m_seat;
		if(player.m_seat < m_spectators.size())
			m_spectators.insert(player.m_seat, p);
		else
			m_spectators.append(p);
	}
	else
	{
		kDebug() << "** insert player at #" << player.m_seat;
		if(player.m_seat < m_players.size())
			m_players.insert(player.m_seat, p);
		else
			m_players.append(p);
	}
}

bool Module::isGGZ()
{
	if(getenv("GGZMODE")) return true;
	else return false;
}

Module *Module::instance()
{
	return s_module;
}

//#include "module.moc"
//#include "module_private.moc"
