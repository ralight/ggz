/*
 * File: engine_main.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/30/97
 *
 * This file contains the spades playing engine to be used on the
 * server side.  This was written in a modular style for ease of
 * editing and to open the possibility for different implementation of
 * the varios functions
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


#include <unistd.h>   /* For sleep */
#include <signal.h>
#include <stdio.h>

#include <card.h>
#include <engine.h>
#include <err_func.h>


/* Global data */
int      bids[4];                     /* array of bids */
int      kneels[4];                   /* who is kneeling */
int       tally[4];                    /* Trick tally */

Card hands[4][13], trick[4];

int main( int argc, char* argv[] ) {

  int    dealer=0, lead,              /* index of dealer and leader */
         trickNum, trickWin,          /* trick counter and who won */
         bags[2],                     /* Bag tally */
         scores[2],                   /* Scores */
         gameOver=0,
         sessionOver=0;

  Card deck[52];

  signal (SIGINT, termination_handler);
  signal (SIGPIPE, termination_handler);

  GetGameInfo();
  log_msg("Spades game running");
  
  while( !sessionOver ) {
    Randomize();
    InitScores( scores, bags );
    gameOver = 0;
    while (!gameOver) {
      
      DeckInit( deck );
      DeckShuffle( deck );
      DeckDeal( deck, hands );
      SortHands( hands );
      SendHands( hands );

      lead=(dealer+1)%4;
      SendLead( lead );
      GetAndSendBids( lead, bids, kneels, hands );

      InitTallys( tally );
      for( trickNum=0; trickNum<13; trickNum++ ) {
	SendLead( lead );
	GetAndSendTricks( lead, hands, trick, tally, bids, kneels );
	trickWin=CalcTrickWin( lead, trick );
	UpdateTallys( trickWin, tally );
	SendTallys( trickWin, tally );
	lead=trickWin;
      }
      
      gameOver=UpdateScores( tally, bids, scores, kneels, bags );
      SendScores( scores );
      dealer++;
      /*gameOver++; */
    
    } /* while (!gameOver) */

    if ( (sessionOver=!(QueryNewGame())))
	    SendGameOver();
    
    SendNewGame(!sessionOver);
  
  } /* while (!sessionOver) */

  svNetClose();

  return 0;
}
