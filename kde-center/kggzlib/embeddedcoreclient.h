#ifndef EMBEDDED_CORE_CLIENT_H
#define EMBEDDED_CORE_CLIENT_H

#include <QMainWindow>

#include <kggzcore/room.h>

#include "kggzlib_export.h"

class PlayerList;
class TableList;
class KChat;

class KGGZLIB_EXPORT EmbeddedCoreClient : public QMainWindow
{
	Q_OBJECT
	public:
		EmbeddedCoreClient();

		void slotChat(QString sender, QString message, KGGZCore::Room::ChatType type);

		void slotLaunch();
		void slotJoin();
		void slotSpectate();
		// FIXME: slotMotd() as well? + slotChatEntered!
	private slots:
		void slotTable(const KGGZCore::Table& table, int pos);
	private:
		void handleRoomlist();

		//KGGZCore::CoreClient *m_core;
		KChat *m_chat;
		PlayerList *m_players;
		TableList *m_tables;

		QAction *m_action_launch;
		QAction *m_action_join;
		QAction *m_action_spectate;

		unsigned int m_tablenum;
};

#endif

