//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KCONNECTX_WIN_H
#define KCONNECTX_WIN_H

// KConnectX includes
#include "kconnectx.h"

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qmap.h>

// Forward declarations
class KPopupMenu;

// The game window
class Win : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		Win(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~Win();
		// The game object
		KConnectX *game() const;
		// Menu ids
		enum MenuEntries
		{
			menuquit = 1,
			menuscore = 2,
			menuplayers = 3
		};
		// Status bar entries
		enum StatusEntries
		{
			statusgame = 1
		};
		// Display score
		void score();

	public slots:
		// Receive a status message from the game
		void slotStatus(const QString &status);
		// Receive statistics
		void slotNetworkScore(int wins, int losses, int ties);
		// Menu slot
		void slotMenu(int id);
		// Game is over
		void slotGameOver();

	private:
		// The game itself
		KConnectX *m_game;
		// The menus;
		KPopupMenu *m_menugame, *m_menuggz;
};

#endif

