#ifndef KGGZ_BASE_H
#define KGGZ_BASE_H

// KDE includes
#include <ktmainwindow.h>
#include <kmenubar.h>

// Qt includes
#include <qiconset.h>

class KGGZBase : public KTMainWindow
{
	Q_OBJECT
	public:
		KGGZBase(char *name);
		~KGGZBase();

	protected:
		QIconSet kggzGetIcon(int menuid);

		enum Menuitems
		{
			MENU_GGZ_CONNECT,
			MENU_GGZ_DISCONNECT,
			MENU_GGZ_STARTSERVER,
			MENU_GGZ_STOPSERVER,
			MENU_GGZ_QUIT,

			MENU_CLIENT_STARTUP,
			MENU_CLIENT_CHAT,
			MENU_CLIENT_TABLES,
			MENU_CLIENT_PLAYERS,
			MENU_CLIENT_HELP,

			MENU_GAME_INFO,
			MENU_GAME_LAUNCH,
			MENU_GAME_JOIN,
			MENU_GAME_UPDATE,
			MENU_GAME_GRUBBY,
			MENU_GAME_NEWS,

			MENU_PREFERENCES_SETTINGS,
			MENU_PREFERENCES_PLAYERINFO,
			MENU_PREFERENCES_HOSTS,
			MENU_PREFERENCES_FTP,
			MENU_PREFERENCES_GAMES,
			MENU_PREFERENCES_THEMES,
			MENU_PREFERENCES_PREFERENCES,

			MENU_ROOMS_SLOTS // MENU_ROOMS_SLOTS _must_ be the last element
		};

		KMenuBar *m_menu;
};

#endif
