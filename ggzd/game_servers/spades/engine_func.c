/*
 * File: engine_func.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/30/97
 *
 * This file contains the support functions for the spades engines.
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


#include <config.h>         /* Site specific config */

#include <stdio.h>          /* For fprintf */
#include <stdlib.h>         /* For exit */
#include <time.h>           /* Used for randomize */
#include <unistd.h>         /* For read, write, etc */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#include <card.h>
#include <engine.h>
#include <socketfunc.h>
#include <protocols.h>
#include <err_func.h>

/* Global game information variable */
gameInfo_t gameInfo;

/* log output flag*/
int log = 1;

/* Globals for card counting */
int played[4];
int suits[4][4];

inline void ReadIntOrDie( int msgsock, int* message ) {
  
  int status;
  
  status = readint( msgsock, message );
  
  if( status < 0 ) {
    dbg_msg( "[%d]: Network error receiving int\n", getpid() );
    svNetClose();
    Quit(-1);
  }
  else if( status == 0 ) {
    dbg_msg( "[%d]: Error: socket closed\n", getpid() );
    svNetClose();
    Quit(-1);
  }
}


inline void WriteIntOrDie( int msgsock, int message ) {
  
  int status;
  
  status = writeint( msgsock, message );
  
  if( status <= 0 ) {
    dbg_msg( "[%d]: Network error sending int\n", getpid() );
    svNetClose();
    Quit(-1);
  }
}


int num_comp_play(unsigned char mask) {

  int i, count = 0;

  for (i=0; i<8; i++) {
    count += (mask/128);
    mask = (mask << 1);
  }

  return count;
}


void svNetClose( void ) {
  int i;
  for( i=0; i<4; i++) {
    if( gameInfo.playerSock[i] != SOCK_COMP ) 
	close( gameInfo.playerSock[i] );
  }
}


void Randomize( void ) { 
  srand((unsigned)time((time_t *)0)); 
}


void DeckInit( Card deck[52] ) {
  int i;
  
  for( i=0; i<52; i++)
      deck[i] = i;
}


void DeckShuffle( Card deck[52] ) {

  int i,j,least,temp;
  int index[52];
  Card tmp_card;
  
  for( i=0; i<52; i++ )
    index[i]=rand();

  /*Sort random numbers and order cards as well */

  for (i=0; i<52; i++) {
    least=i;
    for(j=i+1; j<52; j++) {
      if ( index[j] < index[least] )
	least=j;
    }
    if ( least != i ) {
      temp=index[i];
      index[i]=index[least];
      index[least]=temp;
      tmp_card=deck[i];
      deck[i]=deck[least];
      deck[least]=tmp_card;
    }
  }
}


void DeckDeal( Card deck[52], Card hands[4][13] ) {

  int i,j;

  for( i=0; i<13; i++) 
    for( j=0; j<4; j++) 
      hands[j][i]=deck[i*4+j];
}  
  

void SortHands( Card hands[4][13] ) {

  int i,j,k,least;
  Card tmp;

  for( k=0; k<4; k++) {
    for (i=0; i<13; i++) {
      least=i;
      for(j=i+1; j<13; j++) {
	if ( hands[k][j] < hands[k][least] )
	    least=j;
      }
      if ( least != i ) {
	tmp=hands[k][i];
	hands[k][i]=hands[k][least];
	hands[k][least]=tmp;
      }
    }
  }
}


void GetGameInfo( void ) {
  
  int i, j, op, status = 0;
  char ret;
  
  ReadIntOrDie(STDIN_FILENO, &op);
  if (op != REQ_GAME_LAUNCH) {
    ret = -1;
    WriteIntOrDie(STDOUT_FILENO, RSP_GAME_LAUNCH);
    write(STDOUT_FILENO, &ret, 1);
    exit(-1);
  }
  
  ReadOptions();

  /* Should check for validity here , but who cares? */
  WriteIntOrDie(STDOUT_FILENO, RSP_GAME_LAUNCH);
  status = 0;
  write(1, &status, 1);
  
  gameInfo.gameNum = 0;
  gameInfo.gamePid = getpid();
  

  for (i=0; i<4; i++) {
	  if( gameInfo.playerSock[i] == SOCK_COMP ) {
		  continue;
	  }
	  dbg_msg("[%d]: Sending info to %s\n", getpid(), gameInfo.players[i]);
	  WriteIntOrDie( gameInfo.playerSock[i], gameInfo.gameNum );
	  WriteIntOrDie( gameInfo.playerSock[i], i );
	  WriteIntOrDie( gameInfo.playerSock[i], gameInfo.gamePid );
	  for(j=0; j<4; j++) {
		  if (writestring(gameInfo.playerSock[i], gameInfo.players[j]) < 0 ) {
			  dbg_msg( "[%d]: Network error sending string\n", getpid() );
			  svNetClose();
			  Quit(-1);
		  }
	  }

    ReadIntOrDie( gameInfo.playerSock[i], &status );
    
    if( status == i ) {
      if( log )
	  dbg_msg( "[%d]: %s received player list\n", getpid(), 
		 gameInfo.players[i] );
    }
    else {
      svNetClose();
      if( log )
	dbg_msg( "[%d]: Error: player %d returned %d\n", getpid(), i, 
		 status);
      Quit(-1);
    }
  } /* for i=1 to 4*/
}


int ReadOptions( void ) {
  
	int assign, i, seats, status = -1;
  
	dbg_msg("[%d]: Reading options from server\n", getpid());

	if ( (status = read(STDIN_FILENO, &gameInfo.opt, 12)) < 0) 
		dbg_msg( "[%d]: Error reading options\n", getpid() );
	else if (readint(STDIN_FILENO, &seats) < 0) 
		dbg_msg( "[%d]: Error reading number of slots", getpid() );
	else {
		for(i=0; i<4; i++) {
			readint( STDIN_FILENO, &assign);
			if (assign == -2) {
				gameInfo.playerSock[i] = SOCK_COMP;
				gameInfo.players[i] = "AI";
			} else {
				readstring(STDIN_FILENO, &gameInfo.players[i]);
				ReadIntOrDie(STDIN_FILENO,
					     &gameInfo.playerSock[i] );

			}
			dbg_msg( "[%d]: Player %d has fd %d and name %s\n",
				 getpid(), i, gameInfo.playerSock[i],
				 gameInfo.players[i]);

			gameInfo.clientPids[i] = 0;
		}

		status = 0;

#ifdef DEBUG
		dbg_msg( "[%d]: bitOpt: %d, endGame: %d, minBid: %d\n", 
			 getpid(), gameInfo.opt.bitOpt, gameInfo.opt.endGame, 
			 gameInfo.opt.minBid);
#endif
		if (log) {
			switch (gameInfo.opt.bitOpt & MSK_GAME) {
			case GAME_SPADES: 
				dbg_msg("[%d]: Game choice is spades\n",
					getpid());
				dbg_msg("[%d]: Game ends at %d points\n",
					getpid(), gameInfo.opt.endGame);
				dbg_msg("[%d]: Minimum bid of %d\n",
					getpid(), gameInfo.opt.minBid);
				if (gameInfo.opt.bitOpt & MSK_NILS)
					dbg_msg("[%d]: Nil bids allowed\n",
						getpid() );
				else
					dbg_msg("[%d]: Nil bids prohibited\n",
						getpid());
			
				if (gameInfo.opt.bitOpt & MSK_BAGS)
					dbg_msg("[%d]: Penalty imposed for 10 bags\n", getpid() );
				else
					dbg_msg("[%d]: No penalty for bags\n",
						getpid() );
				break;
			case GAME_HEARTS: 
				dbg_msg("[%d]: Game choice is hearts\n",
					getpid());
				break;
			}
			if (gameInfo.playerSock[1] == SOCK_COMP)
				dbg_msg("[%d]: Player 1 computer player\n",
					getpid());
			if (gameInfo.playerSock[2] == SOCK_COMP)
				dbg_msg("[%d]: Player 2 computer player\n",
					getpid());
			if (gameInfo.playerSock[3] == SOCK_COMP)
				dbg_msg("[%d]: Player 3 computer player\n",
					getpid());
		}
	}
	
	return status;
}


void SendHands( Card hands[4][13] ) {

  int i,j, status;

  for( i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    for (j=0; j<13; j++) {
      WriteIntOrDie( gameInfo.playerSock[i], hands[i][j] );
    }
    
    ReadIntOrDie( gameInfo.playerSock[i], &status);

    if( status == i ) {
      if( log )
	  dbg_msg(  "[%d]: %s received hand\n", getpid(), gameInfo.players[i] );
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
      svNetClose();
      Quit(-1);
    }
  }
}


void SendLead( int lead ) {

  int i,status;
  
  for (i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP) continue;
    WriteIntOrDie( gameInfo.playerSock[i], lead );
    ReadIntOrDie( gameInfo.playerSock[i], &status );
    if( status == i ) {
      if( log )
	  dbg_msg(  "[%d]: %s received lead\n", getpid(), gameInfo.players[i] );
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
      svNetClose();
      Quit(-1);
    }
  }
}


void GetAndSendBids(int lead, int bids[4], int kneels[4], Card hands[4][13]) {
  
  int i, curPlayer, bid, bidStatus;

  bids[0] = bids[1] = bids[2] = bids[3] = BID_INVALID;
  
  for( i=0; i<4; i++) {
    curPlayer=(lead+i)%4;
    
    if( gameInfo.playerSock[curPlayer] == SOCK_COMP ) {
      bid = AIBid( curPlayer );
      bidStatus = 0;
    }
    else {
      bidStatus = -1;
    }
    
    while( bidStatus != 0) {
      bidStatus = 0;
      
      ReadIntOrDie( gameInfo.playerSock[curPlayer], &bid );
      
      if( bid < -1 || bid > 13 ) {
	if( log )
	    dbg_msg(  "[%d]: Invalid bid received for %s\n", getpid(),
		   gameInfo.players[curPlayer] );
	bidStatus = -1;
      }
      /* Check for minimum bid */
      else if( i > 1 && ( (bid <=0 && (bids[(lead+i-2)%4] < gameInfo.opt.minBid) ) ||   
			 (bid > 0 && (bid+bids[(lead+i-2)%4] < gameInfo.opt.minBid )))) {
	
	if( log )
	    dbg_msg(  "[%d]: Bid below min received for %s\n", getpid(),
		   gameInfo.players[curPlayer] );
	bidStatus = -2;
      }
      
      WriteIntOrDie( gameInfo.playerSock[curPlayer], bidStatus );

    } /* while (bidStatus != 0) */
    
    if ( bid == BID_NIL) {
      if( log )
	  dbg_msg(  "[%d]: Player %d bid nil\n", getpid(), curPlayer );
      kneels[curPlayer] = 1;
      bids[curPlayer] = 0;
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Bid of %d received for %s\n", getpid(), bid,
		  gameInfo.players[curPlayer] );
      kneels[curPlayer] = 0;
      bids[curPlayer] = bid;
    }
    SendBid( bid, curPlayer );
  }
}
  

void SendBid( int bid, int player ) {

  int i, status;
  
  for (i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    if (i !=player) {
      WriteIntOrDie( gameInfo.playerSock[i], bid );
      ReadIntOrDie( gameInfo.playerSock[i], &status );
      if( status == i ) {
	if( log )
	    dbg_msg(  "[%d]: %s received %s's bid\n", getpid(), 
		   gameInfo.players[i], gameInfo.players[player] );
      }
      else {
	if( log )
	    dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
	svNetClose();
	Quit(-1);
      }
    }
  }
}


void GetAndSendTricks( int lead, Card hands[4][13], Card trick[4], 
		       int tally[4], int bids[4], int kneels[4] ) {

  int i, j, curPlayer, handIndex, badCard;
  char leadSuit;
  Card playedCard;

  trick[0] = trick[1] = trick[2] = trick[3] = BLANK_CARD;
      
  for( i=0; i<4; i++) {
    curPlayer=(lead+i)%4;

    if( gameInfo.playerSock[curPlayer] == SOCK_COMP ) {
      handIndex = AIPlay(curPlayer, lead );
      playedCard = hands[curPlayer][handIndex]; 
      if ( curPlayer == lead ) { 
	leadSuit=card_suit_char(playedCard);
      }
      PlayCard( curPlayer, handIndex, hands, trick, lead, leadSuit );
      badCard = 0;
    }
    else {
      badCard = -1;
    }
    
    while( badCard != 0 ) {
      badCard=0;

      ReadIntOrDie( gameInfo.playerSock[curPlayer], &handIndex );
      
      /* Check first for re-playing cards*/
      playedCard = hands[curPlayer][handIndex];

      if( playedCard == BLANK_CARD ) {
	badCard = -1;
	if( log )
	  dbg_msg(  "[%d]: %s tried to replay a card\n", getpid(),
		  gameInfo.players[curPlayer] );
      }
      else if( curPlayer == lead ) { /* good card led */
	leadSuit = card_suit_char(playedCard);
      } 
      else if(card_suit_char(playedCard) != leadSuit ) { /* check for renege */
	for( j=0; j<13; j++ ) {
	  if( card_suit_char(hands[curPlayer][j]) == leadSuit ) {
	    badCard = -2;
	    if( log ) {
	      dbg_msg(  "[%d]: %s tried to renege with the %s\n", getpid(),
		      gameInfo.players[curPlayer], 
		      card_name(playedCard,LONG_NAME) );
	      dbg_msg(  "[%d]: - %s still in %dth in hand\n", getpid(),
		      card_name(hands[curPlayer][j],LONG_NAME), j);
	    }
	    break;
	  }
	}
      }

      if( badCard == 0) {
	PlayCard(curPlayer, handIndex, hands, trick, lead, leadSuit);
      }
      
      WriteIntOrDie( gameInfo.playerSock[curPlayer], badCard );

    } /* while( badCard != 0 );*/

    SendTrick( trick[curPlayer], curPlayer );
    
  } /*endfor (i=0to3) */
}


void PlayCard(int num, int index, Card hands[4][13], Card trick[4], int lead, 
	      int lead_suit) {
  
  int i, s, r, high;
  Card card = hands[num][index];


  if( log ) {
    if( num == lead) {
      dbg_msg(  "[%d]: %s led the %s\n", getpid(), gameInfo.players[num],
	      card_name(card,LONG_NAME) );
    }
    else {
      dbg_msg(  "[%d]: %s played the %s\n", getpid(), gameInfo.players[num],
	      card_name(card,LONG_NAME) );
    }
  }
  
  /*
   * Remove card from player's hand.
   */
  hands[num][index] = BLANK_CARD;
  
  /*
   * Determine the highest card played so far.
   */
  high = lead;
  for( i = (lead+1)%4; i != num; i = (i+1) % 4 ) {
    if( card_comp(trick[i], trick[high]) > 0 )  {
      high = i;
    }
  }
  
  /*
   * Set state variables.
   */
  s = card_suit_num(card);
  r = card_rank(card);
  trick[num] = card;
  played[s] |= (1<<r);
  if( s != lead_suit) {
    suits[num][lead_suit] = 0;	/* he must be void */
#if 0
    if( kneels[num] && tally[num] == 0 && s != SPADES ) {
      /* He should have sluffed his highest in this suit. */
      for (i = r + 1; i < 13; i++)
	  suit[num][s] &= ~(1<<i);
    }
#endif
  }

#if 0
  if( s == lead_suit kneels[num] && tally[0] == 0) {
    if(!(suits[num][s] & (1<<r))) {
      /*
       * If we didn't think this player could have this card, then he's
       * doing something funky and all bets are off.  He might have anything.
       */
      suits[num][s] = (~played[s]) & 0x1fff;
    }
    else if( card < trick[high] ) {
      /*
       * nil bids don't have any between this card and highest play
       */
      for(i = r + 1; i < card_rank(trick[high]); i++)
	  suits[num][s] &= ~(1<<i);
    }
  }
#endif
  

  /*
   * Ok, nobody has this card any more.
   */
  for(i = 0; i < 4; i++) {
      suits[i][s] &= ~(1<<r);
  }

  
}


void SendTrick( Card play, int player ) {

  int i, status;

  for( i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    if( i !=player ) {
      WriteIntOrDie( gameInfo.playerSock[i], play );
      ReadIntOrDie( gameInfo.playerSock[i], &status );
      if( status == i ) {
	if( log )
	    dbg_msg(  "[%d]: %s received %s's play\n", getpid(), 
		   gameInfo.players[i], gameInfo.players[player] );
      }
      else {
	if( log )
	    dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
	svNetClose();
	Quit(-1);
      }
    }
  }
}


int CalcTrickWin( int lead, Card trick[4] ) {
  
  int i, win=lead;
  
  for( i=(lead+1)%4; i != lead; i=(i+1)%4 ) {
    if( card_comp( trick[i], trick[win] ) > 0 )
      win = i;
  }
  if( log )
      dbg_msg(  "[%d]: %s won the trick\n", getpid(), gameInfo.players[win] );
  
  return win;
}


void InitTallys( int tally[4] ) {
  
  int i,j;
  
  for(i=0; i<4; i++) {
    tally[i]=0;
    played[i]=0;
    for(j=0; j<4; j++)
	suits[i][j] = 0x1fff;
  }
}


void UpdateTallys( int winner, int tally[4] ) {
  tally[winner]++;
}


void SendTallys( int winner, int tally[4] ) {

  int i, j, status;

  for(i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    WriteIntOrDie( gameInfo.playerSock[i], winner );
    for (j=0; j<4; j++)
      WriteIntOrDie( gameInfo.playerSock[i], tally[j] );
    ReadIntOrDie( gameInfo.playerSock[i], &status );
    if( status == i ) {
      if( log )
	  dbg_msg(  "[%d]: %s received tally update\n", getpid(), 
		 gameInfo.players[i] );
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
      svNetClose();
      Quit(-1);
    }
  }
}


void InitScores( int scores[2], int bags[2] ) {
  scores[0]=scores[1]=bags[0]=bags[1]=0;
}


int UpdateScores( int tally[4], int bids[4], int scores[2], int kneels[4], 
		 int bags[2] ) {

  int i;

  /* Add/Subtract bids and bags */
  for( i=0; i<2; i++) {
    if ( (tally[i]+tally[i+2]) >= (bids[i]+bids[i+2]) ) {
      bags[i] = bags[i] + (tally[i]+tally[i+2]) - (bids[i]+bids[i+2]);
      scores[i] = scores[i] + (bids[i]+bids[i+2])*10 +
	(tally[i]+tally[i+2]) - (bids[i]+bids[i+2]);
      if ( bags[i] >= 10 ) {
	bags[i]-=10;
	scores[i]-=100;
      }
    }
    else
      scores[i] = scores[i] - (bids[i]+bids[i+2])*10;
  }

  /*Check for kneeling*/
  for( i=0; i<4; i++) {
    if( log )
	dbg_msg(  "[%d]: Checking for Player %d nil bids\n", getpid(), i);
    if( kneels[i] == 1 ) {
      if( log )
	  dbg_msg(  "[%d]: Player %d bid nil -", getpid(), i);
      if( tally[i] == 0 ) {
	if( log )
	    dbg_msg(  "and made it.\n" );
	scores[ (i%2) ]+=100;
      }
      else {
	if( log )
	    dbg_msg(  "and lost it.\n" );
	scores[ (i%2) ]-=100;
      }
    }
  }

  if( log )
      dbg_msg(  "[%d]: Team 0: %d\tTeam 1: %d\n", getpid(), scores[0], scores[1]);

  return ( (scores[0] >= gameInfo.opt.endGame || scores[1] >= gameInfo.opt.endGame)
	  && scores[0] != scores[1] );

}


void SendScores( int scores[2] ) {

  int i, status,
  gameOver = ( scores[0] >= gameInfo.opt.endGame || scores[1] >= gameInfo.opt.endGame ) && 
             ( scores[0] != scores[1] );

  /* Put victor team number in 3rd bit */
  if( gameOver ) {
    gameOver = gameOver | ( ( scores[1] > scores[0] ) << 2 );
  }

  for( i=0; i<4; i++ ) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    WriteIntOrDie( gameInfo.playerSock[i], scores[0]);
    WriteIntOrDie( gameInfo.playerSock[i], scores[1]);

    /*Send first player a new-game query requestin 2nd bit */
    if( i==0 && gameOver ) {  
      WriteIntOrDie( gameInfo.playerSock[i], gameOver | GAME_QUERY );
    }
    else {
      WriteIntOrDie( gameInfo.playerSock[i], gameOver );
    }

    ReadIntOrDie( gameInfo.playerSock[i], &status );
    if( status == i ) {
      if( log )
	  dbg_msg(  "[%d]: %s received Scores\n", getpid(), gameInfo.players[i]);
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
      svNetClose();
      Quit(-1);
    }
  }
}


int QueryNewGame( void ) {
  
  int i, again, status;
  
  /* Non-zero value for again means play again*/
  ReadIntOrDie( gameInfo.playerSock[0], &again );
  if( log )
      dbg_msg(  "[%d]: Play again: %d\n", getpid(), again );
  WriteIntOrDie( gameInfo.playerSock[0], 0 );
  
  
  /* Send new game flag to everyone (including first player)*/
  for( i=0; i<4; i++) {
    if( gameInfo.playerSock[i] == SOCK_COMP ) continue;
    WriteIntOrDie( gameInfo.playerSock[i], again);
    ReadIntOrDie( gameInfo.playerSock[i], &status );
    if( status == i ) {
      if( log )
	  dbg_msg(  "[%d]: %s received New Game Status\n", getpid(), gameInfo.players[i] );
    }
    else {
      if( log )
	  dbg_msg(  "[%d]: Error: player %d returned %d\n", getpid(), i, status);
      svNetClose();
      Quit(-1);
    }
  }

  return ( again !=0 );
  
}
	

void SendGameOver(void) {

  WriteIntOrDie(STDOUT_FILENO, MSG_GAME_OVER);
  WriteIntOrDie(STDOUT_FILENO, 0);
  /* FIXME: Send player stats */
}


RETSIGTYPE termination_handler(int signum) {

  signal (signum, SIG_DFL);

  switch( signum ) {
  /* We let our error checker handle broken pipes */
  case SIGPIPE: break;
  case SIGUSR1:
    /* Death from above */
    svNetClose();
    Quit(-1);
    break;
  default:
    dbg_msg( "[%d]: Closing sockets for game %d\n", getpid(), gameInfo.gameNum );
    svNetClose();
    raise(signum);
  }
}


/*
 * Do the correct exit procedure for whether we're server or client
 */
void Quit( int status ) {
  
  if( gameInfo.opt.bitOpt & MSK_OFFLINE ) {
    _exit( status );
  }
  else {
    exit( status );
  }
}
    


 
