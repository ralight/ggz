#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <kmainwindow.h>

class Board;

class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();
		enum MenuItems
		{
			menugamenew,
			menugamequit,
			menuplayerremis,
			menuplayerloose,
			menutheme1,
			menutheme2,
			menutheme3
		};

	public slots:
		void slotMenu(int id);

	private:
		Board *board;
};

#endif

