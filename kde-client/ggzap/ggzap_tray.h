// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GGZAP_TRAY_H
#define GGZAP_TRAY_H

// KDE includes
#include <ksystemtray.h>

// Qt includes
#include <qpopupmenu.h>

// Forward declarations
class GGZapGame;
class GGZapConfig;

// The tray incon class for GGZap
class GGZapTray : public KSystemTray
{
	Q_OBJECT
	public:
		GGZapTray(QWidget *parent = NULL, const char *name = NULL);
		~GGZapTray();

		enum MenuItems
		{
			menuquit,
			menuminimize,
			menulaunch,
			menucancel,
			menuconfigure,
			menuabout
		};

		enum States
		{
			stateidle,
			stateactive
		};

	signals:
		void signalLaunch(QString name, QString frontend);
		void signalCancel();

	public slots:
		void slotLaunch(int gameid);
		void slotMenu(int id);
		
	private:
		void contextMenuAboutToShow(KPopupMenu *menu);

		QPopupMenu *m_menu;
		GGZapGame *m_game;
		int m_state;
};

#endif

