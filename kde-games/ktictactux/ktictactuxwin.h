//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_WIN_H
#define KTICTACTUX_WIN_H

// KTicTacTux includes
#include "ktictactux.h"

// KDE includes
#include <kmainwindow.h>

// Forward declarations
class KPopupMenu;

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
			menuscore,
			menuquit,
			menuthemenew,
			menuthemeclassic,
			menuthemesymbols
		};
		// Enable network functionality
		void enableNetwork(bool enabled);
		// Display score
		void score();
		// Change the theme
		void changeTheme(int theme);

	public slots:
		// Receive a status message from the game
		void slotStatus(const QString &status);
		// Receive a new score
		void slotScore(const QString &score);
		// Receive statistics
		void slotNetworkScore(int wins, int losses);
		// Menu slot
		void slotMenu(int id);

	private:
		// The game itself
		KTicTacTux *m_tux;
		// The menus;
		KPopupMenu *mgame, *mtheme;
		// Game network status
		bool m_networked;
};

#endif

