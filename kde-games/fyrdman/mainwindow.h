#ifndef FYRDMAN_MAINWINDOW_H
#define FYRDMAN_MAINWINDOW_H

#include <kmainwindow.h>
#include <qptrlist.h>

class KPopupMenu;
class Map;
class Network;
class QSocketNotifier;
class Level;

class MainWindow : public KMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
		~MainWindow();

		void enableNetwork();

	public slots:
		void slotMenu(int id);
		void slotData();
		void slotMove(int x, int y, int x2, int y2);

	private:
		void levelSelector();
		void synchronize();

		bool checkMove(int self);
		void aiMove();

		enum Items
		{
			game_new,
			game_info,
			game_sync,
			game_quit,
			option_map,
			option_possession,
			option_knights,
			option_animation,
			option_map_bayeux,
			option_map_map,
			option_map_haroldsreturn
		};

		enum Statuscodes
		{
			status_state,
			status_level,
			status_task
		};

		KPopupMenu *gamemenu, *displaymenu, *optionmenu, *backgroundmenu;
		Map *map;
		Network *network;
		QSocketNotifier *sn, *snc;
		QPtrList<Level> m_levels;
		int m_movex, m_movey, m_movex2, m_movey2;
		int m_self;
};

#endif

