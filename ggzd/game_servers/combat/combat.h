/*
 * File: combat.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Common (client/server) description of game protocol/structures
 *
 * Copyright (C) 2000 Ismael Orenstein.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#define PROTOCOL_VERSION 1

/* Combat Protocol Version 0.0.1
 *
 * g : ggz server (gserv)
 * s : combat server (cserv)
 * 1c: one player
 * nc: all human players
 *
 * ### g --> s	: REQ_GAME_LAUNCH ###
 *
 * Then the cserv inits the options to NULL (to safely check later
 * if they are set), and runs ggz_game_launch()
 *
 * ### g --> s  : REQ_GAME_JOIN ###
 *
 * The cserv does:
 *
 * ggz_player_join()
 * Sends to the joining player his seat, the number of players
 * and the protocol version
 * 		[ s --> 1c : CBT_MSG_SEAT (int)SEAT (int)PLAYERS (int)PROTOCOL_VERSION ]
 * If this is the first player to join the table, it requests the
 * game options from him.
 * 		[ s --> 1c : CBT_REQ_OPTIONS ]
 * If the options are already set, he sends the options to the player
 * 		[ s --> 1c : CBT_MSG_OPTIONS (string)OPTIONS_STRING ]
 * Then he sends the player list to all the players
 * 		[ s --> nc : CBT_MSG_PLAYERS (PLAYER_LIST) ]
 * If there are no more open seats and the options are all set, 
 * it starts the game (requests all the players to set up their units)
 * 		[ s --> nc : CBT_REQ_SETUP ]
 * If the game has already started, it sends:
 * 		[ s --> 1c : CBT_MSG_SYNC (int)TURN (str)SYNC_STR ]
 *
 * ### g --> s : REQ_GAME_LEAVE ###
 *
 * ggz_player_leave
 * Sends the player list
 * 		[ s --> nc : CBT_MSG_PLAYERS (PLAYER_LIST) ]
 * The clients always stops the game when he receives a player list,
 * and starts waiting for a CBT_MSG_START.
 *
 * ### s <-- 1c: CBT_MSG_OPTIONS (string)OPTIONS_STRING ###
 *
 * The options_string is written like this:
 * (++ is concatenation)
 *
 * map_width ++ map_height ++ map_data ++ army_data ++ \0
 *
 * map_data: The terrain data. Composed of width*height bytes like:
 * 
 * 	00000000
 * 	----
 * 	Owner data. It can be:
 * 		0000 -> No one. This land belongs to no player
 * 		0001
 * 		.... -> This land belongs to player (N-1)
 * 		1111
 * 	The player can only put its units, on the preparation time, on
 * 	a tile that belongs to him.

 * 	00000000
 * 	    ----
 * 	Terrain data. It can be:
 * 		0000 -> Open terrain
 * 		0001 -> Null terrain (for use in maps that aren't rectangles)
 * 		0010 -> Lake
 * 		0011
 * 		.... -> Other terrain types that may be created
 * 		1111
 * 		
 * 	army_data: The initial army for all the players. 12 bytes, in
 * 	this order:
 *
 * 	0 -> Number of flags
 * 	1 -> Number of bombs
 * 	2 -> Number of spys
 * 	3 -> Number of scouts
 * 	4 -> Number of miners
 * 	5 -> Number of sergeants
 * 	6 -> Number of lieutenant
 * 	7 -> Number of captain
 * 	8 -> Number of majors
 * 	9 -> Number of colonels
 * 	10 -> Number of generals
 * 	11 -> Number of marshalls
 * 	 
 * The server then check for the validity of this options. Things to
 * check:
 * 	- The players have at least 1 flag?
 * 	- The players have at least 1 moving unit?
 * 	- The map has enough starting positions for all players?
 *
 * 	If they are valid, it sends them to all the players
 * 		[ s --> nc : CBT_MSG_OPTIONS (string)OPTIONS_STRING ]
 * 	If they aren't valid, it requests them again. The client should
 * 	tell the player why he is seeing the options dialog _again_
 * 		[ s --> 1c : CBT_REQ_OPTIONS ]
 *
 * 	// Until here up, I have thought a lot... from now on, I'm not 
 *  sure if this is the right direction //
 *
 * The client should allow the player to set his units up right from
 * the moment he received the game options, but can't allow him to
 * send them until he received a CBT_REQ_SETUP
 *
 * 1c --> s : CBT_MSG_SETUP (string)ARMY_SETUP 
 * Where ARMY_SETUP is a string of len N, where N is the number
 * of tiles belonging to this player. Each byte will be like this:
 *
 * 00000000
 * ----
 *    Owner code: Just like in the OPTIONS_STRING, although it
 *    can't be NULL!
 *
 * 00000000
 *     ----
 *    Unit code (the definitions are later on this header file)
 * If this player has more tiles than the number of starting positiions,
 * then he can have some with a unit code of NULL.
 *
 * The owner code isn't really necessary for the protocol, but it
 * makes easier to just copy the values to the server variables... :) 
 *
 * The server should check if the player isn't cheating here! If the
 * setup he sent is not valid, it sends another REQ_SETUP
 * 	[ s --> 1c : CBT_REQ_SETUP ]
 * A good client should also check for the validity of the setup before
 * sending it.
 *
 * When all the players have correctly set their units up, then the
 * server starts up.
 * 	[ s --> nc : CBT_MSG_START ]
 *
 * The game always starts with player number 0. The client should keep
 * track of the current turn, to not allow anyone to send a move out
 * of his turn (And the server can't accept those!)
 *
 * ### s --> nc : CBT_MSG_START ###
 * 
 * When the client receives a CBT_MSG_START, he then can do two things:
 * if its his turn, it waits until the player moves, and then send:
 * 	[ 1c --> s : CBT_REQ_MOVE (int)FROM (int)TO ]
 * either its his turn or not, it waits to receive from the server one
 * of:
 *  [ s --> nc : CBT_MSG_MOVE (int)FROM (int)TO ]
 *  [ s --> nc : CBT_MSG_ATTACK (int)FROM (char)WHO (int)TO (char)WHO ]
 *  And then it changes the game state and changes the current turn.
 *
 * The server, after sending a CBT_MSG_START, enters playing state,
 * and waits until a CBT_REQ_MOVE. Then he does:
 *
 * ### 1c --> s : CBT_REQ_MOVE (int)FROM (int)TO ###
 *
 * First thing to check: It is the turn of this player?
 * If it is, continue.
 *
 * Then check if this move valid. The following stuff is important:
 * 	- The FROM unit belongs to this player?
 * 	- The FROM unit is a moving one?
 *  - Is the TO tile OPEN?
 * 	- The move is up/down or left/right?
 * 		- X(FROM) == X(TO)     or
 * 	- Y(FROM) == Y(TO) 
 * 	(X and Y are macro included on this header)
 *  - If the FROM unit is not a scout, then
 *    - abs(X(FROM) - X(TO)) == 1    or
 *    - abs(Y(FROM) - Y(TO)) == 1
 *  - If it is a scout, it checks for all the tiles between the
 *  FROM and the TO, if all of them are OPEN and there isn't any
 *  enemy between them.
 *
 * If the TO tile is not a enemy, it sends:
 *  [ s --> nc : CBT_MSG_MOVE (int)FROM (int)TO ]
 *	(note that the unit code isn't reported!)
 * If the TO tile is a enemy, it sends:
 *  [ s --> nc : CBT_MSG_ATTACK (int)FROM (char)WHO (int)TO (char)WHO ]
 *  (note: Maybe it should send the ATTACK msg only to the players
 *  that are taking part on the attack, and send a MSG_MOVE to the
 *  others)
 *
 *  It then checks if the game is over. Must check:
 *  	- TODO: Must see what are the winning conditions on multiplayer
 *  	stratego
 *
 *  If it is over, it sends
 *  	[ s --> nc : CBT_MSG_GAMEOVER ??? ]
 *  	TODO: Must see what needs to be sent: The winners? The losers?
 *
 *  If the game is not over, it changes to the next turn and waits
 *  until a REQ_MOVE is sent.
 *
 *  MISCELLANEOUS:
 *
 *  The player can be out of sync with the server for some network
 *  error... or maybe he has just joined a running game. If the
 *  client wants to be in sync, he sends:
 *  	[ 1c --> s : CBT_REQ_SYNC ]
 *  And the server must answer with
 *  	[ s --> 1c : CBT_MSG_SYNC (int)TURN (str)SYNC_STR ]
 *  Where SYNC_STR is the current map data, composed of:
 *
 * 	00000000
 * 	----
 * 		Owner data
 *
 * 	00000000
 * 	    ----
 * 	  Unit data. If its the unit of the requesting player, then it
 * 	  sends the correct data. Else, it sends 1111 (UNKNOWN)
 *
 * I guess this is all... at least for now...
 * TODO: Add PLAY_AGAIN feature
*/

// Terrain codes
#define T_OPEN 0x00   // 0000
#define T_NULL 0x01   // 0001
#define T_LAKE 0x02   // 0010

// Unit codes
#define U_FLAG			 0x00	// 0000
#define U_BOMB			 0x01	// 0001
#define U_SPY				 0x02	// 0010
#define U_SCOUT			 0x03	// 0011
#define U_MINER			 0x04	// 0100
#define U_SERGEANT	 0x05	// 0101
#define U_LIEUTENANT 0x06	// 0110
#define U_CAPTAIN		 0x07	// 0111
#define U_MAJOR			 0x08	// 1000
#define U_COLONEL		 0x09	// 1001
#define U_GENERAL		 0x0A	// 1010
#define U_MARSHALL	 0x0B	// 1011
#define U_EMPTY 		 0x0C // 1100 
#define U_UNKNOWN    0x0F	// 1111

// Takes a OWNER number and returns a code to be added to U/T
// Note that player 0 is stored as 0001 (as 0000 must be no player)
#define OWNER(NUM) ((NUM+1)<<4)

// Gets the owner part of the code
#define GET_OWNER(NUM) ((NUM>>4)-1)

// Get the last part of the code (U/T)
#define LAST(NUM) (NUM&15)

// Transform Cartesian(X,Y) into index number
#define CART(X,Y,WIDTH) ((Y-1)*WIDTH+(X-1))

// Game states
#define CBT_STATE_INIT 0
#define CBT_STATE_WAIT 1
#define CBT_STATE_SETUP 2
#define CBT_STATE_PLAYING 3
#define CBT_STATE_DONE 4

// Game protocol
#define CBT_MSG_SEAT 0
#define CBT_REQ_OPTIONS 2
#define CBT_MSG_OPTIONS 3


// Tile struct
typedef struct tile_struct {
	unsigned char type;
	unsigned char unit;
} tile;

// Game struct
typedef struct combat_game_struct {
	tile *map;
	char width;
	char height;
	char **army;
	int state;
} combat_game;

// Commom functions
char *combat_options_string_write(char *, combat_game *);
void combat_options_string_read(char *, combat_game *, int);
