#ifndef KTICTACTUX_PROTO_H
#define KTICTACTUX_PROTO_H

// GGZ includes
#include <easysock.h>

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
			sndsync = 6
		};

		// Opcodes to be sent to the server
		enum ClientMessages
		{
			sndmove = 0,
			reqsync = 1
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
		// Seat number
		int num;
		// Both seats
		int seats[2];
		// Both names
		char names[2][17];

		// The board representation
		char board[3][3];
		// The current game state
		char state;
		// Holds a move
		int move;
		// The player who is on
		char turn;

		// Connect to the socket
		void connect();
		// Initialize protocol
		void init();

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

		// Send decision on a new game
		int sendOptions();
		// Send out own move
		int sendMyMove();
};

#endif

