#ifndef KTICTACTUX_H
#define KTICTACTUX_H

// KDE includes
#include <kmainwindow.h>

// KTicTacTux includes
#include "config.h"
#include "ktictactuxproto.h"

// Qt includes
#include "qwhiteframe.h"
#include <qlabel.h>
#include <qevent.h>

// Player setup
#define PLAYER_NONE    0
#define PLAYER_HUMAN   1
#define PLAYER_AI      2
#define PLAYER_NETWORK 3

class KTicTacTux : public KMainWindow
{
	Q_OBJECT
	public:
		KTicTacTux(QWidget *parent = NULL, char *name = NULL);
		~KTicTacTux();
		// Set opponent type
		void setOpponent(int type);
		// Set up the game
		void init();

	public slots:
		void slotSelected(QWidget *widget);
		void slotNetwork();

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

		//QFrame *fboard;
		QWhiteFrame *frame[3][3];
		WId m_firstid;
		int m_turn;
		QLabel *label;
		int m_x, m_y;
		int m_score_opp, m_score_you;
		int m_winner;
		int m_opponent;
		KTicTacTuxProto *proto;
};

#endif
