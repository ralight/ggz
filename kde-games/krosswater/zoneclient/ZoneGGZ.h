#ifndef ZONE_GGZ_H
#define ZONE_GGZ_H

#include <ZoneProtocols.h>

#define ZONE_SEAT_OPEN -1
#define ZONE_SEAT_BOT -2

#define ZONEDEBUG(fmt...) printf("ZONE: "fmt##);
#define ZONEERROR(fmt...) printf("# ZONE ERROR: "fmt##);

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

		enum GGZProtoMisc
		{
			gamelaunch = REQ_GAME_LAUNCH,
			gamejoin = REQ_GAME_JOIN,
			gameleave = REQ_GAME_LEAVE,
			gameover = RSP_GAME_OVER
		};
};

#endif
