#ifndef GGZAP_TRAY_H
#define GGZAP_TRAY_H

#include <ksystemtray.h>

#include <qpopupmenu.h>

class GGZapGame;
class GGZapConfig;

class GGZapTray : public KSystemTray
{
	Q_OBJECT
	public:
		GGZapTray(QWidget *parent = NULL, const char *name = NULL);
		~GGZapTray();

		enum MenuItems
		{
			menuquit,
			menuminimize,
			menulaunch,
			menucancel,
			menuconfigure
		};

	signals:
		void signalLaunch(char *name, char *frontend);
		void signalCancel();

	public slots:
		void slotLaunch(int gameid);
		void slotMenu(int id);
		
	private:
		void contextMenuAboutToShow(KPopupMenu *menu);

		QPopupMenu *m_menu;
		GGZapGame *m_game;
		GGZapConfig *m_config;
		int m_launched;
};

#endif

