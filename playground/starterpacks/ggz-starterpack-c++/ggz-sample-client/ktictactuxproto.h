/////////////////////////////////////////////////////////////////////
//
// KTicTacTux: Tic-Tac-Toe game for KDE 4
// http://www.ggzgamingzone.org/gameclients/ktictactux/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_PROTO_H
#define KTICTACTUX_PROTO_H

#include <qobject.h>

#include "proto.h"

// Forward declarations
namespace KGGZMod
{
	class Module;
};

// Generic TicTacToe client protocol handler
class KTicTacTuxProto
{
	public:
		// Constructor
		KTicTacTuxProto();
		// Destructor
		~KTicTacTuxProto();

		// Collection of known errors
		enum Errors
		{
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errfull = -4
		};

		// All available game states
		enum States
		{
			stateinit = 0,
			statewait = 1,
			statemove = 2,
			statedone = 3
		};

		// Possible owners of each field
		enum BoardOwners
		{
			none = 0,
			player = -5,
			opponent = -2
		};

		// A winner, if any
		qint8 winner;
		// Holds a move
		int move;

		// Both seats
		int seats[2];
		// Both names
		char *names[2];
		// Own seat number sent by game server
		int seatnum;

		// The board representation
		BoardOwners board[3][3];
		// The current game state
		States state;
		// The player who is on
		qint8 turn;

		// Statistics
		bool stats_record;
		int stats_wins;
		int stats_losses;
		int stats_ties;
		int stats_forfeits;

		// Initialize protocol
		void init();

		// Get the seats
		//int getSeat();
		// Get the player names
		//int getPlayers();
		// Reveive move status
		void handleMoveStatus(const rspmove& move);
		// Receive the network opponent's move
		void handleOpponentMove(const msgmove& move);
		// Request synchronization
		void handleSync(const sndsync& sync);
		// Get statistics
		void getStatistics();

		// Send out own move
		void sendMyMove();
		// Synchronize game
		void sendSync();

		// GGZ game module object
		KGGZMod::Module *mod;
		// GGZ auto-generated packet reader
		tictactoe *proto;

		// Seat number
		int num();
};

#endif

