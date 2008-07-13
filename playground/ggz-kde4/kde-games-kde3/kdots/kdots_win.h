///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_WIN_H
#define KDOTS_WIN_H

#include <kmainwindow.h>
#include <kmenubar.h>

class KDots;
class KDotsAbout;
class KDotsHelp;
class KDotsReplay;

class KDotsWin : public KMainWindow
{
	Q_OBJECT
	public:
		KDotsWin(bool ggzmode);
		~KDotsWin();

		enum MenuItems
		{
			menuhelp,
			menuabout,
			menuquit,
			menusync,
			menureplay,
			menuseats
		};
	
	public slots:
		void slotMenu(int id);
		void slotStatus(const char *message);
		void slotColor(const QColor& color);

	private:
		KDots *m_dots;
		KDotsAbout *kdots_about;
		KDotsHelp *kdots_help;
		KDotsReplay *kdots_replay;
		QWidget *m_color;
};

#endif

