/////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
/////////////////////////////////////////////////////////////

#ifndef KCC_PROTO_H
#define KCC_PROTO_H

#include <ggzmod.h>

// Forward declarations
class KCC;

// Generic KCC client protocol handler
class KCCProto
{
	public:
		// Constructor
		KCCProto(KCC *game);
		// Destructor
		~KCCProto();

		// Opcodes which come from the server
		enum ServerMessages
		{
			cc_msg_seat = 0,
			cc_msg_players = 1,
			cc_msg_move = 2,
			cc_msg_gameover = 3,
			cc_req_move = 10,
			cc_rsp_move = 20,
			cc_msg_sync = 4
		};

		// Opcodes to be sent to the server
		enum ClientMessages
		{
			cc_snd_move = 0,
			cc_req_sync = 10,
			cc_req_newgame = 11
		};

		// Collection of known errors
		enum Errors
		{
			errnone = 0,
			errstate = -1,
			errother = -99
		};

		// All available game states
		enum States
		{
			statenone = 0,
			stateinit = 2,
			statewait = 3,
			statemove = 4,
			statedone = 5
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
		// Total number of seats
		int max;
		// All seats
		int seats[6];
		// All names
		char names[6][17];

		// The board representation
		char board[17][17];
		// The current game state
		char state;
		// Holds a move
		int move;
		// The player who is on
		char turn;
		// Statistics
		int stats[2];
		// Game status
		char status;

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

		// Send out own move
		int sendMyMove(int x, int y, int x2, int y2);
		// Synchronize game
		void sendSync();
		// Fetch statistics
		void sendStatistics();

		// GGZMod object
		GGZMod *mod;
		// Game object
		KCC *gameobject;
		// Self object
		static KCCProto *self;

		// Callback for control channel
		static void handle_server(GGZMod *mod, GGZModEvent e, void *data);
		// Dispatch loop
		void dispatch();
};

#endif

