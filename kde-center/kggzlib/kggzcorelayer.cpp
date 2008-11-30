#include "kggzcorelayer.h"

#include <kggzcore/room.h>
#include <kggzcore/misc.h>
#include <kggzcore/table.h>
#include <kggzcore/player.h>

#include <klocale.h>
#include <qdebug.h>

KGGZCoreLayer::KGGZCoreLayer(QObject *parent, QString protengine, QString protversion)
: QObject(parent),
	m_core(0)
{
	m_protversion = protversion;
	m_protengine = protengine;
	m_typedrooms = false;
}

KGGZCoreLayer::~KGGZCoreLayer()
{
	delete m_core;
}

void KGGZCoreLayer::activity(QString activity)
{
	qDebug("=== %s ===", qPrintable(activity));
}

void KGGZCoreLayer::checkTables()
{
	QList<KGGZCore::Table> tables = m_core->room()->tables();
	qDebug() << "#count:" << tables.size();
	for(int i = 0; i < tables.size(); i++)
	{
		KGGZCore::Table table = tables.at(i);
		qDebug() << "# -" << table.description();
		QList<KGGZCore::Player> players = table.players();
		for(int j = 0; j < players.size(); j++)
		{
			KGGZCore::Player p = players.at(j);
			qDebug() << "#  -" << p.name() << p.type();
			if(p.type() == KGGZCore::Player::reserved)
			{
				if(m_core->username() == p.name())
				{
					qDebug("~~~~ invitation to table %i (%s)!", i, qPrintable(table.description()));
				}
			}
		}
	}
}

void KGGZCoreLayer::ggzcore(QString uri)
{
	m_core = new KGGZCore::CoreClient(this, true);

	connect(m_core,
		SIGNAL(signalFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)),
		SLOT(slotFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)));
	connect(m_core,
		SIGNAL(signalAnswer(KGGZCore::CoreClient::AnswerMessage)),
		SLOT(slotAnswer(KGGZCore::CoreClient::AnswerMessage)));
	connect(m_core,
		SIGNAL(signalEvent(KGGZCore::CoreClient::EventMessage)),
		SLOT(slotEvent(KGGZCore::CoreClient::EventMessage)));

	activity(i18n("Connecting..."));

	m_core->setUrl(uri);
	m_core->setUsername("foo-dev"); // FIXME: always give fully-qualified URLs
	m_core->initiateLogin();
}

void KGGZCoreLayer::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			if(error == KGGZCore::Error::no_status)
			{
				activity(i18n("Negotiating..."));
			}
			else
			{
				activity(i18n("Connection failed!"));
				notready();
			}
			break;
		case KGGZCore::CoreClient::negotiation:
			if(error == KGGZCore::Error::no_status)
			{
				activity(i18n("Logging in..."));
			}
			else
			{
				activity(i18n("Negotiation failed!"));
				notready();
			}
			break;
		case KGGZCore::CoreClient::login:
			if(error == KGGZCore::Error::no_status)
			{
				activity(i18n("Entering the room..."));
			}
			else
			{
				activity(i18n("Login failed!"));
				notready();
			}
			break;
		case KGGZCore::CoreClient::roomenter:
			activity(i18n("In the room %1!", m_core->room()->name()));
			//roominfo();
			connect(m_core->room(),
				SIGNAL(signalFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)),
				SLOT(slotFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)));
			connect(m_core->room(),
				SIGNAL(signalAnswer(KGGZCore::Room::AnswerMessage)),
				SLOT(slotAnswer(KGGZCore::Room::AnswerMessage)));
			connect(m_core->room(),
				SIGNAL(signalEvent(KGGZCore::Room::EventMessage)),
				SLOT(slotEvent(KGGZCore::Room::EventMessage)));
			emit signalRoomReady(true);
			break;
		case KGGZCore::CoreClient::chat:
			break;
		case KGGZCore::CoreClient::channel:
			break;
		case KGGZCore::CoreClient::logout:
			break;
	}
}

void KGGZCoreLayer::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			if(m_typedrooms)
				switchroom();
			m_typedrooms = true;
			break;
		case KGGZCore::CoreClient::typelist:
			if(m_typedrooms)
				switchroom();
			m_typedrooms = true;
			break;
		case KGGZCore::CoreClient::motd:
			break;
	}
}

void KGGZCoreLayer::switchroom()
{
	bool canswitch = false;
	QList<KGGZCore::Room*> rooms = m_core->rooms();
	if(rooms.size() > 0)
	{
		for(int i = 0; i < rooms.size(); i++)
		{
			KGGZCore::Room *room = rooms.at(i);
			KGGZCore::GameType gt = room->gametype();
			qDebug("ROOM: %s/%s vs. %s/%s",
				qPrintable(gt.protocolEngine()),
				qPrintable(m_protengine),
				qPrintable(gt.protocolVersion()),
				qPrintable(m_protversion));
			if((gt.protocolEngine() == m_protengine)
			&& (gt.protocolVersion() == m_protversion))
			{
				m_core->initiateRoomChange(room->name());
				activity(i18n("Switch to room %1", room->name()));
				canswitch = true;
				break;
			}
		}
		qDeleteAll(rooms);
		rooms.clear();
	}

	if(!canswitch)
	{
		activity(i18n("No suitable room or game server found"));
		notready();
	}
}

void KGGZCoreLayer::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::state_changed:
			if(m_core->state() == KGGZCore::reconnecting)
				activity(i18n("Reconnecting..."));
			break;
		case KGGZCore::CoreClient::players_changed:
			//roominfo();
			break;
		case KGGZCore::CoreClient::rooms_changed:
			break;
		case KGGZCore::CoreClient::neterror:
			break;
		case KGGZCore::CoreClient::protoerror:
			break;
		case KGGZCore::CoreClient::libraryerror:
			break;
	}
}

void KGGZCoreLayer::slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	// FIXME!
	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::Room::tablelaunched:
			activity(i18n("A table has been launched"));
			break;
		case KGGZCore::Room::tablejoined:
			break;
		case KGGZCore::Room::tableleft:
			break;
	}
}

void KGGZCoreLayer::slotAnswer(KGGZCore::Room::AnswerMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::playerlist:
			break;
		case KGGZCore::Room::tablelist:
			qDebug() << "#tables";
			checkTables();
			//roominfo();
			break;
	}
}

void KGGZCoreLayer::slotEvent(KGGZCore::Room::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::chat:
			//chat(i18n("Players are chatting..."));
			break;
		case KGGZCore::Room::enter:
			break;
		case KGGZCore::Room::leave:
			break;
		case KGGZCore::Room::tableupdate:
			qDebug() << "#tables(update)";
			checkTables();
			//roominfo();
			break;
		case KGGZCore::Room::playerlag:
			break;
		case KGGZCore::Room::stats:
			break;
		case KGGZCore::Room::count:
			break;
		case KGGZCore::Room::perms:
			break;
		case KGGZCore::Room::libraryerror:
			break;
	}
}

void KGGZCoreLayer::launchmodule()
{
	//KGGZCore::Table *table = new KGGZCore::Table("*embedded*");
	//table->launch(m_core);

	connect(m_core->room(),
		SIGNAL(signalModuleReady()),
		SLOT(slotModuleReady()));
	connect(m_core->room(),
		SIGNAL(signalTableReady()),
		SLOT(slotTableReady()));

	m_core->room()->launchmodule();
}

void KGGZCoreLayer::slotModuleReady()
{
	emit signalReady(true);
}

void KGGZCoreLayer::notready()
{
	m_core->deleteLater();
	m_core = NULL;
	emit signalReady(false);
}

void KGGZCoreLayer::slotTableReady()
{
	m_core->room()->launchtable(m_seats);
}

void KGGZCoreLayer::configureTable(QList<KGGZCore::Player> seats)
{
	//seats.prepend(KGGZCore::Player(m_core->username(), KGGZCore::Player::player));
	m_seats = seats;
}

void KGGZCoreLayer::launch()
{
	launchmodule();
}

#include "kggzcorelayer.moc"
