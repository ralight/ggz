//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_H
#define KTICTACTUX_H

// KDE includes
#include <qwidget.h>

// KTicTacTux includes
#include "config.h"
#include "ktictactuxproto.h"

// Qt includes
#include "qwhiteframe.h"
#include <qevent.h>

// Player setup
#define PLAYER_NONE    0
#define PLAYER_HUMAN   1
#define PLAYER_AI      2
#define PLAYER_NETWORK 3

// TicTacToe GUI for KDE
class KTicTacTux : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KTicTacTux(QWidget *parent = NULL, char *name = NULL);
		// Destructor
		~KTicTacTux();
		// Set opponent type
		void setOpponent(int type);
		// Set up the game
		void init();

	public slots:
		// Evaluate user input
		void slotSelected(QWidget *widget);
		// Evaluate network input
		void slotNetwork();

	signals:
		// Emit the game status
		void signalStatus(QString status);

	private:
		// Wait for bot or network input
		void opponentTurn();
		// Wait for player input
		void yourTurn();
		// Get player type
		int getPlayer(int seat);
		// Activate next player or bot
		void getNextTurn();
		// Check whether the game is over
		int gameOver();
		// Ask for another game, showing current results
		void announce(QString str);
		// Update the status bar
		void status(QString str);
		// Receive optimum move
		void getAI();
		// Partial AI resolvation
		void getAIAt(int xo, int yo, int xp, int yp);
		// Result in a modulo triplet
		int trip(int value);
		// Show all assigned fields
		void drawBoard();

		// Array of boxes for the images
		QWhiteFrame *frame[3][3];
		// ID of the first box
		WId m_firstid;
		// The player who does the next move
		int m_turn;
		// Coordinates of any field
		int m_x, m_y;
		// The scores
		int m_score_opp, m_score_you;
		// Indicates that there's a winner
		int m_winner;
		// Indicates that there'll be a winner
		int m_seewinner;
		// Type of the opponent
		int m_opponent;
		// Internal protocol class
		KTicTacTuxProto *proto;
};

#endif

