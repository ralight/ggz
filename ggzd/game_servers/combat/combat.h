/*
 * File: combat.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Common (client/server) description of game protocol/structures
 * $Id: combat.h 6899 2005-01-25 08:59:25Z jdorje $
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

#include "ggzdmod.h"

#define PROTOCOL_VERSION 10

/* Combat Protocol Version 1.0
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
 * Then he sends the player list to all the players
 * 		[ s --> nc : CBT_MSG_PLAYERS (PLAYER_LIST) ]
 * If the options are already set, he sends the options to the player
 * 		[ s --> 1c : CBT_MSG_OPTIONS (string)OPTIONS_STRING ]
 * It now depends on the current state:
 *  CBT_STATE_WAIT:
 *      If this is the first player to join the table, it requests the
 *      game options from him.
 * 		  [ s --> 1c : CBT_REQ_OPTIONS ]
 *      If there are no more open seats and the options are all set, 
 *      it starts the game (requests all the players to set up their units)
 * 		  [ s --> nc : CBT_REQ_SETUP ]
 *  CBT_STATE_SETUP:
 *      It tells the player that we are at SETUP state
 *      [ s --> 1c : CBT_REQ_SETUP ]
 *  CBT_STATE_PLAYING:
 *      It tells the player what are the units
 *  		[ s --> 1c : CBT_MSG_SYNC (int)TURN (str)SYNC_STR ]
 *  		Also tells the player that we are already playing
 *  		[ s --> 1c : CBT_MSG_START ]
 *  CBT_STATE_DONE:
 *      Tells the player what are the units
 *  		[ s --> 1c : CBT_MSG_SYNC (int)TURN (str)SYNC_STR ]
 *  		Also tells the player who won
 *    	[ s --> nc : CBT_MSG_GAMEOVER (int)WINNER ]
 *
 * ### g --> s : REQ_GAME_LEAVE ###
 *
 * ggz_player_leave
 * Sends the player list
 * 		[ s --> nc : CBT_MSG_PLAYERS (PLAYER_LIST) ]
 * The client should always stop the game if it's already playing and he
 * receives a player list
 * Now it depends on the current state
 * CBT_STATE_WAIT:
 *  If this player is the host, discard him as host.
 *  Make the next player as host
 *  If he isn't the host, then just wait until all the players join
 * CBT_STATE_SETUP:
 *  If this player has already setup his units, discard them.
 *  Else, just wait
 * CBT_STATE_PLAYING:
 *  Wait for another player
 *
 * ### s <-- 1c: CBT_MSG_OPTIONS (string)OPTIONS_STRING ###
 *
 * The options_string is written like this:
 * (++ is concatenation)
 *
 * map_width ++ map_height ++ map_data ++ army_data ++ game_options ++ \0
 *
 * To each value in the string is then added 1 (except to the closing \0), so
 * that the result looks like a string, and then sent using es_write_string
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
 * 	game_options: To avoid problems between wrong versions of the protocol,
 * 	all the game options are enclosed in packets, like this:
 *
 * 	(byte)OPTION_IDENTIFER ++ OPTION_DATA ++ \0
 *
 * 	The client then receives each packet and reads the OPTION_IDENTIFIER. If it
 * 	is a know one, then it reads the OPTION_DATA and does what is appropriated.
 * 	If it isn't, then it should tell the user (probably with a msgbox), and try
 * 	to run the game (for some options it should be able to run without problem)
 *
 * OPTIONS_IDENTIFIERS:
 *
 *  01h -> Map Name
 *
 *    It's the name of this map, so that the client may know which is the
 *    default name when saving it. THe OPTION_DATA is a string (*including* a
 *    closing \0) with the name of the map
 *
 *  02h -> Binary Options Pack 1
 *  03h -> Binary Options Pack 2
 *
 *    Each one is a pack of 8 binary options. The OPTION_DATA of each is one
 *    byte (8 bits). Each one of options has an id. The options ids are ||ed in
 *    the OPTION_DATA. The options are (in order):
 *
 *      - OPT_OPEN_MAP                \
 *      - OPT_ONE_TIME_BOMB           |
 *      - OPT_TERRORIST_SPY           |\
 *      - OPT_MOVING_BOMB             | > Pack 1
 *      - OPT_SUPER_SCOUT             |/
 *      - OPT_MOVING_FLAG             |
 *      - OPT_RANDOM_OUTCOME          |
 *      - OPT_ALLOW_DIAGONAL          /
 *
 *      - OPT_UNKNOWN_VICTOR          \
 *      - OPT_SILENT_DEFENSE          |
 *      - OPT_SILENT_OFFENSE          |\
 *      - OPT_RANDOM_SETUP            | > Pack 2
 *      - OPT_SF_SERGEANT             |/
 *      - OPT_RUSH_ATTACK             |
 *      - OPT_HIDE_UNIT_LIST          |
 *      - OPT_SHOW_ENEMY_UNITS        /
 *
 *   After having their ids ||ed, the options are negated, so that it can never
 *   result in a value of 255 as option (that would result in a value of 0 to
 *   the the final string, and it would cause an error when sending it)
 *   
 *   Example: 
 *     If I have set OPT_SUPER_SCOUT (1<<4), OPT_RANDOM_SETUP (1<11), 
 *     OPT_RUSH_ATTACK (1<<13) and OPT_ALLOW_DIAGONAL (1<<7)
 *    
 *     In the first pack we have OPT_SUPER_SCOUT and OPT_ALLOW_DIAGONAL
 *
 *     10010000
 *     |  |
 *     |  -> OPT_SUPER_SCOUT
 *     -> OPT_ALLOW_DIAGONAL
 *
 *     Then we negate the byte, and the final result is
 *
 *     01101111
 *
 *     For the second pack we have
 *
 *     00101000
 *       | |
 *       | -> OPT_RANDOM_SETUP
 *       -> OPT_RUSH_ATTACK
 *
 *     Negating the byte we have
 *
 *     11010111
 *
 *     Then we send: 2 111 0 3 215 0
 *                   |  |  | |  |  |
 *                   |  |  | |  |  -> closing \0 for the second pack
 *                   |  |  | |  -> second pack data
 *                   |  |  | -> O_BIN2
 *                   |  |  -> closing \0 for the first pack
 *                   |  -> first pack data
 *                   -> O_BIN1
 *
 *     Just remember that, in fact, we send 3 112 1 4 216 1, as we are sending a
 *     *string*, so it can't have any \0 value (so everything is added 1)
 *
 * If that player doesn't want to be the host, he can send a NULL option
 * string, wich will make the server ask the next player to be the host
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
 * then he can have some with a unit code of EMPTY.
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
 *  [ s --> nc : CBT_MSG_ATTACK (int)FROM (int)WHO (int)TO (int)WHO ]
 *  Where the WHO can be negative, which means that they won the combat
 *  The OWNER code is _NOT_ enconded in the WHO
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
 *  Where the WHO can be negative, which means that they won the combat
 *  The OWNER code is _NOT_ enconded in the WHO
 *  (note: Maybe it should send the ATTACK msg only to the players
 *  that are taking part on the attack, and send a MSG_MOVE to the
 *  others)
 *  If we have set some option that hides the results of the attack (like
 *  OPT_UNKNOWN_VICTOR), then we send the U_UNKNOWN code as the attacking
 *  unit to the players that do not own that unit
 *
 *  It then checks if the game is over. Must check:
 *  	- TODO: Must see what are the winning conditions on multiplayer
 *  	stratego
 *
 *  If it is over, it sends
 *  	[ s --> nc : CBT_MSG_GAMEOVER (int)WINNER ]
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
 *  	[ s --> 1c : CBT_MSG_SYNC (str)SYNC_STR ]
 *  Where SYNC_STR is the current map data, composed of:
 *
 * (char)CURRENT_TURN +
 * (char)CURRENT_STATE +
 * (str)MAP_DATA:
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
 * The client may have forgotten the options for some weird reason. If he
 * wants to get them again, he can just send:
 * [ 1c --> s : CBT_REQ_OPTIONS ]
 *
 * I guess this is all... at least for now...
 * TODO: Add PLAY_AGAIN feature
 * TODO: More options are always good
 * TODO: Multiplayer considerations
 */

// Terrain codes
#define T_OPEN 0x00	// 0000
#define T_NULL 0x01	// 0001
#define T_LAKE 0x02	// 0010

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
#define U_UNKNOWN    0x0C	// 1100
#define U_EMPTY      0x0F	// 1111

// Options codes
#define O_NAME       0x01	// Name of the map
#define O_BIN1       0x02	// Binary options pack 1
#define O_BIN2       0x03	// Binary options pack 2

// Binary options code
#define OPT_OPEN_MAP         (1<<0)
#define OPT_ONE_TIME_BOMB    (1<<1)
#define OPT_TERRORIST_SPY    (1<<2)
#define OPT_MOVING_BOMB      (1<<3)
#define OPT_SUPER_SCOUT      (1<<4)
#define OPT_MOVING_FLAG      (1<<5)
#define OPT_RANDOM_OUTCOME   (1<<6)
#define OPT_ALLOW_DIAGONAL   (1<<7)
#define OPT_UNKNOWN_VICTOR   (1<<8)
#define OPT_SILENT_DEFENSE   (1<<9)
#define OPT_SILENT_OFFENSE   (1<<10)
#define OPT_RANDOM_SETUP     (1<<11)
#define OPT_SF_SERGEANT      (1<<12)
#define OPT_RUSH_ATTACK      (1<<13)
#define OPT_HIDE_UNIT_LIST   (1<<14)
#define OPT_SHOW_ENEMY_UNITS (1<<15)

// Takes a OWNER number and returns a code to be added to U/T
// Note that player 0 is stored as 0001 (as 0000 must be no player)
#define OWNER(NUM) ((NUM+1)<<4)

// Gets the owner part of the code
#define GET_OWNER(NUM) ((NUM>>4)-1)

// Get the last part of the code (U/T)
#define LAST(NUM) (NUM&15)

// Get the first part of the code (OWNER CODE)
#define FIRST(NUM) (NUM&240)

// Transform Cartesian(X,Y) into index number
#define CART(X,Y,WIDTH) ((Y-1)*WIDTH+(X-1))

// Transform index number into cartesian
#define X(I, WIDTH) (I%WIDTH)
#define Y(I, WIDTH) (I/WIDTH)

// Get a cbt_game and returns the default number of army
#define ARMY(OPTIONS, TYPE) (OPTIONS->army[OPTIONS->number][TYPE])

// Next turn
#define NEXT(X,Y) (X >= (Y-1) ? 0 : X+1)

// Game states
#define CBT_STATE_NULL -1	// When he doesn't know his state
#define CBT_STATE_INIT 0
#define CBT_STATE_WAIT 1
#define CBT_STATE_SETUP 2
#define CBT_STATE_PLAYING 3
#define CBT_STATE_DONE 4

// Game protocol
#define CBT_MSG_SEAT 0
#define CBT_MSG_PLAYERS 1
#define CBT_REQ_OPTIONS 2
#define CBT_MSG_OPTIONS 3
#define CBT_REQ_SETUP 4
#define CBT_MSG_SETUP 5
#define CBT_MSG_START 6
#define CBT_REQ_MOVE 7
#define CBT_MSG_MOVE 8
#define CBT_MSG_ATTACK 9
#define CBT_MSG_GAMEOVER 10
#define CBT_REQ_SYNC 11
#define CBT_MSG_SYNC 12

/* CHECK MOVE RETURN VALUES */

// Error messages
#define CBT_ERROR_SOCKET -1
#define CBT_ERROR_OUTRANGE -2
#define CBT_ERROR_WRONGTURN -3
#define CBT_ERROR_SUICIDAL -4
#define CBT_ERROR_NOTOPEN -5
#define CBT_ERROR_CRAZY -6
#define CBT_ERROR_NOTMOVING -7

// Invalid move errors
#define CBT_ERROR_MOVE_NOMOVE -8
#define CBT_ERROR_MOVE_DIAGONAL -9
#define CBT_ERROR_MOVE_BIGMOVE -10
#define CBT_ERROR_MOVE_SCOUT -11

// Invalid options
#define CBT_ERROR_OPTIONS_FLAGS -12
#define CBT_ERROR_OPTIONS_MOVING -13
#define CBT_ERROR_OPTIONS_SIZE -14

// Ok messages
#define CBT_CHECK_MOVE 1
#define CBT_CHECK_ATTACK 2
#define CBT_CHECK_ATTACK_RUSH 3

// Random number
#define RANDOM(N) (1+(int)((float)N*random()/(RAND_MAX+1.0)))





// Tile struct
typedef struct tile_struct {
	unsigned char type;
	unsigned char unit;
} tile;

// Game struct
typedef struct combat_game_struct {
	GGZdMod *ggz;
	tile *map;
	char width;
	char height;
	char **army;
	char *name;
	int state;
	int turn;
	int number;
	unsigned long int options;
} combat_game;

// Commom functions
unsigned char *combat_options_string_write(combat_game *, int for_hash);
int combat_options_string_read(unsigned char *optstr, combat_game *);
int combat_check_move(combat_game *, int, int);
int combat_options_check(combat_game *);
char *combat_options_describe(combat_game *, int short_desc);
void combat_random_setup(combat_game *, int player);
