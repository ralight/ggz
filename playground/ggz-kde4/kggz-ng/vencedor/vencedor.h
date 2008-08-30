#ifndef VENCEDOR_H
#define VENCEDOR_H

#include <QMainWindow>

#include <kggzcore/coreclient.h>
#include <kggzcore/room.h>

class PlayerList;
class RoomList;
class TableList;
class KChat;

class Vencedor : public QMainWindow
{
	Q_OBJECT
	public:
		Vencedor(QString url);

	private slots:
		void slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void slotEvent(KGGZCore::CoreClient::EventMessage message);
		void slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::Room::AnswerMessage message);
		void slotEvent(KGGZCore::Room::EventMessage message);

		void slotConnect();
		void slotAbout();
		void slotLaunch();

	private:
		KGGZCore::CoreClient *m_core;
		KChat *m_chat;
		PlayerList *m_players;
		RoomList *m_rooms;
		TableList *m_tables;
};

#endif
