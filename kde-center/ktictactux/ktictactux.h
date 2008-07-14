//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KTICTACTUX_H
#define KTICTACTUX_H

// KTicTacTux includes
#include "ktictactuxproto.h"

// GGZ-KDE-Games includes
#include <kggzmod/module.h>

// KDE includes
#include <qwidget.h>

// Qt includes
#include <qevent.h>

// Player setup
#define PLAYER_NONE    0
#define PLAYER_HUMAN   1
#define PLAYER_AI      2
#define PLAYER_NETWORK 3

// Forward declaration
class QWhiteFrame;
class QPaintEvent;
class QLabel;

// TicTacToe GUI for KDE
class KTicTacTux : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KTicTacTux();
		// Destructor
		~KTicTacTux();
		// Set opponent type
		void setOpponent(int type);
		// Sets the theme
		void setTheme(QString t1, QString t2);
		// Set up the game
		void init();
		// Request a synchonization
		void sync();
		// Request network scores
		void statistics();
		// Network operation
		void network();
		// Display seats dialog
		void seats();

		//GGZMod *getMod();

	public slots:
		// Evaluate user input
		void slotSelected(QWidget *widget);
		// Network input (quantized packets) from GGZ
		void slotPacket();
		// Network error
		void slotError();
		// Input from GGZ
		void slotEvent(const KGGZMod::Event& event);

	signals:
		// Emit the game status
		void signalStatus(const QString &status);
		// Emit the score
		void signalScore(const QString &score);
		// Emit statistics
		void signalNetworkScore(int wins, int losses, int ties);
		// Emit game over status
		void signalGameOver();

	protected:
		void paintEvent(QPaintEvent *event);

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
		// Theme image placeholders
		QString m_t1, m_t2;
		// The surrounding area
		QLabel *m_container;
		// old container size
		QSize m_oldsize;
};

#endif

