/*
 * File: client_func.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
 * $Id: client_func.c 6333 2004-11-12 02:27:20Z jdorje $
 *
 * This file contains the support functions which do the dirty work of
 * playing spades.  This file is an attempt to remain modular so that
 * it can be modified without affecting play, or having to greatly
 * modify the client_main.cc file which contains the general playing
 * algorithm
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


#include <config.h>	/* Site config data */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>
#include <unistd.h>	/* For getopt et. al */
#include <stdlib.h>	/* For exit, atoi */
#include <signal.h>	/* For signal */
#include <sys/wait.h>

#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif

#include <string.h>	/* For strcpy */

#include <ggz.h>	/* libggz */
#include <ggz_common.h>
#include <ggzmod.h>

#include "dlg_players.h"

#include "card.h"
#include "display.h"
#include "client.h"
#include "options.h"
#include "socketfunc.h"
#include "gtk_connect.h"
#include "gtk_dlg_options.h"

#define basename(path) (strrchr(path,'/')==NULL) ? path : strrchr(path, '/')+1

/* Global state of game variable */
extern gameState_t gameState;

extern guint spadesHandle;

/* Game options */
option_t options;

static GGZMod *ggzmod = NULL;

/* Event handlers for ggzmod */
static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e, void *data);


static gboolean handle_ggz(GIOChannel * channel, GIOCondition cond,
			   gpointer data)
{
	return (ggzmod_dispatch(ggzmod) >= 0);
}


int CheckReadInt(int msgsock, int *message)
{

	int status, success = NET_OK;

	status = readint(msgsock, message);

	if (status < 0) {
		DisplayError("Network error receiving int");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	} else if (status == 0) {
		DisplayError("Socket closed while reading int");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	}

	return success;
}


int CheckReadString(int msgsock, char **message)
{

	int status, success = NET_OK;

	status = readstring(msgsock, message);

	if (status < 0) {
		DisplayError("Network error receiving string");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	} else if (status == 0) {
		DisplayError("Socket closed while reading string");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	}

	return success;

}


int CheckWriteInt(int msgsock, int message)
{

	int status, success = NET_OK;

	status = writeint(msgsock, message);

	if (status <= 0) {
		DisplayError("Network error sending int");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	}

	return success;
}


int CheckWriteString(int msgsock, const char *message)
{

	int status, success = NET_OK;

	status = writestring(msgsock, message);

	if (status <= 0) {
		DisplayError("Network error sending string");
		NetClose();
		DisplayCleanup();
		success = NET_FAIL;
	}

	return success;
}

/*
 * One time application init.
 */
void AppInit(void)
{
	int i;
	GIOChannel *channel;

	gameState.get_opt = FALSE;
	gameState.gameSegment = ST_GET_ID;

	options.endGame = 100;
	options.minBid = 3;
	options.bitOpt = GAME_SPADES | MSK_NILS | MSK_BAGS | MSK_COMP_1
	    | MSK_COMP_2 | MSK_COMP_3;

	/* Init dynamic memory to NULL */
	gameState.record = NULL;

	for (i = 0; i < 4; i++)
		gameState.players[i] = NULL;

	/* use libggzmod to connect to GGZ.  --JDS */
	ggzmod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SERVER,
			   &handle_ggzmod_server);
	init_player_list(ggzmod);
	ggzmod_connect(ggzmod);

	channel = g_io_channel_unix_new(ggzmod_get_fd(ggzmod));
	spadesHandle = g_io_add_watch(channel, G_IO_IN, handle_ggz, NULL);
}


static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e, void *data)
{
	int fd = *(int *)data;
	GIOChannel *channel;

	ggzmod_set_state(ggzmod, GGZMOD_STATE_PLAYING);
	gameState.spadesSock = fd;
	channel = g_io_channel_unix_new(fd);
	spadesHandle = g_io_add_watch(channel, G_IO_IN,
				      ReadServerSocket, NULL);
}


/*
 * Initialize values for each session
 */
void SessionInit(void)
{

	int i;

	gameState.gameSegment = ST_REGISTER;

	gameState.gameOver = 0;
	gameState.sessionOver = 0;
	gameState.gameCount = 0;
	gameState.spadesSock = -1;

	if (gameState.record != NULL) {
		free(gameState.record);
	}
	gameState.record = NULL;

	for (i = 0; i < 4; i++) {
		if (gameState.players[i] != NULL) {
			free(gameState.players[i]);
			gameState.players[i] = NULL;
		}
	}

}


/*
 * Initialize values for each game
 */
void GameInit(void)
{

	gameState.curPlayer = 0;
	gameState.curTrick = 0;
	gameState.lead = 0;
	gameState.gameOver = 0;
	gameState.scores[0] = 0;
	gameState.scores[1] = 0;
	gameState.bids[0] = BID_BLANK;
	gameState.bids[1] = BID_BLANK;
	gameState.bids[2] = BID_BLANK;
	gameState.bids[3] = BID_BLANK;
	gameState.tallys[0] = 0;
	gameState.tallys[1] = 0;
	gameState.tallys[2] = 0;
	gameState.tallys[3] = 0;

}


/**
 * Parse command line options into gameState variable
 */
int ParseOptions(int argc, char *argv[])
{
	int option, status = 0;

	opterr = 0;

	while ((option = getopt(argc, argv, "Vos:n:p:t:")) != -1) {
		switch (option) {
		case 'n':
			gameState.userName = optarg;
			break;
		case 'o':
			gameState.get_opt = TRUE;
			break;
		case 'V':
			g_printerr
			    ("Gnu Gaming Zone spades client version %s\n",
			     VERSION);
			status = -1;
			break;
		default:
			g_printerr("Usage: %s [-V] [-o] [-n name]\n",
				   basename(argv[0]));
			status = -1;
			break;
		}
	}
	return status;
}


/**
 * Create socket to talk to spades server
 */
int Register(void)
{

	gameState.spadesSock =
	    makesocket(gameState.serverPort, CLIENT, gameState.serverName);

	if (gameState.spadesSock < -2)
		close(gameState.spadesSock);

	return gameState.spadesSock;

}


/**
 * Register with taunt server
 */
int RegisterTaunt(void)
{

	int tsStatus, status = NET_FAIL;

	gameState.tauntSock =
	    makesocket(gameState.tauntPort, CLIENT, gameState.serverName);

	if (gameState.tauntSock > 0
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.gameNum) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.gamePid) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.playerId) == NET_OK
	    && CheckWriteInt(gameState.tauntSock, getpid()) == NET_OK) {

		status = CheckReadInt(gameState.tauntSock, &tsStatus);
	}

	return (status == NET_OK ? tsStatus : status);
}


/**
 * Read playerId from server.  If successful, send back name and 
 * process id as response.
 */
int GetPlayerId(void)
{

	int status;
	g_printerr("Getting my ID\n");

	if ((status =
	     CheckReadInt(gameState.spadesSock,
			  &gameState.playerId) == NET_OK))
		g_printerr("My ID is %d\n", gameState.playerId);

	/* If we're player 0, read in game options */
	if (gameState.playerId == 0)
		OptionsDialog(NULL, NULL);

#if 0
	NET_OK
	    && gameState.playerId > -1
	    && CheckWriteString(gameState.spadesSock,
				gameState.userName) == NET_OK
	    && (gameState.playerId != 0
		|| (CheckWriteInt(gameState.spadesSock, -2) == NET_OK
		    && CheckWriteInt(gameState.spadesSock,
				     options.bitOpt) == NET_OK
		    && CheckWriteInt(gameState.spadesSock,
				     options.endGame) == NET_OK
		    && CheckWriteInt(gameState.spadesSock,
				     options.minBid) == NET_OK)) {

		status = CheckWriteInt(gameState.spadesSock, getpid());

	}
#endif

	return status;
}


/**
 * Read game information from server
 */
int GetGame(void)
{

	int status = NET_FAIL;

	ggz_debug("main", "Getting data");
	if (CheckReadInt(gameState.spadesSock, &(gameState.gameNum)) ==
	    NET_OK
	    && CheckReadInt(gameState.spadesSock,
			    &(gameState.playerId)) == NET_OK)
		status =
		    CheckReadInt(gameState.spadesSock,
				 &(gameState.gamePid));

	ggz_debug("main", "%d is my ID", gameState.playerId);
	return status;

}


/**
 * Read player information from server
 */
int GetPlayers(void)
{

	int i, status = NET_FAIL;

	for (i = 0; i < 4; i++) {
		if ((status =
		     CheckReadString(gameState.spadesSock,
				     &gameState.players[i])) == NET_FAIL)
			break;
		ggz_debug("main", "Player %d is named %s", i,
			  gameState.players[i]);
	}

	/*Send back id as OK */
	if (status == NET_OK) {
		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	return status;

}



/**
 * Get player's hand as array of ints from server
 * and use it to initialize hand 
 */
int GetHand(void)
{

	int i, index, status = NET_FAIL;

	for (i = 0; i < 13; i++) {
		if ((status = CheckReadInt(gameState.spadesSock, &index))
		    == NET_OK) {
			gameState.hand[i] = index;
		} else
			break;
	}

	/*Send back id as OK */
	if (status == NET_OK) {
		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	return status;
}


/**
 * Get number of player who leads off trick
 */
int GetLead(void)
{

	int status = NET_FAIL;

	if (CheckReadInt(gameState.spadesSock, &(gameState.lead)) ==
	    NET_OK) {
		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	return status;

}


int GetBid(void)
{

	int status = NET_FAIL;

	if (CheckReadInt(gameState.spadesSock,
			 &(gameState.bids[gameState.curPlayer])) ==
	    NET_OK) {
		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	return status;

}


int SendMyBid(void)
{

	int bidStatus, status = NET_FAIL;

	if (CheckWriteInt(gameState.spadesSock,
			  gameState.bids[gameState.playerId]) == NET_OK) {
		status = CheckReadInt(gameState.spadesSock, &bidStatus);
	}

	return (status == NET_OK ? bidStatus : status);
}


int SendCard(int handIndex)
{

	int cardStatus, status = NET_FAIL;

	if (CheckWriteInt(gameState.spadesSock, handIndex) == NET_OK) {
		status = CheckReadInt(gameState.spadesSock, &cardStatus);
	}

	return (status == NET_OK ? cardStatus : status);
}


int SendTaunt(char *buf)
{

	int status = NET_FAIL;

	if (CheckWriteInt(gameState.tauntSock, gameState.gameNum) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.gamePid) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.playerId) == NET_OK
	    && CheckWriteInt(gameState.tauntSock, getpid()) == NET_OK) {

		status = CheckWriteString(gameState.tauntSock, buf);
	}

	return status;
}


Card GetPlayedCard(void)
{

	int index, status = NET_FAIL;
	Card temp;

	if (CheckReadInt(gameState.spadesSock, &index) == NET_OK) {
		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	if (status == NET_OK) {
		temp = index;
	} else {
		temp = BLANK_CARD;
	}
	return temp;
}


/**
  * Get tally updates from server
  */
int GetTallys(void)
{

	int i, trickWin, status = NET_FAIL;

	if (CheckReadInt(gameState.spadesSock, &trickWin) == NET_OK) {
		DisplayStatusLine("%s won the trick",
				  gameState.players[trickWin]);

		for (i = 0; i < 4; i++) {
			if ((status = CheckReadInt(gameState.spadesSock,
						   &(gameState.
						     tallys[i]))) ==
			    NET_FAIL) {
				break;
			}
		}

		/*Send back id as OK */
		if (status == NET_OK) {
			status =
			    CheckWriteInt(gameState.spadesSock,
					  gameState.playerId);
		}

	}
	return status;
}


/**
  * Get scores from server
  */
int GetScores(void)
{

	int status = NET_FAIL;

	if (CheckReadInt(gameState.spadesSock, &(gameState.scores[0])) ==
	    NET_OK
	    && CheckReadInt(gameState.spadesSock,
			    &(gameState.scores[1])) == NET_OK
	    && CheckReadInt(gameState.spadesSock,
			    &gameState.gameOver) == NET_OK) {

		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
#ifdef DEBUG
		DisplayStatusLine("Game over : %d", gameState.gameOver);
#endif
	}

	/* Increment game counter and add to session statistics */
	if (status == NET_OK && (gameState.gameOver & GAME_OVER)) {
		gameState.gameCount++;
		gameState.record =
		    (int *)realloc(gameState.record,
				   2 * (gameState.gameCount) *
				   sizeof(int));
		gameState.record[2 * gameState.gameCount - 2] =
		    gameState.scores[0];
		gameState.record[2 * gameState.gameCount - 1] =
		    gameState.scores[1];

	}

	return status;
}


int GetNewGame(void)
{

	int newGame, status = NET_FAIL;

	if (CheckReadInt(gameState.spadesSock, &newGame) == NET_OK) {
		gameState.sessionOver = !(newGame);

#ifdef DEBUG
		DisplayStatusLine("Get new game value : %d",
				  gameState.sessionOver);
#endif

		status =
		    CheckWriteInt(gameState.spadesSock,
				  gameState.playerId);
	}

	return status;

}


int SendQuery(int again)
{

	int queryStatus, status = NET_FAIL;

	if (CheckWriteInt(gameState.spadesSock, again) == NET_OK) {
		status = CheckReadInt(gameState.spadesSock, &queryStatus);
	}

	return status;
}


void UpdateGame(void)
{

	/* Perform state specific tasks **before** waiting for input */
	switch (gameState.gameSegment) {
	case ST_REGISTER:
	case ST_GET_ID:
	case ST_GET_GAME:
	case ST_GET_NAMES:
	case ST_REG_TAUNT:
	case ST_GET_HAND:
		gameState.gameSegment++;
		break;

	case ST_GET_LEAD:
	case ST_GET_BIDDER:
		gameState.curPlayer = gameState.lead;
		gameState.gameSegment++;
		break;

	case ST_GET_BIDS:
		gameState.curPlayer = (gameState.curPlayer + 1) % 4;
		if (gameState.curPlayer == gameState.lead) {
			gameState.curTrick = 0;
			gameState.gameSegment++;
		}
		break;

	case ST_GET_TRICK:
		gameState.curPlayer = (gameState.curPlayer + 1) % 4;
		if (gameState.curPlayer == gameState.lead) {
			gameState.gameSegment++;
			gameState.curTrick++;
		}
		break;

	case ST_GET_TALLY:
		if (gameState.curTrick < 13)
			gameState.gameSegment = ST_GET_LEAD;
		else
			gameState.gameSegment++;
		break;

	case ST_GET_SCORE:
		if (gameState.gameOver)
			gameState.gameSegment++;
		else
			gameState.gameSegment = ST_GET_HAND;
		break;

	case ST_END_GAME:
		switch (gameState.sessionOver) {

		case 0:	/* Let's play another round, eh? */
			gameState.gameSegment = ST_GET_HAND;
			break;
		case 1:	/* Session is over, goodnight! */
			gameState.gameOver = GAME_OVER;
			break;
		}
		break;

	}	/*switch( gameState.gameSegment ) */

#ifdef DEBUG
	DisplayStatusLine("Current game state : %d",
			  gameState.gameSegment);
#endif

	DisplayPrompt();
}


void NetClose(void)
{
	g_printerr("I'm dying 2\n");
	g_source_remove(spadesHandle);
	if (ggzmod_disconnect(ggzmod) < 0)
		exit(-2);	/* is this the desired behavior? */
	ggzmod_free(ggzmod);
}


RETSIGTYPE die(int sig)
{

	/* We let our error checker handle broken pipes */
	if (sig != SIGPIPE) {
		NetClose();
		DisplayCleanup();
		signal(sig, SIG_DFL);	/* Reset old signal handler */
		raise(sig);	/* Re-send signal so other cleanup gets done */
	}
}
