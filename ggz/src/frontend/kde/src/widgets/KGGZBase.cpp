// Header file
#include "KGGZBase.h"

// Abstract KGGZ includes
#include "KGGZCommon.h"

KGGZBase::KGGZBase(char *name)
: KTMainWindow(name)
{
	m_menu = new KMenuBar(this);

	KGGZDEBUGF("KGGZBase::KGGZBase()\n");

	setCaption("KGGZ - [offline]");
}

KGGZBase::~KGGZBase()
{
}

QIconSet KGGZBase::kggzGetIcon(int menuid)
{
	char *icon = NULL;
	QIconSet iconset;

	switch(menuid)
	{
		case MENU_GGZ_CONNECT:
			icon = "connect.png";
			break;
		case MENU_GGZ_DISCONNECT:
			icon = "disconnect.png";
			break;
		case MENU_GGZ_STARTSERVER:
			icon = "startserver.png";
			break;
		case MENU_GGZ_STOPSERVER:
			icon = "stopserver.png";
			break;
		case MENU_GGZ_QUIT:
			icon = "quit.png";
			break;
		case MENU_CLIENT_STARTUP:
			icon = "startup.png";
			break;
		case MENU_CLIENT_CHAT:
			icon = "chat.png";
			break;
		case MENU_CLIENT_TABLES:
			icon = "tables.png";
			break;
		case MENU_CLIENT_PLAYERS:
			icon = "players.png";
			break;
		case MENU_CLIENT_HELP:
			icon = "browser.png";
			break;
		case MENU_GAME_LAUNCH:
			icon = "launch.png";
			break;
		case MENU_GAME_JOIN:
			icon = "join.png";
			break;
		case MENU_GAME_UPDATE:
			icon = "update.png";
			break;
		case MENU_GAME_GRUBBY:
			icon = "grubby.png";
			break;
		case MENU_PREFERENCES_SETTINGS:
		case MENU_PREFERENCES_PLAYERINFO:
		case MENU_PREFERENCES_HOSTS:
		case MENU_PREFERENCES_FTP:
		case MENU_PREFERENCES_GAMES:
		case MENU_PREFERENCES_THEMES:
			icon = "pref.png";
			break;
		case MENU_PREFERENCES_PREFERENCES:
			icon = "preferences.png";
			break;
		default:
			icon = "unknown.png";
			break;
	}

	if(menuid >= MENU_ROOMS_SLOTS)
	{
		icon = "unknown.png";
	}

	iconset = QIconSet(QPixmap(QString(KGGZ_DIRECTORY "/images/icons/") + QString(icon)));
	return iconset;
}
