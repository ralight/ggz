#ifndef FYRDMAN_MAINWINDOW_H
#define FYRDMAN_MAINWINDOW_H

#include "fyrdman.h"

#include <kmainwindow.h>

#include <qptrlist.h>

class KPopupMenu;
class Map;
class Network;
class QSocketNotifier;
class Level;

class MainWindow : public KMainWindow, public FyrdmanDcop
{
	Q_OBJECT
	public:
		MainWindow();
		~MainWindow();

		void enableNetwork();

	k_dcop:
		ASYNC newLevel(QString level);

	public slots:
		void slotMenu(int id);
		void slotData();
		void slotMove(int x, int y, int x2, int y2);

	private:
		void levelSelector();
		void scanLevels(QString basedir);
		void scanNewLevels();
		void levelInformation();
		void unitInformation(int num);
		void gameInformation();

		void synchronize();

		bool checkMove(int self, bool execute);
		int cellValue(int bot, int x, int y, bool neightbours);
		void aiMove();

		enum Items
		{
			game_new,
			game_info,
			game_unitinfo,
			game_gameinfo,
			game_sync,
			game_seats,
			game_newlevels,
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

