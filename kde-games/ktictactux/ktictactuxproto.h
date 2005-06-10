//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_PROTO_H
#define KTICTACTUX_PROTO_H

#include <ggzmod.h>

// Forward declarations
class KTicTacTux;

// Generic TicTacToe client protocol handler
class KTicTacTuxProto
{
	public:
		// Constructor
		KTicTacTuxProto(KTicTacTux *game);
		// Destructor
		~KTicTacTuxProto();

		// Opcodes which come from the server
		enum ServerMessages
		{
			msgseat = 0,
			msgplayers = 1,
			msgmove = 2,
			msggameover = 3,
			reqmove = 4,
			rspmove = 5,
			sndsync = 6,
			sndstats = 7
		};

		// Opcodes to be sent to the server
		enum ClientMessages
		{
			sndmove = 0,
			reqsync = 1,
			reqstats = 2
		};

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
		char winner;

		// Game socket
		int fd;
		// Game control socket
		int fdcontrol;
		// Seat number
		int num;
		// Both seats
		int seats[2];
		// Both names
		char names[2][17];

		// The board representation
		BoardOwners board[3][3];
		// The current game state
		States state;
		// Holds a move
		int move;
		// The player who is on
		char turn;
		// Statistics
		int stats[2];

		// Connect to the socket
		void connect();
		// Initialize protocol
		void init();
		// Shutdown game
		void shutdown();

		// Read opcode
		int getOp();
		// Get the seats
		int getSeat();
		// Get the player names
		int getPlayers();
		// Reveive move status
		int getMoveStatus();
		// Receive the network opponent's move
		int getOpponentMove();
		// Request synchronization
		int getSync();
		// Ask whether the game is over
		int getGameOver();
		// Get statistics
		void getStatistics();

		// Send decision on a new game
		int sendOptions();
		// Send out own move
		int sendMyMove();
		// Synchronize game
		void sendSync();
		// Fetch statistics
		void sendStatistics();

		// GGZMod object
		GGZMod *mod;
		// Game object
		KTicTacTux *gameobject;
		// Self object
		static KTicTacTuxProto *self;

		// Callback for control channel
		static void handle_server(GGZMod *mod, GGZModEvent e,
					  const void *data);
		// Dispatch loop
		void dispatch();
};

#endif

