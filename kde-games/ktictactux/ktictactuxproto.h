#ifndef KTICTACTUX_PROTO_H
#define KTICTACTUX_PROTO_H

#include <easysock.h>

class KTicTacTuxProto
{
	public:
		KTicTacTuxProto();
		~KTicTacTuxProto();

		enum ServerMessages
		{
			msgseat = 0,
			msgplayers = 1,
			msgmove = 2,
			msggameover = 3,
			reqmove = 4,
			rspmove = 5,
			sndsync = 6
		};

		enum ClientMessages
		{
			sndmove = 0,
			reqsync = 1
		};

		enum Errors
		{
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errfull = -4
		};

		enum States
		{
			stateinit = 0,
			statewait = 1,
			statemove = 2,
			statedone = 3
		};

		enum BoardOwners
		{
			none = 0,
			player = -5,
			opponent = -2
		};

		char winner;

		int fd;
		int num;
		int seats[2];
		char names[2][17];

		char board[3][3];
		char state;
		int move;
		char move_count;

		void connect();
		void init();

		int getSeat();
		int getPlayers();
		int getMoveStatus();
		int getOpponentMove();
		int getSync();
		int getGameOver();

		int sendOptions();
		int sendMyMove();
};

#endif
