#ifndef TESTKGGZCORE_H
#define TESTKGGZCORE_H

#include <QObject>

#include <kggzcore/coreclient.h>

class TestKGGZCore : public QObject
{
	Q_OBJECT
	public:
		TestKGGZCore(QString url);

	private slots:
		void slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void slotEvent(KGGZCore::CoreClient::EventMessage message);

	private:
		KGGZCore::CoreClient *m_core;
};

#endif
