#ifndef KGGZ_H
#define KGGZ_H

#include <kpopupmenu.h>

// Qt includes
#include <qevent.h>
#include <qwidget.h>

// Abstract KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCallback.h"

// KGGZ includes
#include "KGGZLaunch.h"
#include "KGGZConnect.h"

// GGZCore++
#include "GGZCore.h"
#include "GGZCoreConf.h"
#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"
#include "GGZCoreGame.h"
#include "GGZCoreGametype.h"

// Forward declarations
class KGGZWorkspace;
class KGGZSplash;
#ifdef KGGZ_BROWSER
class KGGZBrowser;
#endif

class KGGZ : public QWidget
{
	Q_OBJECT
	public:
		KGGZ(QWidget *parent = NULL, char *name = NULL);
		~KGGZ();

		enum Collectortypes
		{
			COLLECTOR_ROOM,
			COLLECTOR_SERVER,
			COLLECTOR_GAME
		};

		enum Viewtypes
		{
			VIEW_CHAT,
			VIEW_USERS,
			VIEW_TABLES,
                        VIEW_SPLASH,
			VIEW_BROWSER
		};

		enum Menusignals
		{
			MENUSIG_DISCONNECT,
			MENUSIG_ROOMLIST,
			MENUSIG_LOGIN,
			MENUSIG_SERVERSTART,
			MENUSIG_SERVERSTOP
		};

	signals:
		void signalMenu(int signal);
		void signalRoom(char *roomname);

	public slots:
		void slotConnected(const char *host, int port, const char *username, const char *password, int mode, int server);

		void menuConnect();
		void menuDisconnect();
		void menuServerLaunch();
		void menuServerKill();
		void menuView(int viewtype);
		void menuGameLaunch();
		void menuGameJoin();
		void menuGameInfo();
		void menuRoom(int room);

		void slotChat(char *text);
		void slotLaunch();

	protected:
		void resizeEvent(QResizeEvent *e);
		void timerEvent(QTimerEvent *e);
		void serverCollector(unsigned int id, void* data);
		void roomCollector(unsigned int id, void* data);
		void gameCollector(unsigned int id, void* data);
		static GGZHookReturn hookOpenCollector(unsigned int id, void* event_data, void* user_data);

	private:
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

		void slotLaunchGame(GGZCoreGametype *gametype);

		KGGZWorkspace *m_workspace;
		GGZCoreServer *kggzserver;
		GGZCoreRoom *kggzroom;
		GGZCoreGame *kggzgame;
		KGGZCallback *kggzservercallback, *kggzroomcallback, *kggzgamecallback;
		char *m_save_username, *m_save_password, *m_save_host;
		int m_save_loginmode, m_save_port;
		int m_lock;
		KGGZLaunch *m_launch;
		KGGZConnect *m_connect;
		GGZCore *m_core;
		GGZCoreConf *m_config;
		KGGZSplash *m_splash;
		KPopupMenu *m_menu_client, *m_menu_rooms, *m_menu_ggz, *m_menu_game, *m_menu_preferences;
#ifdef KGGZ_BROWSER
		KGGZBrowser *m_browser;
#endif
};

#endif
