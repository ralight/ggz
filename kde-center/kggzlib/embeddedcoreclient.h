#ifndef EMBEDDED_CORE_CLIENT_H
#define EMBEDDED_CORE_CLIENT_H

#include <QObject>

#include <kggzcore/room.h>

#include "kggzlib_export.h"

class PlayerList;
class TableList;
class KChat;
class RoomList;

class QAction;

namespace KGGZCore
{
	class CoreClient;
}

class KGGZLIB_EXPORT EmbeddedCoreClient : public QObject
{
	Q_OBJECT
	public:
		EmbeddedCoreClient(KGGZCore::CoreClient *core, bool withrooms);

		KChat *widget_chat();
		PlayerList *widget_players();
		TableList *widget_tables();
		RoomList *widget_rooms();

		QAction *action_launch();
		QAction *action_join();
		QAction *action_spectate();

	private slots:
		void slotChat(QString sender, QString message, KGGZCore::Room::ChatType type);

		void slotLaunch();
		void slotJoin();
		void slotSpectate();
		// FIXME: slotMotd() as well? + slotChatEntered!

		void slotTable(const KGGZCore::Table& table, int pos);
	private:
		void handleRoomlist();

		KGGZCore::CoreClient *m_core;

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

