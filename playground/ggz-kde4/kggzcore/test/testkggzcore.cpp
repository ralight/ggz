#include "testkggzcore.h"

TestKGGZCore::TestKGGZCore()
{
	m_core = new KGGZCore::CoreClient(this);

	m_core->setUrl("ggz://guest9999@localhost:5688");

	m_core->initiateLogin();
}

void TestKGGZCore::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	qDebug("slotFeedback!");

	Q_UNUSED(message);
	Q_UNUSED(error);
}

void TestKGGZCore::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	qDebug("slotAnswer!");

	Q_UNUSED(message);
}

void TestKGGZCore::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	qDebug("slotEvent!");

	Q_UNUSED(message);
}

