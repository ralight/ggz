#ifndef FYRDMAN_MAINWINDOW_H
#define FYRDMAN_MAINWINDOW_H

#include <kmainwindow.h>

class MainWindow : public KMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
		~MainWindow();

	public slots:
		void slotMenu(int id);

	private:
		enum Items
		{
			game_info,
			game_sync,
			game_quit,
			option_map,
			option_possession,
			option_knights,
			option_animation
		};
};

#endif

