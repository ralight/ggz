#ifndef KGGZ_H
#define KGGZ_H

#include <ktmainwindow.h>
#include <qevent.h>
#include <kpopupmenu.h>

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

	enum Menuitems {MENU_GGZ_CONNECT,
			MENU_GGZ_DISCONNECT,
			MENU_GGZ_STARTSERVER,
			MENU_GGZ_STOPSERVER,
			MENU_GGZ_QUIT,

			MENU_CLIENT_STARTUP,
			MENU_CLIENT_CHAT,
			MENU_CLIENT_TABLES,
			MENU_CLIENT_PLAYERS,
			MENU_CLIENT_HELP,

			MENU_GAME_LAUNCH,
			MENU_GAME_JOIN,

			MENU_PREFERENCES_SETTINGS,
			MENU_PREFERENCES_PROFILES,
			MENU_PREFERENCES_GAMES,
			MENU_PREFERENCES_PREFERENCES,

			MENU_ROOMS_SLOTS}; // MENU_ROOMS_SLOTS _must_ be the last element
public slots:
	void handleMenu(int id);
protected:
	void resizeEvent(QResizeEvent *e);
private:
	void hideChilds();
};

#endif
