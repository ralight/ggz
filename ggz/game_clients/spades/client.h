/*
 * File: client.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
 *
 * This rather empty file declares the client function which actually
 * runs most of the client side of the spades game
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef _CLIENT_H
#define _CLIENT_H

#include <card.h>

#define ST_REGISTER    0
#define ST_GET_ID      1
#define ST_GET_GAME    2
#define ST_GET_NAMES   3
#define ST_REG_TAUNT   4
#define ST_GET_HAND    5
#define ST_GET_BIDDER  6
#define ST_GET_BIDS    7
#define ST_GET_LEAD    8
#define ST_GET_TRICK   9
#define ST_GET_TALLY   10
#define ST_GET_SCORE   11
#define ST_END_GAME    12

#define BID_BLANK  -2
#define BID_KNEEL  -1

#define NET_FAIL      -3
#define NET_OK         0

#define GAME_OVER  1
#define GAME_QUERY 2

typedef struct gameState_t {

	char get_opt;

	int gameSegment;
	int saveSegment;

	char *serverName;
	int spadesSock;
	int tauntSock;
	int serverPort;
	int tauntPort;

	int gameNum;
	int gamePid;
	int playerId;

	char *userName;
	char *password;
	int login_type;
	char *players[4];

	Card hand[13];
	int curPlayer;
	int curTrick;
	int lead;
	int bids[4];
	int tallys[4];
	int scores[2];
	int *record;
	int gameCount;
	int gameOver;
	int sessionOver;

} gameState_t;


/* Socket error checking functions */
int CheckReadInt(int, int *);
int CheckReadString(int, char **);
int CheckWriteInt(int, int);
int CheckWriteString(int, const char *);

/* Various init routines */
void AppInit(void);
void SessionInit(void);
void GameInit(void);
int ParseOptions(int, char **);

/* Registering with servers*/
int Register(void);
int RegisterTaunt(void);

/* Get data from server */
int GetPlayerId(void);
int GetGame(void);
int GetPlayers(void);
int GetHand(void);
int GetLead(void);
int GetBid(void);
Card GetPlayedCard(void);
int GetTallys(void);
int GetScores(void);
int GetNewGame(void);

/* Send data to server */
int SendMyBid(void);
int SendCard(int);
int SendTaunt(char *);
int SendQuery(int);

/* Handle game state variable*/
void UpdateGame(void);

/* Network cleanup function */
void NetClose(void);

/* Signal handler */
void die(int);
void DeadEngine(int);

#endif
