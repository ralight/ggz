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

// Header file
#include "ZoneGGZModUI.h"

// GGZ includes
#include <ggz.h>

// System includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

// Static variables
ZoneGGZModUI *ZoneGGZModUI::self;

// Constructor
ZoneGGZModUI::ZoneGGZModUI(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	self = this;

	for(int i = 0; i < 16; i++)
	{
		strcpy(ZonePlayers[i], "");
		ZoneSeats[i] = -1;
	}
	ZoneSeat = -1;
	ZonePlayernum = -1;
	ZoneMaxplayers = 0;
	ZoneGamePlayers = 0;
	m_ready = 0;
	m_turn = 0;

	zone_fd = -1;
	zone_fd_ctl = -1;

	mod = NULL;
}

// Destructor
ZoneGGZModUI::~ZoneGGZModUI()
{
	if(mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}
}

// private: create socket, connect to fd
int ZoneGGZModUI::zoneCreateFd(char *modulename)
{
	/*zone_fd = 3;*/

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_server);
	ggzmod_connect(mod);
	zone_fd_ctl = ggzmod_get_fd(mod);
	return 0;
}

// public: register game module
void ZoneGGZModUI::ZoneRegister(char *modulename)
{
	if(zoneCreateFd(modulename) == -1)
	{
		ZONEERROR("couldn't create socket\n");
		return;
	}

	zone_sn_ctl = new QSocketNotifier(zone_fd_ctl, QSocketNotifier::Read);
	connect(zone_sn_ctl, SIGNAL(activated(int)), SLOT(slotZoneControlInput()));

	setCaption(modulename);
}

// private: read from control channel
void ZoneGGZModUI::slotZoneControlInput()
{
	ggzmod_dispatch(mod);
}

// private: get standard input from network
void ZoneGGZModUI::slotZoneInput()
{
	int op;

	if (ggz_read_int(zone_fd, &op) < 0)
	{
		ZONEERROR("couldn't read op\n");
		return;
	}

	ZONEDEBUG("** motherobject (RAW) received: %i\n", op);

	switch(op)
	{
		case ZoneGGZ::seat:
			ZONEDEBUG("--> zoneGetSeats()\n");
			zoneGetSeats();
			break;
		case ZoneGGZ::players:
			ZONEDEBUG("--> zoneGetPlayers()\n");
			zoneGetPlayers();
			break;
		case ZoneGGZ::rules:
			ZONEDEBUG("--> zoneGetRules()\n");
			zoneGetRules();
			break;
		case ZoneGGZ::turn:
			ZONEDEBUG("--> zoneCallTurn()\n");
			zoneCallTurn();
			break;
		case ZoneGGZ::turnover:
			ZONEDEBUG("--> zoneCallTurnOver()\n");
			zoneCallTurnOver();
			break;
		case ZoneGGZ::over:
			ZONEDEBUG("--> zoneCallOver()\n");
			zoneCallOver();
			break;
		case ZoneGGZ::invalid:
			ZONEDEBUG("--> zoneCallInvalid()\n");
			zoneCallInvalid();
			break;
		default:
			emit signalZoneInput(op);
	}
}

// private: receive player's seat
void ZoneGGZModUI::zoneGetSeats()
{
	if(ggz_read_int(zone_fd, &ZoneSeat) < 0)
	{
		ZONEERROR("couldn't read ZoneSeat\n");
		return;
	}
	ZONEDEBUG("** received seat: %i\n", ZoneSeat);
}

// private: get all player and bot names and seat assignments
void ZoneGGZModUI::zoneGetPlayers()
{
	if (ggz_read_int(zone_fd, &ZonePlayernum) < 0)
	{
		ZONEERROR("couldn't read ZonePlayernum\n");
		return;
	}
	ZONEDEBUG("** received playernum: %i\n", ZonePlayernum);

	m_ready = 1;
	m_players = 0;
	ZoneGamePlayers = 0;
	for(int i = 0; i < ZonePlayernum; i++)
	{
		if (ggz_read_int(zone_fd, &ZoneSeats[i]) < 0)
		{
			ZONEERROR("couldn't read ZoneSeats[]\n");
			return;
		}
		ZONEDEBUG("** received ZoneSeats[i]: %i\n", ZoneSeats[i]);

		if(ZoneSeats[i] != ZONE_SEAT_OPEN)
		{
			if (ggz_read_string(zone_fd, (char*)&ZonePlayers[i], 17) < 0)
			{
				ZONEERROR("couldn't read ZonePlayers[]\n");
				return;
			}
			ZONEDEBUG("** received player: %s\n", ZonePlayers[i]);
			m_players++;
			ZoneGamePlayers++;
		}
		else m_ready = 0;
	}
}

// private: receive rules
void ZoneGGZModUI::zoneGetRules()
{
	if(ggz_read_int(zone_fd, &ZoneRules) < 0)
	{
		ZONEERROR("couldn't read ZoneRules\n");
		return;
	}
	if(ggz_read_int(zone_fd, &ZoneMaxplayers) < 0)
	{
		ZONEERROR("couldn't read ZoneMaxplayers\n");
		return;
	}
	ZONEDEBUG("** received rules: %i\n", ZoneRules);
	ZONEDEBUG("** received maxplayers: %i\n", ZoneMaxplayers);
	if(m_ready)
	{
		ZONEDEBUG("...signalZoneReady\n");
		emit signalZoneReady();
	}
	if(ZoneRules == ZoneGGZ::realtime) m_turn = 1;
}

int ZoneGGZModUI::fd()
{
	return zone_fd;
}

void ZoneGGZModUI::zoneCallTurn()
{
	m_turn = 1;
	emit signalZoneTurn();
}

void ZoneGGZModUI::zoneCallTurnOver()
{
	m_turn = 0;
	emit signalZoneTurnOver();
}

void ZoneGGZModUI::zoneCallOver()
{
	emit signalZoneOver();
}

int ZoneGGZModUI::zoneTurn()
{
	return m_turn;
}

void ZoneGGZModUI::zoneCallInvalid()
{
	emit signalZoneInvalid();
}

int ZoneGGZModUI::zoneMe()
{
	return ZoneSeat;
}

int ZoneGGZModUI::zonePlayers()
{
	return m_players;
}

void ZoneGGZModUI::handle_server(GGZMod *mod, GGZModEvent e, const void *data)
{
	self->zone_fd = *(const int *)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);

	self->zone_sn = new QSocketNotifier(self->zone_fd, QSocketNotifier::Read, self);
	self->connect(self->zone_sn, SIGNAL(activated(int)), self, SLOT(slotZoneInput()));
}

