#ifndef KGGZ_BASE_H
#define KGGZ_BASE_H

#include "KGGZ.h"
#include "KGGZAbout.h"

// KDE includes
#include <ktmainwindow.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <ksimpleconfig.h>

// Qt includes
#include <qiconset.h>

class KGGZBase : public KTMainWindow
{
	Q_OBJECT
	public:
		KGGZBase(char *name = NULL);
		~KGGZBase();

	public slots:
		void slotMenu(int id);
		void slotAboutGGZ();
		void slotMenuSignal(int signal);
		void slotRoom(char *roomname);
		void slotCaption(char *caption);
		void slotState(int state);

	private:
		QIconSet kggzGetIcon(int menuid);

		enum MenuItems
		{
			MENU_GGZ_TEST,
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
		KGGZ *kggz;
		KGGZAbout *m_about;
		KPopupMenu *m_menu_client, *m_menu_rooms, *m_menu_ggz, *m_menu_game, *m_menu_preferences;
		int m_rooms;
		KSimpleConfig *konfig;
};

#endif
