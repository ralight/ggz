//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_WIN_H
#define KTICTACTUX_WIN_H

// KTicTacTux includes
#include "ktictactux.h"

// KDE includes
#include <kmainwindow.h>

// The game window
class KTicTacTuxWin : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		KTicTacTuxWin(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KTicTacTuxWin();
		// The game object
		KTicTacTux *tux();
		// Menu ids
		enum MenuEntries
		{
			menusync,
			menuquit
		};

	public slots:
		// Receive a status message from the game
		void slotStatus(QString status);
		// Menu slot
		void slotMenu(int id);

	private:
		// The game itself
		KTicTacTux *m_tux;
};

#endif

