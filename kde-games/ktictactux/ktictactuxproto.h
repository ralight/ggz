//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_PROTO_H
#define KTICTACTUX_PROTO_H

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
		// Holds a move
		int move;

		// Both seats
		int seats[2];
		// Both names
		char names[2][17];
		// Own seat number sent by game server
		int seatnum;

		// The board representation
		BoardOwners board[3][3];
		// The current game state
		States state;
		// The player who is on
		char turn;

		// Statistics
		bool stats_record;
		int stats_wins;
		int stats_losses;
		int stats_ties;
		int stats_forfeits;

		// Initialize protocol
		void init();

		// Read opcode
		int getOp();
		// Read packet size, if any
		int getPacksize();
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

		// Send packet size, if any
		int sendPacksize(int packsize);
		// Send decision on a new game
		int sendOptions();
		// Send out own move
		int sendMyMove();
		// Synchronize game
		void sendSync();

		// GGZ game module object
		KGGZMod::Module *mod;
		// Socket to game server
		int fd;

		// Seat number
		int num();
};

#endif

