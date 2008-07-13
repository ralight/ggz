// ZoneServer - small C++ game support library
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

#ifndef ZONE_GGZ_H
#define ZONE_GGZ_H

// System includes
#include <stdio.h>

// GGZ includes
#include <ggz_common.h>

#define ZONE_SEAT_OPEN GGZ_SEAT_OPEN
#define ZONE_SEAT_BOT GGZ_SEAT_BOT
#define ZONE_SEAT_PLAYER GGZ_SEAT_PLAYER

#define ZONEDEBUG(fmt, args...) printf("ZONE: " fmt, ##args);
#define ZONEERROR(fmt, args...) printf("# ZONE ERROR: " fmt, ##args);
//#define ZONEDEBUG
//#define ZONEERROR

class ZoneGGZ
{
	public:
		enum ZoneGameModes
		{
			unitialized = 0,
			realtime = 1,
			turnbased = 2
		};

		enum GGZServerEvents
		{
			launch = 0,
			join = 1,
			leave = 2
		};

		enum GGZServerStates
		{
			preinit = -1,
			init = 0,
			wait = 1,
			playing = 2,
			done = 3
		};

		enum GGZProtoRequests
		{
			initreq = 0,
			syncreq = 1
		};

		enum GGZProtoErrors
		{
			error = -1
		};

		enum GGZProtoMessages
		{
			seat = 0,
			players = 1,
			rules = 2,
			turn = 3,
			turnover = 4,
			over = 5,
			invalid = 6
		};

		enum GGZProtoSends
		{
			sync = 2
		};

		/*enum GGZProtoMisc
		{
			gamelaunch = REQ_GAME_LAUNCH,
			gamejoin = REQ_GAME_JOIN,
			gameleave = REQ_GAME_LEAVE,
			gameover = RSP_GAME_OVER
		};*/
};

#endif

