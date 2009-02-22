#ifndef VENCEDOR_H
#define VENCEDOR_H

#include <QMainWindow>

#include <kggzcore/coreclient.h>
#include <kggzcore/room.h>

class PlayerList;
class RoomList;
class TableList;
class KChat;
class QAction;

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
		void slotChat(QString sender, QString message, KGGZCore::Room::ChatType type);

		void slotConnect();
		void slotDisconnect();
		void slotAbout();
		void slotHelp();
		void slotConfig();
		void slotLaunch();
		void slotJoin();
		void slotSpectate();
		void slotModules();

		void slotRoom(const QString& name);
		void slotChatEntered(int id, const QString& msg);
		void slotTable(const KGGZCore::Table& table, int pos);

		void slotFavourite(const QString& roomname, bool favourite);

	private:
		void enable(bool enabled);
		void connection(const QString& url);
		void handleRoomlist();
		void handleSession();

		KGGZCore::CoreClient *m_core;
		KChat *m_chat;
		PlayerList *m_players;
		RoomList *m_rooms;
		TableList *m_tables;
		QAction *m_action_launch;
		QAction *m_action_join;
		QAction *m_action_spectate;
		QAction *m_action_connect;
		QAction *m_action_disconnect;
		unsigned int m_tablenum;
};

#endif
