#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <kmainwindow.h>
#include <qstringlist.h>

class Board;

class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();
		enum MenuItems
		{
			menugamenew = 1,
			menugamequit = 2,
			menuplayerremis = 3,
			menuplayerloose = 4,
			menuthemes = 100,
			menuvariants = 200
		};

	public slots:
		void slotMenu(int id);

	private:
		Board *board;
		QStringList themelist, variantslist;
};

#endif

