//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_H
#define KTICTACTUX_H

// KDE includes
#include <qwidget.h>

// KTicTacTux includes
#include "config.h"
#include "kccproto.h"

// Qt includes
#include <qevent.h>

// Player setup
#define PLAYER_NONE    0
#define PLAYER_HUMAN   1
#define PLAYER_AI      2
#define PLAYER_NETWORK 3

// TicTacToe GUI for KDE
class KCC : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KCC(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KCC();
		// Set opponent type
		void setOpponent(int type);
		// Set up the game
		void init();
		// Request a synchonization
		void sync();
		// Request network scores
		void statistics();
		// Network operation
		void network();

	public slots:
		// Evaluate user input
		void slotSelected(QWidget *widget);
		// Evaluate network input
		void slotNetwork();
		// Evaluate network control input
		void slotDispatch();

	signals:
		// Emit the game status
		void signalStatus(const QString &status);
		// Emit the score
		void signalScore(const QString &score);
		// Emit statistics
		void signalNetworkScore(int wins, int losses);
		// Emit game over status
		void signalGameOver();

	protected:
		void mousePressEvent(QMouseEvent *e);
		void paintEvent(QPaintEvent *e);

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
		// Show all assigned fields
		void drawBoard();

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
		KCCProto *proto;
		// Theme image placeholders
		QString m_t1, m_t2;

		// Move source and destination
		int m_fx, m_fy, m_tx, m_ty;
};

#endif

