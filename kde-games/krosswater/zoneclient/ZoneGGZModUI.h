// ZoneClient - small C++ game support library
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

#ifndef ZONE_GGZ_MOD_UI_H
#define ZONE_GGZ_MOD_UI_H

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qwidget.h>
#include <qsocketnotifier.h>

// Zone includes
#include "ZoneGGZ.h"

// GGZ includes
#include <ggzmod.h>

class ZoneGGZModUI : public KMainWindow
{
	Q_OBJECT
	public:
		ZoneGGZModUI(QWidget *parent = NULL, const char *name = NULL);
		~ZoneGGZModUI();
		void ZoneRegister(char *modulename);
		int fd();
		int zoneTurn();
		int zoneMe();
		int zonePlayers();

		int ZoneSeat;
		int ZoneSeats[16];
		int ZonePlayernum;
		char ZonePlayers[16][17];
		int ZoneRules, ZoneMaxplayers;
		int ZoneGamePlayers;

	signals:
		void signalZoneReady();
		void signalZoneInput(int op);
		void signalZoneTurn();
		void signalZoneTurnOver();
		void signalZoneOver();
		void signalZoneInvalid();

	private slots:
		void slotZoneInput();
		void slotZoneControlInput();

	private:
		int zoneCreateFd(char *modulename);
		void zoneGetSeats();
		void zoneGetPlayers();
		void zoneGetRules();
		void zoneCallTurn();
		void zoneCallTurnOver();
		void zoneCallOver();
		void zoneCallInvalid();

		static void handle_server(GGZMod *mod, GGZModEvent e, const void *data);

		QSocketNotifier *zone_sn, *zone_sn_ctl;
		int zone_fd, zone_fd_ctl;
		int m_ready;
		int m_turn;
		int m_players;
		GGZMod *mod;
		static ZoneGGZModUI *self;
};

#endif

