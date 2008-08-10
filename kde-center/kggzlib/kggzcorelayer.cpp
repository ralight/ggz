#include "kggzcorelayer.h"

#include <kggzcore/room.h>
#include <kggzcore/misc.h>
#include <kggzcore/table.h>
#include <kggzcore/player.h>
 
#include <klocale.h>
#include <qdebug.h>

KGGZCoreLayer::KGGZCoreLayer(QObject *parent)
: QObject(parent),
	m_core(0)
{
}
 
KGGZCoreLayer::~KGGZCoreLayer()
{
	delete m_core;
}

void KGGZCoreLayer::activity(QString activity)
{
	qDebug("=== %s ===", qPrintable(activity));
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
	m_core->setUsername("foo-dev");
	m_core->initiateLogin();
} 
 
void KGGZCoreLayer::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Negotiating..."));
			else
				activity(i18n("Connection failed!"));
			break;
		case KGGZCore::CoreClient::negotiation:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Logging in..."));
			else
				activity(i18n("Negotiation failed!"));
			break;
		case KGGZCore::CoreClient::login:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Entering the room..."));
			else
				activity(i18n("Login failed!"));
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
			launchtable();
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
	int num;

	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			num = m_core->roomnames().count();
			if(num > 0)
			{
				// FIXME: depend on room type!
				QString roomname = "Tic-Tac-Toe";
				//KConfigGroup cg = config();
				//QString roomname = cg.readEntry("roomname");
				//if((roomname.isEmpty()) | (!m_core->roomnames().contains(roomname)))
				//	roomname = m_core->roomnames().at(0);
				m_core->initiateRoomChange(roomname);
			}
			break;
		case KGGZCore::CoreClient::typelist:
			break;
		case KGGZCore::CoreClient::motd:
			break;
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
			m_core->room()->launchmodule();
			emit signalReady();
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
			//checkTables();
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
			//checkTables();
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

void KGGZCoreLayer::launchtable()
{
	//KGGZCore::Table *table = new KGGZCore::Table("*embedded*");
	//table->launch(m_core);

	m_core->room()->launch();
}
 
#include "kggzcorelayer.moc"
