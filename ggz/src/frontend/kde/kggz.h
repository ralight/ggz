#ifndef KGGZ_H
#define KGGZ_H

#include <ktmainwindow.h>
#include <qevent.h>
#include <kpopupmenu.h>

#include "kggz_grubby.h"
#ifdef USE_FTP
#include "kggz_update.h"
#endif
#include "kggz_playerinfo.h"

class KGGZ : public KTMainWindow
{
Q_OBJECT
public:
	KGGZ();
	~KGGZ();
	static KPopupMenu *menuRooms();
	static KPopupMenu *menuGGZ();
	static KPopupMenu *menuClient();
	static KPopupMenu *menuGame();
	static QIconSet getIcon(int menuid);

	enum Menuitems {MENU_GGZ_CONNECT,
			MENU_GGZ_DISCONNECT,
#ifdef USE_SERVER
			MENU_GGZ_STARTSERVER,
			MENU_GGZ_STOPSERVER,
#endif
			MENU_GGZ_QUIT,

			MENU_CLIENT_STARTUP,
			MENU_CLIENT_CHAT,
			MENU_CLIENT_TABLES,
			MENU_CLIENT_PLAYERS,
			MENU_CLIENT_HELP,

			MENU_GAME_LAUNCH,
			MENU_GAME_JOIN,
			MENU_GAME_UPDATE,
			MENU_GAME_GRUBBY,

			MENU_PREFERENCES_SETTINGS,
			MENU_PREFERENCES_PLAYERINFO,
			MENU_PREFERENCES_HOSTS,
			MENU_PREFERENCES_FTP,
			MENU_PREFERENCES_GAMES,
			MENU_PREFERENCES_PREFERENCES,

			MENU_ROOMS_SLOTS}; // MENU_ROOMS_SLOTS _must_ be the last element
public slots:
	void handleMenu(int id);
protected:
	void resizeEvent(QResizeEvent *e);
private:
	void hideChilds();

	KGGZ_Grubby *m_grubby;
#ifdef USE_FTP
	KGGZ_Update *m_update;
#endif
	KGGZ_Playerinfo *m_playerinfo;
};

#endif
