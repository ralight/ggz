/*
 * File: engine.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
 *
 * This rather empty file declares the engine function which actually
 * runs most of the server side of the spades game
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


#ifndef _ENGINE_H
#define _ENGINE_H

#include <config.h>

#include <card.h>
#include <options.h>

#define MAX_GAME 5
#define MIN_BID 3
#define END_GAME 500
#define GAME_QUERY 2

#define SOCK_INVALID -1
#define SOCK_COMP    -2

#define PID_INVALID  -1
#define PID_OPTION   -2

#define BID_INVALID  -2
#define BID_NIL  -1

typedef struct gameInfo_t {
  int gameNum;
  int gamePid;
  int num_humans;
  int clientPids[4];
  char* players[4];
  int playerSock[4];
  int ggz_sock;
  option_t opt;
} gameInfo_t;


int AIBid( int );
int AIPlay( int, int);

void engine( void );

void ReadIntOrDie( int, int* );

void WriteIntOrDie( int, int );

int ggz_init(void);

void ReadOptions(void);

void svNetClose( void );

void Randomize( void );

void DeckInit( Card[52] );

void DeckShuffle( Card[52] );

void DeckDeal( Card[52], Card(*)[13] );

void SortHands( Card(*)[13] );

void GetGameInfo( void );

void SendHands( Card(*)[13] );

void SendLead( int );

void GetAndSendBids( int, int[4], int[4], Card(*)[13] );

void SendBid( int, int );

void GetAndSendTricks( int, Card(*)[13], Card[4], int[4], int[4], int[4] );

void PlayCard(int, int, Card(*)[13], Card[4], int, int);

void SendTrick( Card, int );

int CalcTrickWin( int, Card[4] );

void InitTallys( int[4] );

void UpdateTallys( int, int[4] );

void SendTallys( int, int[4] );

void InitScores( int[4], int[2] );

int UpdateScores( int[4], int[4], int[2], int[4], int[2] );

void SendScores( int[2] );

int QueryNewGame( void );

void SendNewGame(int);

void SendGameOver(void);

RETSIGTYPE termination_handler( int );

void Quit( int );

#endif
