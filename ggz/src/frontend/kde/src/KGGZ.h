#ifndef KGGZ_H
#define KGGZ_H

// KDE includes
#include <kpopupmenu.h>

// Qt includes
#include <qevent.h>

// Abstract KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCallback.h"

// KGGZ includes
#include "KGGZBase.h"
#include "KGGZLaunch.h"
#include "KGGZAbout.h"

// GGZCore++
#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"
#include "GGZCoreGame.h"

// Forward declaration
class KGGZWorkspace;

class KGGZ : public KGGZBase
{
	Q_OBJECT
	public:
		KGGZ();
		~KGGZ();

		enum Collectortypes
		{
			COLLECTOR_ROOM,
			COLLECTOR_SERVER,
			COLLECTOR_GAME
		};

	public slots:
		void slotHandleMenu(int id);
		void slotConnect(const char *host, int port, const char *username, const char *password, int mode, int server);
		void slotDisconnect();

		void slotChat(char *text);
		void slotLaunch();

		void slotAboutGGZ();

	protected:
		void resizeEvent(QResizeEvent *e);
		void timerEvent(QTimerEvent *e);
		void serverCollector(unsigned int id, void* data);
		void roomCollector(unsigned int id, void* data);
		void gameCollector(unsigned int id, void* data);
		static GGZHookReturn hookOpenCollector(unsigned int id, void* event_data, void* user_data);

	private:
		void serverLaunch();
		void serverKill();
		void dialogConnection();
		void dispatcher();
		void dispatch_free(char *var, char *description);
		void dispatch_delete(void *object, char *description);

		void attachRoomCallbacks();
		void detachRoomCallbacks();
		void attachServerCallbacks();
		void detachServerCallbacks();
		void attachGameCallbacks();
		void detachGameCallbacks();

		void listPlayers();
		void listTables();

		KPopupMenu *m_menu_client, *m_menu_rooms, *m_menu_ggz, *m_menu_game, *m_menu_preferences;
		KGGZWorkspace *m_workspace;
		GGZCoreServer *kggzserver;
		GGZCoreRoom *kggzroom;
		GGZCoreGame *kggzgame;
		KGGZCallback *kggzservercallback, *kggzroomcallback, *kggzgamecallback;
		char *m_save_username, *m_save_password, *m_save_host;
		int m_save_loginmode, m_save_rooms, m_save_port;
		int m_lock;
		KGGZLaunch *m_launch;
		KGGZAbout *m_about;
};

#endif
