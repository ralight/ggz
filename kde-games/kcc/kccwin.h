///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

#ifndef KCC_WIN_H
#define KCC_WIN_H

// KCC includes
#include "kcc.h"

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qmap.h>

// Forward declarations
class KPopupMenu;

// The game window
class KCCWin : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		KCCWin(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KCCWin();
		// The game object
		KCC *kcc();
		// Menu ids
		enum MenuEntries
		{
			menusync = 1,
			menuscore = 2,
			menutheme = 3,
			menuquit = 4,
			menuthemes = 10
		};
		// Enable network functionality
		void enableNetwork(bool enabled);
		// Display score
		void score();

	public slots:
		// Receive a status message from the game
		void slotStatus(const QString &status);
		// Receive a new score
		void slotScore(const QString &score);
		// Receive statistics
		void slotNetworkScore(int wins, int losses);
		// Menu slot
		void slotMenu(int id);
		// Game is over
		void slotGameOver();

	private:
		// The game itself
		KCC *m_kcc;
		// The menus;
		KPopupMenu *mgame;
		// Game network status
		bool m_networked;
};

#endif

