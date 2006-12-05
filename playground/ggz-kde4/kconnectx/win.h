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
#include <QMap>

// Forward declarations
class KMenu;
class KAction;
class QAction;

// The game window
class Win : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		Win(QWidget *parent = 0);
		// Destructor
		~Win();
		// The game object
		KConnectX *game() const;
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
		void slotMenu(QAction*);
		// Game is over
		void slotGameOver();

	private:
		// The game itself
		KConnectX *m_game;
		// The menus;
		KMenu *m_menugame, *m_menuggz;
		KAction *m_actexit, *m_actggz, *m_acthistory;
};

#endif

