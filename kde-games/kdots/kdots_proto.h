#ifndef KDOTS_PROTO_H
#define KDOTS_PROTO_H

#define MAX_WIDTH 25
#define MAX_HEIGHT 25

#include <easysock.h>

class KDotsProto
{
	public:
		KDotsProto();
		~KDotsProto();

		enum Errors
		{
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errfull = -4
		};

		enum ServerMessages
		{
			msgseat = 0,
			msgplayers = 1,
			msgmoveh = 2,
			msgmovev = 3,
			msggameover = 4,
			reqmove = 5,
			rspmove = 6,
			sndsync = 7,
			msgoptions = 8,
			reqoptions = 9
		};

		enum ClientMessages
		{
			sndmoveh = 0,
			sndmovev = 1,
			reqsync = 2,
			sndoptions = 3,
			reqnewgame = 4
		};

		enum States
		{
			stateinit = 0,
			statewait = 1,
			statemove = 2,
			statedone = 3,
			stateopponent = 4,
			statechoose = 5
		};

		enum Events
		{
			eventlaunch = 0,
			eventjoin = 1,
			eventleave = 2,
			eventmoveh = 3,
			eventmovev = 4
		};

		/*enum Directions
		{
			horizontal = 0,
			vertical = 1
		};*/

		void connect();
		void getPlayers();
		void getSeat();
		void sendMove(int x, int y, int direction);
		void sendOptions(int wantwidth, int wantheight);
		void getOptions();
		void getMove();
		void getOppMove(int direction);
		
		int state;
		int fd;
		int num;
		char players[2][32];
		char width, height;
		int turn;
		int movex, movey;
		int m_lastx, m_lasty;
		int m_lastdir;
};

#endif

