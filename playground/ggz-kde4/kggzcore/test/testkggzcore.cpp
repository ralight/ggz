#include "testkggzcore.h"

#include <kggzcore/room.h>
#include <kggzcore/misc.h>

TestKGGZCore::TestKGGZCore(QString url)
{
	qDebug("GGZ: connect to %s", qPrintable(url));

	m_core = new KGGZCore::CoreClient(this);

	connect(m_core,
		SIGNAL(signalFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)),
		SLOT(slotFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)));
	connect(m_core,
		SIGNAL(signalAnswer(KGGZCore::CoreClient::AnswerMessage)),
		SLOT(slotAnswer(KGGZCore::CoreClient::AnswerMessage)));
	connect(m_core,
		SIGNAL(signalEvent(KGGZCore::CoreClient::EventMessage)),
		SLOT(slotEvent(KGGZCore::CoreClient::EventMessage)));

	m_core->setUrl(url);
	m_core->initiateLogin();
}

void TestKGGZCore::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	KGGZCore::Room *room;

	Q_UNUSED(error);

	qDebug("slotFeedback!");

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			break;
		case KGGZCore::CoreClient::negotiation:
			break;
		case KGGZCore::CoreClient::login:
			break;
		case KGGZCore::CoreClient::roomenter:
			//if(error == KGGZCore::Error::no_status)
			qDebug("We're in the room! Let's get some statistics");
			room = m_core->room();
			qDebug("Room name: %s", qPrintable(room->name()));
			break;
		case KGGZCore::CoreClient::chat:
			break;
		case KGGZCore::CoreClient::channel:
			break;
		case KGGZCore::CoreClient::logout:
			break;
	}
}

void TestKGGZCore::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	qDebug("slotAnswer!");

	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			qDebug("room 0 is %s", qPrintable(m_core->roomnames().at(0)));
			m_core->initiateRoomChange(m_core->roomnames().at(0));
			break;
		case KGGZCore::CoreClient::typelist:
			break;
		case KGGZCore::CoreClient::motd:
			qDebug("MOTD web url: %s", qPrintable(m_core->textmotd()));
			qDebug("MOTD web text:\n%s", qPrintable(m_core->webmotd()));
			break;
	}
}

void TestKGGZCore::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	qDebug("slotEvent!");

	switch(message)
	{
		case KGGZCore::CoreClient::state_changed:
			qDebug("* state: %s", qPrintable(KGGZCore::Misc::statename(m_core->state())));
			break;
		case KGGZCore::CoreClient::players_changed:
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

