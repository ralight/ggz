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
#include <QWidget>

class QPushButton;
class QFrame;
class QGridLayout;

// KConnectX GUI for KDE
class KConnectX : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KConnectX(QWidget *parent = 0);
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

	private:
		// Ask for another game, showing current results
		void announce(QString str);
		// Perform a move
		void doMove(QString colour, int column);

		// Internal protocol class
		Proto *m_proto;
		// Grid with columns and buttons
		QMap<const QPushButton*, int> m_buttons;
		QMap<int, QFrame*> m_widgets;
		QMap<int, int> m_stacks;
		// Layout for the grid
		QGridLayout *m_widgetbox;
};

#endif

