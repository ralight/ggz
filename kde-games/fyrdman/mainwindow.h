#ifndef FYRDMAN_MAINWINDOW_H
#define FYRDMAN_MAINWINDOW_H

#include <kmainwindow.h>

class KPopupMenu;
class Map;
class Network;

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

	private:
		void levelSelector();

		enum Items
		{
			game_new,
			game_info,
			game_sync,
			game_quit,
			option_map,
			option_possession,
			option_knights,
			option_animation
		};

		KPopupMenu *displaymenu, *optionmenu;
		Map *map;
		Network *network;
};

#endif

