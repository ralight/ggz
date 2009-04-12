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
class EmbeddedCoreClient;

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
		void slotMotd();
		void slotModules();

		void slotRoom(const QString& name);
		void slotChatEntered(int id, const QString& msg);

		void slotFavourite(const QString& roomname, bool favourite);

	private:
		void enable(bool enabled);
		void connection(const QString& url);
		void handleRoomlist();
		void handleSession();

		KGGZCore::CoreClient *m_core;
		QAction *m_action_connect;
		QAction *m_action_disconnect;
		QAction *m_action_motd;
		EmbeddedCoreClient *m_ecc;
};

#endif
