/*
 * File: gtk_connect.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 *
 * This fils contains functions for connecting with the server
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


#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk_connect.h>
#include <gtk_dlg_options.h>
#include <gtk_io.h>
#include <gtk_play.h>
#include <gtk_taunt.h>
#include <gtk_dlg_error.h>
#include <client.h>
#include <display.h>
#include <socketfunc.h>
#include <options.h>

/* Delay before clearing table */
#define CLEAR_DELAY 3000

/* Global state of game variable */
extern gameState_t gameState;
extern playArea_t *playArea;
extern option_t options;
extern GtkWidget *tauntBox;
extern GtkWidget *detail_window;

/* Various local handles */
/*static guint tauntHandle = 0;*/
guint spadesHandle = 0;

static gint TableClearTimer(gpointer);



/*
 * disconnect from server, close sockets, etc.
 */
void Disconnect(GtkWidget * widget, gpointer data)
{
	/* Remove only those inputs which are enabled */
	switch (gameState.gameSegment) {
	case ST_GET_HAND:
	case ST_GET_BIDDER:
	case ST_GET_BIDS:
	case ST_GET_LEAD:
	case ST_GET_TRICK:
	case ST_GET_TALLY:
	case ST_GET_SCORE:
	case ST_END_GAME:
	case ST_GET_ID:
	case ST_GET_GAME:
	case ST_GET_NAMES:
	case ST_REG_TAUNT:
		if (spadesHandle) {
			gdk_input_remove(spadesHandle);
			spadesHandle = 0;
		}
	default:
		break;
	}

	/* Close out network connection */
	NetClose();



}


/**
 * Begin offline game (counterpart to Connect() )
 */
void PlayOffline(GtkWidget * pBar)
{
}


/**
 * Read data from the Spades server 
 */
void ReadServerSocket(gpointer data, gint source, GdkInputCondition cond)
{

	Card playedCard;

	switch (gameState.gameSegment) {
	case ST_REGISTER:
		break;
	case ST_GET_ID:
		if (GetPlayerId() == NET_FAIL)
			ConnectDialogReset(data);
		else if (gameState.playerId < 0) {
			ConnectDialogReset(data);
			DisplayError
			    ("Sorry, server full.  Try again later");
		} else {
			UpdateGame();
		}
		break;

	case ST_GET_GAME:
		if (GetGame() == NET_FAIL)
			ConnectDialogReset(data);
		else
			UpdateGame();
		break;

	case ST_GET_NAMES:
		if (GetPlayers() == NET_FAIL)
			ConnectDialogReset(data);
		else {
			UpdateGame();

			/* We're done registering now, so close the dialog box */
			SafeCloseDialog();

			/* Next game State */
			gameState.gameSegment = ST_GET_HAND;

			/* Game is starting now so initialize and clear the screen */
			GameInit();
			DisplayTable();
			DisplayTallys();
			DisplayScores();
		}
		break;

	case ST_GET_HAND:
		DisplayStatusLine("Getting Hand");
		if (GetHand() == NET_OK) {
			DisplayHand();
			DisplayTable();
			DisplayScores();
			UpdateGame();
		}
		break;

	case ST_GET_BIDDER:
		if (GetLead() == NET_OK) {
			DisplayStatusLine("%s bids first.",
					  gameState.players[gameState.
							    lead]);
			UpdateGame();
		}
		break;

	case ST_GET_BIDS:
		if (gameState.curPlayer == gameState.playerId) {
			DisplayError("Network error");
		}

		if (GetBid() == NET_OK) {
			if (gameState.bids[gameState.curPlayer] ==
			    BID_KNEEL)
				DisplayStatusLine("%s bid nil",
						  gameState.players
						  [gameState.curPlayer]);
			else
				DisplayStatusLine("%s bid %d",
						  gameState.players
						  [gameState.curPlayer],
						  gameState.bids
						  [gameState.curPlayer]);

			DisplayTallys();
			UpdateGame();
			if (gameState.gameSegment == ST_GET_LEAD) {
				DisplayScores();
			}
		}
		break;

	case ST_GET_LEAD:
		if (GetLead() == NET_OK) {
			DisplayStatusLine("%s leads",
					  gameState.players[gameState.
							    lead]);
			UpdateGame();
		}
		break;

	case ST_GET_TRICK:
		if (gameState.curPlayer == gameState.playerId) {
			DisplayError("Network error");
		}

		playedCard = GetPlayedCard();
		DisplayPlayedCard(playedCard, gameState.curPlayer,
				  gameState.playerId);

		UpdateGame();
		break;

	case ST_GET_TALLY:
		if (GetTallys() == NET_OK) {
			DisplayTallys();
			/* Set a timeout for when to clear screen */
			gtk_timeout_add(CLEAR_DELAY,
					TableClearTimer, NULL);
			/* Don't accept server input until we've cleared the table */
			gdk_input_remove(spadesHandle);
		}
		break;

	case ST_GET_SCORE:
		if (GetScores() == NET_OK) {
			/* Clear tallys and bids before next hand/game */
			gameState.bids[0] = BID_BLANK;
			gameState.bids[1] = BID_BLANK;
			gameState.bids[2] = BID_BLANK;
			gameState.bids[3] = BID_BLANK;
			gameState.tallys[0] = 0;
			gameState.tallys[1] = 0;
			gameState.tallys[2] = 0;
			gameState.tallys[3] = 0;
			DisplayScores();
			DisplayTallys();
			if (gameState.gameOver & GAME_OVER) {
				DisplayStatusLine("Game Over");
				DisplayRecord();
			}
			UpdateGame();
		}
		break;

	case ST_END_GAME:
		if (GetNewGame() == NET_OK) {
			UpdateGame();
			switch (gameState.sessionOver) {

			case 1:	/* Session is over, so disconnect */
				gameState.gameSegment = ST_REGISTER;
				gameState.gameOver = 0;
				gameState.sessionOver = 0;
				gameState.gameCount = 0;
				gtk_main_quit();
				break;
			case 0:	/* Let's play another round, eh? */
				GameInit();
				DisplayTable();
				DisplayTallys();
				DisplayScores();
				break;
			}
		}
		break;
	}	/* switch( gameState.gameSegment ) */
}


/* 
 * Read data from the TauntServer(tm)
 */
void ReadTauntSocket(gpointer data, gint source, GdkInputCondition cond)
{
	int l;
	char *message;

	if (CheckReadInt(gameState.tauntSock, &l) == NET_OK
	    && CheckReadString(gameState.tauntSock, &message) == NET_OK) {

		DisplayTaunt(message, l);
		free(message);

	}
}

/* 
 * Timer for clearing table after a trick
 */ static gint TableClearTimer(gpointer data)
{
	DisplayTable();
	UpdateGame();
	/* Don't accept server input until we've cleared the table */
	spadesHandle =
	    gdk_input_add(gameState.spadesSock, GDK_INPUT_READ,
			  ReadServerSocket, NULL);

	return FALSE;

}
