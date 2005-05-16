///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

#ifndef KCC_H
#define KCC_H

// KCC includes
#include "kccproto.h"

// Qt includes
#include <qevent.h>
#include <qwidget.h>

// Player setup
#define PLAYER_NONE    0
#define PLAYER_HUMAN   1
#define PLAYER_AI      2
#define PLAYER_NETWORK 3

// KCC GUI for KDE
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
		// Set number of opponents
		void setPlayers(int players);
		// Set up the game
		void init();
		// Request a synchonization
		void sync();
		// Request network scores
		void statistics();
		// Network operation
		void network();
		// Theme change
		void setTheme(QString theme, QString themetype);
		// Parent central widget
		QWidget *widget();

	public slots:
		// Evaluate user input
		void slotSelected(QWidget *widget);
		// Evaluate network input
		void slotNetwork();
		// Evaluate network control input
		void slotDispatch();
		// Delayed opponent moves
		void slotOpponentMove();

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
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void paintEvent(QPaintEvent *e);

	private slots:
		// Wait for player input
		void slotYourMove();

	private:
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
		// Toggle external board
		void toggleBoard();

		// Movement helper methods
		QPoint newPoint(const QPoint& current, int direction);
		bool findTarget(const QPoint& current, const QPoint& target, bool jumps);

		enum Directions
		{
			left,
			leftup,
			rightup,
			right,
			rightdown,
			leftdown
		};

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
		// Current theme
		QString m_theme, m_themetype;

		// Mouse moving
		int m_mx, m_my, m_moved;
		// Move source and destination
		int m_fx, m_fy, m_tx, m_ty, m_ax, m_ay;
		// Move steps
		QValueList<QPoint> m_waypoints;

		// Fake parent
		QWidget *m_parent, *m_parentmain;
};

#endif

