///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_WIN_H
#define KDOTS_WIN_H

#include <ktmainwindow.h>
#include <kmenubar.h>

#include "kdots.h"
#include "kdots_about.h"
#include "kdots_help.h"

class KDotsWin : public KTMainWindow
{
	Q_OBJECT
	public:
		KDotsWin(char *name = NULL);
		~KDotsWin();

	enum MenuItems
	{
		menuhelp,
		menuabout,
		menuquit,
		menusync
	};
	
	public slots:
		void slotMenu(int id);
		void slotStatus(const char *message);
	private:
		KDots *m_dots;
		KDotsAbout *kdots_about;
		KDotsHelp *kdots_help;
};

#endif

