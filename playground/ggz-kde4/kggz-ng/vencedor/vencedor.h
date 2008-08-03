#ifndef VENCEDOR_H
#define VENCEDOR_H

#include <QMainWindow>

#include <kggzcore/coreclient.h>

class QTreeWidget;

class Vencedor : public QMainWindow
{
	Q_OBJECT
	public:
		Vencedor(QString url);

	private slots:
		void slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void slotEvent(KGGZCore::CoreClient::EventMessage message);

	private:
		KGGZCore::CoreClient *m_core;
		QTreeWidget *m_roomwidget;
};

#endif
