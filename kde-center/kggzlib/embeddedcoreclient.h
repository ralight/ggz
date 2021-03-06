#ifndef EMBEDDED_CORE_CLIENT_H
#define EMBEDDED_CORE_CLIENT_H

#include <QObject>

#include <kggzcore/room.h>

#include "kggzlib_export.h"

class PlayerList;
class TableList;
class KChat;
class RoomList;
class KGGZCoreLayer;

class QAction;

namespace KGGZCore
{
	class CoreClient;
}

class KGGZLIB_EXPORT EmbeddedCoreClient : public QObject
{
	Q_OBJECT
	public:
		EmbeddedCoreClient(KGGZCore::CoreClient *core, KGGZCoreLayer *layer, bool withrooms);

		void setCore(KGGZCore::CoreClient *core);

		KChat *widget_chat();
		PlayerList *widget_players();
		TableList *widget_tables();
		RoomList *widget_rooms();

		QAction *action_launch();
		QAction *action_join();
		QAction *action_spectate();

	private slots:
		void slotLaunch();
		void slotJoin();
		void slotSpectate();
		// FIXME: slotMotd() as well?

		void slotChatEntered(int id, const QString& msg);
		void slotTable(const KGGZCore::Table& table, int pos);

		void slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::Room::AnswerMessage message);
		void slotEvent(KGGZCore::Room::EventMessage message);
		void slotChat(QString sender, QString message, KGGZCore::Room::ChatType type);

	private:
		void handleRoomlist();
		void handleTablelist();

		KGGZCore::CoreClient *m_core;
		KGGZCoreLayer *m_corelayer;

		KChat *m_chat;
		PlayerList *m_players;
		TableList *m_tables;
		RoomList *m_rooms;

		QAction *m_action_launch;
		QAction *m_action_join;
		QAction *m_action_spectate;

		unsigned int m_tablenum;
};

#endif

