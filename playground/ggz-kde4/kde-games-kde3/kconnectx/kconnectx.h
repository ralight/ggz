//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KCONNECTX_H
#define KCONNECTX_H

// KConnectX includes
#include "proto.h"

// Qt includes
#include <qevent.h>
#include <qwidget.h>

class QPushButton;
class QFrame;
class QGridLayout;
class QHBoxLayout;

// KConnectX GUI for KDE
class KConnectX : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KConnectX(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KConnectX();
		// Request network scores
		void statistics();
		// Get protocol object
		Proto *proto() const;

	public slots:
		// Evaluate user input
		void slotInput();
		// Evaluate network input
		void slotOptionsRequest(char minboardwidth, char maxboardwidth,
			char minboardheight, char maxboardheight,
			char minconnectlength, char maxconnectlength);
		void slotOptions(char boardwidth, char boardheight, char connectlength);
		void slotMoveRequest();
		void slotMoveResponse(char movestatus, int move);
		void slotMove(int move);
		void slotOver(char winner);

	signals:
		// Emit the game status
		void signalStatus(const QString &status);
		// Emit statistics
		void signalNetworkScore(int wins, int losses, int ties);
		// Emit game over status
		void signalGameOver();

	protected:
		void resizeEvent(QResizeEvent *e);

	private:
		// Ask for another game, showing current results
		void announce(QString str);
		// Perform a move
		void doMove(QString colour, int column);
		// Draw the game board
		void drawBoard();

		// Internal protocol class
		Proto *m_proto;
		// Grid with columns and buttons
		QMap<const QPushButton*, int> m_buttons;
		QMap<int, QFrame*> m_widgets;
		QMap<int, int> m_stacks;
		QMap<int, QString> m_colours;
		// Layout for the grid
		QGridLayout *m_widgetbox;
		int m_boardwidth, m_boardheight;
};

#endif

