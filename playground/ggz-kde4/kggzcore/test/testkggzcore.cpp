#include "testkggzcore.h"

#include <kggzcore/room.h>

TestKGGZCore::TestKGGZCore()
{
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

	m_core->setUrl("ggz://guest9999@localhost:5688");
	m_core->initiateLogin();
}

void TestKGGZCore::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	qDebug("slotFeedback!");

	if((message == KGGZCore::CoreClient::roomenter) && (error == KGGZCore::Error::no_status))
	{
		qDebug("We're in the room! Let's get some statistics");
		KGGZCore::Room *room = m_core->room();
		qDebug("Room name: %s", qPrintable(room->name()));
	}
}

void TestKGGZCore::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	qDebug("slotAnswer!");

	if(message == KGGZCore::CoreClient::roomlist)
	{
		qDebug("room 0 is %s", qPrintable(m_core->roomnames().at(0)));
		m_core->initiateRoomChange(m_core->roomnames().at(0));
	}
}

void TestKGGZCore::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	qDebug("slotEvent!");

	Q_UNUSED(message);
}

