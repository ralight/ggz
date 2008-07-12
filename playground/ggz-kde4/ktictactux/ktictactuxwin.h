//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_WIN_H
#define KTICTACTUX_WIN_H

// KTicTacTux includes
#include "ktictactux.h"

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qmap.h>

// Forward declarations
class KPopupMenu;
class QAction;

// The game window
class KTicTacTuxWin : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		KTicTacTuxWin();
		// Destructor
		~KTicTacTuxWin();
		// Enable network functionality
		void enableNetwork(bool enabled);
		// Display score
		void score();
		// Change the theme
		void changeTheme(QString theme);
		// Read in all available themes
		void loadThemes();

	public slots:
		// Receive a status message from the game
		void slotStatus(const QString &status);
		// Receive a new score
		void slotScore(const QString &score);
		// Receive statistics
		void slotNetworkScore(int wins, int losses, int ties);
		// Menu slot
		void slotMenu(QAction *action);
		// Game is over
		void slotGameOver();

	private:
		// The game itself
		KTicTacTux *m_tux;
		// The menus;
		KPopupMenu *mgame, *mtheme, *mggz;
		// Game network status
		bool m_networked;
		// List of themes
		QMap<QString, QString> m_themes, m_player1, m_player2;
		QMap<int, QString> m_themenames;

		// Menu ids
		QAction *action_sync;
		QAction *action_score;
		QAction *action_theme;
		QAction *action_quit;
		QAction *action_ggzplayers;
		QAction *action_themes;
};

#endif

